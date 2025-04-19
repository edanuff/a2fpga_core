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
        bufp->chgSData(oldp+212,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0]),16);
        bufp->chgSData(oldp+213,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[1]),16);
        bufp->chgSData(oldp+214,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[2]),16);
        bufp->chgSData(oldp+215,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[3]),16);
        bufp->chgSData(oldp+216,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[4]),16);
        bufp->chgSData(oldp+217,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[5]),16);
        bufp->chgSData(oldp+218,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[6]),16);
        bufp->chgSData(oldp+219,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[7]),16);
        bufp->chgSData(oldp+220,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[8]),16);
        bufp->chgSData(oldp+221,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[9]),16);
        bufp->chgSData(oldp+222,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[10]),16);
        bufp->chgSData(oldp+223,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[11]),16);
        bufp->chgSData(oldp+224,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[12]),16);
        bufp->chgSData(oldp+225,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[13]),16);
        bufp->chgSData(oldp+226,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[14]),16);
        bufp->chgSData(oldp+227,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[15]),16);
        bufp->chgCData(oldp+228,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r),8);
        bufp->chgCData(oldp+229,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r),8);
        bufp->chgSData(oldp+230,((((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_a_r) 
                                   << 8U) | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_a_r))),16);
        bufp->chgCData(oldp+231,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_a_r),8);
        bufp->chgCData(oldp+232,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_a_r),8);
        bufp->chgCData(oldp+233,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_a_r),8);
        bufp->chgCData(oldp+234,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r),8);
        bufp->chgCData(oldp+235,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r),8);
        bufp->chgCData(oldp+236,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                 [1U]),8);
        bufp->chgCData(oldp+237,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                 [2U]),8);
        bufp->chgCData(oldp+238,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+239,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+240,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+241,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+242,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+243,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgCData(oldp+244,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o
                                 [0U]),8);
        bufp->chgBit(oldp+245,((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r))));
        bufp->chgBit(oldp+246,((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)))));
        bufp->chgCData(oldp+247,((1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r))),5);
        bufp->chgCData(oldp+248,((7U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r) 
                                        >> 3U))),3);
        bufp->chgCData(oldp+249,((7U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r))),3);
        bufp->chgBit(oldp+250,((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r))));
        bufp->chgCData(oldp+251,((3U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r) 
                                        >> 1U))),2);
        bufp->chgBit(oldp+252,((1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                [1U])));
        bufp->chgCData(oldp+253,((3U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                        [1U] >> 1U))),2);
        bufp->chgBit(oldp+254,((1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                [2U])));
        bufp->chgCData(oldp+255,((3U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                        [2U] >> 1U))),2);
        bufp->chgIData(oldp+256,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_a_r),24);
        bufp->chgSData(oldp+257,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_addr_r),16);
        bufp->chgCData(oldp+258,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r),5);
        bufp->chgCData(oldp+259,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r),5);
        bufp->chgCData(oldp+260,((0x1fU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r) 
                                           >> 1U))),5);
        bufp->chgCData(oldp+261,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_max_w),5);
        bufp->chgCData(oldp+262,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r),6);
        bufp->chgCData(oldp+263,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_b_r),8);
        bufp->chgSData(oldp+264,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_b_r),16);
        bufp->chgCData(oldp+265,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r),5);
        bufp->chgBit(oldp+266,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r));
        bufp->chgCData(oldp+267,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r),5);
        bufp->chgBit(oldp+268,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_w));
        bufp->chgBit(oldp+269,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_step_w));
        bufp->chgBit(oldp+270,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r));
        bufp->chgBit(oldp+271,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_am_req_r));
        bufp->chgBit(oldp+272,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_wds_reset_req_r));
        bufp->chgBit(oldp+273,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_osc_halt_req_r));
        bufp->chgBit(oldp+274,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_unhalt_req_r));
        bufp->chgBit(oldp+275,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_load_req_r));
        bufp->chgBit(oldp+276,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_load_req_r));
        bufp->chgBit(oldp+277,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_add_req_r));
        bufp->chgBit(oldp+278,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_reset_req_r));
        bufp->chgBit(oldp+279,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_acc_reset_req_r));
        bufp->chgSData(oldp+280,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_r),16);
        bufp->chgBit(oldp+281,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_reset_req));
        bufp->chgBit(oldp+282,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_update_req));
        bufp->chgBit(oldp+283,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__prev_cs_n_r));
        bufp->chgCData(oldp+284,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r),3);
        bufp->chgSData(oldp+285,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r),16);
        bufp->chgSData(oldp+286,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r),16);
        bufp->chgSData(oldp+287,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r),16);
        bufp->chgSData(oldp+288,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r),16);
        bufp->chgSData(oldp+289,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r),16);
        bufp->chgSData(oldp+290,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r),16);
        bufp->chgSData(oldp+291,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[0]),16);
        bufp->chgSData(oldp+292,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[1]),16);
        bufp->chgSData(oldp+293,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[2]),16);
        bufp->chgSData(oldp+294,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[3]),16);
        bufp->chgSData(oldp+295,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[4]),16);
        bufp->chgSData(oldp+296,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[5]),16);
        bufp->chgSData(oldp+297,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[6]),16);
        bufp->chgSData(oldp+298,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[7]),16);
        bufp->chgSData(oldp+299,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[8]),16);
        bufp->chgSData(oldp+300,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[9]),16);
        bufp->chgSData(oldp+301,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[10]),16);
        bufp->chgSData(oldp+302,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[11]),16);
        bufp->chgSData(oldp+303,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[12]),16);
        bufp->chgSData(oldp+304,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[13]),16);
        bufp->chgSData(oldp+305,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[14]),16);
        bufp->chgSData(oldp+306,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[15]),16);
        bufp->chgSData(oldp+307,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[0]),16);
        bufp->chgSData(oldp+308,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[1]),16);
        bufp->chgSData(oldp+309,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[2]),16);
        bufp->chgSData(oldp+310,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[3]),16);
        bufp->chgSData(oldp+311,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[4]),16);
        bufp->chgSData(oldp+312,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[5]),16);
        bufp->chgSData(oldp+313,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[6]),16);
        bufp->chgSData(oldp+314,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[7]),16);
        bufp->chgSData(oldp+315,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[8]),16);
        bufp->chgSData(oldp+316,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[9]),16);
        bufp->chgSData(oldp+317,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[10]),16);
        bufp->chgSData(oldp+318,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[11]),16);
        bufp->chgSData(oldp+319,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[12]),16);
        bufp->chgSData(oldp+320,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[13]),16);
        bufp->chgSData(oldp+321,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[14]),16);
        bufp->chgSData(oldp+322,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[15]),16);
        bufp->chgCData(oldp+323,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r),2);
        bufp->chgCData(oldp+324,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r),4);
        bufp->chgBit(oldp+325,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[0]));
        bufp->chgBit(oldp+326,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[1]));
        bufp->chgBit(oldp+327,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[2]));
        bufp->chgCData(oldp+328,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+329,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[1]),5);
        bufp->chgCData(oldp+330,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[2]),5);
        bufp->chgIData(oldp+331,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[0]),24);
        bufp->chgIData(oldp+332,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[1]),24);
        bufp->chgIData(oldp+333,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[2]),24);
        bufp->chgBit(oldp+334,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+335,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[1]));
        bufp->chgBit(oldp+336,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[2]));
        bufp->chgBit(oldp+337,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+338,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[1]));
        bufp->chgBit(oldp+339,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[2]));
        bufp->chgIData(oldp+340,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[0]),24);
        bufp->chgIData(oldp+341,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[1]),24);
        bufp->chgIData(oldp+342,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[2]),24);
        bufp->chgIData(oldp+343,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[3]),24);
        bufp->chgIData(oldp+344,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[4]),24);
        bufp->chgIData(oldp+345,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[5]),24);
        bufp->chgIData(oldp+346,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[6]),24);
        bufp->chgIData(oldp+347,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[7]),24);
        bufp->chgIData(oldp+348,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[8]),24);
        bufp->chgIData(oldp+349,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[9]),24);
        bufp->chgIData(oldp+350,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[10]),24);
        bufp->chgIData(oldp+351,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[11]),24);
        bufp->chgIData(oldp+352,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[12]),24);
        bufp->chgIData(oldp+353,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[13]),24);
        bufp->chgIData(oldp+354,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[14]),24);
        bufp->chgIData(oldp+355,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[15]),24);
        bufp->chgIData(oldp+356,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[16]),24);
        bufp->chgIData(oldp+357,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[17]),24);
        bufp->chgIData(oldp+358,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[18]),24);
        bufp->chgIData(oldp+359,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[19]),24);
        bufp->chgIData(oldp+360,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[20]),24);
        bufp->chgIData(oldp+361,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[21]),24);
        bufp->chgIData(oldp+362,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[22]),24);
        bufp->chgIData(oldp+363,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[23]),24);
        bufp->chgIData(oldp+364,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[24]),24);
        bufp->chgIData(oldp+365,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[25]),24);
        bufp->chgIData(oldp+366,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[26]),24);
        bufp->chgIData(oldp+367,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[27]),24);
        bufp->chgIData(oldp+368,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[28]),24);
        bufp->chgIData(oldp+369,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[29]),24);
        bufp->chgIData(oldp+370,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[30]),24);
        bufp->chgIData(oldp+371,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[31]),24);
        bufp->chgBit(oldp+372,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+373,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+374,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+375,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+376,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                               [0U]));
        bufp->chgBit(oldp+377,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+378,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+379,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+380,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+381,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+382,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+383,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+384,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                               [1U]));
        bufp->chgBit(oldp+385,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                               [1U]));
        bufp->chgBit(oldp+386,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+387,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+388,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+389,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+390,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+391,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+392,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                               [2U]));
        bufp->chgBit(oldp+393,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                               [2U]));
        bufp->chgBit(oldp+394,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+395,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+396,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+397,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+398,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+399,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+400,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+401,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+402,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgIData(oldp+403,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__data_curr_r),24);
        bufp->chgCData(oldp+404,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[0]),8);
        bufp->chgCData(oldp+405,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[1]),8);
        bufp->chgCData(oldp+406,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[2]),8);
        bufp->chgCData(oldp+407,((0x1fU & ((IData)(1U) 
                                           + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r)))),5);
        bufp->chgBit(oldp+408,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+409,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[1]));
        bufp->chgBit(oldp+410,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[2]));
        bufp->chgBit(oldp+411,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[3]));
        bufp->chgBit(oldp+412,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgBit(oldp+413,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[1]));
        bufp->chgBit(oldp+414,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[2]));
        bufp->chgCData(oldp+415,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+416,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[1]),8);
        bufp->chgCData(oldp+417,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[2]),8);
        bufp->chgCData(oldp+418,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+419,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+420,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+421,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+422,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+423,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+424,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+425,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+426,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+427,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+428,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+429,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+430,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+431,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+432,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+433,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+434,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+435,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+436,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+437,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+438,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+439,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+440,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+441,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+442,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+443,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+444,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+445,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+446,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+447,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+448,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+449,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+450,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+451,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+452,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+453,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+454,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+455,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                               [1U]));
        bufp->chgBit(oldp+456,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+457,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+458,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+459,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+460,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                               [2U]));
        bufp->chgBit(oldp+461,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+462,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+463,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+464,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+465,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                               [1U]));
        bufp->chgBit(oldp+466,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+467,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+468,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+469,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+470,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                               [2U]));
        bufp->chgBit(oldp+471,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+472,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+473,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+474,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+475,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                               [3U]));
        bufp->chgBit(oldp+476,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+477,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+478,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+479,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+480,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+481,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+482,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+483,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+484,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o[0]),8);
        bufp->chgCData(oldp+485,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_a_r),8);
        bufp->chgBit(oldp+486,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+487,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+488,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+489,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+490,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+491,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+492,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+493,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+494,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+495,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+496,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+497,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+498,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+499,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+500,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+501,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+502,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+503,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+504,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+505,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+506,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+507,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+508,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+509,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+510,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+511,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+512,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+513,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+514,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+515,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+516,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+517,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+518,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+519,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+520,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+521,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+522,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+523,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+524,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+525,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+526,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+527,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+528,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+529,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+530,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+531,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+532,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+533,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+534,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+535,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o[0]),8);
        bufp->chgCData(oldp+536,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_a_r),8);
        bufp->chgBit(oldp+537,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+538,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+539,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+540,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+541,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+542,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+543,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+544,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+545,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+546,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+547,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+548,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+549,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+550,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+551,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+552,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+553,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+554,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+555,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+556,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+557,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+558,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+559,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+560,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+561,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+562,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+563,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+564,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+565,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+566,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+567,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+568,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+569,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+570,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+571,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+572,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+573,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+574,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+575,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+576,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+577,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+578,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+579,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+580,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+581,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+582,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+583,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+584,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+585,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgBit(oldp+586,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[0]));
        bufp->chgBit(oldp+587,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[1]));
        bufp->chgBit(oldp+588,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[2]));
        bufp->chgCData(oldp+589,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[0]),5);
        bufp->chgCData(oldp+590,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[1]),5);
        bufp->chgCData(oldp+591,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[2]),5);
        bufp->chgSData(oldp+592,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[0]),16);
        bufp->chgSData(oldp+593,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[1]),16);
        bufp->chgSData(oldp+594,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[2]),16);
        bufp->chgBit(oldp+595,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[0]));
        bufp->chgBit(oldp+596,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[1]));
        bufp->chgBit(oldp+597,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[2]));
        bufp->chgBit(oldp+598,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+599,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[1]));
        bufp->chgBit(oldp+600,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[2]));
        bufp->chgSData(oldp+601,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[0]),16);
        bufp->chgSData(oldp+602,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[1]),16);
        bufp->chgSData(oldp+603,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[2]),16);
        bufp->chgSData(oldp+604,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[3]),16);
        bufp->chgSData(oldp+605,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[4]),16);
        bufp->chgSData(oldp+606,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[5]),16);
        bufp->chgSData(oldp+607,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[6]),16);
        bufp->chgSData(oldp+608,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[7]),16);
        bufp->chgSData(oldp+609,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[8]),16);
        bufp->chgSData(oldp+610,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[9]),16);
        bufp->chgSData(oldp+611,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[10]),16);
        bufp->chgSData(oldp+612,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[11]),16);
        bufp->chgSData(oldp+613,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[12]),16);
        bufp->chgSData(oldp+614,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[13]),16);
        bufp->chgSData(oldp+615,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[14]),16);
        bufp->chgSData(oldp+616,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[15]),16);
        bufp->chgSData(oldp+617,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[16]),16);
        bufp->chgSData(oldp+618,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[17]),16);
        bufp->chgSData(oldp+619,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[18]),16);
        bufp->chgSData(oldp+620,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[19]),16);
        bufp->chgSData(oldp+621,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[20]),16);
        bufp->chgSData(oldp+622,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[21]),16);
        bufp->chgSData(oldp+623,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[22]),16);
        bufp->chgSData(oldp+624,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[23]),16);
        bufp->chgSData(oldp+625,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[24]),16);
        bufp->chgSData(oldp+626,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[25]),16);
        bufp->chgSData(oldp+627,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[26]),16);
        bufp->chgSData(oldp+628,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[27]),16);
        bufp->chgSData(oldp+629,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[28]),16);
        bufp->chgSData(oldp+630,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[29]),16);
        bufp->chgSData(oldp+631,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[30]),16);
        bufp->chgSData(oldp+632,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[31]),16);
        bufp->chgBit(oldp+633,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_en_r));
        bufp->chgIData(oldp+634,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index),32);
        bufp->chgCData(oldp+635,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_addr_r),5);
        bufp->chgBit(oldp+636,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ctrl__DOT__found));
        bufp->chgBit(oldp+637,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
                               [1U]));
        bufp->chgBit(oldp+638,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
                               [1U]));
        bufp->chgBit(oldp+639,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+640,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+641,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+642,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+643,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+644,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+645,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
                               [2U]));
        bufp->chgBit(oldp+646,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
                               [2U]));
        bufp->chgBit(oldp+647,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                                | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
        bufp->chgBit(oldp+648,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+649,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+650,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+651,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
        bufp->chgBit(oldp+652,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+653,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+654,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgCData(oldp+655,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o[0]),8);
        bufp->chgBit(oldp+656,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+657,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+658,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+659,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+660,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+661,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+662,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+663,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+664,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+665,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+666,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+667,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+668,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+669,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+670,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+671,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+672,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+673,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+674,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+675,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+676,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+677,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+678,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+679,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+680,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+681,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+682,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+683,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+684,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+685,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+686,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+687,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+688,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+689,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+690,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+691,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+692,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+693,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+694,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+695,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+696,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+697,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+698,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+699,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+700,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+701,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+702,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+703,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+704,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+705,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o[0]),8);
        bufp->chgBit(oldp+706,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+707,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[1]));
        bufp->chgBit(oldp+708,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+709,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+710,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+711,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+712,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+713,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+714,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+715,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+716,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+717,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+718,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+719,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+720,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+721,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+722,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+723,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+724,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+725,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+726,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+727,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+728,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+729,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+730,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+731,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+732,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+733,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+734,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+735,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+736,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+737,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+738,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+739,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+740,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+741,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+742,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+743,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+744,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+745,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+746,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+747,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+748,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+749,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+750,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+751,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+752,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
                               [1U]));
        bufp->chgBit(oldp+753,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+754,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+755,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+756,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+757,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+758,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+759,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+760,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+761,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o[0]),8);
        bufp->chgBit(oldp+762,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+763,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[1]));
        bufp->chgBit(oldp+764,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[2]));
        bufp->chgBit(oldp+765,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[3]));
        bufp->chgBit(oldp+766,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+767,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+768,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+769,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+770,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+771,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+772,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+773,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+774,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+775,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+776,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+777,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+778,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+779,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+780,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+781,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+782,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+783,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+784,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+785,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+786,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+787,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+788,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+789,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+790,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+791,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+792,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+793,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+794,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+795,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+796,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+797,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+798,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+799,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+800,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+801,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+802,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+803,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+804,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+805,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                               [1U]));
        bufp->chgBit(oldp+806,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+807,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+808,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+809,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+810,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                               [2U]));
        bufp->chgBit(oldp+811,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+812,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+813,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+814,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+815,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                               [3U]));
        bufp->chgBit(oldp+816,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+817,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+818,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+819,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+820,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+821,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+822,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+823,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+824,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o[0]),8);
        bufp->chgBit(oldp+825,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r[0]));
        bufp->chgBit(oldp+826,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r[0]));
        bufp->chgCData(oldp+827,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r[0]),8);
        bufp->chgCData(oldp+828,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[0]),8);
        bufp->chgCData(oldp+829,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[1]),8);
        bufp->chgCData(oldp+830,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[2]),8);
        bufp->chgCData(oldp+831,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[3]),8);
        bufp->chgCData(oldp+832,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[4]),8);
        bufp->chgCData(oldp+833,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[5]),8);
        bufp->chgCData(oldp+834,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[6]),8);
        bufp->chgCData(oldp+835,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[7]),8);
        bufp->chgCData(oldp+836,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[8]),8);
        bufp->chgCData(oldp+837,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[9]),8);
        bufp->chgCData(oldp+838,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[10]),8);
        bufp->chgCData(oldp+839,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[11]),8);
        bufp->chgCData(oldp+840,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[12]),8);
        bufp->chgCData(oldp+841,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[13]),8);
        bufp->chgCData(oldp+842,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[14]),8);
        bufp->chgCData(oldp+843,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[15]),8);
        bufp->chgCData(oldp+844,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[16]),8);
        bufp->chgCData(oldp+845,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[17]),8);
        bufp->chgCData(oldp+846,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[18]),8);
        bufp->chgCData(oldp+847,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[19]),8);
        bufp->chgCData(oldp+848,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[20]),8);
        bufp->chgCData(oldp+849,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[21]),8);
        bufp->chgCData(oldp+850,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[22]),8);
        bufp->chgCData(oldp+851,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[23]),8);
        bufp->chgCData(oldp+852,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[24]),8);
        bufp->chgCData(oldp+853,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[25]),8);
        bufp->chgCData(oldp+854,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[26]),8);
        bufp->chgCData(oldp+855,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[27]),8);
        bufp->chgCData(oldp+856,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[28]),8);
        bufp->chgCData(oldp+857,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[29]),8);
        bufp->chgCData(oldp+858,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[30]),8);
        bufp->chgCData(oldp+859,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[31]),8);
        bufp->chgBit(oldp+860,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r
                               [0U]));
        bufp->chgBit(oldp+861,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+862,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+863,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
        bufp->chgBit(oldp+864,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
        bufp->chgBit(oldp+865,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r
                               [0U]));
        bufp->chgBit(oldp+866,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
        bufp->chgBit(oldp+867,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
        bufp->chgBit(oldp+868,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
        bufp->chgBit(oldp+869,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
        bufp->chgIData(oldp+870,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk4__DOT__i),32);
        bufp->chgIData(oldp+871,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk5__DOT__i),32);
        bufp->chgIData(oldp+872,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__i),32);
        bufp->chgCData(oldp+873,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
        bufp->chgCData(oldp+874,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__data_w),8);
        bufp->chgCData(oldp+875,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__vol_s),8);
        bufp->chgCData(oldp+876,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk2__DOT__shift_w),5);
        bufp->chgIData(oldp+877,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__high_bit_w),32);
        bufp->chgBit(oldp+878,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__overflow));
        bufp->chgBit(oldp+879,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__zero_byte_w));
        bufp->chgCData(oldp+880,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_hi_mask_w),8);
        bufp->chgSData(oldp+881,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_w),16);
        bufp->chgSData(oldp+882,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__addr_w),16);
        bufp->chgCData(oldp+883,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk5__DOT__ca),4);
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[3U]))) {
        bufp->chgCData(oldp+884,(vlSelfRef.doc5503_harness__DOT__freq_low[0]),8);
        bufp->chgCData(oldp+885,(vlSelfRef.doc5503_harness__DOT__freq_low[1]),8);
        bufp->chgCData(oldp+886,(vlSelfRef.doc5503_harness__DOT__freq_low[2]),8);
        bufp->chgCData(oldp+887,(vlSelfRef.doc5503_harness__DOT__freq_low[3]),8);
        bufp->chgCData(oldp+888,(vlSelfRef.doc5503_harness__DOT__freq_low[4]),8);
        bufp->chgCData(oldp+889,(vlSelfRef.doc5503_harness__DOT__freq_low[5]),8);
        bufp->chgCData(oldp+890,(vlSelfRef.doc5503_harness__DOT__freq_low[6]),8);
        bufp->chgCData(oldp+891,(vlSelfRef.doc5503_harness__DOT__freq_low[7]),8);
        bufp->chgCData(oldp+892,(vlSelfRef.doc5503_harness__DOT__freq_low[8]),8);
        bufp->chgCData(oldp+893,(vlSelfRef.doc5503_harness__DOT__freq_low[9]),8);
        bufp->chgCData(oldp+894,(vlSelfRef.doc5503_harness__DOT__freq_low[10]),8);
        bufp->chgCData(oldp+895,(vlSelfRef.doc5503_harness__DOT__freq_low[11]),8);
        bufp->chgCData(oldp+896,(vlSelfRef.doc5503_harness__DOT__freq_low[12]),8);
        bufp->chgCData(oldp+897,(vlSelfRef.doc5503_harness__DOT__freq_low[13]),8);
        bufp->chgCData(oldp+898,(vlSelfRef.doc5503_harness__DOT__freq_low[14]),8);
        bufp->chgCData(oldp+899,(vlSelfRef.doc5503_harness__DOT__freq_low[15]),8);
        bufp->chgCData(oldp+900,(vlSelfRef.doc5503_harness__DOT__freq_low[16]),8);
        bufp->chgCData(oldp+901,(vlSelfRef.doc5503_harness__DOT__freq_low[17]),8);
        bufp->chgCData(oldp+902,(vlSelfRef.doc5503_harness__DOT__freq_low[18]),8);
        bufp->chgCData(oldp+903,(vlSelfRef.doc5503_harness__DOT__freq_low[19]),8);
        bufp->chgCData(oldp+904,(vlSelfRef.doc5503_harness__DOT__freq_low[20]),8);
        bufp->chgCData(oldp+905,(vlSelfRef.doc5503_harness__DOT__freq_low[21]),8);
        bufp->chgCData(oldp+906,(vlSelfRef.doc5503_harness__DOT__freq_low[22]),8);
        bufp->chgCData(oldp+907,(vlSelfRef.doc5503_harness__DOT__freq_low[23]),8);
        bufp->chgCData(oldp+908,(vlSelfRef.doc5503_harness__DOT__freq_low[24]),8);
        bufp->chgCData(oldp+909,(vlSelfRef.doc5503_harness__DOT__freq_low[25]),8);
        bufp->chgCData(oldp+910,(vlSelfRef.doc5503_harness__DOT__freq_low[26]),8);
        bufp->chgCData(oldp+911,(vlSelfRef.doc5503_harness__DOT__freq_low[27]),8);
        bufp->chgCData(oldp+912,(vlSelfRef.doc5503_harness__DOT__freq_low[28]),8);
        bufp->chgCData(oldp+913,(vlSelfRef.doc5503_harness__DOT__freq_low[29]),8);
        bufp->chgCData(oldp+914,(vlSelfRef.doc5503_harness__DOT__freq_low[30]),8);
        bufp->chgCData(oldp+915,(vlSelfRef.doc5503_harness__DOT__freq_low[31]),8);
        bufp->chgCData(oldp+916,(vlSelfRef.doc5503_harness__DOT__freq_high[0]),8);
        bufp->chgCData(oldp+917,(vlSelfRef.doc5503_harness__DOT__freq_high[1]),8);
        bufp->chgCData(oldp+918,(vlSelfRef.doc5503_harness__DOT__freq_high[2]),8);
        bufp->chgCData(oldp+919,(vlSelfRef.doc5503_harness__DOT__freq_high[3]),8);
        bufp->chgCData(oldp+920,(vlSelfRef.doc5503_harness__DOT__freq_high[4]),8);
        bufp->chgCData(oldp+921,(vlSelfRef.doc5503_harness__DOT__freq_high[5]),8);
        bufp->chgCData(oldp+922,(vlSelfRef.doc5503_harness__DOT__freq_high[6]),8);
        bufp->chgCData(oldp+923,(vlSelfRef.doc5503_harness__DOT__freq_high[7]),8);
        bufp->chgCData(oldp+924,(vlSelfRef.doc5503_harness__DOT__freq_high[8]),8);
        bufp->chgCData(oldp+925,(vlSelfRef.doc5503_harness__DOT__freq_high[9]),8);
        bufp->chgCData(oldp+926,(vlSelfRef.doc5503_harness__DOT__freq_high[10]),8);
        bufp->chgCData(oldp+927,(vlSelfRef.doc5503_harness__DOT__freq_high[11]),8);
        bufp->chgCData(oldp+928,(vlSelfRef.doc5503_harness__DOT__freq_high[12]),8);
        bufp->chgCData(oldp+929,(vlSelfRef.doc5503_harness__DOT__freq_high[13]),8);
        bufp->chgCData(oldp+930,(vlSelfRef.doc5503_harness__DOT__freq_high[14]),8);
        bufp->chgCData(oldp+931,(vlSelfRef.doc5503_harness__DOT__freq_high[15]),8);
        bufp->chgCData(oldp+932,(vlSelfRef.doc5503_harness__DOT__freq_high[16]),8);
        bufp->chgCData(oldp+933,(vlSelfRef.doc5503_harness__DOT__freq_high[17]),8);
        bufp->chgCData(oldp+934,(vlSelfRef.doc5503_harness__DOT__freq_high[18]),8);
        bufp->chgCData(oldp+935,(vlSelfRef.doc5503_harness__DOT__freq_high[19]),8);
        bufp->chgCData(oldp+936,(vlSelfRef.doc5503_harness__DOT__freq_high[20]),8);
        bufp->chgCData(oldp+937,(vlSelfRef.doc5503_harness__DOT__freq_high[21]),8);
        bufp->chgCData(oldp+938,(vlSelfRef.doc5503_harness__DOT__freq_high[22]),8);
        bufp->chgCData(oldp+939,(vlSelfRef.doc5503_harness__DOT__freq_high[23]),8);
        bufp->chgCData(oldp+940,(vlSelfRef.doc5503_harness__DOT__freq_high[24]),8);
        bufp->chgCData(oldp+941,(vlSelfRef.doc5503_harness__DOT__freq_high[25]),8);
        bufp->chgCData(oldp+942,(vlSelfRef.doc5503_harness__DOT__freq_high[26]),8);
        bufp->chgCData(oldp+943,(vlSelfRef.doc5503_harness__DOT__freq_high[27]),8);
        bufp->chgCData(oldp+944,(vlSelfRef.doc5503_harness__DOT__freq_high[28]),8);
        bufp->chgCData(oldp+945,(vlSelfRef.doc5503_harness__DOT__freq_high[29]),8);
        bufp->chgCData(oldp+946,(vlSelfRef.doc5503_harness__DOT__freq_high[30]),8);
        bufp->chgCData(oldp+947,(vlSelfRef.doc5503_harness__DOT__freq_high[31]),8);
        bufp->chgCData(oldp+948,(vlSelfRef.doc5503_harness__DOT__volume[0]),8);
        bufp->chgCData(oldp+949,(vlSelfRef.doc5503_harness__DOT__volume[1]),8);
        bufp->chgCData(oldp+950,(vlSelfRef.doc5503_harness__DOT__volume[2]),8);
        bufp->chgCData(oldp+951,(vlSelfRef.doc5503_harness__DOT__volume[3]),8);
        bufp->chgCData(oldp+952,(vlSelfRef.doc5503_harness__DOT__volume[4]),8);
        bufp->chgCData(oldp+953,(vlSelfRef.doc5503_harness__DOT__volume[5]),8);
        bufp->chgCData(oldp+954,(vlSelfRef.doc5503_harness__DOT__volume[6]),8);
        bufp->chgCData(oldp+955,(vlSelfRef.doc5503_harness__DOT__volume[7]),8);
        bufp->chgCData(oldp+956,(vlSelfRef.doc5503_harness__DOT__volume[8]),8);
        bufp->chgCData(oldp+957,(vlSelfRef.doc5503_harness__DOT__volume[9]),8);
        bufp->chgCData(oldp+958,(vlSelfRef.doc5503_harness__DOT__volume[10]),8);
        bufp->chgCData(oldp+959,(vlSelfRef.doc5503_harness__DOT__volume[11]),8);
        bufp->chgCData(oldp+960,(vlSelfRef.doc5503_harness__DOT__volume[12]),8);
        bufp->chgCData(oldp+961,(vlSelfRef.doc5503_harness__DOT__volume[13]),8);
        bufp->chgCData(oldp+962,(vlSelfRef.doc5503_harness__DOT__volume[14]),8);
        bufp->chgCData(oldp+963,(vlSelfRef.doc5503_harness__DOT__volume[15]),8);
        bufp->chgCData(oldp+964,(vlSelfRef.doc5503_harness__DOT__volume[16]),8);
        bufp->chgCData(oldp+965,(vlSelfRef.doc5503_harness__DOT__volume[17]),8);
        bufp->chgCData(oldp+966,(vlSelfRef.doc5503_harness__DOT__volume[18]),8);
        bufp->chgCData(oldp+967,(vlSelfRef.doc5503_harness__DOT__volume[19]),8);
        bufp->chgCData(oldp+968,(vlSelfRef.doc5503_harness__DOT__volume[20]),8);
        bufp->chgCData(oldp+969,(vlSelfRef.doc5503_harness__DOT__volume[21]),8);
        bufp->chgCData(oldp+970,(vlSelfRef.doc5503_harness__DOT__volume[22]),8);
        bufp->chgCData(oldp+971,(vlSelfRef.doc5503_harness__DOT__volume[23]),8);
        bufp->chgCData(oldp+972,(vlSelfRef.doc5503_harness__DOT__volume[24]),8);
        bufp->chgCData(oldp+973,(vlSelfRef.doc5503_harness__DOT__volume[25]),8);
        bufp->chgCData(oldp+974,(vlSelfRef.doc5503_harness__DOT__volume[26]),8);
        bufp->chgCData(oldp+975,(vlSelfRef.doc5503_harness__DOT__volume[27]),8);
        bufp->chgCData(oldp+976,(vlSelfRef.doc5503_harness__DOT__volume[28]),8);
        bufp->chgCData(oldp+977,(vlSelfRef.doc5503_harness__DOT__volume[29]),8);
        bufp->chgCData(oldp+978,(vlSelfRef.doc5503_harness__DOT__volume[30]),8);
        bufp->chgCData(oldp+979,(vlSelfRef.doc5503_harness__DOT__volume[31]),8);
        bufp->chgCData(oldp+980,(vlSelfRef.doc5503_harness__DOT__data_sample[0]),8);
        bufp->chgCData(oldp+981,(vlSelfRef.doc5503_harness__DOT__data_sample[1]),8);
        bufp->chgCData(oldp+982,(vlSelfRef.doc5503_harness__DOT__data_sample[2]),8);
        bufp->chgCData(oldp+983,(vlSelfRef.doc5503_harness__DOT__data_sample[3]),8);
        bufp->chgCData(oldp+984,(vlSelfRef.doc5503_harness__DOT__data_sample[4]),8);
        bufp->chgCData(oldp+985,(vlSelfRef.doc5503_harness__DOT__data_sample[5]),8);
        bufp->chgCData(oldp+986,(vlSelfRef.doc5503_harness__DOT__data_sample[6]),8);
        bufp->chgCData(oldp+987,(vlSelfRef.doc5503_harness__DOT__data_sample[7]),8);
        bufp->chgCData(oldp+988,(vlSelfRef.doc5503_harness__DOT__data_sample[8]),8);
        bufp->chgCData(oldp+989,(vlSelfRef.doc5503_harness__DOT__data_sample[9]),8);
        bufp->chgCData(oldp+990,(vlSelfRef.doc5503_harness__DOT__data_sample[10]),8);
        bufp->chgCData(oldp+991,(vlSelfRef.doc5503_harness__DOT__data_sample[11]),8);
        bufp->chgCData(oldp+992,(vlSelfRef.doc5503_harness__DOT__data_sample[12]),8);
        bufp->chgCData(oldp+993,(vlSelfRef.doc5503_harness__DOT__data_sample[13]),8);
        bufp->chgCData(oldp+994,(vlSelfRef.doc5503_harness__DOT__data_sample[14]),8);
        bufp->chgCData(oldp+995,(vlSelfRef.doc5503_harness__DOT__data_sample[15]),8);
        bufp->chgCData(oldp+996,(vlSelfRef.doc5503_harness__DOT__data_sample[16]),8);
        bufp->chgCData(oldp+997,(vlSelfRef.doc5503_harness__DOT__data_sample[17]),8);
        bufp->chgCData(oldp+998,(vlSelfRef.doc5503_harness__DOT__data_sample[18]),8);
        bufp->chgCData(oldp+999,(vlSelfRef.doc5503_harness__DOT__data_sample[19]),8);
        bufp->chgCData(oldp+1000,(vlSelfRef.doc5503_harness__DOT__data_sample[20]),8);
        bufp->chgCData(oldp+1001,(vlSelfRef.doc5503_harness__DOT__data_sample[21]),8);
        bufp->chgCData(oldp+1002,(vlSelfRef.doc5503_harness__DOT__data_sample[22]),8);
        bufp->chgCData(oldp+1003,(vlSelfRef.doc5503_harness__DOT__data_sample[23]),8);
        bufp->chgCData(oldp+1004,(vlSelfRef.doc5503_harness__DOT__data_sample[24]),8);
        bufp->chgCData(oldp+1005,(vlSelfRef.doc5503_harness__DOT__data_sample[25]),8);
        bufp->chgCData(oldp+1006,(vlSelfRef.doc5503_harness__DOT__data_sample[26]),8);
        bufp->chgCData(oldp+1007,(vlSelfRef.doc5503_harness__DOT__data_sample[27]),8);
        bufp->chgCData(oldp+1008,(vlSelfRef.doc5503_harness__DOT__data_sample[28]),8);
        bufp->chgCData(oldp+1009,(vlSelfRef.doc5503_harness__DOT__data_sample[29]),8);
        bufp->chgCData(oldp+1010,(vlSelfRef.doc5503_harness__DOT__data_sample[30]),8);
        bufp->chgCData(oldp+1011,(vlSelfRef.doc5503_harness__DOT__data_sample[31]),8);
        bufp->chgCData(oldp+1012,(vlSelfRef.doc5503_harness__DOT__addr_pointer[0]),8);
        bufp->chgCData(oldp+1013,(vlSelfRef.doc5503_harness__DOT__addr_pointer[1]),8);
        bufp->chgCData(oldp+1014,(vlSelfRef.doc5503_harness__DOT__addr_pointer[2]),8);
        bufp->chgCData(oldp+1015,(vlSelfRef.doc5503_harness__DOT__addr_pointer[3]),8);
        bufp->chgCData(oldp+1016,(vlSelfRef.doc5503_harness__DOT__addr_pointer[4]),8);
        bufp->chgCData(oldp+1017,(vlSelfRef.doc5503_harness__DOT__addr_pointer[5]),8);
        bufp->chgCData(oldp+1018,(vlSelfRef.doc5503_harness__DOT__addr_pointer[6]),8);
        bufp->chgCData(oldp+1019,(vlSelfRef.doc5503_harness__DOT__addr_pointer[7]),8);
        bufp->chgCData(oldp+1020,(vlSelfRef.doc5503_harness__DOT__addr_pointer[8]),8);
        bufp->chgCData(oldp+1021,(vlSelfRef.doc5503_harness__DOT__addr_pointer[9]),8);
        bufp->chgCData(oldp+1022,(vlSelfRef.doc5503_harness__DOT__addr_pointer[10]),8);
        bufp->chgCData(oldp+1023,(vlSelfRef.doc5503_harness__DOT__addr_pointer[11]),8);
        bufp->chgCData(oldp+1024,(vlSelfRef.doc5503_harness__DOT__addr_pointer[12]),8);
        bufp->chgCData(oldp+1025,(vlSelfRef.doc5503_harness__DOT__addr_pointer[13]),8);
        bufp->chgCData(oldp+1026,(vlSelfRef.doc5503_harness__DOT__addr_pointer[14]),8);
        bufp->chgCData(oldp+1027,(vlSelfRef.doc5503_harness__DOT__addr_pointer[15]),8);
        bufp->chgCData(oldp+1028,(vlSelfRef.doc5503_harness__DOT__addr_pointer[16]),8);
        bufp->chgCData(oldp+1029,(vlSelfRef.doc5503_harness__DOT__addr_pointer[17]),8);
        bufp->chgCData(oldp+1030,(vlSelfRef.doc5503_harness__DOT__addr_pointer[18]),8);
        bufp->chgCData(oldp+1031,(vlSelfRef.doc5503_harness__DOT__addr_pointer[19]),8);
        bufp->chgCData(oldp+1032,(vlSelfRef.doc5503_harness__DOT__addr_pointer[20]),8);
        bufp->chgCData(oldp+1033,(vlSelfRef.doc5503_harness__DOT__addr_pointer[21]),8);
        bufp->chgCData(oldp+1034,(vlSelfRef.doc5503_harness__DOT__addr_pointer[22]),8);
        bufp->chgCData(oldp+1035,(vlSelfRef.doc5503_harness__DOT__addr_pointer[23]),8);
        bufp->chgCData(oldp+1036,(vlSelfRef.doc5503_harness__DOT__addr_pointer[24]),8);
        bufp->chgCData(oldp+1037,(vlSelfRef.doc5503_harness__DOT__addr_pointer[25]),8);
        bufp->chgCData(oldp+1038,(vlSelfRef.doc5503_harness__DOT__addr_pointer[26]),8);
        bufp->chgCData(oldp+1039,(vlSelfRef.doc5503_harness__DOT__addr_pointer[27]),8);
        bufp->chgCData(oldp+1040,(vlSelfRef.doc5503_harness__DOT__addr_pointer[28]),8);
        bufp->chgCData(oldp+1041,(vlSelfRef.doc5503_harness__DOT__addr_pointer[29]),8);
        bufp->chgCData(oldp+1042,(vlSelfRef.doc5503_harness__DOT__addr_pointer[30]),8);
        bufp->chgCData(oldp+1043,(vlSelfRef.doc5503_harness__DOT__addr_pointer[31]),8);
        bufp->chgCData(oldp+1044,(vlSelfRef.doc5503_harness__DOT__control[0]),8);
        bufp->chgCData(oldp+1045,(vlSelfRef.doc5503_harness__DOT__control[1]),8);
        bufp->chgCData(oldp+1046,(vlSelfRef.doc5503_harness__DOT__control[2]),8);
        bufp->chgCData(oldp+1047,(vlSelfRef.doc5503_harness__DOT__control[3]),8);
        bufp->chgCData(oldp+1048,(vlSelfRef.doc5503_harness__DOT__control[4]),8);
        bufp->chgCData(oldp+1049,(vlSelfRef.doc5503_harness__DOT__control[5]),8);
        bufp->chgCData(oldp+1050,(vlSelfRef.doc5503_harness__DOT__control[6]),8);
        bufp->chgCData(oldp+1051,(vlSelfRef.doc5503_harness__DOT__control[7]),8);
        bufp->chgCData(oldp+1052,(vlSelfRef.doc5503_harness__DOT__control[8]),8);
        bufp->chgCData(oldp+1053,(vlSelfRef.doc5503_harness__DOT__control[9]),8);
        bufp->chgCData(oldp+1054,(vlSelfRef.doc5503_harness__DOT__control[10]),8);
        bufp->chgCData(oldp+1055,(vlSelfRef.doc5503_harness__DOT__control[11]),8);
        bufp->chgCData(oldp+1056,(vlSelfRef.doc5503_harness__DOT__control[12]),8);
        bufp->chgCData(oldp+1057,(vlSelfRef.doc5503_harness__DOT__control[13]),8);
        bufp->chgCData(oldp+1058,(vlSelfRef.doc5503_harness__DOT__control[14]),8);
        bufp->chgCData(oldp+1059,(vlSelfRef.doc5503_harness__DOT__control[15]),8);
        bufp->chgCData(oldp+1060,(vlSelfRef.doc5503_harness__DOT__control[16]),8);
        bufp->chgCData(oldp+1061,(vlSelfRef.doc5503_harness__DOT__control[17]),8);
        bufp->chgCData(oldp+1062,(vlSelfRef.doc5503_harness__DOT__control[18]),8);
        bufp->chgCData(oldp+1063,(vlSelfRef.doc5503_harness__DOT__control[19]),8);
        bufp->chgCData(oldp+1064,(vlSelfRef.doc5503_harness__DOT__control[20]),8);
        bufp->chgCData(oldp+1065,(vlSelfRef.doc5503_harness__DOT__control[21]),8);
        bufp->chgCData(oldp+1066,(vlSelfRef.doc5503_harness__DOT__control[22]),8);
        bufp->chgCData(oldp+1067,(vlSelfRef.doc5503_harness__DOT__control[23]),8);
        bufp->chgCData(oldp+1068,(vlSelfRef.doc5503_harness__DOT__control[24]),8);
        bufp->chgCData(oldp+1069,(vlSelfRef.doc5503_harness__DOT__control[25]),8);
        bufp->chgCData(oldp+1070,(vlSelfRef.doc5503_harness__DOT__control[26]),8);
        bufp->chgCData(oldp+1071,(vlSelfRef.doc5503_harness__DOT__control[27]),8);
        bufp->chgCData(oldp+1072,(vlSelfRef.doc5503_harness__DOT__control[28]),8);
        bufp->chgCData(oldp+1073,(vlSelfRef.doc5503_harness__DOT__control[29]),8);
        bufp->chgCData(oldp+1074,(vlSelfRef.doc5503_harness__DOT__control[30]),8);
        bufp->chgCData(oldp+1075,(vlSelfRef.doc5503_harness__DOT__control[31]),8);
        bufp->chgCData(oldp+1076,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[0]),8);
        bufp->chgCData(oldp+1077,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[1]),8);
        bufp->chgCData(oldp+1078,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[2]),8);
        bufp->chgCData(oldp+1079,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[3]),8);
        bufp->chgCData(oldp+1080,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[4]),8);
        bufp->chgCData(oldp+1081,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[5]),8);
        bufp->chgCData(oldp+1082,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[6]),8);
        bufp->chgCData(oldp+1083,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[7]),8);
        bufp->chgCData(oldp+1084,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[8]),8);
        bufp->chgCData(oldp+1085,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[9]),8);
        bufp->chgCData(oldp+1086,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[10]),8);
        bufp->chgCData(oldp+1087,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[11]),8);
        bufp->chgCData(oldp+1088,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[12]),8);
        bufp->chgCData(oldp+1089,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[13]),8);
        bufp->chgCData(oldp+1090,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[14]),8);
        bufp->chgCData(oldp+1091,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[15]),8);
        bufp->chgCData(oldp+1092,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[16]),8);
        bufp->chgCData(oldp+1093,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[17]),8);
        bufp->chgCData(oldp+1094,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[18]),8);
        bufp->chgCData(oldp+1095,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[19]),8);
        bufp->chgCData(oldp+1096,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[20]),8);
        bufp->chgCData(oldp+1097,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[21]),8);
        bufp->chgCData(oldp+1098,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[22]),8);
        bufp->chgCData(oldp+1099,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[23]),8);
        bufp->chgCData(oldp+1100,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[24]),8);
        bufp->chgCData(oldp+1101,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[25]),8);
        bufp->chgCData(oldp+1102,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[26]),8);
        bufp->chgCData(oldp+1103,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[27]),8);
        bufp->chgCData(oldp+1104,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[28]),8);
        bufp->chgCData(oldp+1105,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[29]),8);
        bufp->chgCData(oldp+1106,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[30]),8);
        bufp->chgCData(oldp+1107,(vlSelfRef.doc5503_harness__DOT__resolution_table_size[31]),8);
        bufp->chgCData(oldp+1108,(vlSelfRef.doc5503_harness__DOT__osc_int),8);
        bufp->chgCData(oldp+1109,(vlSelfRef.doc5503_harness__DOT__osc_en),8);
        bufp->chgIData(oldp+1110,(vlSelfRef.doc5503_harness__DOT__unnamedblk1__DOT__i),32);
    }
    bufp->chgBit(oldp+1111,(vlSelfRef.clk_i));
    bufp->chgBit(oldp+1112,(vlSelfRef.reset_n_i));
    bufp->chgBit(oldp+1113,(vlSelfRef.clk_en_i));
    bufp->chgBit(oldp+1114,(vlSelfRef.irq_n_o));
    bufp->chgBit(oldp+1115,(vlSelfRef.cs_n_i));
    bufp->chgBit(oldp+1116,(vlSelfRef.we_n_i));
    bufp->chgCData(oldp+1117,(vlSelfRef.addr_i),8);
    bufp->chgCData(oldp+1118,(vlSelfRef.data_i),8);
    bufp->chgCData(oldp+1119,(vlSelfRef.data_o),8);
    bufp->chgSData(oldp+1120,(vlSelfRef.wave_address_o),16);
    bufp->chgBit(oldp+1121,(vlSelfRef.wave_rd_o));
    bufp->chgBit(oldp+1122,(vlSelfRef.wave_data_ready_i));
    bufp->chgCData(oldp+1123,(vlSelfRef.wave_data_i),8);
    bufp->chgSData(oldp+1124,(vlSelfRef.mono_mix_o),16);
    bufp->chgSData(oldp+1125,(vlSelfRef.left_mix_o),16);
    bufp->chgSData(oldp+1126,(vlSelfRef.right_mix_o),16);
    bufp->chgCData(oldp+1127,(((0x80U & (IData)(vlSelfRef.addr_i))
                                ? ((0x40U & (IData)(vlSelfRef.addr_i))
                                    ? ((0x20U & (IData)(vlSelfRef.addr_i))
                                        ? ((0U == (0x1fU 
                                                   & (IData)(vlSelfRef.addr_i)))
                                            ? (IData)(vlSelfRef.doc5503_harness__DOT__osc_int)
                                            : ((1U 
                                                == 
                                                (0x1fU 
                                                 & (IData)(vlSelfRef.addr_i)))
                                                ? (IData)(vlSelfRef.doc5503_harness__DOT__osc_en)
                                                : 0U))
                                        : vlSelfRef.doc5503_harness__DOT__resolution_table_size
                                       [(0x1fU & (IData)(vlSelfRef.addr_i))])
                                    : ((0x20U & (IData)(vlSelfRef.addr_i))
                                        ? vlSelfRef.doc5503_harness__DOT__control
                                       [(0x1fU & (IData)(vlSelfRef.addr_i))]
                                        : vlSelfRef.doc5503_harness__DOT__addr_pointer
                                       [(0x1fU & (IData)(vlSelfRef.addr_i))]))
                                : ((0x40U & (IData)(vlSelfRef.addr_i))
                                    ? ((0x20U & (IData)(vlSelfRef.addr_i))
                                        ? vlSelfRef.doc5503_harness__DOT__data_sample
                                       [(0x1fU & (IData)(vlSelfRef.addr_i))]
                                        : vlSelfRef.doc5503_harness__DOT__volume
                                       [(0x1fU & (IData)(vlSelfRef.addr_i))])
                                    : ((0x20U & (IData)(vlSelfRef.addr_i))
                                        ? vlSelfRef.doc5503_harness__DOT__freq_high
                                       [(0x1fU & (IData)(vlSelfRef.addr_i))]
                                        : vlSelfRef.doc5503_harness__DOT__freq_low
                                       [(0x1fU & (IData)(vlSelfRef.addr_i))])))),8);
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
    vlSymsp->TOP.__Vm_traceActivity[3U] = 0U;
}
