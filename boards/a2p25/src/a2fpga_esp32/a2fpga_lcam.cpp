#include "a2fpga_lcam.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_heap_caps.h"
#include "esp_rom_gpio.h"
#include "soc/gpio_sig_map.h"
#include "soc/lcd_cam_struct.h"
#include "soc/lcd_cam_reg.h"
#include "soc/gdma_struct.h"
#include "soc/gdma_reg.h"

// ---------- Packet / DMA sizing ----------
static const int BYTES_PER_WORD   = 8;   // 8 data nibbles => 8 bytes (low nibble used)
static const int STOP_BYTES       = 2;   // VSYNC + stopper
static const int PACK_BYTES       = BYTES_PER_WORD + STOP_BYTES;  // 10 total
static const int CHUNK_BYTES      = 16;  // >= PACK_BYTES
static const int DESC_COUNT       = 2;   // ping-pong
#define GDMA_CH                  0

// ---------- CAM signal macro compatibility ----------
#ifndef CAM_PCLK_IDX
#  ifdef CAM_PCLK_IN_IDX
#    define CAM_PCLK_IDX CAM_PCLK_IN_IDX
#  else
#    error "CAM_PCLK_IDX / CAM_PCLK_IN_IDX not defined by this Arduino core."
#  endif
#endif
#ifndef CAM_V_SYNC_IDX
#  ifdef CAM_V_SYNC_IN_IDX
#    define CAM_V_SYNC_IDX CAM_V_SYNC_IN_IDX
#  else
#    error "CAM_V_SYNC_IDX / CAM_V_SYNC_IN_IDX not defined by this Arduino core."
#  endif
#endif
#ifndef CAM_H_ENABLE_IDX
#  ifdef CAM_H_ENABLE_IN_IDX
#    define CAM_H_ENABLE_IDX CAM_H_ENABLE_IN_IDX
#  endif
#endif
#ifndef CAM_DATA_IN0_IDX
#  error "CAM_DATA_IN0_IDX..CAM_DATA_IN3_IDX not defined (S3 camera signals missing?)."
#endif
#ifndef GPIO_MATRIX_CONST_ONE_INPUT
#  define GPIO_MATRIX_CONST_ONE_INPUT  0x38
#endif

// ---------- DMA descriptor compatibility ----------
#if __has_include("hal/dma_types.h")
  #include "hal/dma_types.h"
  typedef dma_descriptor_t DESC_T;
  static inline void      desc_set_buf(DESC_T* d, uint8_t* buf){ d->buffer = buf; }
  static inline uint8_t*  desc_get_buf(DESC_T* d){ return (uint8_t*)d->buffer; }
  static inline void      desc_set_next(DESC_T* d, DESC_T* n){ d->next = n; }
  static inline void      desc_prep(DESC_T* d, uint8_t* buf, size_t len, DESC_T* next){
    memset(d, 0, sizeof(*d));
    d->buffer = buf;
    d->dw0.size = len;
    d->dw0.length = len;
    d->dw0.owner = 1;
    d->dw0.suc_eof = 0;
    d->next = next;
  }
  static inline void      desc_rearm(DESC_T* d){ d->dw0.owner = 1; d->dw0.suc_eof = 0; }
  static inline uint32_t  desc_len(DESC_T* d){ return d->dw0.length; }
#else
  typedef struct {
    volatile uint32_t size    :12;
    volatile uint32_t length  :12;
    volatile uint32_t suc_eof :1;
    volatile uint32_t owner   :1;
    volatile uint32_t rsvd    :6;
    void*    buffer;
    void*    next;
  } DESC_T;
  static inline void      desc_set_buf(DESC_T* d, uint8_t* buf){ d->buffer = buf; }
  static inline uint8_t*  desc_get_buf(DESC_T* d){ return (uint8_t*)d->buffer; }
  static inline void      desc_set_next(DESC_T* d, DESC_T* n){ d->next = n; }
  static inline void      desc_prep(DESC_T* d, uint8_t* buf, size_t len, DESC_T* next){
    memset(d, 0, sizeof(*d));
    d->buffer = buf;
    d->size = len;
    d->length = len;
    d->owner = 1;
    d->suc_eof = 0;
    d->next = next;
  }
  static inline void      desc_rearm(DESC_T* d){ d->owner = 1; d->suc_eof = 0; }
  static inline uint32_t  desc_len(DESC_T* d){ return d->length; }
#endif

// ---------- Lock-free SPSC ring ----------
static const uint32_t RB_SIZE = 1024;      // power of two
static uint32_t       rb_data[RB_SIZE];
static volatile uint32_t rb_head = 0;      // producer writes
static volatile uint32_t rb_tail = 0;      // consumer writes
static volatile uint32_t rb_drops = 0;

