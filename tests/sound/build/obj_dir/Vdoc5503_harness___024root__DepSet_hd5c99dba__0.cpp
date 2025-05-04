// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vdoc5503_harness.h for the primary calling header

#include "Vdoc5503_harness__pch.h"
#include "Vdoc5503_harness___024root.h"

void Vdoc5503_harness___024root___ico_sequent__TOP__0(Vdoc5503_harness___024root* vlSelf);

void Vdoc5503_harness___024root___eval_ico(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_ico\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vdoc5503_harness___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vdoc5503_harness___024root___ico_sequent__TOP__0(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___ico_sequent__TOP__0\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.srff__DOT__edge_r_w = ((~ (IData)(vlSelfRef.srff__DOT__prev_r_r)) 
                                     & (IData)(vlSelfRef.r));
    vlSelfRef.srff__DOT__edge_s_w = ((~ (IData)(vlSelfRef.srff__DOT__prev_s_r)) 
                                     & (IData)(vlSelfRef.s));
    vlSelfRef.q = (((~ (IData)(vlSelfRef.srff__DOT__edge_r_w)) 
                    & (IData)(vlSelfRef.srff__DOT__edge_s_w)) 
                   | (IData)(vlSelfRef.srff__DOT__q_r));
}

void Vdoc5503_harness___024root___eval_triggers__ico(Vdoc5503_harness___024root* vlSelf);

bool Vdoc5503_harness___024root___eval_phase__ico(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_phase__ico\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vdoc5503_harness___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vdoc5503_harness___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vdoc5503_harness___024root___eval_act(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_act\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vdoc5503_harness___024root___nba_sequent__TOP__0(Vdoc5503_harness___024root* vlSelf);
void Vdoc5503_harness___024root___nba_sequent__TOP__1(Vdoc5503_harness___024root* vlSelf);

void Vdoc5503_harness___024root___eval_nba(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_nba\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((2ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vdoc5503_harness___024root___nba_sequent__TOP__0(vlSelf);
        vlSelfRef.__Vm_traceActivity[1U] = 1U;
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vdoc5503_harness___024root___nba_sequent__TOP__1(vlSelf);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
    }
}

VL_INLINE_OPT void Vdoc5503_harness___024root___nba_sequent__TOP__0(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___nba_sequent__TOP__0\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*7:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r = 0;
    CData/*5:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r = 0;
    CData/*2:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r = 0;
    CData/*5:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r = 0;
    CData/*1:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r = 0;
    CData/*0:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r = 0;
    CData/*7:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r = 0;
    IData/*23:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r = 0;
    IData/*23:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r = 0;
    IData/*23:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r = 0;
    SData/*15:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r = 0;
    SData/*15:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r = 0;
    SData/*15:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r = 0;
    CData/*3:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 0;
    CData/*7:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_fl_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_fl_r = 0;
    CData/*7:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r = 0;
    IData/*23:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r = 0;
    SData/*15:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r = 0;
    SData/*15:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r = 0;
    CData/*7:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_fh_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_fh_r = 0;
    CData/*7:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r = 0;
    CData/*7:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r = 0;
    CData/*7:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r = 0;
    CData/*7:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__partner_control_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__partner_control_r = 0;
    IData/*23:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 0;
    SData/*15:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v0 = 0;
    IData/*23:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v2;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v2 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v2;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v2 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v3;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v3 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v3;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v3 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v4;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v4 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1 = 0;
    IData/*23:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v2;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v2 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v2;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v2 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v1 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v3;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v3 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v5;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v5 = 0;
    // Body
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_fl_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_fl_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_fh_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_fh_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__partner_control_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0 = 0U;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1 = 0U;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v2 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v3 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v1 = 0U;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0U;
    if (vlSelfRef.reset_n_i) {
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r 
            = (0x3fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r)));
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__phase_timer_w 
            = (0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__phase_timer_w)));
        if (vlSelfRef.clk_en_i) {
            __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r)));
            if ((7U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r))) {
                if ((0U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r = 0U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r = 1U;
                } else if ((1U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
                    if (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r) 
                         == (0x1fU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r) 
                                      >> 1U)))) {
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r = 2U;
                    } else {
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r 
                            = (0x3fU & ((IData)(1U) 
                                        + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r)));
                    }
                } else if ((2U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r = 3U;
                } else if ((3U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r = 0U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r = 1U;
                }
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r = 0U;
            }
            if ((3U == (3U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r)))) {
                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__phase_timer_w = 0U;
            }
        }
    } else {
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r = 0U;
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__phase_timer_w = 0U;
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r = 0U;
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r = 0U;
    }
    if (vlSelfRef.reset_n_i) {
        if (VL_UNLIKELY((vlSelfRef.wave_data_ready_i))) {
            VL_WRITEF_NX("DEBUG WAVE_DATA_READY: cycle=%1#, data=0x%02x, last_osc=%2#, addr=%04x\n",0,
                         2,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r,
                         8,(IData)(vlSelfRef.wave_data_i),
                         5,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r,
                         16,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o));
            __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r 
                = vlSelfRef.wave_data_i;
        }
    } else {
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r = 0x80U;
    }
    vlSelfRef.doc5503_harness__DOT__dbg_ready = 1U;
    vlSelfRef.doc5503_harness__DOT__dbg_acc = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r;
    vlSelfRef.doc5503_harness__DOT__dbg_cycle_max = 
        (0x1fU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r) 
                  >> 1U));
    vlSelfRef.doc5503_harness__DOT__dbg_osc_en = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r;
    vlSelfRef.doc5503_harness__DOT__dbg_osc_state = 
        (7U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r));
    vlSelfRef.doc5503_harness__DOT__dbg_output = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r;
    vlSelfRef.doc5503_harness__DOT__dbg_wds = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r;
    vlSelfRef.doc5503_harness__DOT__dbg_vol = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r;
    vlSelfRef.doc5503_harness__DOT__dbg_cycle = (0x1fU 
                                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r));
    vlSelfRef.doc5503_harness__DOT__dbg_control = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r;
    vlSelfRef.dbg_clk_count_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r;
    if (vlSelfRef.reset_n_i) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o = 0U;
        if (vlSelfRef.wave_data_ready_i) {
            __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r = 1U;
        }
        if (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E) 
             & (IData)(vlSelfRef.ready_o))) {
            if ((1U & (~ (IData)(vlSelfRef.cs_n_i)))) {
                if ((0xe0U == (IData)(vlSelfRef.addr_i))) {
                    if (vlSelfRef.we_n_i) {
                        VL_WRITEF_NX("REGISTER READ @ %t: Reading 0x%02x from osc_int_r (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r),
                                     8,vlSelfRef.addr_i,
                                     1,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E));
                        vlSelfRef.data_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r;
                    } else {
                        VL_WRITEF_NX("REGISTER WRITE @ %t: Writing 0x%02x to osc_int_r (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.data_i),
                                     8,vlSelfRef.addr_i,
                                     1,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E));
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r 
                            = vlSelfRef.data_i;
                    }
                } else if ((0xe1U == (IData)(vlSelfRef.addr_i))) {
                    if (vlSelfRef.we_n_i) {
                        VL_WRITEF_NX("REGISTER READ @ %t: Reading 0x%02x from osc_en_r (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r),
                                     8,vlSelfRef.addr_i,
                                     1,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E));
                        vlSelfRef.data_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r;
                    } else {
                        VL_WRITEF_NX("REGISTER WRITE @ %t: Writing 0x%02x to osc_en_r (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.data_i),
                                     8,vlSelfRef.addr_i,
                                     1,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E));
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r 
                            = vlSelfRef.data_i;
                    }
                } else if ((0xdfU >= (IData)(vlSelfRef.addr_i))) {
                    if (vlSelfRef.we_n_i) {
                        if (VL_UNLIKELY(((0U == (7U 
                                                 & ((IData)(vlSelfRef.addr_i) 
                                                    >> 5U)))))) {
                            VL_WRITEF_NX("REGISTER READ @ %t: Reading 0x%02x from fl_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                         64,VL_TIME_UNITED_Q(1),
                                         -12,8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fl_r
                                         [(0x1fU & (IData)(vlSelfRef.addr_i))],
                                         5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                         8,(IData)(vlSelfRef.addr_i),
                                         1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                            vlSelfRef.data_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fl_r
                                [(0x1fU & (IData)(vlSelfRef.addr_i))];
                        } else if (VL_UNLIKELY(((1U 
                                                 == 
                                                 (7U 
                                                  & ((IData)(vlSelfRef.addr_i) 
                                                     >> 5U)))))) {
                            VL_WRITEF_NX("REGISTER READ @ %t: Reading 0x%02x from fh_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                         64,VL_TIME_UNITED_Q(1),
                                         -12,8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fh_r
                                         [(0x1fU & (IData)(vlSelfRef.addr_i))],
                                         5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                         8,(IData)(vlSelfRef.addr_i),
                                         1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                            vlSelfRef.data_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fh_r
                                [(0x1fU & (IData)(vlSelfRef.addr_i))];
                        } else if (VL_UNLIKELY(((2U 
                                                 == 
                                                 (7U 
                                                  & ((IData)(vlSelfRef.addr_i) 
                                                     >> 5U)))))) {
                            VL_WRITEF_NX("REGISTER READ @ %t: Reading 0x%02x from vol_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                         64,VL_TIME_UNITED_Q(1),
                                         -12,8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__vol_r
                                         [(0x1fU & (IData)(vlSelfRef.addr_i))],
                                         5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                         8,(IData)(vlSelfRef.addr_i),
                                         1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                            vlSelfRef.data_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__vol_r
                                [(0x1fU & (IData)(vlSelfRef.addr_i))];
                        } else if (VL_UNLIKELY(((3U 
                                                 == 
                                                 (7U 
                                                  & ((IData)(vlSelfRef.addr_i) 
                                                     >> 5U)))))) {
                            VL_WRITEF_NX("REGISTER READ @ %t: Reading 0x%02x from wds_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                         64,VL_TIME_UNITED_Q(1),
                                         -12,8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wds_r
                                         [(0x1fU & (IData)(vlSelfRef.addr_i))],
                                         5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                         8,(IData)(vlSelfRef.addr_i),
                                         1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                            vlSelfRef.data_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wds_r
                                [(0x1fU & (IData)(vlSelfRef.addr_i))];
                        } else if (VL_UNLIKELY(((4U 
                                                 == 
                                                 (7U 
                                                  & ((IData)(vlSelfRef.addr_i) 
                                                     >> 5U)))))) {
                            VL_WRITEF_NX("REGISTER READ @ %t: Reading 0x%02x from wtp_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                         64,VL_TIME_UNITED_Q(1),
                                         -12,8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wtp_r
                                         [(0x1fU & (IData)(vlSelfRef.addr_i))],
                                         5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                         8,(IData)(vlSelfRef.addr_i),
                                         1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                            vlSelfRef.data_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wtp_r
                                [(0x1fU & (IData)(vlSelfRef.addr_i))];
                        } else if (VL_UNLIKELY(((5U 
                                                 == 
                                                 (7U 
                                                  & ((IData)(vlSelfRef.addr_i) 
                                                     >> 5U)))))) {
                            VL_WRITEF_NX("REGISTER READ @ %t: Reading 0x%02x from control_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                         64,VL_TIME_UNITED_Q(1),
                                         -12,8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r
                                         [(0x1fU & (IData)(vlSelfRef.addr_i))],
                                         5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                         8,(IData)(vlSelfRef.addr_i),
                                         1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                            vlSelfRef.data_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r
                                [(0x1fU & (IData)(vlSelfRef.addr_i))];
                        } else if (VL_UNLIKELY(((6U 
                                                 == 
                                                 (7U 
                                                  & ((IData)(vlSelfRef.addr_i) 
                                                     >> 5U)))))) {
                            VL_WRITEF_NX("REGISTER READ @ %t: Reading 0x%02x from rts_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                         64,VL_TIME_UNITED_Q(1),
                                         -12,8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__rts_r
                                         [(0x1fU & (IData)(vlSelfRef.addr_i))],
                                         5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                         8,(IData)(vlSelfRef.addr_i),
                                         1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                            vlSelfRef.data_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__rts_r
                                [(0x1fU & (IData)(vlSelfRef.addr_i))];
                        }
                    } else if (VL_UNLIKELY(((0U == 
                                             (7U & 
                                              ((IData)(vlSelfRef.addr_i) 
                                               >> 5U)))))) {
                        VL_WRITEF_NX("REGISTER WRITE @ %t: Writing 0x%02x to fl_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.data_i),
                                     5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                     8,(IData)(vlSelfRef.addr_i),
                                     1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0 
                            = vlSelfRef.data_i;
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0 
                            = (0x1fU & (IData)(vlSelfRef.addr_i));
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0 = 1U;
                    } else if (VL_UNLIKELY(((1U == 
                                             (7U & 
                                              ((IData)(vlSelfRef.addr_i) 
                                               >> 5U)))))) {
                        VL_WRITEF_NX("REGISTER WRITE @ %t: Writing 0x%02x to fh_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.data_i),
                                     5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                     8,(IData)(vlSelfRef.addr_i),
                                     1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0 
                            = vlSelfRef.data_i;
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0 
                            = (0x1fU & (IData)(vlSelfRef.addr_i));
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0 = 1U;
                    } else if (VL_UNLIKELY(((2U == 
                                             (7U & 
                                              ((IData)(vlSelfRef.addr_i) 
                                               >> 5U)))))) {
                        VL_WRITEF_NX("REGISTER WRITE @ %t: Writing 0x%02x to vol_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.data_i),
                                     5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                     8,(IData)(vlSelfRef.addr_i),
                                     1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0 
                            = vlSelfRef.data_i;
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0 
                            = (0x1fU & (IData)(vlSelfRef.addr_i));
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0 = 1U;
                    } else if (VL_UNLIKELY(((3U == 
                                             (7U & 
                                              ((IData)(vlSelfRef.addr_i) 
                                               >> 5U)))))) {
                        VL_WRITEF_NX("REGISTER WRITE @ %t: Writing 0x%02x to wds_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.data_i),
                                     5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                     8,(IData)(vlSelfRef.addr_i),
                                     1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0 
                            = vlSelfRef.data_i;
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0 
                            = (0x1fU & (IData)(vlSelfRef.addr_i));
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0 = 1U;
                    } else if (VL_UNLIKELY(((4U == 
                                             (7U & 
                                              ((IData)(vlSelfRef.addr_i) 
                                               >> 5U)))))) {
                        VL_WRITEF_NX("REGISTER WRITE @ %t: Writing 0x%02x to wtp_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.data_i),
                                     5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                     8,(IData)(vlSelfRef.addr_i),
                                     1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0 
                            = vlSelfRef.data_i;
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0 
                            = (0x1fU & (IData)(vlSelfRef.addr_i));
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0 = 1U;
                    } else if (VL_UNLIKELY(((5U == 
                                             (7U & 
                                              ((IData)(vlSelfRef.addr_i) 
                                               >> 5U)))))) {
                        VL_WRITEF_NX("REGISTER WRITE @ %t: Writing 0x%02x to control_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.data_i),
                                     5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                     8,(IData)(vlSelfRef.addr_i),
                                     1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0 
                            = vlSelfRef.data_i;
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0 
                            = (0x1fU & (IData)(vlSelfRef.addr_i));
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0 = 1U;
                    } else if (VL_UNLIKELY(((6U == 
                                             (7U & 
                                              ((IData)(vlSelfRef.addr_i) 
                                               >> 5U)))))) {
                        VL_WRITEF_NX("REGISTER WRITE @ %t: Writing 0x%02x to rts_r[%2#] (addr=0x%02x), E=%1#\n",0,
                                     64,VL_TIME_UNITED_Q(1),
                                     -12,8,(IData)(vlSelfRef.data_i),
                                     5,(0x1fU & (IData)(vlSelfRef.addr_i)),
                                     8,(IData)(vlSelfRef.addr_i),
                                     1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E);
                        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0 
                            = vlSelfRef.data_i;
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0 
                            = (0x1fU & (IData)(vlSelfRef.addr_i));
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0 = 1U;
                    }
                }
            }
        } else if ((2U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
            if (VL_LIKELY(((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r))))) {
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 
                    = (0xfU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 1U;
                if ((0xfU == (0xfU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r)))) {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r = 0U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r = 0U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r = 0U;
                }
            } else {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 
                    = (0xffffU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r
                       [(0xfU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r))]);
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 
                    = (0xfU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 1U;
                VL_WRITEF_NX("DEBUG CHANNEL[%2#]: value=%8d\n",0,
                             4,(0xfU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r)),
                             24,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r
                             [(0xfU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r))]);
                if (VL_UNLIKELY(((0xfU == (0xfU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r)))))) {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r 
                        = (0xffffU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r);
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r 
                        = (0xffffU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r);
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r 
                        = (0xffffU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r);
                    VL_WRITEF_NX("DEBUG FINAL MIX: mono=%8d, left=%8d, right=%8d (mix regs: %6d, %6d, %6d)\n",0,
                                 24,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r,
                                 24,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r,
                                 24,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r,
                                 16,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r),
                                 16,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r,
                                 16,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r));
                }
            }
        } else if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
            if (((((((((0U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r)) 
                       | (1U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) 
                      | (2U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) 
                     | (3U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) 
                    | (4U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) 
                   | (5U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) 
                  | (6U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) 
                 | (7U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r)))) {
                if ((0U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                    if (VL_UNLIKELY((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r))) {
                        VL_WRITEF_NX("DEBUG OSC_IDLE: Added wave data 0x%02x to oscillator %2#\n",0,
                                     8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r,
                                     5,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r));
                        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1 
                            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r;
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1 
                            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r;
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1 = 1U;
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r = 0U;
                    }
                    if ((0U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r))) {
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 1U;
                    }
                    if (VL_UNLIKELY((((0U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r)) 
                                      & (0U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r)))))) {
                        VL_WRITEF_NX("DEBUG REGS: osc_en=%x, control[0]=%x, vol[0]=%x, fl[0]=%x, fh[0]=%x, curr_vol=%x, E=%1#\nDEBUG MIX OUTPUTS: mono=%6d, left=%6d, right=%6d\n",0,
                                     8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r,
                                     8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r
                                     [0U],8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__vol_r
                                     [0U],8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fl_r
                                     [0U],8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fh_r
                                     [0U],8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r,
                                     1,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E),
                                     16,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r,
                                     16,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r),
                                     16,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r);
                    }
                } else if (VL_UNLIKELY(((1U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))))) {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_fl_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fl_r
                        [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))];
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wtp_r
                        [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))];
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_rts_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__rts_r
                        [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))];
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__acc_r
                        [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))];
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r = 0U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r = 0U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 2U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_fh_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fh_r
                        [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))];
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__vol_r
                        [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))];
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wds_r
                        [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))];
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r
                        [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))];
                    VL_WRITEF_NX("DEBUG OSC_START: osc=%2#, freq=%x/%x, vol=%x, wds=%x, control=%x, channel=%2#, halt=%1#\n",0,
                                 6,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r,
                                 8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fh_r
                                 [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))],
                                 8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fl_r
                                 [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))],
                                 8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__vol_r
                                 [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))],
                                 8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wds_r
                                 [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))],
                                 8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r
                                 [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))],
                                 4,(0xfU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r
                                            [(0x1fU 
                                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))] 
                                            >> 4U)),
                                 1,(1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r
                                    [(0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))]));
                } else if ((2U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__partner_control_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r
                        [(0x1fU & (1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r)))];
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 3U;
                } else if ((3U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r
                        [(0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r)))];
                    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__halt_w) {
                        if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_mode_w))) {
                            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v0 
                                = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r));
                            __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v0 = 1U;
                        }
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 9U;
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r = 0x80U;
                    } else {
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r 
                            = (0xffffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r 
                                            + (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_fh_r) 
                                                << 8U) 
                                               | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_fl_r))));
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 4U;
                    }
                } else if ((4U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r;
                    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1 
                        = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r));
                    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1 = 1U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 5U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r 
                        = (0xffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r 
                                      >> (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_shift_w)));
                } else if ((5U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__high_bit_w 
                        = ((7U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wts_w))
                            ? 0xfU : (8U | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wts_w)));
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__overflow 
                        = (1U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r) 
                                 >> (0xfU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__high_bit_w)));
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__zero_byte_w 
                        = (0U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r));
                    if (VL_LIKELY((((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__overflow) 
                                    | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__zero_byte_w))))) {
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v2 
                            = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r));
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v2 = 1U;
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r 
                            = ((1U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_mode_w) 
                                      | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__zero_byte_w)))
                                ? 7U : (((2U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_mode_w)) 
                                         & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r)))
                                         ? 6U : 9U));
                    } else {
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__ptr_hi_mask_w 
                            = (0xffU & ((IData)(0xffU) 
                                        << (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wts_w)));
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__ptr_w 
                            = (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__ptr_hi_mask_w) 
                                & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r)) 
                               << 8U);
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o = 1U;
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r 
                            = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r));
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 9U;
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__addr_w 
                            = ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r) 
                               | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__ptr_w));
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o 
                            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__addr_w;
                        VL_WRITEF_NX("DEBUG WAVE_READ: osc=%2#, addr=0x%04x, wtp=0x%02x, mask=0x%02x, ptr=0x%04x\n",0,
                                     6,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r,
                                     16,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r),
                                     8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r,
                                     8,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__ptr_hi_mask_w),
                                     16,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__unnamedblk2__DOT__ptr_w);
                    }
                } else if ((6U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v3 
                        = (0x1fU & (1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r)));
                    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v3 = 1U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 9U;
                } else {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r = 0x80U;
                    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1 
                        = (1U | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r));
                    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1 
                        = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r));
                    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1 = 1U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r 
                        = ((3U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_mode_w))
                            ? 8U : 9U);
                }
            } else if ((8U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2 
                    = (0xfeU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_r));
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2 
                    = (0x1fU & (1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r)));
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2 = 1U;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v4 
                    = (0x1fU & (1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r)));
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 9U;
            } else if ((9U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                if (VL_LIKELY((((2U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_mode_w)) 
                                & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r))))) {
                    if (((0x1eU > (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r)) 
                         & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_halt_w)))) {
                        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1 
                            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r;
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1 
                            = (0x1fU & ((IData)(1U) 
                                        + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r)));
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1 = 1U;
                    }
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                } else {
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__data_w 
                        = (0x80U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r));
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__vol_s 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r
                        [vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_ca_w];
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 0xaU;
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__output_w 
                        = (0xffffU & VL_MULS_III(16, 
                                                 (0xffffU 
                                                  & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__data_w))), 
                                                 (0xffffU 
                                                  & VL_EXTENDS_II(16,9, (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__vol_s)))));
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__output_w;
                    VL_WRITEF_NX("DEBUG OSC_OUT: osc=%2#, data=0x%02x (signed=%4d), vol=0x%02x, output=%6d\n",0,
                                 6,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r,
                                 8,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r),
                                 8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__data_w,
                                 8,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r),
                                 16,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__output_w);
                }
            } else if (VL_UNLIKELY(((0xaU == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 
                    = (0xffffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r 
                                    + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r)));
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_ca_w;
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 1U;
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
                    = (0xffffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
                                    + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r)));
                if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_ca_w))) {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
                        = (0xffffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
                                        + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r)));
                } else {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
                        = (0xffffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
                                        + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r)));
                }
                VL_WRITEF_NX("DEBUG OSC_MIX: osc=%2#, ca=%2#, data=%x, vol=%x, output=%5#, channel_sum=%8d, mode=%1#, halt=%1#\n",0,
                             6,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r,
                             4,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_ca_w),
                             8,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r,
                             8,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r),
                             16,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r,
                             24,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r,
                             2,(IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_mode_w),
                             1,vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__halt_w);
            }
        } else {
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v1 
                = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v1 = 1U;
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v1 
                = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v2 
                = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v2 
                = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v1 
                = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v1 
                = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v3 
                = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v5 
                = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
        }
    } else {
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r = 0U;
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o = 0U;
    }
    vlSelfRef.dbg_osc_state_o = vlSelfRef.doc5503_harness__DOT__dbg_osc_state;
    vlSelfRef.dbg_output_o = vlSelfRef.doc5503_harness__DOT__dbg_output;
    vlSelfRef.dbg_wds_o = vlSelfRef.doc5503_harness__DOT__dbg_wds;
    vlSelfRef.dbg_vol_o = vlSelfRef.doc5503_harness__DOT__dbg_vol;
    vlSelfRef.dbg_cycle_o = vlSelfRef.doc5503_harness__DOT__dbg_cycle;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__last_wave_data_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__data_pending_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_timer_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_fl_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_fl_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wtp_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_fh_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_fh_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_vol_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__partner_control_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_acc_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_wds_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__channel_sum_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_output_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E 
        = (1U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__clk_count_r) 
                 >> 2U));
    vlSelfRef.dbg_E_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__E;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_osc_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0] = 0U;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fh_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wtp_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__rts_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__vol_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__vol_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v1;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wds_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wds_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v1;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__acc_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v0] = 0U;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__acc_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v1;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v2) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__acc_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v2] = 0U;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v3) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__acc_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v3] = 0U;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v1;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v2;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__acc_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v4] = 0U;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fl_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fl_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fl_r__v1] = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__fh_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__fh_r__v1] = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__vol_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__vol_r__v2] = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wds_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wds_r__v2] = 0x80U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wtp_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__wtp_r__v1] = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__rts_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__rts_r__v1] = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__control_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__control_r__v3] = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__acc_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__acc_r__v5] = 0U;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    }
    vlSelfRef.wave_rd_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o;
    vlSelfRef.mono_mix_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r;
    vlSelfRef.left_mix_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r;
    vlSelfRef.right_mix_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_r));
    vlSelfRef.wave_address_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o;
    vlSelfRef.ready_o = (0U != (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_state_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_shift_w 
        = (0x1fU & (((IData)(9U) + (7U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_rts_r))) 
                    - (7U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_rts_r) 
                             >> 3U))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_wts_w 
        = (7U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_rts_r) 
                 >> 3U));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_mode_w 
        = (3U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r) 
                 >> 1U));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_ca_w 
        = (0xfU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__curr_control_r) 
                   >> 4U));
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
}

VL_INLINE_OPT void Vdoc5503_harness___024root___nba_sequent__TOP__1(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___nba_sequent__TOP__1\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.srff__DOT__prev_s_r = vlSelfRef.s;
    vlSelfRef.srff__DOT__prev_r_r = vlSelfRef.r;
    if (vlSelfRef.srff__DOT__edge_r_w) {
        vlSelfRef.srff__DOT__q_r = 0U;
    } else if (vlSelfRef.srff__DOT__edge_s_w) {
        vlSelfRef.srff__DOT__q_r = 1U;
    }
    vlSelfRef.srff__DOT__edge_s_w = ((~ (IData)(vlSelfRef.srff__DOT__prev_s_r)) 
                                     & (IData)(vlSelfRef.s));
    vlSelfRef.srff__DOT__edge_r_w = ((~ (IData)(vlSelfRef.srff__DOT__prev_r_r)) 
                                     & (IData)(vlSelfRef.r));
    vlSelfRef.q = (((~ (IData)(vlSelfRef.srff__DOT__edge_r_w)) 
                    & (IData)(vlSelfRef.srff__DOT__edge_s_w)) 
                   | (IData)(vlSelfRef.srff__DOT__q_r));
}

void Vdoc5503_harness___024root___eval_triggers__act(Vdoc5503_harness___024root* vlSelf);

bool Vdoc5503_harness___024root___eval_phase__act(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_phase__act\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<2> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vdoc5503_harness___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vdoc5503_harness___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vdoc5503_harness___024root___eval_phase__nba(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_phase__nba\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vdoc5503_harness___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_harness___024root___dump_triggers__ico(Vdoc5503_harness___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_harness___024root___dump_triggers__nba(Vdoc5503_harness___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_harness___024root___dump_triggers__act(Vdoc5503_harness___024root* vlSelf);
#endif  // VL_DEBUG

void Vdoc5503_harness___024root___eval(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ __VicoIterCount;
    CData/*0:0*/ __VicoContinue;
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VicoIterCount = 0U;
    vlSelfRef.__VicoFirstIteration = 1U;
    __VicoContinue = 1U;
    while (__VicoContinue) {
        if (VL_UNLIKELY(((0x64U < __VicoIterCount)))) {
#ifdef VL_DEBUG
            Vdoc5503_harness___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../../hdl/sound/../../hdl/support/srff.v", 1, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vdoc5503_harness___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vdoc5503_harness___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../../hdl/sound/../../hdl/support/srff.v", 1, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vdoc5503_harness___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../../hdl/sound/../../hdl/support/srff.v", 1, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vdoc5503_harness___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vdoc5503_harness___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vdoc5503_harness___024root___eval_debug_assertions(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_debug_assertions\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.s & 0xfeU)))) {
        Verilated::overWidthError("s");}
    if (VL_UNLIKELY(((vlSelfRef.r & 0xfeU)))) {
        Verilated::overWidthError("r");}
    if (VL_UNLIKELY(((vlSelfRef.clk_i & 0xfeU)))) {
        Verilated::overWidthError("clk_i");}
    if (VL_UNLIKELY(((vlSelfRef.reset_n_i & 0xfeU)))) {
        Verilated::overWidthError("reset_n_i");}
    if (VL_UNLIKELY(((vlSelfRef.clk_en_i & 0xfeU)))) {
        Verilated::overWidthError("clk_en_i");}
    if (VL_UNLIKELY(((vlSelfRef.cs_n_i & 0xfeU)))) {
        Verilated::overWidthError("cs_n_i");}
    if (VL_UNLIKELY(((vlSelfRef.we_n_i & 0xfeU)))) {
        Verilated::overWidthError("we_n_i");}
    if (VL_UNLIKELY(((vlSelfRef.wave_data_ready_i & 0xfeU)))) {
        Verilated::overWidthError("wave_data_ready_i");}
}
#endif  // VL_DEBUG
