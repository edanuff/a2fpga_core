// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vdoc5503_reg_harness.h for the primary calling header

#include "Vdoc5503_reg_harness__pch.h"
#include "Vdoc5503_reg_harness___024root.h"

void Vdoc5503_reg_harness___024root___eval_act(Vdoc5503_reg_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_reg_harness___024root___eval_act\n"); );
    Vdoc5503_reg_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vdoc5503_reg_harness___024root___nba_sequent__TOP__0(Vdoc5503_reg_harness___024root* vlSelf);

void Vdoc5503_reg_harness___024root___eval_nba(Vdoc5503_reg_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_reg_harness___024root___eval_nba\n"); );
    Vdoc5503_reg_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vdoc5503_reg_harness___024root___nba_sequent__TOP__0(vlSelf);
        vlSelfRef.__Vm_traceActivity[1U] = 1U;
    }
}

VL_INLINE_OPT void Vdoc5503_reg_harness___024root___nba_sequent__TOP__0(Vdoc5503_reg_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_reg_harness___024root___nba_sequent__TOP__0\n"); );
    Vdoc5503_reg_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*3:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w = 0;
    IData/*24:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__temp_acc;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__temp_acc = 0;
    IData/*23:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__curr_acc_mask_w;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__curr_acc_mask_w = 0;
    IData/*31:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__high_bit_w;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__high_bit_w = 0;
    CData/*0:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__overflow;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__overflow = 0;
    CData/*7:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__data_w;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__data_w = 0;
    SData/*8:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__vol_s;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__vol_s = 0;
    SData/*15:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__output_w;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__output_w = 0;
    SData/*15:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__curr_wave_addr_w;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__curr_wave_addr_w = 0;
    CData/*7:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__ptr_hi_mask_w;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__ptr_hi_mask_w = 0;
    SData/*15:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__ptr_w;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__ptr_w = 0;
    SData/*15:0*/ __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__addr_w;
    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__addr_w = 0;
    CData/*5:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r = 0;
    CData/*2:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__clk_count_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__clk_count_r = 0;
    CData/*4:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r = 0;
    CData/*1:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r = 0;
    CData/*0:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r = 0;
    CData/*0:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 0;
    CData/*0:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_pending_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_pending_r = 0;
    IData/*23:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r = 0;
    IData/*23:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r = 0;
    IData/*23:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r = 0;
    CData/*7:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r = 0;
    CData/*7:0*/ __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v0;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v0;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 0;
    SData/*15:0*/ __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0;
    IData/*23:0*/ __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1;
    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0 = 0;
    IData/*23:0*/ __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0 = 0;
    // Body
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__clk_count_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__clk_count_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_pending_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_pending_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r;
    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 0U;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0U;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0U;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0 = 0U;
    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_we_r) {
        __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_din_r;
        __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fl_r;
        __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_we_r) {
        __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_din_r;
        __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fh_r;
        __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_we_r) {
        __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_din_r;
        __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_vol_r;
        __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_we_r) {
        __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_din_r;
        __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wds_r;
        __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_we_r) {
        __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_din_r;
        __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wtp_r;
        __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r) {
        __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_din_r;
        __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r;
        __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_we_r) {
        __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_din_r;
        __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_rts_r;
        __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_we_r) {
        __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_din_r;
        __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_acc_r;
        __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.reset_n_i) {
        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r 
            = (0x3fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r)));
        if (vlSelfRef.clk_en_i) {
            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__clk_count_r 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__clk_count_r)));
            if ((7U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__clk_count_r))) {
                if ((0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r = 0U;
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r = 1U;
                } else if ((1U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
                    if (((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r) 
                         == (0x1fU & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r) 
                                      >> 1U)))) {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r = 2U;
                    } else {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r 
                            = (0x1fU & ((IData)(1U) 
                                        + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r)));
                    }
                } else if ((2U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r = 3U;
                } else if ((3U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r = 0U;
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r = 1U;
                }
                __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r = 0U;
            }
        }
        vlSelfRef.wave_rd_o = 0U;
        if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_access_r) {
            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r = 1U;
        }
        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fl_r 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fh_r 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_vol_r 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wds_r 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wtp_r 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_rts_r 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_acc_r 
            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_we_r = 0U;
        if (vlSelfRef.wave_data_ready_i) {
            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_pending_r = 1U;
        }
        if ((2U & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
            if ((1U & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
                if ((0x10U > (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r))) {
                    if ((0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r))) {
                        if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r) {
                            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 0U;
                            if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                                if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                    if ((0U == (7U 
                                                & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                   >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_dout_w;
                                    } else if ((1U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_dout_w;
                                    } else if ((2U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_dout_w;
                                    } else if ((3U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_dout_w;
                                    } else if ((4U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_dout_w;
                                    } else if ((5U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                                    } else if ((6U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_dout_w;
                                    }
                                } else {
                                    vlSelfRef.data_o 
                                        = ((0xe0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                            ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r)
                                            : ((0xe1U 
                                                == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                                ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r)
                                                : 0U));
                                }
                            }
                        }
                    }
                    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 
                        = (0xfU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
                    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 1U;
                    if ((0xfU == (0xfU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r)))) {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r = 0U;
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r = 0U;
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r = 0U;
                    }
                } else {
                    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r) {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r = 0U;
                        if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 1U;
                            if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                  >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fl_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                } else if ((1U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fh_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                } else if ((2U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_vol_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                } else if ((3U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wds_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                } else if ((4U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wtp_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                } else if ((5U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                } else if ((6U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_rts_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                }
                            }
                        } else {
                            vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                            if ((0xe0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                            } else if ((0xe1U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                            } else if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                  >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_din_r 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_we_r = 1U;
                                } else if ((1U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_din_r 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_we_r = 1U;
                                } else if ((2U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_din_r 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_we_r = 1U;
                                } else if ((3U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_din_r 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_we_r = 1U;
                                } else if ((4U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_din_r 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_we_r = 1U;
                                } else if ((5U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_din_r 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r = 1U;
                                } else if ((6U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_din_r 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_we_r = 1U;
                                }
                            }
                        }
                    }
                    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r) {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 0U;
                        if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                            if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                  >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_dout_w;
                                } else if ((1U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_dout_w;
                                } else if ((2U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_dout_w;
                                } else if ((3U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_dout_w;
                                } else if ((4U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_dout_w;
                                } else if ((5U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                                } else if ((6U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_dout_w;
                                }
                            } else {
                                vlSelfRef.data_o = 
                                    ((0xe0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                      ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r)
                                      : ((0xe1U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                          ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r)
                                          : 0U));
                            }
                        }
                    }
                }
            } else if ((0x10U > (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r))) {
                if ((0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r))) {
                    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r) {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 0U;
                        if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                            if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                  >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_dout_w;
                                } else if ((1U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_dout_w;
                                } else if ((2U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_dout_w;
                                } else if ((3U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_dout_w;
                                } else if ((4U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_dout_w;
                                } else if ((5U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                                } else if ((6U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.data_o 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_dout_w;
                                }
                            } else {
                                vlSelfRef.data_o = 
                                    ((0xe0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                      ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r)
                                      : ((0xe1U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                          ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r)
                                          : 0U));
                            }
                        }
                    }
                }
                __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0 
                    = ((0x8000U & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r
                                   [(0xfU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r))] 
                                   >> 8U)) | (0x7fffU 
                                              & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r
                                                 [(0xfU 
                                                   & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r))] 
                                                 >> 3U)));
                __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0 
                    = (0xfU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
                __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 1U;
                if ((0xfU == (0xfU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r)))) {
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__mono_mix_r 
                        = ((0x8000U & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
                                       >> 8U)) | (0x7fffU 
                                                  & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
                                                     >> 3U)));
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__left_mix_r 
                        = ((0x8000U & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
                                       >> 8U)) | (0x7fffU 
                                                  & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
                                                     >> 3U)));
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__right_mix_r 
                        = ((0x8000U & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
                                       >> 8U)) | (0x7fffU 
                                                  & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
                                                     >> 3U)));
                }
            } else {
                if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r) {
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r = 0U;
                    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 1U;
                        if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fl_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fh_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_vol_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wds_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wtp_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_rts_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            }
                        }
                    } else {
                        vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                        if ((0xe0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r 
                                = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                        } else if ((0xe1U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r 
                                = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                        } else if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_we_r = 1U;
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_we_r = 1U;
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_we_r = 1U;
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_we_r = 1U;
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_we_r = 1U;
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r = 1U;
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_we_r = 1U;
                            }
                        }
                    }
                }
                if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r) {
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 0U;
                    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                        if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_dout_w;
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_dout_w;
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_dout_w;
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_dout_w;
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_dout_w;
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_dout_w;
                            }
                        } else {
                            vlSelfRef.data_o = ((0xe0U 
                                                 == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                                 ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r)
                                                 : 
                                                ((0xe1U 
                                                  == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                                  ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r)
                                                  : 0U));
                        }
                    }
                }
            }
        } else if ((1U & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r))) {
            __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w 
                = ((0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r))
                    ? 0U : (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r));
            if ((8U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                if ((4U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                    if ((2U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                        if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r) {
                            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r = 0U;
                            if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                                __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 1U;
                                if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                    if ((0U == (7U 
                                                & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                   >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fl_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                    } else if ((1U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fh_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                    } else if ((2U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_vol_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                    } else if ((3U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wds_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                    } else if ((4U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wtp_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                    } else if ((5U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                    } else if ((6U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_rts_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                                    }
                                }
                            } else {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                if ((0xe0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                } else if ((0xe1U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r 
                                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                } else if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                    if ((0U == (7U 
                                                & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                   >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_din_r 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_we_r = 1U;
                                    } else if ((1U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_din_r 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_we_r = 1U;
                                    } else if ((2U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_din_r 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_we_r = 1U;
                                    } else if ((3U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_din_r 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_we_r = 1U;
                                    } else if ((4U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_din_r 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_we_r = 1U;
                                    } else if ((5U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_din_r 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r = 1U;
                                    } else if ((6U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_din_r 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_we_r = 1U;
                                    }
                                }
                            }
                        }
                        if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r) {
                            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 0U;
                            if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                                if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                                    if ((0U == (7U 
                                                & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                   >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_dout_w;
                                    } else if ((1U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_dout_w;
                                    } else if ((2U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_dout_w;
                                    } else if ((3U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_dout_w;
                                    } else if ((4U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_dout_w;
                                    } else if ((5U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                                    } else if ((6U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.data_o 
                                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_dout_w;
                                    }
                                } else {
                                    vlSelfRef.data_o 
                                        = ((0xe0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                            ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r)
                                            : ((0xe1U 
                                                == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                                ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r)
                                                : 0U));
                                }
                            }
                        }
                        if ((0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r))) {
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 1U;
                        }
                    } else if ((1U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r = 1U;
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_din_r 
                            = (0xfeU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_control_r));
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                    } else {
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r 
                            = (1U ^ (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r));
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r = 1U;
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_din_r 
                            = (0xfeU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__partner_control_r));
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_acc_r 
                            = (1U ^ (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r));
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_we_r = 1U;
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_din_r = 0U;
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                    }
                } else if ((2U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                    if ((1U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                        if ((1U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_mode_w) 
                                   | (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__halt_zero_r)))) {
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r = 1U;
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_din_r 
                                = (1U | (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_control_r));
                        }
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                        if ((3U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_mode_w))) {
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0xcU;
                        } else if (((3U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__partner_mode_w)) 
                                    & (~ (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r)))) {
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0xdU;
                        }
                    } else {
                        if ((2U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_mode_w))) {
                            if ((1U & (~ (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r)))) {
                                if ((1U & (~ (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__prev_halt_w)))) {
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_acc_r 
                                        = (0x1fU & 
                                           ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r) 
                                            - (IData)(1U)));
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_we_r = 1U;
                                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_din_r = 0U;
                                }
                            }
                        }
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0xbU;
                    }
                } else if ((1U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__temp_acc 
                        = (0x1ffffffU & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_acc_r 
                                         + (((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_fh_r) 
                                             << 8U) 
                                            | (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_fl_r))));
                    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__curr_acc_mask_w 
                        = (0xffU | (0xffff00U & ((((IData)(1U) 
                                                   << 
                                                   (0x1fU 
                                                    & ((IData)(9U) 
                                                       + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_res_w)))) 
                                                  - (IData)(1U)) 
                                                 << 8U)));
                    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__high_bit_w 
                        = ((IData)(0x11U) + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_res_w));
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_we_r = 1U;
                    __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__overflow 
                        = ((0x18U >= (0x1fU & __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__high_bit_w)) 
                           && (1U & (__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__temp_acc 
                                     >> (0x1fU & __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__high_bit_w))));
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_din_r 
                        = (__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__temp_acc 
                           & __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__curr_acc_mask_w);
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                    if (__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_acc__19__overflow) {
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0xaU;
                    }
                } else {
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
                        = (0xffffffU & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
                                        + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_output_r)));
                    if ((1U & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_ca_w))) {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
                            = (0xffffffU & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
                                            + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_output_r)));
                    } else {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
                            = (0xffffffU & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
                                            + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_output_r)));
                    }
                    __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 
                        = (0xffffffU & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_sum_r 
                                        + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_output_r)));
                    __VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_ca_w;
                    __VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 1U;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 9U;
                }
            } else if ((4U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                if ((2U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                    if ((1U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                        if (((2U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_mode_w)) 
                             & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r))) {
                            if (((0x1eU > (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r)) 
                                 & (~ (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_halt_w)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_vol_r 
                                    = (0x1fU & ((IData)(1U) 
                                                + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r)));
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_we_r = 1U;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_wds_r;
                            }
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 9U;
                        } else {
                            __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__data_w 
                                = (0x80U ^ (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_wds_r));
                            __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__vol_s 
                                = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_vol_r;
                            __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__output_w 
                                = (0xffffU & VL_MULS_III(16, 
                                                         (0xffffU 
                                                          & VL_EXTENDS_II(16,8, (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__data_w))), 
                                                         (0xffffU 
                                                          & VL_EXTENDS_II(16,9, (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__vol_s)))));
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_sum_r 
                                = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r
                                [vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_ca_w];
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 8U;
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_output_r 
                                = __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_out__21__unnamedblk2__DOT__output_w;
                        }
                    } else if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_pending_r) {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_pending_r = 0U;
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_we_r = 1U;
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_din_r 
                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_r;
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_wds_r 
                            = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_r;
                        if ((0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_r))) {
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__halt_zero_r = 1U;
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0xaU;
                        } else {
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 7U;
                        }
                    } else if ((0x28U < (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r))) {
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                    }
                } else if ((1U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__halt_w) {
                        if ((1U & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_mode_w))) {
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_we_r = 1U;
                            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_din_r = 0U;
                        }
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                    } else {
                        __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__curr_wave_addr_w 
                            = (0xffffU & (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_acc_r 
                                          >> (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_shift_w)));
                        __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__ptr_hi_mask_w 
                            = (0xffU & ((IData)(0xffU) 
                                        << (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_wts_w)));
                        __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__ptr_w 
                            = (((IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__ptr_hi_mask_w) 
                                & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_wtp_r)) 
                               << 8U);
                        vlSelfRef.wave_rd_o = 1U;
                        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 6U;
                        __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__addr_w 
                            = ((IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__curr_wave_addr_w) 
                               | (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__ptr_w));
                        vlSelfRef.wave_address_o = __Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_request_data__23__unnamedblk1__DOT__addr_w;
                    }
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__prev_control_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                } else {
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_control_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r 
                        = (0x1fU & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r) 
                                    - (IData)(1U)));
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 5U;
                }
            } else if ((2U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                if ((1U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__partner_control_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r 
                        = (0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r)));
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 4U;
                } else {
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_fl_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_dout_w;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_fh_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_dout_w;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_vol_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_dout_w;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_wds_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_dout_w;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_wtp_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_dout_w;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_control_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_rts_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_dout_w;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_acc_r 
                        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_dout_w;
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r 
                        = (1U ^ (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r));
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 3U;
                }
            } else if ((1U & (IData)(__Vtask_doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_osc__10__osc_state_w))) {
                if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r) {
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 0U;
                    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                        if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_dout_w;
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_dout_w;
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_dout_w;
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_dout_w;
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_dout_w;
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_dout_w;
                            }
                        } else {
                            vlSelfRef.data_o = ((0xe0U 
                                                 == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                                 ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r)
                                                 : 
                                                ((0xe1U 
                                                  == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                                  ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r)
                                                  : 0U));
                        }
                    }
                }
                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_wave_addr_r = 0U;
                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_output_r = 0U;
                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__halt_zero_r = 0U;
                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 2U;
            } else {
                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r) {
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r = 0U;
                    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 1U;
                        if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fl_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fh_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_vol_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wds_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wtp_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_rts_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r));
                            }
                        }
                    } else {
                        vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                        if ((0xe0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r 
                                = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                        } else if ((0xe1U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r 
                                = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                        } else if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_we_r = 1U;
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_we_r = 1U;
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_we_r = 1U;
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_we_r = 1U;
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_we_r = 1U;
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r = 1U;
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_din_r 
                                    = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r;
                                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_we_r = 1U;
                            }
                        }
                    }
                }
                if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r) {
                    __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 0U;
                    if (vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r) {
                        if ((0xdfU >= (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_dout_w;
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_dout_w;
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_dout_w;
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_dout_w;
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_dout_w;
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w;
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.data_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_dout_w;
                            }
                        } else {
                            vlSelfRef.data_o = ((0xe0U 
                                                 == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                                 ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r)
                                                 : 
                                                ((0xe1U 
                                                  == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r))
                                                  ? (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r)
                                                  : 0U));
                        }
                    }
                }
                if ((0U == (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r))) {
                    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 1U;
                }
            }
        } else {
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fl_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fh_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_vol_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wds_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wtp_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_rts_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_acc_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r));
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_we_r = 1U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_we_r = 1U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_we_r = 1U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_we_r = 1U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_we_r = 1U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r = 1U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_we_r = 1U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_we_r = 1U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_din_r = 0U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_din_r = 0U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_din_r = 0U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_din_r = 0U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_din_r = 0U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_din_r = 0U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_din_r = 0U;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_din_r = 0U;
        }
        if (vlSelfRef.wave_data_ready_i) {
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_r 
                = vlSelfRef.wave_data_i;
        }
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_access_r = 0U;
        if ((1U & (~ (IData)(vlSelfRef.cs_n_i)))) {
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r 
                = vlSelfRef.addr_i;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r 
                = vlSelfRef.data_i;
            vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r 
                = vlSelfRef.we_n_i;
            if (((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__prev_cs_n_r) 
                 | ((~ (IData)(vlSelfRef.we_n_i)) & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__prev_we_n_r)))) {
                vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_access_r = 1U;
            }
        }
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__prev_we_n_r 
            = vlSelfRef.we_n_i;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__prev_cs_n_r 
            = vlSelfRef.cs_n_i;
    } else {
        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r = 0U;
        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__clk_count_r = 0U;
        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r = 0U;
        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r = 0U;
        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_pending_r = 0U;
        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r = 0U;
        __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
        vlSelfRef.wave_address_o = 0U;
        vlSelfRef.wave_rd_o = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__halt_zero_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_we_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_r = 0x80U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_addr_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_data_r = 0U;
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_we_n_r = 1U;
    }
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__clk_count_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__clk_count_r;
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r__v0;
    }
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__host_request_pending_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__device_response_pending_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_osc_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_pending_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__loaded_wds_pending_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_timer_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_mono_mix_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_left_mix_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_right_mix_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_int_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__osc_en_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r 
        = __Vdly__doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r;
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v0) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v0] = 0U;
    }
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1] 
            = __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__next_channel_r__v1;
    }
    if (__VdlySet__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0) {
        vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r[__VdlyDim0__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0] 
            = __VdlyVal__doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    }
    vlSelfRef.ready_o = (0U != (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__cycle_state_r));
    vlSelfRef.mono_mix_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__mono_mix_r;
    vlSelfRef.left_mix_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__left_mix_r;
    vlSelfRef.right_mix_o = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__right_mix_r;
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__prev_halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__prev_control_r));
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__next_control_r));
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__partner_mode_w 
        = (3U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__partner_control_r) 
                 >> 1U));
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fl_dout_w 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__fl_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fl_r];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_fh_dout_w 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__fh_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_fh_r];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_vol_dout_w 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__vol_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_vol_r];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wds_dout_w 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__wds_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wds_r];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_wtp_dout_w 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__wtp_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_wtp_r];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_control_dout_w 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__control_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_control_r];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_rts_dout_w 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__rts_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_rts_r];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_acc_dout_w 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__acc_ram__DOT__osc_reg_r
        [vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__target_osc_acc_r];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_control_r));
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_mode_w 
        = (3U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_control_r) 
                 >> 1U));
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_ca_w 
        = (0xfU & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_control_r) 
                   >> 4U));
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_shift_w 
        = (0x1fU & (((IData)(9U) + (7U & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_rts_r))) 
                    - (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_rts_r) 
                             >> 3U))));
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_wts_w 
        = (7U & ((IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_rts_r) 
                 >> 3U));
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_res_w 
        = (7U & (IData)(vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__curr_rts_r));
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[0U] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [0U];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[1U] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [1U];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[2U] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [2U];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[3U] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [3U];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[4U] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [4U];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[5U] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [5U];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[6U] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [6U];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[7U] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [7U];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[8U] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [8U];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[9U] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [9U];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[0xaU] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [0xaU];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[0xbU] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [0xbU];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[0xcU] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [0xcU];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[0xdU] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [0xdU];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[0xeU] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [0xeU];
    vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_o[0xfU] 
        = vlSelfRef.doc5503_reg_harness__DOT__doc5503_inst__DOT__channel_r
        [0xfU];
}