static inline bool rb_push(uint32_t w) {
  uint32_t h = rb_head;
  uint32_t n = (h + 1) & (RB_SIZE - 1);
  if (n == rb_tail) { rb_drops++; return false; }
  rb_data[h] = w;
  rb_head = n;
  return true;
}
static inline bool rb_pop(uint32_t* out) {
  uint32_t t = rb_tail;
  if (t == rb_head) return false;
  *out = rb_data[t];
  rb_tail = (t + 1) & (RB_SIZE - 1);
  return true;
}
static inline void rb_reset(){ rb_head = rb_tail = 0; rb_drops = 0; }

// ---------- Runtime state ----------
static uint8_t  s_clk_inv = 0;            // 0=rising, 1=falling
static uint32_t word_print_every = 512;
static volatile uint32_t words_seen = 0;   // bumped by consumer
static volatile uint8_t s_log_level = 0;   // 0=off, 1=errors, 2=all

static uint8_t* s_buf      = nullptr;     // DESC_COUNT * CHUNK_BYTES
static DESC_T*  s_desc     = nullptr;     // [DESC_COUNT], 16-byte aligned

static TaskHandle_t s_poller_task   = nullptr;
static TaskHandle_t s_consumer_task = nullptr;

// ---------- Helpers ----------
static inline void route_in(int gpio, int sig_idx, bool inv=false) {
  esp_rom_gpio_connect_in_signal(gpio, sig_idx, inv);
}
static inline void route_const_one_to(int sig_idx) {
#ifdef CAM_H_ENABLE_IDX
  esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, sig_idx, false);
#endif
}
static inline bool dma_eof() { return GDMA.channel[GDMA_CH].in.int_st.in_suc_eof; }
static inline void dma_ack_eof() { GDMA.channel[GDMA_CH].in.int_clr.in_suc_eof = 1; }
static inline volatile DESC_T* dma_eof_desc() { return (volatile DESC_T*)GDMA.channel[GDMA_CH].in.suc_eof_des_addr; }

static inline uint32_t pack_word_lsn_first(const uint8_t *b) {
  uint32_t w = 0;
  for (int i = 0; i < BYTES_PER_WORD; ++i) {
    uint8_t nib = b[i] & 0x0F;
    w |= ((uint32_t)nib) << (4*i);
  }
  return w;
}

void lcam_print_status() {
  uint8_t d =
    ((uint8_t)digitalRead(PIN_CAM_D3) << 3) |
    ((uint8_t)digitalRead(PIN_CAM_D2) << 2) |
    ((uint8_t)digitalRead(PIN_CAM_D1) << 1) |
    ((uint8_t)digitalRead(PIN_CAM_D0) << 0);
  Serial.printf("CLK:%d VSYNC:%d D[3:0]=0x%X  edge=%u  words=%lu drops=%lu\n",
    digitalRead(PIN_CAM_PCLK), digitalRead(PIN_CAM_VSYNC), d, s_clk_inv,
    (unsigned long)words_seen, (unsigned long)rb_drops);
  Serial.printf("GDMA: IN_ST=0x%08X INLINK=0x%08X EOF_DES=0x%08X\n",
    (unsigned)GDMA.channel[GDMA_CH].in.int_st.val,
    (unsigned)GDMA.channel[GDMA_CH].in.link.addr,
    (unsigned)GDMA.channel[GDMA_CH].in.suc_eof_des_addr);
}

