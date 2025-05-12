// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vdoc5503_verilog_test.h for the primary calling header

#include "Vdoc5503_verilog_test__pch.h"
#include "Vdoc5503_verilog_test___024root.h"

VlCoroutine Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__0(Vdoc5503_verilog_test___024root* vlSelf);
VlCoroutine Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__1(Vdoc5503_verilog_test___024root* vlSelf);
VlCoroutine Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__2(Vdoc5503_verilog_test___024root* vlSelf);

void Vdoc5503_verilog_test___024root___eval_initial(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_initial\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vm_traceActivity[1U] = 1U;
    Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__1(vlSelf);
    Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__2(vlSelf);
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__doc5503_verilog_test__DOT__clk__0 
        = vlSelfRef.doc5503_verilog_test__DOT__clk;
}

VL_INLINE_OPT VlCoroutine Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__0(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.doc5503_verilog_test__DOT__clk = 0U;
    while (1U) {
        co_await vlSelfRef.__VdlySched.delay(0x2710ULL, 
                                             nullptr, 
                                             "doc5503_verilog_test.sv", 
                                             56);
        vlSelfRef.doc5503_verilog_test__DOT__clk = 
            (1U & (~ (IData)(vlSelfRef.doc5503_verilog_test__DOT__clk)));
    }
}

VL_INLINE_OPT VlCoroutine Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__2(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__2\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x12a05f200ULL, 
                                         nullptr, "doc5503_verilog_test.sv", 
                                         257);
    VL_WRITEF_NX("Simulation timed out\n",0);
    VL_FINISH_MT("doc5503_verilog_test.sv", 259, "");
}

void Vdoc5503_verilog_test___024root___ico_sequent__TOP__0(Vdoc5503_verilog_test___024root* vlSelf);