void Vdoc5503_reg_harness___024root___eval_triggers__act(Vdoc5503_reg_harness___024root* vlSelf);

bool Vdoc5503_reg_harness___024root___eval_phase__act(Vdoc5503_reg_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_reg_harness___024root___eval_phase__act\n"); );
    Vdoc5503_reg_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vdoc5503_reg_harness___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vdoc5503_reg_harness___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vdoc5503_reg_harness___024root___eval_phase__nba(Vdoc5503_reg_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_reg_harness___024root___eval_phase__nba\n"); );
    Vdoc5503_reg_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vdoc5503_reg_harness___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_reg_harness___024root___dump_triggers__nba(Vdoc5503_reg_harness___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_reg_harness___024root___dump_triggers__act(Vdoc5503_reg_harness___024root* vlSelf);
#endif  // VL_DEBUG

void Vdoc5503_reg_harness___024root___eval(Vdoc5503_reg_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_reg_harness___024root___eval\n"); );
    Vdoc5503_reg_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vdoc5503_reg_harness___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("harness/doc5503_reg_harness.sv", 4, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vdoc5503_reg_harness___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("harness/doc5503_reg_harness.sv", 4, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vdoc5503_reg_harness___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vdoc5503_reg_harness___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vdoc5503_reg_harness___024root___eval_debug_assertions(Vdoc5503_reg_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_reg_harness___024root___eval_debug_assertions\n"); );
    Vdoc5503_reg_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
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