// ---------- Bring-up ----------
static esp_err_t setup_lcd_cam_once() {
  if (!s_buf) {
    s_buf = (uint8_t*)heap_caps_aligned_alloc(16, DESC_COUNT * CHUNK_BYTES,
             MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!s_buf) { Serial.println("DMA buffer alloc failed"); return ESP_ERR_NO_MEM; }
    memset(s_buf, 0xBB, DESC_COUNT * CHUNK_BYTES);
  }
  if (!s_desc) {
    s_desc = (DESC_T*)heap_caps_aligned_alloc(16, DESC_COUNT * sizeof(DESC_T),
              MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!s_desc) { Serial.println("Desc alloc failed"); return ESP_ERR_NO_MEM; }
    memset(s_desc, 0, DESC_COUNT * sizeof(DESC_T));
  }

  periph_module_enable(PERIPH_LCD_CAM_MODULE);
  periph_module_reset(PERIPH_LCD_CAM_MODULE);
  periph_module_enable(PERIPH_GDMA_MODULE);
  periph_module_reset(PERIPH_GDMA_MODULE);

  LCD_CAM.cam_ctrl1.cam_reset       = 1;
  LCD_CAM.cam_ctrl1.cam_afifo_reset = 1;
  delayMicroseconds(3);
  LCD_CAM.cam_ctrl1.cam_reset       = 0;
  LCD_CAM.cam_ctrl1.cam_afifo_reset = 0;

  LCD_CAM.cam_ctrl.cam_clk_sel        = 2;    // 160MHz/4 = 40MHz base
  LCD_CAM.cam_ctrl.cam_clkm_div_a     = 0;
  LCD_CAM.cam_ctrl.cam_clkm_div_b     = 0;
  LCD_CAM.cam_ctrl.cam_clkm_div_num   = 4;
  LCD_CAM.cam_ctrl.cam_update         = 1;

  gpio_set_direction((gpio_num_t)PIN_CAM_PCLK,  GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_VSYNC, GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D0,    GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D1,    GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D2,    GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D3,    GPIO_MODE_INPUT);

  route_in(PIN_CAM_PCLK,  CAM_PCLK_IDX,   false);
  route_in(PIN_CAM_VSYNC, CAM_V_SYNC_IDX, false);
  route_in(PIN_CAM_D0,    CAM_DATA_IN0_IDX, false);
  route_in(PIN_CAM_D1,    CAM_DATA_IN1_IDX, false);
  route_in(PIN_CAM_D2,    CAM_DATA_IN2_IDX, false);
  route_in(PIN_CAM_D3,    CAM_DATA_IN3_IDX, false);

#ifdef CAM_H_ENABLE_IDX
  route_const_one_to(CAM_H_ENABLE_IDX);
#endif

  GDMA.channel[GDMA_CH].in.conf0.in_rst           = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_rst           = 0;
  GDMA.channel[GDMA_CH].in.conf0.indscr_burst_en  = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_data_burst_en = 1;

  for (int i = 0; i < DESC_COUNT; ++i) {
    uint8_t* buf = s_buf + i * CHUNK_BYTES;
    DESC_T*  nxt = (i+1 < DESC_COUNT) ? &s_desc[i+1] : nullptr;  // stop on EOF; we'll re-arm
    desc_prep(&s_desc[i], buf, CHUNK_BYTES, nxt);
  }

  GDMA.channel[GDMA_CH].in.peri_sel.sel = 5;      // 5 = LCD_CAM

  GDMA.channel[GDMA_CH].in.int_ena.val = 0;
  GDMA.channel[GDMA_CH].in.int_ena.in_suc_eof = 1;
  GDMA.channel[GDMA_CH].in.link.addr  = (uint32_t)&s_desc[0];
  GDMA.channel[GDMA_CH].in.link.start = 1;

  // VSYNC -> EOF; STOP on EOF (we will immediately restart)
  LCD_CAM.cam_ctrl.cam_vs_eof_en         = 1;
  LCD_CAM.cam_ctrl.cam_stop_en           = 1;
  LCD_CAM.cam_ctrl.cam_vsync_filter_thres= 0;
  LCD_CAM.cam_ctrl.cam_byte_order        = 0;
  LCD_CAM.cam_ctrl.cam_bit_order         = 0;
  LCD_CAM.cam_ctrl.cam_update            = 1;

  LCD_CAM.cam_ctrl1.cam_2byte_en          = 0;                     // 8-bit
  LCD_CAM.cam_ctrl1.cam_rec_data_bytelen  = CHUNK_BYTES - 1;       // allow >10; EOF is VSYNC
  LCD_CAM.cam_ctrl1.cam_clk_inv           = s_clk_inv;
  LCD_CAM.cam_ctrl1.cam_de_inv            = 0;
  LCD_CAM.cam_ctrl1.cam_vh_de_mode_en     = 0;
  LCD_CAM.cam_ctrl.cam_update             = 1;

  Serial.println("LCD_CAM + GDMA configured (VSYNC->EOF, stop-on-EOF, immediate re-arm)");
  return ESP_OK;
}

static void lcdcam_restart_after_eof() {
  for (int i = 0; i < DESC_COUNT; ++i) desc_rearm(&s_desc[i]);
  GDMA.channel[GDMA_CH].in.link.start = 1;
  LCD_CAM.cam_ctrl1.cam_start = 1;
}

// ---------- EOF processing (producer) ----------
static inline void on_eof_process() {
  volatile DESC_T* vd = dma_eof_desc();
  DESC_T* d = (DESC_T*)vd;
  uint8_t* base = desc_get_buf(d);

  uint32_t got = desc_len(d);
  if (got == 0 || got > (uint32_t)CHUNK_BYTES) got = CHUNK_BYTES;

  // Correct: extras (VSYNC+stopper) are at the *front* of the chunk.
  // Skip exactly (got - BYTES_PER_WORD) leading bytes, then pack 8.
  uint32_t extras = (got > (uint32_t)BYTES_PER_WORD) ? (got - (uint32_t)BYTES_PER_WORD) : 0;
  const uint8_t* p = base + extras;

  // Safety clamp in case something unexpected happens
  if (extras + BYTES_PER_WORD > got) p = base;

  const uint32_t w = pack_word_lsn_first(p);

  // Push to SPSC ring (drop if full)
  rb_push(w);

  dma_ack_eof();
  lcdcam_restart_after_eof();
}

