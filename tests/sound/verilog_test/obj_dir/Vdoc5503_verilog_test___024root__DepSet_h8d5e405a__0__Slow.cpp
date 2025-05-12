// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vdoc5503_verilog_test.h for the primary calling header

#include "Vdoc5503_verilog_test__pch.h"
#include "Vdoc5503_verilog_test___024root.h"

VL_ATTR_COLD void Vdoc5503_verilog_test___024root___eval_static__TOP(Vdoc5503_verilog_test___024root* vlSelf);
VL_ATTR_COLD void Vdoc5503_verilog_test___024root____Vm_traceActivitySetAll(Vdoc5503_verilog_test___024root* vlSelf);

VL_ATTR_COLD void Vdoc5503_verilog_test___024root___eval_static(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_static\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vdoc5503_verilog_test___024root___eval_static__TOP(vlSelf);
    Vdoc5503_verilog_test___024root____Vm_traceActivitySetAll(vlSelf);
}

VL_ATTR_COLD void Vdoc5503_verilog_test___024root___eval_static__TOP(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_static__TOP\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__halt_zero_r = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0U;
}

VL_ATTR_COLD void Vdoc5503_verilog_test___024root___eval_final(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_final\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_verilog_test___024root___dump_triggers__stl(Vdoc5503_verilog_test___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vdoc5503_verilog_test___024root___eval_phase__stl(Vdoc5503_verilog_test___024root* vlSelf);

VL_ATTR_COLD void Vdoc5503_verilog_test___024root___eval_settle(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_settle\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ __VstlIterCount;
    CData/*0:0*/ __VstlContinue;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    __VstlContinue = 1U;
    while (__VstlContinue) {
        if (VL_UNLIKELY(((0x64U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vdoc5503_verilog_test___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("../../../hdl/support/srff.v", 1, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vdoc5503_verilog_test___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelfRef.__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_verilog_test___024root___dump_triggers__stl(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___dump_triggers__stl\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VstlTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vdoc5503_verilog_test___024root___stl_sequent__TOP__0(Vdoc5503_verilog_test___024root* vlSelf);

VL_ATTR_COLD void Vdoc5503_verilog_test___024root___eval_stl(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_stl\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vdoc5503_verilog_test___024root___stl_sequent__TOP__0(vlSelf);
        Vdoc5503_verilog_test___024root____Vm_traceActivitySetAll(vlSelf);
    }
}

VL_ATTR_COLD void Vdoc5503_verilog_test___024root___stl_sequent__TOP__0(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___stl_sequent__TOP__0\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.doc5503_verilog_test__DOT__clk_en = (0U 
                                                   == 
                                                   (7U 
                                                    & vlSelfRef.doc5503_verilog_test__DOT__cycle_count));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_control_r));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_mode_w 
        = (3U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_control_r) 
                 >> 1U));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_ca_w 
        = (0xfU & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_control_r) 
                   >> 4U));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_shift_w 
        = (0x1fU & (((IData)(9U) + (7U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_rts_r))) 
                    - (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_rts_r) 
                             >> 3U))));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_wts_w 
        = (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_rts_r) 
                 >> 3U));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_res_w 
        = (7U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_rts_r));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__partner_mode_w 
        = (3U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__partner_control_r) 
                 >> 1U));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_control_r));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__prev_halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__prev_control_r));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_dout_w 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_acc_r];
    vlSelfRef.srff__DOT__edge_r_w = ((~ (IData)(vlSelfRef.srff__DOT__prev_r_r)) 
                                     & (IData)(vlSelfRef.r));
    vlSelfRef.srff__DOT__edge_s_w = ((~ (IData)(vlSelfRef.srff__DOT__prev_s_r)) 
                                     & (IData)(vlSelfRef.s));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[0U] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [0U];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[1U] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [1U];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[2U] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [2U];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[3U] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [3U];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[4U] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [4U];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[5U] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [5U];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[6U] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [6U];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[7U] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [7U];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[8U] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [8U];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[9U] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [9U];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[0xaU] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [0xaU];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[0xbU] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [0xbU];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[0xcU] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [0xcU];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[0xdU] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [0xdU];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[0xeU] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [0xeU];
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_o[0xfU] 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r
        [0xfU];
    vlSelfRef.q = (((~ (IData)(vlSelfRef.srff__DOT__edge_r_w)) 
                    & (IData)(vlSelfRef.srff__DOT__edge_s_w)) 
                   | (IData)(vlSelfRef.srff__DOT__q_r));
}

VL_ATTR_COLD void Vdoc5503_verilog_test___024root___eval_triggers__stl(Vdoc5503_verilog_test___024root* vlSelf);