void Vdoc5503_verilog_test___024root___eval_ico(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_ico\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vdoc5503_verilog_test___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vdoc5503_verilog_test___024root___ico_sequent__TOP__0(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___ico_sequent__TOP__0\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

void Vdoc5503_verilog_test___024root___eval_triggers__ico(Vdoc5503_verilog_test___024root* vlSelf);

bool Vdoc5503_verilog_test___024root___eval_phase__ico(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_phase__ico\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vdoc5503_verilog_test___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vdoc5503_verilog_test___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vdoc5503_verilog_test___024root___act_sequent__TOP__0(Vdoc5503_verilog_test___024root* vlSelf);

void Vdoc5503_verilog_test___024root___eval_act(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_act\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((2ULL & vlSelfRef.__VactTriggered.word(0U))) {
        Vdoc5503_verilog_test___024root___act_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vdoc5503_verilog_test___024root___act_sequent__TOP__0(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___act_sequent__TOP__0\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.doc5503_verilog_test__DOT__clk_en = (0U 
                                                   == 
                                                   (7U 
                                                    & vlSelfRef.doc5503_verilog_test__DOT__cycle_count));
}

void Vdoc5503_verilog_test___024root___nba_sequent__TOP__0(Vdoc5503_verilog_test___024root* vlSelf);
void Vdoc5503_verilog_test___024root___nba_sequent__TOP__1(Vdoc5503_verilog_test___024root* vlSelf);

void Vdoc5503_verilog_test___024root___eval_nba(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_nba\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((2ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vdoc5503_verilog_test___024root___nba_sequent__TOP__0(vlSelf);
        vlSelfRef.__Vm_traceActivity[3U] = 1U;
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vdoc5503_verilog_test___024root___nba_sequent__TOP__1(vlSelf);
        vlSelfRef.__Vm_traceActivity[4U] = 1U;
    }
}

VL_INLINE_OPT void Vdoc5503_verilog_test___024root___nba_sequent__TOP__0(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___nba_sequent__TOP__0\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*3:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w = 0;
    IData/*24:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__temp_acc;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__temp_acc = 0;
    IData/*23:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__curr_acc_mask_w;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__curr_acc_mask_w = 0;
    IData/*31:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__high_bit_w;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__high_bit_w = 0;
    CData/*0:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__overflow;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__overflow = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__data_w;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__data_w = 0;
    SData/*8:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__vol_s;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__vol_s = 0;
    SData/*15:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__output_w;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__output_w = 0;
    SData/*15:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__curr_wave_addr_w;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__curr_wave_addr_w = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__ptr_hi_mask_w;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__ptr_hi_mask_w = 0;
    SData/*15:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__ptr_w;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__ptr_w = 0;
    SData/*15:0*/ __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__addr_w;
    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__addr_w = 0;
    IData/*31:0*/ __Vdly__doc5503_verilog_test__DOT__cycle_count;
    __Vdly__doc5503_verilog_test__DOT__cycle_count = 0;
    CData/*5:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r = 0;
    CData/*2:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__clk_count_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__clk_count_r = 0;
    CData/*4:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__curr_osc_r = 0;
    CData/*1:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_state_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_state_r = 0;
    CData/*0:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r = 0;
    CData/*0:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0;
    CData/*0:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r = 0;
    IData/*23:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r = 0;
    IData/*23:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r = 0;
    IData/*23:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r = 0;
    CData/*7:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_int_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_int_r = 0;
    CData/*7:0*/ __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_en_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_en_r = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v0;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v0;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v0 = 0;
    SData/*15:0*/ __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0;
    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0 = 0;
    IData/*23:0*/ __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1;
    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0 = 0;
    IData/*23:0*/ __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0;
    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0;
    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0 = 0;
    // Body
    __Vdly__doc5503_verilog_test__DOT__cycle_count 
        = vlSelfRef.doc5503_verilog_test__DOT__cycle_count;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__clk_count_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__clk_count_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__curr_osc_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_state_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_state_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_int_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_int_r;
    __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_en_r 
        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v0 = 0U;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1 = 0U;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0 = 0U;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0 = 0U;
    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0 = 0U;
    if (VL_UNLIKELY((vlSelfRef.doc5503_verilog_test__DOT__wave_rd))) {
        VL_WRITEF_NX("Wave read at addr=0x%04x, providing data=0x%02x\n",0,
                     16,vlSelfRef.doc5503_verilog_test__DOT__wave_address,
                     8,(IData)(vlSelfRef.doc5503_verilog_test__DOT__wave_data));
        vlSelfRef.doc5503_verilog_test__DOT__wave_data_ready = 1U;
        vlSelfRef.doc5503_verilog_test__DOT__wave_data = 0x80U;
    } else {
        vlSelfRef.doc5503_verilog_test__DOT__wave_data_ready = 0U;
    }
    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_we_r) {
        __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_din_r;
        __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r;
        __VdlySet__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_we_r) {
        __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_din_r;
        __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r;
        __VdlySet__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r) {
        __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_din_r;
        __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r;
        __VdlySet__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r) {
        __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_din_r;
        __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r;
        __VdlySet__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_we_r) {
        __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_din_r;
        __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r;
        __VdlySet__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r) {
        __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_din_r;
        __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r;
        __VdlySet__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_we_r) {
        __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_din_r;
        __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r;
        __VdlySet__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_we_r) {
        __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_din_r;
        __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_acc_r;
        __VdlySet__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0 = 1U;
    }
    if (vlSelfRef.doc5503_verilog_test__DOT__reset_n) {
        __Vdly__doc5503_verilog_test__DOT__cycle_count 
            = ((IData)(1U) + vlSelfRef.doc5503_verilog_test__DOT__cycle_count);
        __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r 
            = (0x3fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r)));
        if ((0U == (7U & vlSelfRef.doc5503_verilog_test__DOT__cycle_count))) {
            __Vdly__doc5503_verilog_test__DOT__dut__DOT__clk_count_r 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__clk_count_r)));
            if ((7U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__clk_count_r))) {
                if ((0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_state_r))) {
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__curr_osc_r = 0U;
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_state_r = 1U;
                } else if ((1U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_state_r))) {
                    if (((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r) 
                         == (0x1fU & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r) 
                                      >> 1U)))) {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_state_r = 2U;
                    } else {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__curr_osc_r 
                            = (0x1fU & ((IData)(1U) 
                                        + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r)));
                    }
                } else if ((2U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_state_r))) {
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_state_r = 3U;
                } else if ((3U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_state_r))) {
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__curr_osc_r = 0U;
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_state_r = 1U;
                }
                __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r = 0U;
            }
        }
        vlSelfRef.doc5503_verilog_test__DOT__wave_rd = 0U;
        if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_access_r) {
            __Vdly__doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r = 1U;
        }
        __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_acc_r 
            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_we_r = 0U;
        if (vlSelfRef.doc5503_verilog_test__DOT__wave_data_ready) {
            __Vdly__doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r = 1U;
        }
        if ((2U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_state_r))) {
            if ((1U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_state_r))) {
                if ((0x10U > (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r))) {
                    if ((0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r))) {
                        if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r) {
                            __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0U;
                            if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                                if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                    if ((0U == (7U 
                                                & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                   >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w;
                                    } else if ((1U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w;
                                    } else if ((2U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w;
                                    } else if ((3U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w;
                                    } else if ((4U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w;
                                    } else if ((5U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                                    } else if ((6U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w;
                                    }
                                } else {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                            ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_int_r)
                                            : ((0xe1U 
                                                == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                                ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r)
                                                : 0U));
                                }
                            }
                        }
                    }
                    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v0 
                        = (0xfU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r));
                    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v0 = 1U;
                    if ((0xfU == (0xfU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r)))) {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r = 0U;
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r = 0U;
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r = 0U;
                    }
                } else {
                    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r) {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r = 0U;
                        if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                            __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 1U;
                            if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                  >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                } else if ((1U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                } else if ((2U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                } else if ((3U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                } else if ((4U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                } else if ((5U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                } else if ((6U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                }
                            }
                        } else {
                            vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                            if ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_int_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                            } else if ((0xe1U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_en_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                            } else if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                  >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_din_r 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_we_r = 1U;
                                } else if ((1U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_din_r 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_we_r = 1U;
                                } else if ((2U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_din_r 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r = 1U;
                                } else if ((3U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_din_r 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r = 1U;
                                } else if ((4U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_din_r 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_we_r = 1U;
                                } else if ((5U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_din_r 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = 1U;
                                } else if ((6U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r 
                                        = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_din_r 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_we_r = 1U;
                                }
                            }
                        }
                    }
                    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r) {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0U;
                        if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                            if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                  >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w;
                                } else if ((1U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w;
                                } else if ((2U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w;
                                } else if ((3U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w;
                                } else if ((4U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w;
                                } else if ((5U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                                } else if ((6U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w;
                                }
                            } else {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                        ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_int_r)
                                        : ((0xe1U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                            ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r)
                                            : 0U));
                            }
                        }
                    }
                }
            } else if ((0x10U > (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r))) {
                if ((0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r))) {
                    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r) {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0U;
                        if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                            if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                  >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w;
                                } else if ((1U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w;
                                } else if ((2U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w;
                                } else if ((3U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w;
                                } else if ((4U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w;
                                } else if ((5U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                                } else if ((6U == (7U 
                                                   & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                      >> 5U)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w;
                                }
                            } else {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                        ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_int_r)
                                        : ((0xe1U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                            ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r)
                                            : 0U));
                            }
                        }
                    }
                }
                __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0 
                    = ((0x8000U & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_channel_r
                                   [(0xfU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r))] 
                                   >> 8U)) | (0x7fffU 
                                              & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_channel_r
                                                 [(0xfU 
                                                   & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r))] 
                                                 >> 3U)));
                __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0 
                    = (0xfU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r));
                __VdlySet__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0 = 1U;
                if ((0xfU == (0xfU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r)))) {
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__mono_mix_r 
                        = ((0x8000U & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r 
                                       >> 8U)) | (0x7fffU 
                                                  & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r 
                                                     >> 3U)));
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__left_mix_r 
                        = ((0x8000U & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r 
                                       >> 8U)) | (0x7fffU 
                                                  & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r 
                                                     >> 3U)));
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__right_mix_r 
                        = ((0x8000U & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r 
                                       >> 8U)) | (0x7fffU 
                                                  & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r 
                                                     >> 3U)));
                }
            } else {
                if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r) {
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r = 0U;
                    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 1U;
                        if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            }
                        }
                    } else {
                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                        if ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_int_r 
                                = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                        } else if ((0xe1U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_en_r 
                                = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                        } else if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_we_r = 1U;
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_we_r = 1U;
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r = 1U;
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r = 1U;
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_we_r = 1U;
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = 1U;
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_we_r = 1U;
                            }
                        }
                    }
                }
                if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r) {
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0U;
                    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                        if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w;
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w;
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w;
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w;
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w;
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w;
                            }
                        } else {
                            vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                = ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                    ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_int_r)
                                    : ((0xe1U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                        ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r)
                                        : 0U));
                        }
                    }
                }
            }
        } else if ((1U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_state_r))) {
            __Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w 
                = ((0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r))
                    ? 0U : (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r));
            if ((8U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                if ((4U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                    if ((2U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0U;
                        if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r) {
                            __Vdly__doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r = 0U;
                            if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                                __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 1U;
                                if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                    if ((0U == (7U 
                                                & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                   >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    } else if ((1U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    } else if ((2U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    } else if ((3U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    } else if ((4U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    } else if ((5U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    } else if ((6U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                    }
                                }
                            } else {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                if ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_int_r 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                } else if ((0xe1U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_en_r 
                                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                } else if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                    if ((0U == (7U 
                                                & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                   >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_din_r 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_we_r = 1U;
                                    } else if ((1U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_din_r 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_we_r = 1U;
                                    } else if ((2U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_din_r 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r = 1U;
                                    } else if ((3U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_din_r 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r = 1U;
                                    } else if ((4U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_din_r 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_we_r = 1U;
                                    } else if ((5U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_din_r 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = 1U;
                                    } else if ((6U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r 
                                            = (0x1fU 
                                               & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_din_r 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_we_r = 1U;
                                    }
                                }
                            }
                        }
                        if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r) {
                            __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0U;
                            if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                                if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                                    if ((0U == (7U 
                                                & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                   >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w;
                                    } else if ((1U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w;
                                    } else if ((2U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w;
                                    } else if ((3U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w;
                                    } else if ((4U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w;
                                    } else if ((5U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                                    } else if ((6U 
                                                == 
                                                (7U 
                                                 & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                    >> 5U)))) {
                                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w;
                                    }
                                } else {
                                    vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                        = ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                            ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_int_r)
                                            : ((0xe1U 
                                                == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                                ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r)
                                                : 0U));
                                }
                            }
                        }
                        if ((0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r))) {
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 1U;
                        }
                    } else if ((1U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = 1U;
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_din_r 
                            = (0xfeU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_control_r));
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0U;
                    } else {
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                            = (1U ^ (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r));
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = 1U;
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_din_r 
                            = (0xfeU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__partner_control_r));
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_acc_r 
                            = (1U ^ (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r));
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_we_r = 1U;
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_din_r = 0U;
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0U;
                    }
                } else if ((2U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                    if ((1U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                        if ((1U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_mode_w) 
                                   | (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__halt_zero_r)))) {
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = 1U;
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_din_r 
                                = (1U | (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_control_r));
                        }
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0U;
                        if ((3U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_mode_w))) {
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0xcU;
                        } else if (((3U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__partner_mode_w)) 
                                    & (~ (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r)))) {
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0xdU;
                        }
                    } else {
                        if ((2U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_mode_w))) {
                            if ((1U & (~ (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r)))) {
                                if ((1U & (~ (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__prev_halt_w)))) {
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_acc_r 
                                        = (0x1fU & 
                                           ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r) 
                                            - (IData)(1U)));
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_we_r = 1U;
                                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_din_r = 0U;
                                }
                            }
                        }
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0xbU;
                    }
                } else if ((1U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__temp_acc 
                        = (0x1ffffffU & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_acc_r 
                                         + (((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_fh_r) 
                                             << 8U) 
                                            | (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_fl_r))));
                    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__curr_acc_mask_w 
                        = (0xffU | (0xffff00U & ((((IData)(1U) 
                                                   << 
                                                   (0x1fU 
                                                    & ((IData)(9U) 
                                                       + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_res_w)))) 
                                                  - (IData)(1U)) 
                                                 << 8U)));
                    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__high_bit_w 
                        = ((IData)(0x11U) + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_res_w));
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_we_r = 1U;
                    __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__overflow 
                        = ((0x18U >= (0x1fU & __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__high_bit_w)) 
                           && (1U & (__Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__temp_acc 
                                     >> (0x1fU & __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__high_bit_w))));
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_din_r 
                        = (__Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__temp_acc 
                           & __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__curr_acc_mask_w);
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0U;
                    if (__Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_acc__36__overflow) {
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0xaU;
                    }
                } else {
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r 
                        = (0xffffffU & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r 
                                        + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_output_r)));
                    if ((1U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_ca_w))) {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r 
                            = (0xffffffU & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r 
                                            + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_output_r)));
                    } else {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r 
                            = (0xffffffU & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r 
                                            + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_output_r)));
                    }
                    __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1 
                        = (0xffffffU & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_sum_r 
                                        + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_output_r)));
                    __VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_ca_w;
                    __VdlySet__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1 = 1U;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 9U;
                }
            } else if ((4U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                if ((2U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                    if ((1U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                        if (((2U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_mode_w)) 
                             & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r))) {
                            if (((0x1eU > (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r)) 
                                 & (~ (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_halt_w)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
                                    = (0x1fU & ((IData)(1U) 
                                                + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r)));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r = 1U;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_wds_r;
                            }
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 9U;
                        } else {
                            __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__data_w 
                                = (0x80U ^ (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_wds_r));
                            __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__vol_s 
                                = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_vol_r;
                            __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__output_w 
                                = (0xffffU & VL_MULS_III(16, 
                                                         (0xffffU 
                                                          & VL_EXTENDS_II(16,8, (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__data_w))), 
                                                         (0xffffU 
                                                          & VL_EXTENDS_II(16,9, (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__vol_s)))));
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_sum_r 
                                = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_channel_r
                                [vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_ca_w];
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 8U;
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_output_r 
                                = __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_out__38__unnamedblk2__DOT__output_w;
                        }
                    } else if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r) {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r = 0U;
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r = 1U;
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_din_r 
                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__loaded_wds_r;
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_wds_r 
                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__loaded_wds_r;
                        if ((0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__loaded_wds_r))) {
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__halt_zero_r = 1U;
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0xaU;
                        } else {
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 7U;
                        }
                    } else if ((0x28U < (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r))) {
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0U;
                    }
                } else if ((1U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__halt_w) {
                        if ((1U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_mode_w))) {
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_we_r = 1U;
                            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_din_r = 0U;
                        }
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0U;
                    } else {
                        __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__curr_wave_addr_w 
                            = (0xffffU & (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_acc_r 
                                          >> (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_shift_w)));
                        __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__ptr_hi_mask_w 
                            = (0xffU & ((IData)(0xffU) 
                                        << (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_wts_w)));
                        __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__ptr_w 
                            = (((IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__ptr_hi_mask_w) 
                                & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_wtp_r)) 
                               << 8U);
                        vlSelfRef.doc5503_verilog_test__DOT__wave_rd = 1U;
                        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 6U;
                        __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__addr_w 
                            = ((IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__curr_wave_addr_w) 
                               | (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__ptr_w));
                        vlSelfRef.doc5503_verilog_test__DOT__wave_address 
                            = __Vtask_doc5503_verilog_test__DOT__dut__DOT__osc_request_data__40__unnamedblk1__DOT__addr_w;
                    }
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__prev_control_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                } else {
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_control_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                        = (0x1fU & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r) 
                                    - (IData)(1U)));
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 5U;
                }
            } else if ((2U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                if ((1U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__partner_control_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                        = (0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r)));
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 4U;
                } else {
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_fl_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_fh_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_vol_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_wds_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_wtp_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_control_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_rts_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_acc_r 
                        = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_dout_w;
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                        = (1U ^ (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r));
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 3U;
                }
            } else if ((1U & (IData)(__Vtask_doc5503_verilog_test__DOT__dut__DOT__cycle_osc__27__osc_state_w))) {
                if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r) {
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0U;
                    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                        if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w;
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w;
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w;
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w;
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w;
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w;
                            }
                        } else {
                            vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                = ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                    ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_int_r)
                                    : ((0xe1U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                        ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r)
                                        : 0U));
                        }
                    }
                }
                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_wave_addr_r = 0U;
                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_output_r = 0U;
                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__halt_zero_r = 0U;
                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 2U;
            } else {
                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0U;
                if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r) {
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r = 0U;
                    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                        __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 1U;
                        if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                            }
                        }
                    } else {
                        vlSelfRef.doc5503_verilog_test__DOT__data_out 
                            = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                        if ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_int_r 
                                = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                        } else if ((0xe1U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_en_r 
                                = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                        } else if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_we_r = 1U;
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_we_r = 1U;
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r = 1U;
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r = 1U;
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_we_r = 1U;
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = 1U;
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r 
                                    = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r));
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_din_r 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r;
                                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_we_r = 1U;
                            }
                        }
                    }
                }
                if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r) {
                    __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0U;
                    if (vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r) {
                        if ((0xdfU >= (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))) {
                            if ((0U == (7U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                              >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_dout_w;
                            } else if ((1U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_dout_w;
                            } else if ((2U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_dout_w;
                            } else if ((3U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_dout_w;
                            } else if ((4U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_dout_w;
                            } else if ((5U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_dout_w;
                            } else if ((6U == (7U & 
                                               ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r) 
                                                >> 5U)))) {
                                vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                    = vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_dout_w;
                            }
                        } else {
                            vlSelfRef.doc5503_verilog_test__DOT__data_out 
                                = ((0xe0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                    ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_int_r)
                                    : ((0xe1U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r))
                                        ? (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r)
                                        : 0U));
                        }
                    }
                }
                if ((0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r))) {
                    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 1U;
                }
            }
        } else {
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fl_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r));
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_fh_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r));
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_vol_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r));
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wds_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r));
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_wtp_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r));
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_control_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r));
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_rts_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r));
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_osc_acc_r 
                = (0x1fU & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r));
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_we_r = 1U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_we_r = 1U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r = 1U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r = 1U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_we_r = 1U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = 1U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_we_r = 1U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_we_r = 1U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_din_r = 0U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_din_r = 0U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_din_r = 0U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_din_r = 0U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_din_r = 0U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_din_r = 0U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_din_r = 0U;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_din_r = 0U;
        }
        if (vlSelfRef.doc5503_verilog_test__DOT__wave_data_ready) {
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__loaded_wds_r 
                = vlSelfRef.doc5503_verilog_test__DOT__wave_data;
        }
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_access_r = 0U;
        if ((1U & (~ (IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n)))) {
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r 
                = vlSelfRef.doc5503_verilog_test__DOT__addr;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r 
                = vlSelfRef.doc5503_verilog_test__DOT__data_in;
            vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r 
                = vlSelfRef.doc5503_verilog_test__DOT__we_n;
            if (((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__prev_cs_n_r) 
                 | ((~ (IData)(vlSelfRef.doc5503_verilog_test__DOT__we_n)) 
                    & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__prev_we_n_r)))) {
                vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_access_r = 1U;
            }
        }
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__prev_we_n_r 
            = vlSelfRef.doc5503_verilog_test__DOT__we_n;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__prev_cs_n_r 
            = vlSelfRef.doc5503_verilog_test__DOT__cs_n;
    } else {
        __Vdly__doc5503_verilog_test__DOT__cycle_count = 0U;
        __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r = 0U;
        __Vdly__doc5503_verilog_test__DOT__dut__DOT__clk_count_r = 0U;
        __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_state_r = 0U;
        __Vdly__doc5503_verilog_test__DOT__dut__DOT__curr_osc_r = 0U;
        __Vdly__doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r = 0U;
        __Vdly__doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r = 0U;
        __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_state_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__wave_address = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__wave_rd = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__halt_zero_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fl_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_fh_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_vol_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wds_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_wtp_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_control_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_rts_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__target_acc_we_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__loaded_wds_r = 0x80U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_addr_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_data_r = 0U;
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_we_n_r = 1U;
    }
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__clk_count_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__clk_count_r;
    vlSelfRef.doc5503_verilog_test__DOT__cycle_count 
        = __Vdly__doc5503_verilog_test__DOT__cycle_count;
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__fl_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__fh_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__vol_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__wds_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__wtp_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__control_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__rts_ram__DOT__osc_reg_r__v0;
    }
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0] 
            = __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__acc_ram__DOT__osc_reg_r__v0;
    }
    vlSelfRef.doc5503_verilog_test__DOT__clk_en = (0U 
                                                   == 
                                                   (7U 
                                                    & vlSelfRef.doc5503_verilog_test__DOT__cycle_count));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__host_request_pending_r;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__device_response_pending_r;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__curr_osc_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__curr_osc_r;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__loaded_wds_pending_r;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_state_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_state_r;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__cycle_timer_r;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_mono_mix_r;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_left_mix_r;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__next_right_mix_r;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_int_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_int_r;
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r 
        = __Vdly__doc5503_verilog_test__DOT__dut__DOT__osc_en_r;
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v0) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_channel_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v0] = 0U;
    }
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_channel_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1] 
            = __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__next_channel_r__v1;
    }
    if (__VdlySet__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0) {
        vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__channel_r[__VdlyDim0__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0] 
            = __VdlyVal__doc5503_verilog_test__DOT__dut__DOT__channel_r__v0;
    }
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__prev_halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__prev_control_r));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_halt_w 
        = (1U & (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__next_control_r));
    vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__partner_mode_w 
        = (3U & ((IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__partner_control_r) 
                 >> 1U));
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
}

