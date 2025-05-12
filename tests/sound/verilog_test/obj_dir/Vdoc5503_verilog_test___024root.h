// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vdoc5503_verilog_test.h for the primary calling header

#ifndef VERILATED_VDOC5503_VERILOG_TEST___024ROOT_H_
#define VERILATED_VDOC5503_VERILOG_TEST___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vdoc5503_verilog_test__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vdoc5503_verilog_test___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        VL_IN8(clk,0,0);
        CData/*0:0*/ doc5503_verilog_test__DOT__clk;
        VL_IN8(s,0,0);
        VL_IN8(r,0,0);
        VL_OUT8(q,0,0);
        CData/*0:0*/ srff__DOT__prev_s_r;
        CData/*0:0*/ srff__DOT__prev_r_r;
        CData/*0:0*/ srff__DOT__q_r;
        CData/*0:0*/ srff__DOT__edge_s_w;
        CData/*0:0*/ srff__DOT__edge_r_w;
        CData/*0:0*/ doc5503_verilog_test__DOT__reset_n;
        CData/*0:0*/ doc5503_verilog_test__DOT__clk_en;
        CData/*0:0*/ doc5503_verilog_test__DOT__cs_n;
        CData/*0:0*/ doc5503_verilog_test__DOT__we_n;
        CData/*7:0*/ doc5503_verilog_test__DOT__addr;
        CData/*7:0*/ doc5503_verilog_test__DOT__data_in;
        CData/*7:0*/ doc5503_verilog_test__DOT__data_out;
        CData/*0:0*/ doc5503_verilog_test__DOT__wave_rd;
        CData/*0:0*/ doc5503_verilog_test__DOT__wave_data_ready;
        CData/*7:0*/ doc5503_verilog_test__DOT__wave_data;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__host_addr_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__host_data_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__host_we_n_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__prev_cs_n_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__prev_we_n_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__host_access_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__osc_int_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__osc_en_r;
        CData/*4:0*/ doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r;
        CData/*4:0*/ doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r;
        CData/*4:0*/ doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r;
        CData/*4:0*/ doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r;
        CData/*4:0*/ doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r;
        CData/*4:0*/ doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r;
        CData/*4:0*/ doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r;
        CData/*4:0*/ doc5503_verilog_test__DOT__dut__DOT__target_osc_acc_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__target_fl_we_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__target_fh_we_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__target_wtp_we_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__target_control_we_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__target_rts_we_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__target_acc_we_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_fl_din_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_fh_din_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_vol_din_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_wds_din_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_wtp_din_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_control_din_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_rts_din_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_fl_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_fh_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_vol_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_wds_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_wtp_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_control_r;
    };
    struct {
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_rts_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__partner_control_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__next_control_r;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__prev_control_r;
        CData/*4:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
        CData/*2:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_wts_w;
        CData/*2:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_res_w;
        CData/*4:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_shift_w;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__halt_w;
        CData/*1:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_mode_w;
        CData/*3:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_ca_w;
        CData/*1:0*/ doc5503_verilog_test__DOT__dut__DOT__partner_mode_w;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__next_halt_w;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__prev_halt_w;
        CData/*7:0*/ doc5503_verilog_test__DOT__dut__DOT__loaded_wds_r;
        CData/*5:0*/ doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r;
        CData/*1:0*/ doc5503_verilog_test__DOT__dut__DOT__cycle_state_r;
        CData/*2:0*/ doc5503_verilog_test__DOT__dut__DOT__clk_count_r;
        CData/*3:0*/ doc5503_verilog_test__DOT__dut__DOT__osc_state_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__halt_zero_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r;
        CData/*0:0*/ doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VicoFirstIteration;
        CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__doc5503_verilog_test__DOT__clk__0;
        CData/*0:0*/ __VactContinue;
        SData/*15:0*/ doc5503_verilog_test__DOT__wave_address;
        SData/*15:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_wave_addr_r;
        SData/*15:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_output_r;
        SData/*15:0*/ doc5503_verilog_test__DOT__dut__DOT__mono_mix_r;
        SData/*15:0*/ doc5503_verilog_test__DOT__dut__DOT__left_mix_r;
        SData/*15:0*/ doc5503_verilog_test__DOT__dut__DOT__right_mix_r;
        IData/*31:0*/ doc5503_verilog_test__DOT__cycle_count;
        IData/*23:0*/ doc5503_verilog_test__DOT__dut__DOT__target_acc_din_r;
        IData/*23:0*/ doc5503_verilog_test__DOT__dut__DOT__target_acc_dout_w;
        IData/*23:0*/ doc5503_verilog_test__DOT__dut__DOT__curr_acc_r;
        IData/*23:0*/ doc5503_verilog_test__DOT__dut__DOT__channel_sum_r;
        IData/*23:0*/ doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r;
        IData/*23:0*/ doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r;
        IData/*23:0*/ doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r;
        IData/*31:0*/ __VactIterCount;
        VlUnpacked<SData/*15:0*/, 16> doc5503_verilog_test__DOT__dut__DOT__channel_o;
        VlUnpacked<SData/*15:0*/, 16> doc5503_verilog_test__DOT__dut__DOT__channel_r;
        VlUnpacked<IData/*23:0*/, 16> doc5503_verilog_test__DOT__dut__DOT__next_channel_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r;
        VlUnpacked<CData/*7:0*/, 32> doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r;
        VlUnpacked<IData/*23:0*/, 32> doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r;
        VlUnpacked<CData/*0:0*/, 5> __Vm_traceActivity;
    };
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h49264d22__0;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<3> __VactTriggered;
    VlTriggerVec<3> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vdoc5503_verilog_test__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vdoc5503_verilog_test___024root(Vdoc5503_verilog_test__Syms* symsp, const char* v__name);
    ~Vdoc5503_verilog_test___024root();
    VL_UNCOPYABLE(Vdoc5503_verilog_test___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
