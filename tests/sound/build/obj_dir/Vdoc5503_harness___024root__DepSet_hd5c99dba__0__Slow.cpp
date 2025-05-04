// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vdoc5503_harness.h for the primary calling header

#include "Vdoc5503_harness__pch.h"
#include "Vdoc5503_harness___024root.h"

VL_ATTR_COLD void Vdoc5503_harness___024root___eval_static(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_static\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vdoc5503_harness___024root___eval_initial(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_initial\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__clk_i__0 = vlSelfRef.clk_i;
}

VL_ATTR_COLD void Vdoc5503_harness___024root___eval_final(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_final\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_harness___024root___dump_triggers__stl(Vdoc5503_harness___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vdoc5503_harness___024root___eval_phase__stl(Vdoc5503_harness___024root* vlSelf);

VL_ATTR_COLD void Vdoc5503_harness___024root___eval_settle(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_settle\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vdoc5503_harness___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("../../hdl/sound/../../hdl/support/srff.v", 1, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vdoc5503_harness___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelfRef.__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_harness___024root___dump_triggers__stl(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___dump_triggers__stl\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

VL_ATTR_COLD void Vdoc5503_harness___024root___stl_sequent__TOP__0(Vdoc5503_harness___024root* vlSelf);
VL_ATTR_COLD void Vdoc5503_harness___024root____Vm_traceActivitySetAll(Vdoc5503_harness___024root* vlSelf);

VL_ATTR_COLD void Vdoc5503_harness___024root___eval_stl(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_stl\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vdoc5503_harness___024root___stl_sequent__TOP__0(vlSelf);
        Vdoc5503_harness___024root____Vm_traceActivitySetAll(vlSelf);
    }
}

VL_ATTR_COLD void Vdoc5503_harness___024root___stl_sequent__TOP__0(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___stl_sequent__TOP__0\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.wave_address_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o;
    vlSelfRef.wave_rd_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o;
    vlSelfRef.dbg_clk_count_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r;
    vlSelfRef.mono_mix_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r;
    vlSelfRef.left_mix_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r;
    vlSelfRef.right_mix_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r;
    vlSelfRef.dbg_cycle_o = vlSelfRef.doc5503_harness__DOT__dbg_cycle;
    vlSelfRef.dbg_osc_state_o = vlSelfRef.doc5503_harness__DOT__dbg_osc_state;
    vlSelfRef.dbg_vol_o = vlSelfRef.doc5503_harness__DOT__dbg_vol;
    vlSelfRef.dbg_wds_o = vlSelfRef.doc5503_harness__DOT__dbg_wds;
    vlSelfRef.dbg_output_o = vlSelfRef.doc5503_harness__DOT__dbg_output;
    vlSelfRef.ready_o = (0U != (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_mode_w 
        = (3U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r) 
                 >> 1U));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_ca_w 
        = (0xfU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r) 
                   >> 4U));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_shift_w 
        = (0x1fU & (((IData)(9U) + (7U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_rts_r))) 
                    - (7U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_rts_r) 
                             >> 3U))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wts_w 
        = (7U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_rts_r) 
                 >> 3U));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E 
        = (1U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r) 
                 >> 2U));
    vlSelfRef.dbg_E_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E;
    vlSelfRef.srff__DOT__edge_r_w = ((~ (IData)(vlSelfRef.srff__DOT__prev_r_r)) 
                                     & (IData)(vlSelfRef.r));
    vlSelfRef.srff__DOT__edge_s_w = ((~ (IData)(vlSelfRef.srff__DOT__prev_s_r)) 
                                     & (IData)(vlSelfRef.s));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[2U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[3U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [3U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[4U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [4U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[5U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [5U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[6U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [6U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[7U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [7U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[8U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [8U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[9U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [9U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xaU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xaU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xbU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xbU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xcU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xcU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xdU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xdU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xeU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xeU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xfU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xfU];
    vlSelfRef.q = (((~ (IData)(vlSelfRef.srff__DOT__edge_r_w)) 
                    & (IData)(vlSelfRef.srff__DOT__edge_s_w)) 
                   | (IData)(vlSelfRef.srff__DOT__q_r));
}

VL_ATTR_COLD void Vdoc5503_harness___024root___eval_triggers__stl(Vdoc5503_harness___024root* vlSelf);

VL_ATTR_COLD bool Vdoc5503_harness___024root___eval_phase__stl(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_phase__stl\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vdoc5503_harness___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelfRef.__VstlTriggered.any();
    if (__VstlExecute) {
        Vdoc5503_harness___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_harness___024root___dump_triggers__ico(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___dump_triggers__ico\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
VL_ATTR_COLD void Vdoc5503_harness___024root___dump_triggers__act(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___dump_triggers__act\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VactTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge clk)\n");
    }
    if ((2ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 1 is active: @(posedge clk_i)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_harness___024root___dump_triggers__nba(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___dump_triggers__nba\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VnbaTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge clk)\n");
    }
    if ((2ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 1 is active: @(posedge clk_i)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vdoc5503_harness___024root____Vm_traceActivitySetAll(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root____Vm_traceActivitySetAll\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vm_traceActivity[0U] = 1U;
    vlSelfRef.__Vm_traceActivity[1U] = 1U;
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
}

VL_ATTR_COLD void Vdoc5503_harness___024root___ctor_var_reset(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___ctor_var_reset\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelf->clk = VL_RAND_RESET_I(1);
    vlSelf->s = VL_RAND_RESET_I(1);
    vlSelf->r = VL_RAND_RESET_I(1);
    vlSelf->q = VL_RAND_RESET_I(1);
    vlSelf->clk_i = VL_RAND_RESET_I(1);
    vlSelf->reset_n_i = VL_RAND_RESET_I(1);
    vlSelf->clk_en_i = VL_RAND_RESET_I(1);
    vlSelf->irq_n_o = VL_RAND_RESET_I(1);
    vlSelf->cs_n_i = VL_RAND_RESET_I(1);
    vlSelf->we_n_i = VL_RAND_RESET_I(1);
    vlSelf->addr_i = VL_RAND_RESET_I(8);
    vlSelf->data_i = VL_RAND_RESET_I(8);
    vlSelf->data_o = VL_RAND_RESET_I(8);
    vlSelf->wave_address_o = VL_RAND_RESET_I(16);
    vlSelf->wave_rd_o = VL_RAND_RESET_I(1);
    vlSelf->wave_data_ready_i = VL_RAND_RESET_I(1);
    vlSelf->wave_data_i = VL_RAND_RESET_I(8);
    vlSelf->mono_mix_o = VL_RAND_RESET_I(16);
    vlSelf->left_mix_o = VL_RAND_RESET_I(16);
    vlSelf->right_mix_o = VL_RAND_RESET_I(16);
    vlSelf->dbg_cycle_o = VL_RAND_RESET_I(5);
    vlSelf->dbg_osc_state_o = VL_RAND_RESET_I(3);
    vlSelf->dbg_vol_o = VL_RAND_RESET_I(8);
    vlSelf->dbg_wds_o = VL_RAND_RESET_I(8);
    vlSelf->dbg_output_o = VL_RAND_RESET_I(16);
    vlSelf->dbg_E_o = VL_RAND_RESET_I(1);
    vlSelf->dbg_clk_count_o = VL_RAND_RESET_I(3);
    vlSelf->ready_o = VL_RAND_RESET_I(1);
    vlSelf->srff__DOT__prev_s_r = VL_RAND_RESET_I(1);
    vlSelf->srff__DOT__prev_r_r = VL_RAND_RESET_I(1);
    vlSelf->srff__DOT__q_r = VL_RAND_RESET_I(1);
    vlSelf->srff__DOT__edge_s_w = VL_RAND_RESET_I(1);
    vlSelf->srff__DOT__edge_r_w = VL_RAND_RESET_I(1);
    vlSelf->doc5503_harness__DOT__dbg_cycle = VL_RAND_RESET_I(5);
    vlSelf->doc5503_harness__DOT__dbg_cycle_max = VL_RAND_RESET_I(5);
    vlSelf->doc5503_harness__DOT__dbg_osc_en = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__dbg_osc_state = VL_RAND_RESET_I(3);
    vlSelf->doc5503_harness__DOT__dbg_ready = VL_RAND_RESET_I(1);
    vlSelf->doc5503_harness__DOT__dbg_output = VL_RAND_RESET_I(16);
    vlSelf->doc5503_harness__DOT__dbg_wds = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__dbg_vol = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__dbg_control = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__dbg_acc = VL_RAND_RESET_I(24);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o = VL_RAND_RESET_I(16);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__channel_o[__Vi0] = VL_RAND_RESET_I(16);
    }
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r = VL_RAND_RESET_I(8);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__fl_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__fh_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__vol_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__wds_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__wtp_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__control_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__rts_r[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__acc_r[__Vi0] = VL_RAND_RESET_I(24);
    }
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_fl_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_fh_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_rts_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__partner_control_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__next_control_r = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r = VL_RAND_RESET_I(6);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_wts_w = VL_RAND_RESET_I(3);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_shift_w = VL_RAND_RESET_I(5);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__halt_w = VL_RAND_RESET_I(1);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_mode_w = VL_RAND_RESET_I(2);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_ca_w = VL_RAND_RESET_I(4);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__next_halt_w = VL_RAND_RESET_I(1);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r = VL_RAND_RESET_I(16);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r = VL_RAND_RESET_I(16);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r = VL_RAND_RESET_I(5);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r = VL_RAND_RESET_I(8);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__channel_r[__Vi0] = VL_RAND_RESET_I(16);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[__Vi0] = VL_RAND_RESET_I(24);
    }
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r = VL_RAND_RESET_I(16);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r = VL_RAND_RESET_I(16);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r = VL_RAND_RESET_I(16);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r = VL_RAND_RESET_I(24);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r = VL_RAND_RESET_I(6);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__phase_timer_w = VL_RAND_RESET_I(5);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r = VL_RAND_RESET_I(2);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r = VL_RAND_RESET_I(3);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__E = VL_RAND_RESET_I(1);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = VL_RAND_RESET_I(4);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r = VL_RAND_RESET_I(1);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__high_bit_w = 0;
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__overflow = VL_RAND_RESET_I(1);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__zero_byte_w = VL_RAND_RESET_I(1);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__ptr_hi_mask_w = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__ptr_w = VL_RAND_RESET_I(16);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__addr_w = VL_RAND_RESET_I(16);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__data_w = VL_RAND_RESET_I(8);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__vol_s = VL_RAND_RESET_I(9);
    vlSelf->doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__output_w = VL_RAND_RESET_I(16);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_RAND_RESET_I(1);
    vlSelf->__Vtrigprevexpr___TOP__clk_i__0 = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->__Vm_traceActivity[__Vi0] = 0;
    }
}