VL_ATTR_COLD bool Vdoc5503_verilog_test___024root___eval_phase__stl(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_phase__stl\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vdoc5503_verilog_test___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelfRef.__VstlTriggered.any();
    if (__VstlExecute) {
        Vdoc5503_verilog_test___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_verilog_test___024root___dump_triggers__ico(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___dump_triggers__ico\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VicoTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        VL_DBG_MSGF("         'ico' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_verilog_test___024root___dump_triggers__act(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___dump_triggers__act\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VactTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge clk)\n");
    }
    if ((2ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 1 is active: @(posedge doc5503_verilog_test.clk)\n");
    }
    if ((4ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_verilog_test___024root___dump_triggers__nba(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___dump_triggers__nba\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VnbaTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge clk)\n");
    }
    if ((2ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 1 is active: @(posedge doc5503_verilog_test.clk)\n");
    }
    if ((4ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vdoc5503_verilog_test___024root____Vm_traceActivitySetAll(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root____Vm_traceActivitySetAll\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vm_traceActivity[0U] = 1U;
    vlSelfRef.__Vm_traceActivity[1U] = 1U;
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.__Vm_traceActivity[3U] = 1U;
    vlSelfRef.__Vm_traceActivity[4U] = 1U;
}

VL_ATTR_COLD void Vdoc5503_verilog_test___024root___ctor_var_reset(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___ctor_var_reset\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelf->clk = VL_RAND_RESET_I(1);
    vlSelf->s = VL_RAND_RESET_I(1);
    vlSelf->r = VL_RAND_RESET_I(1);
    vlSelf->q = VL_RAND_RESET_I(1);
    vlSelf->srff__DOT__prev_s_r = VL_RAND_RESET_I(1);
    vlSelf->srff__DOT__prev_r_r = VL_RAND_RESET_I(1);
    vlSelf->srff__DOT__q_r = VL_RAND_RESET_I(1);
    vlSelf->srff__DOT__edge_s_w = VL_RAND_RESET_I(1);
    vlSelf->srff__DOT__edge_r_w = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__clk = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__reset_n = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__clk_en = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__cs_n = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__we_n = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__addr = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__data_in = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__data_out = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__wave_address = VL_RAND_RESET_I(16);
    vlSelf->doc5503_verilog_test__DOT__wave_rd = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__wave_data_ready = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__wave_data = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__cycle_count = VL_RAND_RESET_I(32);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__channel_o[__Vi0] = VL_RAND_RESET_I(16);
    }
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__host_addr_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__host_data_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__host_we_n_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__prev_cs_n_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__prev_we_n_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__host_access_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__osc_int_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__osc_en_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r = VL_RAND_RESET_I(5);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r = VL_RAND_RESET_I(5);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r = VL_RAND_RESET_I(5);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r = VL_RAND_RESET_I(5);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r = VL_RAND_RESET_I(5);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r = VL_RAND_RESET_I(5);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r = VL_RAND_RESET_I(5);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_osc_acc_r = VL_RAND_RESET_I(5);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_fl_we_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_fh_we_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_wtp_we_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_rts_we_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_acc_we_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_fl_din_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_fh_din_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_vol_din_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_wds_din_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_wtp_din_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_control_din_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_rts_din_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_acc_din_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__target_acc_dout_w = VL_RAND_RESET_I(24);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_fl_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_fh_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_vol_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_wds_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_wtp_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_control_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_rts_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__partner_control_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__next_control_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__prev_control_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_osc_r = VL_RAND_RESET_I(5);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_wts_w = VL_RAND_RESET_I(3);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_res_w = VL_RAND_RESET_I(3);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_shift_w = VL_RAND_RESET_I(5);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__halt_w = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_mode_w = VL_RAND_RESET_I(2);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_ca_w = VL_RAND_RESET_I(4);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__partner_mode_w = VL_RAND_RESET_I(2);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__next_halt_w = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__prev_halt_w = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_acc_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_wave_addr_r = VL_RAND_RESET_I(16);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__curr_output_r = VL_RAND_RESET_I(16);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__loaded_wds_r = VL_RAND_RESET_I(8);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__channel_r[__Vi0] = VL_RAND_RESET_I(16);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__next_channel_r[__Vi0] = VL_RAND_RESET_I(24);
    }
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__channel_sum_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__mono_mix_r = VL_RAND_RESET_I(16);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__left_mix_r = VL_RAND_RESET_I(16);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__right_mix_r = VL_RAND_RESET_I(16);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r = VL_RAND_RESET_I(6);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__cycle_state_r = VL_RAND_RESET_I(2);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__clk_count_r = VL_RAND_RESET_I(3);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__osc_state_r = VL_RAND_RESET_I(4);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__halt_zero_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r[__Vi0] = VL_RAND_RESET_I(24);
    }
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_RAND_RESET_I(1);
    vlSelf->__Vtrigprevexpr___TOP__doc5503_verilog_test__DOT__clk__0 = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 5; ++__Vi0) {
        vlSelf->__Vm_traceActivity[__Vi0] = 0;
    }
}
