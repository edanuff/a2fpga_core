// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vdoc5503_harness__Syms.h"


void Vdoc5503_harness___024root__trace_chg_0_sub_0(Vdoc5503_harness___024root* vlSelf, VerilatedVcd::Buffer* bufp);

void Vdoc5503_harness___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root__trace_chg_0\n"); );
    // Init
    Vdoc5503_harness___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vdoc5503_harness___024root*>(voidSelf);
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    // Body
    Vdoc5503_harness___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vdoc5503_harness___024root__trace_chg_0_sub_0(Vdoc5503_harness___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root__trace_chg_0_sub_0\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 1);
    // Body
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[0U]))) {
        bufp->chgBit(oldp+0,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_read_req_i[0]));
        bufp->chgCData(oldp+1,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_read_addr_i[0]),5);
        bufp->chgIData(oldp+2,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_read_data_o[0]),24);
        bufp->chgBit(oldp+3,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_read_req_i[0]));
        bufp->chgCData(oldp+4,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_read_addr_i[0]),5);
        bufp->chgSData(oldp+5,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_read_data_o[0]),16);
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[1U]))) {
        bufp->chgBit(oldp+6,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_req_i[0]));
        bufp->chgCData(oldp+7,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+8,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_data_i[0]),8);
        bufp->chgBit(oldp+9,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_req_i[0]));
        bufp->chgCData(oldp+10,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_addr_i[0]),5);
        bufp->chgBit(oldp+11,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_req_i
                              [0U]));
        bufp->chgBit(oldp+12,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_req_i
                              [0U]));
        bufp->chgBit(oldp+13,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_req_i[0]));
        bufp->chgCData(oldp+14,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+15,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_data_i[0]),8);
        bufp->chgBit(oldp+16,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_req_i[0]));
        bufp->chgCData(oldp+17,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_addr_i[0]),5);
        bufp->chgBit(oldp+18,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_req_i
                              [0U]));
        bufp->chgBit(oldp+19,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_req_i
                              [0U]));
        bufp->chgBit(oldp+20,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_req_i[0]));
        bufp->chgCData(oldp+21,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+22,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_data_i[0]),8);
        bufp->chgBit(oldp+23,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_req_i[0]));
        bufp->chgCData(oldp+24,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_addr_i[0]),5);
        bufp->chgBit(oldp+25,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_req_i
                              [0U]));
        bufp->chgBit(oldp+26,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_req_i
                              [0U]));
        bufp->chgBit(oldp+27,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_req_i[0]));
        bufp->chgCData(oldp+28,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_addr_i[0]),5);
        bufp->chgBit(oldp+29,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_req_i
                              [0U]));
        bufp->chgCData(oldp+30,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i[0]),8);
        bufp->chgCData(oldp+31,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i[1]),8);
        bufp->chgCData(oldp+32,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i[2]),8);
        bufp->chgCData(oldp+33,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i[3]),8);
        bufp->chgBit(oldp+34,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_req_i[0]));
        bufp->chgCData(oldp+35,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_addr_i[0]),5);
        bufp->chgBit(oldp+36,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_req_i
                              [0U]));
        bufp->chgBit(oldp+37,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_req_i[0]));
        bufp->chgCData(oldp+38,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+39,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_data_i[0]),8);
        bufp->chgBit(oldp+40,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_req_i[0]));
        bufp->chgCData(oldp+41,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_addr_i[0]),5);
        bufp->chgBit(oldp+42,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_req_i
                              [0U]));
        bufp->chgBit(oldp+43,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_req_i
                              [0U]));
    }
    if (VL_UNLIKELY(((vlSelfRef.__Vm_traceActivity[1U] 
                      | vlSelfRef.__Vm_traceActivity
                      [2U])))) {
        bufp->chgBit(oldp+44,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__req_cs_w));
        bufp->chgBit(oldp+45,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[0]));
        bufp->chgBit(oldp+46,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[1]));
        bufp->chgBit(oldp+47,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[2]));
        bufp->chgBit(oldp+48,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[3]));
        bufp->chgCData(oldp+49,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+50,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[1]),5);
        bufp->chgCData(oldp+51,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[2]),5);
        bufp->chgCData(oldp+52,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[3]),5);
        bufp->chgCData(oldp+53,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[0]),8);
        bufp->chgCData(oldp+54,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[1]),8);
        bufp->chgCData(oldp+55,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[2]),8);
        bufp->chgCData(oldp+56,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[3]),8);
        bufp->chgBit(oldp+57,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i[0]));
        bufp->chgBit(oldp+58,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i[1]));
        bufp->chgBit(oldp+59,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i[2]));
        bufp->chgCData(oldp+60,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i[0]),5);
        bufp->chgCData(oldp+61,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i[1]),5);
        bufp->chgCData(oldp+62,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i[2]),5);
        bufp->chgBit(oldp+63,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+64,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[1]));
        bufp->chgBit(oldp+65,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[2]));
        bufp->chgBit(oldp+66,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[3]));
        bufp->chgBit(oldp+67,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[0]));
        bufp->chgBit(oldp+68,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[1]));
        bufp->chgBit(oldp+69,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[2]));
        bufp->chgBit(oldp+70,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+71,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+72,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+73,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r));
        bufp->chgIData(oldp+74,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index),32);
        bufp->chgCData(oldp+75,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r),5);
        bufp->chgBit(oldp+76,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
                              [0U]));
        bufp->chgBit(oldp+77,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                                & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                               | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
        bufp->chgBit(oldp+78,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+79,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
                              [1U]));
        bufp->chgBit(oldp+80,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_r_w)) 
                                & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_s_w)) 
                               | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__q_r))));
        bufp->chgBit(oldp+81,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+82,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
                              [2U]));
        bufp->chgBit(oldp+83,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_r_w)) 
                                & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_s_w)) 
                               | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__q_r))));
        bufp->chgBit(oldp+84,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+85,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found));
        bufp->chgBit(oldp+86,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+87,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en));
        bufp->chgBit(oldp+88,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
                              [1U]));
        bufp->chgBit(oldp+89,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                               | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+90,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+91,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
                              [2U]));
        bufp->chgBit(oldp+92,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                               | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+93,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+94,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
                              [3U]));
        bufp->chgBit(oldp+95,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                               | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+96,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+97,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+98,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_w[0]));
        bufp->chgBit(oldp+99,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+100,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+101,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+102,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_en_r));
        bufp->chgIData(oldp+103,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_index),32);
        bufp->chgCData(oldp+104,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_addr_r),5);
        bufp->chgBit(oldp+105,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
        bufp->chgBit(oldp+106,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+107,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ctrl__DOT__found));
        bufp->chgBit(oldp+108,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+109,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+110,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+111,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+112,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_w[0]));
        bufp->chgBit(oldp+113,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+114,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+115,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+116,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_en_r));
        bufp->chgIData(oldp+117,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_index),32);
        bufp->chgCData(oldp+118,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_addr_r),5);
        bufp->chgBit(oldp+119,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
        bufp->chgBit(oldp+120,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+121,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ctrl__DOT__found));
        bufp->chgBit(oldp+122,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+123,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+124,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+125,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+126,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_w[0]));
        bufp->chgBit(oldp+127,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+128,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+129,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+130,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_en_r));
        bufp->chgIData(oldp+131,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_index),32);
        bufp->chgCData(oldp+132,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_addr_r),5);
        bufp->chgBit(oldp+133,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
        bufp->chgBit(oldp+134,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+135,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ctrl__DOT__found));
        bufp->chgBit(oldp+136,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+137,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+138,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+139,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i[0]));
        bufp->chgBit(oldp+140,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i[1]));
        bufp->chgCData(oldp+141,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+142,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_addr_i[1]),5);
        bufp->chgCData(oldp+143,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_data_i[0]),8);
        bufp->chgCData(oldp+144,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_data_i[1]),8);
        bufp->chgBit(oldp+145,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+146,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w[1]));
        bufp->chgBit(oldp+147,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_w[0]));
        bufp->chgBit(oldp+148,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+149,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+150,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+151,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_en_r));
        bufp->chgIData(oldp+152,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_index),32);
        bufp->chgCData(oldp+153,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_addr_r),5);
        bufp->chgBit(oldp+154,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
        bufp->chgBit(oldp+155,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+156,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ctrl__DOT__found));
        bufp->chgBit(oldp+157,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+158,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en));
        bufp->chgBit(oldp+159,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i
                               [0U]));
        bufp->chgBit(oldp+160,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+161,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+162,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i
                               [1U]));
        bufp->chgBit(oldp+163,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+164,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+165,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[0]));
        bufp->chgBit(oldp+166,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[1]));
        bufp->chgBit(oldp+167,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[2]));
        bufp->chgBit(oldp+168,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[3]));
        bufp->chgCData(oldp+169,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+170,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[1]),5);
        bufp->chgCData(oldp+171,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[2]),5);
        bufp->chgCData(oldp+172,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[3]),5);
        bufp->chgBit(oldp+173,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+174,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[1]));
        bufp->chgBit(oldp+175,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[2]));
        bufp->chgBit(oldp+176,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[3]));
        bufp->chgBit(oldp+177,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_w[0]));
        bufp->chgBit(oldp+178,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+179,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+180,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+181,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_en_r));
        bufp->chgIData(oldp+182,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_index),32);
        bufp->chgCData(oldp+183,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_addr_r),5);
        bufp->chgBit(oldp+184,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
        bufp->chgBit(oldp+185,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+186,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ctrl__DOT__found));
        bufp->chgBit(oldp+187,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+188,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en));
        bufp->chgBit(oldp+189,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
                               [1U]));
        bufp->chgBit(oldp+190,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+191,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+192,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
                               [2U]));
        bufp->chgBit(oldp+193,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+194,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+195,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
                               [3U]));
        bufp->chgBit(oldp+196,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+197,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+198,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+199,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_w[0]));
        bufp->chgBit(oldp+200,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+201,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+202,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+203,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_en_r));
        bufp->chgIData(oldp+204,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_index),32);
        bufp->chgCData(oldp+205,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_addr_r),5);
        bufp->chgBit(oldp+206,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
        bufp->chgBit(oldp+207,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+208,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ctrl__DOT__found));
        bufp->chgBit(oldp+209,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+210,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+211,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[2U]))) {
        bufp->chgCData(oldp+212,(vlSelfRef.doc5503_harness__DOT__dbg_cycle),5);
        bufp->chgCData(oldp+213,(vlSelfRef.doc5503_harness__DOT__dbg_cycle_max),5);
        bufp->chgCData(oldp+214,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r),8);
        bufp->chgCData(oldp+215,(vlSelfRef.doc5503_harness__DOT__dbg_osc_state),3);
        bufp->chgBit(oldp+216,(vlSelfRef.doc5503_harness__DOT__dbg_ready));
        bufp->chgSData(oldp+217,(vlSelfRef.doc5503_harness__DOT__dbg_output),16);
        bufp->chgCData(oldp+218,(vlSelfRef.doc5503_harness__DOT__dbg_wds),8);
        bufp->chgCData(oldp+219,(vlSelfRef.doc5503_harness__DOT__dbg_vol),8);
        bufp->chgCData(oldp+220,(vlSelfRef.doc5503_harness__DOT__dbg_control),8);
        bufp->chgIData(oldp+221,(vlSelfRef.doc5503_harness__DOT__dbg_acc),24);
        bufp->chgSData(oldp+222,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o),16);
        bufp->chgBit(oldp+223,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o));
        bufp->chgSData(oldp+224,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_1_r),16);
        bufp->chgSData(oldp+225,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0]),16);
        bufp->chgSData(oldp+226,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[1]),16);
        bufp->chgSData(oldp+227,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[2]),16);
        bufp->chgSData(oldp+228,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[3]),16);
        bufp->chgSData(oldp+229,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[4]),16);
        bufp->chgSData(oldp+230,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[5]),16);
        bufp->chgSData(oldp+231,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[6]),16);
        bufp->chgSData(oldp+232,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[7]),16);
        bufp->chgSData(oldp+233,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[8]),16);
        bufp->chgSData(oldp+234,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[9]),16);
        bufp->chgSData(oldp+235,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[10]),16);
        bufp->chgSData(oldp+236,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[11]),16);
        bufp->chgSData(oldp+237,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[12]),16);
        bufp->chgSData(oldp+238,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[13]),16);
        bufp->chgSData(oldp+239,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[14]),16);
        bufp->chgSData(oldp+240,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[15]),16);
        bufp->chgCData(oldp+241,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r),8);
        bufp->chgSData(oldp+242,((((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_a_r) 
                                   << 8U) | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_a_r))),16);
        bufp->chgCData(oldp+243,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_a_r),8);
        bufp->chgCData(oldp+244,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_a_r),8);
        bufp->chgCData(oldp+245,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_a_r),8);
        bufp->chgCData(oldp+246,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r),8);
        bufp->chgCData(oldp+247,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r),8);
        bufp->chgCData(oldp+248,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                 [1U]),8);
        bufp->chgCData(oldp+249,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                 [2U]),8);
        bufp->chgCData(oldp+250,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+251,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+252,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+253,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+254,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+255,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+256,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgBit(oldp+257,((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r))));
        bufp->chgBit(oldp+258,((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)))));
        bufp->chgCData(oldp+259,((1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r))),5);
        bufp->chgCData(oldp+260,((7U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r) 
                                        >> 3U))),3);
        bufp->chgCData(oldp+261,((7U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r))),3);
        bufp->chgBit(oldp+262,((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r))));
        bufp->chgCData(oldp+263,((3U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r) 
                                        >> 1U))),2);
        bufp->chgBit(oldp+264,((1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                [1U])));
        bufp->chgCData(oldp+265,((3U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                        [1U] >> 1U))),2);
        bufp->chgBit(oldp+266,((1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                [2U])));
        bufp->chgCData(oldp+267,((3U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                        [2U] >> 1U))),2);
        bufp->chgIData(oldp+268,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_a_r),24);
        bufp->chgSData(oldp+269,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_addr_r),16);
        bufp->chgCData(oldp+270,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r),5);
        bufp->chgCData(oldp+271,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r),5);
        bufp->chgCData(oldp+272,((0x1fU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r) 
                                           >> 1U))),5);
        bufp->chgCData(oldp+273,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r),6);
        bufp->chgCData(oldp+274,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_b_r),8);
        bufp->chgSData(oldp+275,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_b_r),16);
        bufp->chgCData(oldp+276,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r),5);
        bufp->chgBit(oldp+277,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r));
        bufp->chgCData(oldp+278,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r),5);
        bufp->chgBit(oldp+279,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_w));
        bufp->chgBit(oldp+280,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_step_w));
        bufp->chgBit(oldp+281,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r));
        bufp->chgBit(oldp+282,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_am_req_r));
        bufp->chgBit(oldp+283,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_wds_reset_req_r));
        bufp->chgBit(oldp+284,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_osc_halt_req_r));
        bufp->chgBit(oldp+285,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_unhalt_req_r));
        bufp->chgBit(oldp+286,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_load_req_r));
        bufp->chgBit(oldp+287,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_load_req_r));
        bufp->chgBit(oldp+288,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_add_req_r));
        bufp->chgBit(oldp+289,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_reset_req_r));
        bufp->chgBit(oldp+290,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_acc_reset_req_r));
        bufp->chgSData(oldp+291,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_r),16);
        bufp->chgBit(oldp+292,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_reset_req));
        bufp->chgBit(oldp+293,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_update_req));
        bufp->chgBit(oldp+294,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__prev_cs_n_r));
        bufp->chgCData(oldp+295,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r),3);
        bufp->chgSData(oldp+296,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r),16);
        bufp->chgSData(oldp+297,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r),16);
        bufp->chgSData(oldp+298,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r),16);
        bufp->chgSData(oldp+299,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r),16);
        bufp->chgSData(oldp+300,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r),16);
        bufp->chgSData(oldp+301,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r),16);
        bufp->chgSData(oldp+302,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[0]),16);
        bufp->chgSData(oldp+303,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[1]),16);
        bufp->chgSData(oldp+304,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[2]),16);
        bufp->chgSData(oldp+305,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[3]),16);
        bufp->chgSData(oldp+306,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[4]),16);
        bufp->chgSData(oldp+307,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[5]),16);
        bufp->chgSData(oldp+308,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[6]),16);
        bufp->chgSData(oldp+309,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[7]),16);
        bufp->chgSData(oldp+310,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[8]),16);
        bufp->chgSData(oldp+311,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[9]),16);
        bufp->chgSData(oldp+312,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[10]),16);
        bufp->chgSData(oldp+313,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[11]),16);
        bufp->chgSData(oldp+314,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[12]),16);
        bufp->chgSData(oldp+315,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[13]),16);
        bufp->chgSData(oldp+316,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[14]),16);
        bufp->chgSData(oldp+317,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[15]),16);
        bufp->chgSData(oldp+318,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[0]),16);
        bufp->chgSData(oldp+319,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[1]),16);
        bufp->chgSData(oldp+320,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[2]),16);
        bufp->chgSData(oldp+321,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[3]),16);
        bufp->chgSData(oldp+322,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[4]),16);
        bufp->chgSData(oldp+323,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[5]),16);
        bufp->chgSData(oldp+324,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[6]),16);
        bufp->chgSData(oldp+325,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[7]),16);
        bufp->chgSData(oldp+326,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[8]),16);
        bufp->chgSData(oldp+327,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[9]),16);
        bufp->chgSData(oldp+328,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[10]),16);
        bufp->chgSData(oldp+329,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[11]),16);
        bufp->chgSData(oldp+330,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[12]),16);
        bufp->chgSData(oldp+331,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[13]),16);
        bufp->chgSData(oldp+332,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[14]),16);
        bufp->chgSData(oldp+333,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[15]),16);
        bufp->chgCData(oldp+334,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r),2);
        bufp->chgCData(oldp+335,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r),4);
        bufp->chgBit(oldp+336,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[0]));
        bufp->chgBit(oldp+337,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[1]));
        bufp->chgBit(oldp+338,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[2]));
        bufp->chgCData(oldp+339,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+340,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[1]),5);
        bufp->chgCData(oldp+341,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[2]),5);
        bufp->chgIData(oldp+342,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[0]),24);
        bufp->chgIData(oldp+343,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[1]),24);
        bufp->chgIData(oldp+344,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[2]),24);
        bufp->chgBit(oldp+345,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+346,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[1]));
        bufp->chgBit(oldp+347,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[2]));
        bufp->chgBit(oldp+348,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+349,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[1]));
        bufp->chgBit(oldp+350,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[2]));
        bufp->chgIData(oldp+351,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[0]),24);
        bufp->chgIData(oldp+352,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[1]),24);
        bufp->chgIData(oldp+353,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[2]),24);
        bufp->chgIData(oldp+354,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[3]),24);
        bufp->chgIData(oldp+355,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[4]),24);
        bufp->chgIData(oldp+356,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[5]),24);
        bufp->chgIData(oldp+357,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[6]),24);
        bufp->chgIData(oldp+358,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[7]),24);
        bufp->chgIData(oldp+359,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[8]),24);
        bufp->chgIData(oldp+360,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[9]),24);
        bufp->chgIData(oldp+361,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[10]),24);
        bufp->chgIData(oldp+362,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[11]),24);
        bufp->chgIData(oldp+363,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[12]),24);
        bufp->chgIData(oldp+364,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[13]),24);
        bufp->chgIData(oldp+365,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[14]),24);
        bufp->chgIData(oldp+366,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[15]),24);
        bufp->chgIData(oldp+367,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[16]),24);
        bufp->chgIData(oldp+368,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[17]),24);
        bufp->chgIData(oldp+369,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[18]),24);
        bufp->chgIData(oldp+370,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[19]),24);
        bufp->chgIData(oldp+371,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[20]),24);
        bufp->chgIData(oldp+372,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[21]),24);
        bufp->chgIData(oldp+373,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[22]),24);
        bufp->chgIData(oldp+374,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[23]),24);
        bufp->chgIData(oldp+375,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[24]),24);
        bufp->chgIData(oldp+376,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[25]),24);
        bufp->chgIData(oldp+377,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[26]),24);
        bufp->chgIData(oldp+378,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[27]),24);
        bufp->chgIData(oldp+379,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[28]),24);
        bufp->chgIData(oldp+380,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[29]),24);
        bufp->chgIData(oldp+381,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[30]),24);
        bufp->chgIData(oldp+382,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[31]),24);
        bufp->chgBit(oldp+383,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+384,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+385,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+386,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+387,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                               [0U]));
        bufp->chgBit(oldp+388,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+389,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+390,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+391,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+392,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+393,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+394,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+395,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                               [1U]));
        bufp->chgBit(oldp+396,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                               [1U]));
        bufp->chgBit(oldp+397,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+398,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+399,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+400,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+401,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+402,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+403,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                               [2U]));
        bufp->chgBit(oldp+404,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                               [2U]));
        bufp->chgBit(oldp+405,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+406,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+407,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+408,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+409,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+410,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+411,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+412,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+413,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgIData(oldp+414,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__data_curr_r),24);
        bufp->chgCData(oldp+415,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[0]),8);
        bufp->chgCData(oldp+416,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[1]),8);
        bufp->chgCData(oldp+417,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[2]),8);
        bufp->chgCData(oldp+418,((0x1fU & ((IData)(1U) 
                                           + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r)))),5);
        bufp->chgBit(oldp+419,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+420,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[1]));
        bufp->chgBit(oldp+421,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[2]));
        bufp->chgBit(oldp+422,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[3]));
        bufp->chgBit(oldp+423,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgBit(oldp+424,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[1]));
        bufp->chgBit(oldp+425,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[2]));
        bufp->chgCData(oldp+426,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+427,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[1]),8);
        bufp->chgCData(oldp+428,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[2]),8);
        bufp->chgCData(oldp+429,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+430,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+431,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+432,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+433,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+434,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+435,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+436,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+437,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+438,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+439,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+440,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+441,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+442,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+443,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+444,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+445,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+446,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+447,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+448,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+449,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+450,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+451,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+452,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+453,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+454,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+455,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+456,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+457,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+458,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+459,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+460,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+461,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+462,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+463,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+464,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+465,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+466,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                               [1U]));
        bufp->chgBit(oldp+467,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+468,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+469,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+470,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+471,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                               [2U]));
        bufp->chgBit(oldp+472,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+473,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+474,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+475,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+476,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                               [1U]));
        bufp->chgBit(oldp+477,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+478,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+479,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+480,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+481,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                               [2U]));
        bufp->chgBit(oldp+482,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+483,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+484,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+485,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+486,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                               [3U]));
        bufp->chgBit(oldp+487,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+488,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+489,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+490,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+491,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+492,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+493,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+494,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+495,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o[0]),8);
        bufp->chgCData(oldp+496,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_a_r),8);
        bufp->chgBit(oldp+497,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+498,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+499,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+500,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+501,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+502,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+503,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+504,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+505,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+506,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+507,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+508,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+509,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+510,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+511,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+512,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+513,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+514,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+515,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+516,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+517,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+518,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+519,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+520,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+521,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+522,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+523,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+524,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+525,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+526,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+527,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+528,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+529,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+530,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+531,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+532,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+533,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+534,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+535,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+536,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+537,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+538,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+539,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+540,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+541,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+542,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+543,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+544,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+545,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+546,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o[0]),8);
        bufp->chgCData(oldp+547,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_a_r),8);
        bufp->chgBit(oldp+548,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+549,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+550,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+551,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+552,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+553,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+554,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+555,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+556,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+557,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+558,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+559,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+560,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+561,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+562,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+563,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+564,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+565,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+566,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+567,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+568,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+569,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+570,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+571,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+572,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+573,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+574,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+575,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+576,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+577,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+578,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+579,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+580,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+581,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+582,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+583,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+584,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+585,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+586,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+587,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+588,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+589,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+590,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+591,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+592,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+593,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+594,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+595,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+596,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgBit(oldp+597,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[0]));
        bufp->chgBit(oldp+598,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[1]));
        bufp->chgBit(oldp+599,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[2]));
        bufp->chgCData(oldp+600,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+601,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[1]),5);
        bufp->chgCData(oldp+602,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[2]),5);
        bufp->chgSData(oldp+603,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[0]),16);
        bufp->chgSData(oldp+604,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[1]),16);
        bufp->chgSData(oldp+605,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[2]),16);
        bufp->chgBit(oldp+606,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+607,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[1]));
        bufp->chgBit(oldp+608,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[2]));
        bufp->chgBit(oldp+609,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+610,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[1]));
        bufp->chgBit(oldp+611,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[2]));
        bufp->chgSData(oldp+612,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[0]),16);
        bufp->chgSData(oldp+613,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[1]),16);
        bufp->chgSData(oldp+614,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[2]),16);
        bufp->chgSData(oldp+615,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[3]),16);
        bufp->chgSData(oldp+616,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[4]),16);
        bufp->chgSData(oldp+617,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[5]),16);
        bufp->chgSData(oldp+618,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[6]),16);
        bufp->chgSData(oldp+619,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[7]),16);
        bufp->chgSData(oldp+620,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[8]),16);
        bufp->chgSData(oldp+621,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[9]),16);
        bufp->chgSData(oldp+622,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[10]),16);
        bufp->chgSData(oldp+623,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[11]),16);
        bufp->chgSData(oldp+624,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[12]),16);
        bufp->chgSData(oldp+625,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[13]),16);
        bufp->chgSData(oldp+626,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[14]),16);
        bufp->chgSData(oldp+627,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[15]),16);
        bufp->chgSData(oldp+628,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[16]),16);
        bufp->chgSData(oldp+629,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[17]),16);
        bufp->chgSData(oldp+630,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[18]),16);
        bufp->chgSData(oldp+631,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[19]),16);
        bufp->chgSData(oldp+632,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[20]),16);
        bufp->chgSData(oldp+633,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[21]),16);
        bufp->chgSData(oldp+634,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[22]),16);
        bufp->chgSData(oldp+635,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[23]),16);
        bufp->chgSData(oldp+636,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[24]),16);
        bufp->chgSData(oldp+637,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[25]),16);
        bufp->chgSData(oldp+638,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[26]),16);
        bufp->chgSData(oldp+639,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[27]),16);
        bufp->chgSData(oldp+640,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[28]),16);
        bufp->chgSData(oldp+641,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[29]),16);
        bufp->chgSData(oldp+642,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[30]),16);
        bufp->chgSData(oldp+643,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[31]),16);
        bufp->chgBit(oldp+644,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+645,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+646,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+647,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+648,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
                               [1U]));
        bufp->chgBit(oldp+649,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
                               [1U]));
        bufp->chgBit(oldp+650,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+651,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+652,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+653,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+654,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+655,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+656,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
                               [2U]));
        bufp->chgBit(oldp+657,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
                               [2U]));
        bufp->chgBit(oldp+658,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+659,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+660,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+661,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+662,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+663,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+664,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+665,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgCData(oldp+666,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o[0]),8);
        bufp->chgBit(oldp+667,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+668,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+669,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+670,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+671,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+672,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+673,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+674,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+675,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+676,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+677,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+678,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+679,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+680,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+681,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+682,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+683,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+684,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+685,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+686,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+687,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+688,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+689,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+690,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+691,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+692,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+693,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+694,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+695,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+696,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+697,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+698,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+699,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+700,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+701,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+702,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+703,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+704,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+705,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+706,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+707,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+708,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+709,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+710,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+711,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+712,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+713,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+714,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+715,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+716,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o[0]),8);
        bufp->chgBit(oldp+717,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+718,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[1]));
        bufp->chgBit(oldp+719,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+720,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+721,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+722,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+723,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+724,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+725,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+726,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+727,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+728,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+729,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+730,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+731,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+732,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+733,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+734,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+735,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+736,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+737,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+738,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+739,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+740,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+741,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+742,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+743,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+744,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+745,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+746,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+747,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+748,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+749,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+750,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+751,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+752,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+753,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+754,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+755,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+756,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+757,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+758,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+759,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+760,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+761,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+762,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+763,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
                               [1U]));
        bufp->chgBit(oldp+764,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+765,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+766,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+767,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+768,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+769,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+770,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+771,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+772,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o[0]),8);
        bufp->chgBit(oldp+773,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+774,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[1]));
        bufp->chgBit(oldp+775,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[2]));
        bufp->chgBit(oldp+776,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[3]));
        bufp->chgBit(oldp+777,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+778,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+779,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+780,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+781,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+782,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+783,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+784,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+785,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+786,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+787,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+788,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+789,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+790,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+791,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+792,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+793,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+794,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+795,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+796,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+797,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+798,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+799,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+800,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+801,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+802,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+803,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+804,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+805,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+806,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+807,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+808,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+809,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+810,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+811,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+812,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+813,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+814,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+815,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+816,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                               [1U]));
        bufp->chgBit(oldp+817,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+818,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+819,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+820,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+821,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                               [2U]));
        bufp->chgBit(oldp+822,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+823,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+824,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+825,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+826,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                               [3U]));
        bufp->chgBit(oldp+827,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+828,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+829,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+830,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+831,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+832,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+833,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+834,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+835,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o[0]),8);
        bufp->chgBit(oldp+836,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+837,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+838,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+839,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+840,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+841,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+842,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+843,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+844,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+845,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+846,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+847,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+848,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+849,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+850,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+851,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+852,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+853,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+854,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+855,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+856,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+857,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+858,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+859,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+860,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+861,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+862,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+863,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+864,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+865,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+866,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+867,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+868,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+869,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+870,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+871,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+872,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+873,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+874,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+875,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+876,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+877,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+878,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+879,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+880,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+881,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+882,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+883,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+884,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgSData(oldp+885,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__data_w),9);
        bufp->chgSData(oldp+886,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__vol_s),9);
        bufp->chgIData(oldp+887,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__mult_result),18);
        bufp->chgCData(oldp+888,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk2__DOT__shift_w),5);
        bufp->chgIData(oldp+889,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__high_bit_w),32);
        bufp->chgBit(oldp+890,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__overflow));
        bufp->chgBit(oldp+891,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__zero_byte_w));
        bufp->chgCData(oldp+892,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_hi_mask_w),8);
        bufp->chgSData(oldp+893,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_w),16);
        bufp->chgSData(oldp+894,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__addr_w),16);
        bufp->chgCData(oldp+895,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk5__DOT__ca),4);
    }
    bufp->chgBit(oldp+896,(vlSelfRef.clk_i));
    bufp->chgBit(oldp+897,(vlSelfRef.reset_n_i));
    bufp->chgBit(oldp+898,(vlSelfRef.clk_en_i));
    bufp->chgBit(oldp+899,(vlSelfRef.irq_n_o));
    bufp->chgBit(oldp+900,(vlSelfRef.cs_n_i));
    bufp->chgBit(oldp+901,(vlSelfRef.we_n_i));
    bufp->chgCData(oldp+902,(vlSelfRef.addr_i),8);
    bufp->chgCData(oldp+903,(vlSelfRef.data_i),8);
    bufp->chgCData(oldp+904,(vlSelfRef.data_o),8);
    bufp->chgSData(oldp+905,(vlSelfRef.wave_address_o),16);
    bufp->chgBit(oldp+906,(vlSelfRef.wave_rd_o));
    bufp->chgBit(oldp+907,(vlSelfRef.wave_data_ready_i));
    bufp->chgCData(oldp+908,(vlSelfRef.wave_data_i),8);
    bufp->chgSData(oldp+909,(vlSelfRef.mono_mix_o),16);
    bufp->chgSData(oldp+910,(vlSelfRef.left_mix_o),16);
    bufp->chgSData(oldp+911,(vlSelfRef.right_mix_o),16);
}

void Vdoc5503_harness___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root__trace_cleanup\n"); );
    // Init
    Vdoc5503_harness___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vdoc5503_harness___024root*>(voidSelf);
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[2U] = 0U;
}