// ---------- Poller task ----------
#define POLL_SPIN_LOOPS  4096

static void poller_task(void*){
  for(;;){
    bool any = false;
    for (int i = 0; i < POLL_SPIN_LOOPS; ++i) {
      if (dma_eof()) {
        on_eof_process();
        any = true;
        i = -1; // keep spinning while busy
      }
    }
    if (!any) vTaskDelay(1);
  }
}

// ---------- Consumer task ----------
static void packet_task(void*){
  uint32_t local_count = 0;
  for(;;){
    uint32_t w;
    if (rb_pop(&w)) {
      local_count++;
      words_seen++;
      if ((s_log_level > 0) && (local_count % word_print_every) == 0) {
        Serial.printf("word[%lu]=0x%08X\n", (unsigned long)words_seen, w);
      }
      // TODO: your real processing here
    } else {
      vTaskDelay(1);
    }
  }
}

esp_err_t lcam_init_tasks() {
  if (s_poller_task != nullptr || s_consumer_task != nullptr) {
    Serial.println("Tasks already running");
    return ESP_ERR_INVALID_STATE;
  }

  // Start consumer first (prio > poller)
  BaseType_t ret1 = xTaskCreatePinnedToCore(packet_task, "lcam_packet", 4096, nullptr, tskIDLE_PRIORITY + 2, &s_consumer_task, 1);
  // Start poller (prio lower)
  BaseType_t ret2 = xTaskCreatePinnedToCore(poller_task, "lcam_poll", 2048, nullptr, tskIDLE_PRIORITY + 1, &s_poller_task, 1);

  if (ret1 != pdPASS || ret2 != pdPASS) {
    Serial.println("Failed to create tasks");
    lcam_cleanup_tasks();
    return ESP_FAIL;
  }
  
  Serial.println("LCD_CAM tasks started (poller + consumer)");
  return ESP_OK;
}

void lcam_cleanup_tasks() {
  if (s_poller_task != nullptr) {
    vTaskDelete(s_poller_task);
    s_poller_task = nullptr;
  }
  if (s_consumer_task != nullptr) {
    vTaskDelete(s_consumer_task);
    s_consumer_task = nullptr;
  }
}

void lcam_start() {
    Serial.println("Init + start LCD_CAM (VSYNC EOF, ISR-woken if available)...");

    if (setup_lcd_cam_once() != ESP_OK) { Serial.println("Setup failed"); return; }

    rb_reset(); words_seen = 0;
    GDMA.channel[GDMA_CH].in.int_clr.val = 0xFFFFFFFF;
    GDMA.channel[GDMA_CH].in.link.addr   = (uint32_t)&s_desc[0];
    GDMA.channel[GDMA_CH].in.link.start  = 1;
    LCD_CAM.cam_ctrl1.cam_clk_inv        = s_clk_inv;
    LCD_CAM.cam_ctrl.cam_update          = 1;
    LCD_CAM.cam_ctrl1.cam_start          = 1;

    // Start tasks
    if (lcam_init_tasks() != ESP_OK) {
        Serial.println("Failed to start tasks");
        return;
    }

    // Optional smoke wait (no polling; ISR/re-arm will run if data arrives)
    uint32_t start_ms = millis(), w0 = words_seen;
    if (SMOKE_MS) {
      uint32_t until = start_ms + SMOKE_MS;
      while ((int32_t)(millis() - until) < 0) {
        delay(1);  // yield cooperatively
      }
    }
    uint32_t gotw = words_seen - w0;
    Serial.printf("VSYNC-EOF test: %lu words in %ums\n", (unsigned long)gotw, (unsigned)SMOKE_MS);
    if (!gotw && SMOKE_MS) {
      Serial.println("No words. Check PCLK burst (10 clocks/packet) and VSYNC on nibble 9.");
    }
}

void lcam_stop() {
  // Stop tasks first
  lcam_cleanup_tasks();
  
  // Stop the LCD_CAM and GDMA
  LCD_CAM.cam_ctrl1.cam_start = 0;
  GDMA.channel[GDMA_CH].in.link.stop = 1;
  Serial.println("Stopped.");
}

void lcam_log_every_n_words(uint32_t n) {
  // Log every N words processed
  if (n < 1) n = 1;
  word_print_every = n;
  Serial.printf("word_print_every=%lu\n", (unsigned long)word_print_every);
}

void lcam_set_logging(uint8_t n) {
  // Set logging level
  s_log_level = n;  
}