VL_INLINE_OPT void Vdoc5503_verilog_test___024root___nba_sequent__TOP__1(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___nba_sequent__TOP__1\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

void Vdoc5503_verilog_test___024root___timing_commit(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___timing_commit\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((! (2ULL & vlSelfRef.__VactTriggered.word(0U)))) {
        vlSelfRef.__VtrigSched_h49264d22__0.commit(
                                                   "@(posedge doc5503_verilog_test.clk)");
    }
}

void Vdoc5503_verilog_test___024root___timing_resume(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___timing_resume\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((2ULL & vlSelfRef.__VactTriggered.word(0U))) {
        vlSelfRef.__VtrigSched_h49264d22__0.resume(
                                                   "@(posedge doc5503_verilog_test.clk)");
    }
    if ((4ULL & vlSelfRef.__VactTriggered.word(0U))) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vdoc5503_verilog_test___024root___eval_triggers__act(Vdoc5503_verilog_test___024root* vlSelf);

bool Vdoc5503_verilog_test___024root___eval_phase__act(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_phase__act\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<3> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vdoc5503_verilog_test___024root___eval_triggers__act(vlSelf);
    Vdoc5503_verilog_test___024root___timing_commit(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vdoc5503_verilog_test___024root___timing_resume(vlSelf);
        Vdoc5503_verilog_test___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vdoc5503_verilog_test___024root___eval_phase__nba(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_phase__nba\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vdoc5503_verilog_test___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_verilog_test___024root___dump_triggers__ico(Vdoc5503_verilog_test___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_verilog_test___024root___dump_triggers__nba(Vdoc5503_verilog_test___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_verilog_test___024root___dump_triggers__act(Vdoc5503_verilog_test___024root* vlSelf);
#endif  // VL_DEBUG

void Vdoc5503_verilog_test___024root___eval(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vdoc5503_verilog_test___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../../../hdl/support/srff.v", 1, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vdoc5503_verilog_test___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vdoc5503_verilog_test___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../../../hdl/support/srff.v", 1, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vdoc5503_verilog_test___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../../../hdl/support/srff.v", 1, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vdoc5503_verilog_test___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vdoc5503_verilog_test___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vdoc5503_verilog_test___024root___eval_debug_assertions(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_debug_assertions\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.s & 0xfeU)))) {
        Verilated::overWidthError("s");}
    if (VL_UNLIKELY(((vlSelfRef.r & 0xfeU)))) {
        Verilated::overWidthError("r");}
}
#endif  // VL_DEBUG
