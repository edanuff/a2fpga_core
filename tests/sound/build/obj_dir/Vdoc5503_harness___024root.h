// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vdoc5503_harness.h for the primary calling header

#ifndef VERILATED_VDOC5503_HARNESS___024ROOT_H_
#define VERILATED_VDOC5503_HARNESS___024ROOT_H_  // guard

#include "verilated.h"


class Vdoc5503_harness__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vdoc5503_harness___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        VL_IN8(clk,0,0);
        VL_IN8(clk_i,0,0);
        VL_IN8(s,0,0);
        VL_IN8(r,0,0);
        VL_OUT8(q,0,0);
        VL_IN8(reset_n_i,0,0);
        VL_IN8(clk_en_i,0,0);
        VL_OUT8(irq_n_o,0,0);
        VL_IN8(cs_n_i,0,0);
        VL_IN8(we_n_i,0,0);
        VL_IN8(addr_i,7,0);
        VL_IN8(data_i,7,0);
        VL_OUT8(data_o,7,0);
        VL_OUT8(wave_rd_o,0,0);
        VL_IN8(wave_data_ready_i,0,0);
        VL_IN8(wave_data_i,7,0);
        VL_OUT8(dbg_cycle_o,4,0);
        VL_OUT8(dbg_osc_state_o,2,0);
        VL_OUT8(dbg_vol_o,7,0);
        VL_OUT8(dbg_wds_o,7,0);
        VL_OUT8(dbg_E_o,0,0);
        VL_OUT8(dbg_clk_count_o,2,0);
        VL_OUT8(ready_o,0,0);
        CData/*0:0*/ srff__DOT__prev_s_r;
        CData/*0:0*/ srff__DOT__prev_r_r;
        CData/*0:0*/ srff__DOT__q_r;
        CData/*0:0*/ srff__DOT__edge_s_w;
        CData/*0:0*/ srff__DOT__edge_r_w;
        CData/*4:0*/ doc5503_harness__DOT__dbg_cycle;
        CData/*4:0*/ doc5503_harness__DOT__dbg_cycle_max;
        CData/*7:0*/ doc5503_harness__DOT__dbg_osc_en;
        CData/*2:0*/ doc5503_harness__DOT__dbg_osc_state;
        CData/*0:0*/ doc5503_harness__DOT__dbg_ready;
        CData/*7:0*/ doc5503_harness__DOT__dbg_wds;
        CData/*7:0*/ doc5503_harness__DOT__dbg_vol;
        CData/*7:0*/ doc5503_harness__DOT__dbg_control;
        CData/*0:0*/ doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_fl_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_fh_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_rts_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__partner_control_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__next_control_r;
        CData/*5:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r;
        CData/*2:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_wts_w;
        CData/*4:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_shift_w;
        CData/*0:0*/ doc5503_harness__DOT__doc5503_inst__DOT__halt_w;
        CData/*1:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_mode_w;
        CData/*3:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_ca_w;
        CData/*0:0*/ doc5503_harness__DOT__doc5503_inst__DOT__next_halt_w;
        CData/*4:0*/ doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r;
        CData/*5:0*/ doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r;
        CData/*4:0*/ doc5503_harness__DOT__doc5503_inst__DOT__phase_timer_w;
        CData/*1:0*/ doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r;
        CData/*2:0*/ doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r;
        CData/*0:0*/ doc5503_harness__DOT__doc5503_inst__DOT__E;
        CData/*3:0*/ doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r;
        CData/*0:0*/ doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r;
    };
    struct {
        CData/*0:0*/ doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__overflow;
        CData/*0:0*/ doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__zero_byte_w;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__ptr_hi_mask_w;
        CData/*7:0*/ doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__data_w;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VicoFirstIteration;
        CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__clk_i__0;
        CData/*0:0*/ __VactContinue;
        VL_OUT16(wave_address_o,15,0);
        VL_OUT16(mono_mix_o,15,0);
        VL_OUT16(left_mix_o,15,0);
        VL_OUT16(right_mix_o,15,0);
        VL_OUT16(dbg_output_o,15,0);
        SData/*15:0*/ doc5503_harness__DOT__dbg_output;
        SData/*15:0*/ doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o;
        SData/*15:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r;
        SData/*15:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r;
        SData/*15:0*/ doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r;
        SData/*15:0*/ doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r;
        SData/*15:0*/ doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r;
        SData/*15:0*/ doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__ptr_w;
        SData/*15:0*/ doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__addr_w;
        SData/*8:0*/ doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__vol_s;
        SData/*15:0*/ doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__output_w;
        IData/*23:0*/ doc5503_harness__DOT__dbg_acc;
        IData/*23:0*/ doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r;
        IData/*23:0*/ doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r;
        IData/*23:0*/ doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r;
        IData/*23:0*/ doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r;
        IData/*23:0*/ doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r;
        IData/*31:0*/ doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__high_bit_w;
        IData/*31:0*/ __VactIterCount;
        VlUnpacked<SData/*15:0*/, 16> doc5503_harness__DOT__doc5503_inst__DOT__channel_o;
        VlUnpacked<CData/*7:0*/, 32> doc5503_harness__DOT__doc5503_inst__DOT__fl_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_harness__DOT__doc5503_inst__DOT__fh_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_harness__DOT__doc5503_inst__DOT__vol_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_harness__DOT__doc5503_inst__DOT__wds_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_harness__DOT__doc5503_inst__DOT__wtp_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_harness__DOT__doc5503_inst__DOT__control_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_harness__DOT__doc5503_inst__DOT__rts_r;
        VlUnpacked<IData/*23:0*/, 32> doc5503_harness__DOT__doc5503_inst__DOT__acc_r;
        VlUnpacked<SData/*15:0*/, 16> doc5503_harness__DOT__doc5503_inst__DOT__channel_r;
        VlUnpacked<IData/*23:0*/, 16> doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r;
        VlUnpacked<CData/*0:0*/, 3> __Vm_traceActivity;
    };
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<2> __VactTriggered;
    VlTriggerVec<2> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vdoc5503_harness__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vdoc5503_harness___024root(Vdoc5503_harness__Syms* symsp, const char* v__name);
    ~Vdoc5503_harness___024root();
    VL_UNCOPYABLE(Vdoc5503_harness___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
