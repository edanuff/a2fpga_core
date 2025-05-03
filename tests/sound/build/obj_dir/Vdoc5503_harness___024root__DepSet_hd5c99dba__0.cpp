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
        vlSelfRef.__Vm_traceActivity[1U] = 1U;
    }
}

VL_INLINE_OPT void Vdoc5503_harness___024root___ico_sequent__TOP__0(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___ico_sequent__TOP__0\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_data_i[0U] 
        = vlSelfRef.data_i;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_data_i[0U] 
        = vlSelfRef.data_i;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_data_i[1U] 
        = vlSelfRef.data_i;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_data_i[0U] 
        = vlSelfRef.data_i;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[3U] 
        = vlSelfRef.data_i;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_data_i[0U] 
        = vlSelfRef.data_i;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i[2U] 
        = vlSelfRef.wave_data_i;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i[3U] 
        = vlSelfRef.data_i;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__req_cs_w 
        = ((~ (IData)(vlSelfRef.cs_n_i)) & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__prev_cs_n_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.data_o = ((0U == (7U & ((IData)(vlSelfRef.addr_i) 
                                      >> 5U))) ? vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o
                        [0U] : ((1U == (7U & ((IData)(vlSelfRef.addr_i) 
                                              >> 5U)))
                                 ? vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o
                                [0U] : ((2U == (7U 
                                                & ((IData)(vlSelfRef.addr_i) 
                                                   >> 5U)))
                                         ? vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o
                                        [0U] : ((3U 
                                                 == 
                                                 (7U 
                                                  & ((IData)(vlSelfRef.addr_i) 
                                                     >> 5U)))
                                                 ? 
                                                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o
                                                [0U]
                                                 : 
                                                ((4U 
                                                  == 
                                                  (7U 
                                                   & ((IData)(vlSelfRef.addr_i) 
                                                      >> 5U)))
                                                  ? 
                                                 vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o
                                                 [0U]
                                                  : 
                                                 ((5U 
                                                   == 
                                                   (7U 
                                                    & ((IData)(vlSelfRef.addr_i) 
                                                       >> 5U)))
                                                   ? 
                                                  vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                                  [0U]
                                                   : 
                                                  ((6U 
                                                    == 
                                                    (7U 
                                                     & ((IData)(vlSelfRef.addr_i) 
                                                        >> 5U)))
                                                    ? 
                                                   vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o
                                                   [0U]
                                                    : 
                                                   ((0xe0U 
                                                     == (IData)(vlSelfRef.addr_i))
                                                     ? (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r)
                                                     : 
                                                    ((0xe1U 
                                                      == (IData)(vlSelfRef.addr_i))
                                                      ? (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r)
                                                      : 0U)))))))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i[0U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_addr_i[1U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[3U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[3U] 
        = (0x1fU & (IData)(vlSelfRef.addr_i));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (~ (IData)(vlSelfRef.we_n_i))) 
           & (0U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (~ (IData)(vlSelfRef.we_n_i))) 
           & (0x20U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (~ (IData)(vlSelfRef.we_n_i))) 
           & (0x80U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (~ (IData)(vlSelfRef.we_n_i))) 
           & (0xc0U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (IData)(vlSelfRef.we_n_i)) 
           & (0U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (IData)(vlSelfRef.we_n_i)) 
           & (0x20U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (IData)(vlSelfRef.we_n_i)) 
           & (0x40U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (IData)(vlSelfRef.we_n_i)) 
           & (0x60U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (IData)(vlSelfRef.we_n_i)) 
           & (0x80U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (IData)(vlSelfRef.we_n_i)) 
           & (0xc0U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i[1U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (~ (IData)(vlSelfRef.we_n_i))) 
           & (0x40U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i[0U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (IData)(vlSelfRef.we_n_i)) 
           & (0xa0U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[2U] 
        = vlSelfRef.wave_data_ready_i;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[3U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (~ (IData)(vlSelfRef.we_n_i))) 
           & (0xa0U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[3U] 
        = (((~ (IData)(vlSelfRef.cs_n_i)) & (~ (IData)(vlSelfRef.we_n_i))) 
           & (0x60U == (0xe0U & (IData)(vlSelfRef.addr_i))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
           [3U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
           [3U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w[1U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[1U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[2U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[1U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[2U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[3U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[1U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[2U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[3U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r;
    if (((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w
          [1U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__found))) 
         & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_index = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_addr_i
            [1U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w
         [1U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i
            [1U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w
         [2U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index = 2U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i
            [2U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w
         [1U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i
            [1U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w
         [2U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index = 2U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i
            [2U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r;
    if (((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w
          [3U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found))) 
         & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index = 3U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i
            [3U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w
         [1U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i
            [1U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w
         [2U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index = 2U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i
            [2U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r;
    if (((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w
          [3U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found))) 
         & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index = 3U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i
            [3U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
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

void Vdoc5503_harness___024root___eval_nba(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_nba\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vdoc5503_harness___024root___nba_sequent__TOP__0(vlSelf);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
    }
}

VL_INLINE_OPT void Vdoc5503_harness___024root___nba_sequent__TOP__0(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___nba_sequent__TOP__0\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*4:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r = 0;
    CData/*0:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__ready_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__ready_r = 0;
    CData/*4:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r = 0;
    CData/*4:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_r = 0;
    CData/*4:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r = 0;
    CData/*2:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 0;
    CData/*1:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r = 0;
    CData/*3:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r = 0;
    CData/*5:0*/ __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r = 0;
    SData/*15:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0;
    IData/*23:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v1 = 0;
    CData/*3:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r__v2;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r__v2 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r__v2;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r__v2 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*1:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r__v4;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r__v4 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r__v4;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r__v4 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*1:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r__v4;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r__v4 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r__v4;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r__v4 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3 = 0;
    CData/*1:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*1:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0 = 0;
    CData/*7:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0 = 0;
    IData/*23:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3 = 0;
    CData/*1:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3 = 0;
    SData/*15:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0 = 0;
    CData/*4:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0 = 0;
    CData/*0:0*/ __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3;
    __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3 = 0;
    CData/*1:0*/ __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3;
    __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3 = 0;
    CData/*0:0*/ __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3 = 0;
    // Body
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r__v2 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r__v4 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r__v4 = 0U;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3 = 0U;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__ready_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 0U;
    __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v1 = 0U;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r;
    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk4__DOT__i = 1U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk5__DOT__i = 1U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk4__DOT__i = 1U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk5__DOT__i = 1U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk4__DOT__i = 2U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk5__DOT__i = 1U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk4__DOT__i = 4U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk5__DOT__i = 1U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk4__DOT__i = 1U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk5__DOT__i = 1U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk4__DOT__i = 4U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk5__DOT__i = 3U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk4__DOT__i = 1U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk5__DOT__i = 1U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk4__DOT__i = 3U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk5__DOT__i = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__unnamedblk4__DOT__i = 3U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__unnamedblk5__DOT__i = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_en_r) {
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_addr_r;
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0 = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT____Vlvbound_hb38b23b0__0 = 1U;
        if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_index))) {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_data_i
                [(1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_index)];
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT____Vlvbound_hb38b23b0__0;
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1 
                = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_index);
            __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1 = 1U;
        } else {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0 = 0U;
        }
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_en_r) {
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_addr_r;
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0 = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT____Vlvbound_hb38b23b0__0 = 1U;
        if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_index))) {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_data_i
                [(1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_index)];
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT____Vlvbound_hb38b23b0__0;
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1 
                = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_index);
            __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1 = 1U;
        } else {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0 = 0U;
        }
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_en_r) {
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_addr_r;
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0 = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT____Vlvbound_hb38b23b0__0 = 1U;
        if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_index))) {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_data_i
                [(1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_index)];
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT____Vlvbound_hb38b23b0__0;
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1 
                = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_index);
            __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1 = 1U;
        } else {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0 = 0U;
        }
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_en_r) {
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_addr_r;
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0 = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT____Vlvbound_hb38b23b0__0 = 1U;
        if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_index))) {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_data_i
                [(1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_index)];
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT____Vlvbound_hb38b23b0__0;
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1 
                = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_index);
            __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1 = 1U;
        } else {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0 = 0U;
        }
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_en_r) {
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_addr_r;
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0 = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT____Vlvbound_h8dbb67ef__0 = 1U;
        if ((2U >= (3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index))) {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i
                [(3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index)];
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT____Vlvbound_h8dbb67ef__0;
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3 
                = (3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index);
            __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3 = 1U;
        } else {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0 = 0U;
        }
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_en_r) {
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_addr_r;
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0 = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT____Vlvbound_h8dbb67ef__0 = 1U;
        if ((2U >= (3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index))) {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i
                [(3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index)];
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT____Vlvbound_h8dbb67ef__0;
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3 
                = (3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index);
            __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3 = 1U;
        } else {
            __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0 = 0U;
        }
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r) {
        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_data_i
            [(1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_index)];
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_addr_r;
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0 = 1U;
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r__v2 
            = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_index);
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r__v2 = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r) {
        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i
            [(3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index)];
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r;
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0 = 1U;
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r__v4 
            = (3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index);
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r__v4 = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r) {
        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i
            [(3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index)];
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r;
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0 = 1U;
        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r__v4 
            = (3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index);
        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r__v4 = 1U;
    }
    if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_en_r)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__i 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_addr_r;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__data_curr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r
            [(0x1fU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__i)];
        if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_en_r) {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT____Vlvbound_hd935d61a__0 = 1U;
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT____Vlvbound_h3d041937__0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
            if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_index))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT____Vlvbound_hd935d61a__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1 = 1U;
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT____Vlvbound_h3d041937__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0 = 1U;
            }
        } else {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_a_r 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
        }
    }
    if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_en_r)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__i 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_addr_r;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__data_curr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r
            [(0x1fU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__i)];
        if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_en_r) {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT____Vlvbound_hd935d61a__0 = 1U;
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT____Vlvbound_h3d041937__0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
            if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_index))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT____Vlvbound_hd935d61a__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1 = 1U;
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT____Vlvbound_h3d041937__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0 = 1U;
            }
        } else {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_a_r 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
        }
    }
    if ((1U & (~ (IData)(vlSelfRef.reset_n_i)))) {
        vlSelfRef.irq_n_o = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r
        [0U];
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
        [2U];
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 0U;
    } else if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
        [3U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
        [3U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
        [2U];
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r) {
        vlSelfRef.doc5503_harness__DOT__dbg_ready = 1U;
        vlSelfRef.doc5503_harness__DOT__dbg_osc_state 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r;
        vlSelfRef.doc5503_harness__DOT__dbg_cycle_max 
            = (0x1fU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r) 
                        >> 1U));
    } else {
        vlSelfRef.doc5503_harness__DOT__dbg_ready = 0U;
    }
    if (vlSelfRef.reset_n_i) {
        if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r)))) {
            __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r 
                = (0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r)));
            if ((0x1fU == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r))) {
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__ready_r = 1U;
            }
        }
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__prev_cs_n_r 
            = vlSelfRef.cs_n_i;
        if ((2U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r))) {
            if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 
                    = ((0x8000U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r
                                   [vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r] 
                                   >> 8U)) | (0x7fffU 
                                              & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r
                                                 [vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r] 
                                                 >> 3U)));
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r;
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0 = 1U;
                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r 
                    = ((0x8000U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
                                   >> 8U)) | (0x7fffU 
                                              & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
                                                 >> 3U)));
                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r 
                    = ((0x8000U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
                                   >> 8U)) | (0x7fffU 
                                              & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
                                                 >> 3U)));
                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r 
                    = ((0x8000U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
                                   >> 8U)) | (0x7fffU 
                                              & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
                                                 >> 3U)));
                if ((0xfU == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r))) {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r = 1U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r = 0U;
                } else {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r 
                        = (0xfU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r)));
                }
            } else {
                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk5__DOT__ca 
                    = (0xfU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_b_r) 
                               >> 4U));
                if ((1U & (~ (IData)((0U != (1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_b_r))))))) {
                    if ((0xfU > (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk5__DOT__ca))) {
                        __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 
                            = (0xffffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r
                                            [vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk5__DOT__ca] 
                                            + VL_EXTENDS_II(24,16, (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_b_r))));
                        __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 
                            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk5__DOT__ca;
                        __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0 = 1U;
                    }
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
                        = (0xffffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r 
                                        + VL_EXTENDS_II(24,16, (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_b_r))));
                    if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk5__DOT__ca))) {
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
                            = (0xffffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r 
                                            + VL_EXTENDS_II(24,16, (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_b_r))));
                    } else {
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
                            = (0xffffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r 
                                            + VL_EXTENDS_II(24,16, (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_b_r))));
                    }
                }
                if ((0U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r))) {
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_out_r 
                        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_b_r;
                }
                if (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r) 
                     == (0x1fU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r) 
                                  >> 1U)))) {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r = 0U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r = 3U;
                } else {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r 
                        = (0x3fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r)));
                }
            }
        } else if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r))) {
            __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r = 0U;
            if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_trigger_r) {
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r = 0U;
            }
            __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r = 0U;
        } else {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r = 0U;
            __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r = 0x1fU;
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r = 0U;
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r = 0U;
            if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_reset_r) {
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v1 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r;
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v1 = 1U;
            }
            __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r;
            __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1 = 1U;
            if ((0xfU == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r))) {
                if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_reset_r) {
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_reset_r = 0U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r = 1U;
                } else {
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r = 2U;
                }
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r = 0U;
            } else {
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r 
                    = (0xfU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r)));
            }
        }
        if (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__req_cs_w) 
             & (~ (IData)(vlSelfRef.we_n_i)))) {
            if ((7U == (7U & ((IData)(vlSelfRef.addr_i) 
                              >> 5U)))) {
                if ((0U == (0x1fU & (IData)(vlSelfRef.addr_i)))) {
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r 
                        = vlSelfRef.data_i;
                }
                if ((0U != (0x1fU & (IData)(vlSelfRef.addr_i)))) {
                    if ((1U == (0x1fU & (IData)(vlSelfRef.addr_i)))) {
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r 
                            = vlSelfRef.data_i;
                    }
                }
            }
        }
        if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_update_req) {
            __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r 
                = (0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r)));
        }
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_trigger_r = 0U;
        if (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r) 
             & (IData)(vlSelfRef.clk_en_i))) {
            if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_step_w) {
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_r 
                    = (0x1fU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_w)
                                 ? 0U : ((IData)(1U) 
                                         + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r))));
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r = 0U;
            } else {
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_r 
                    = (0x1fU & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r));
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r 
                    = (0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r)));
            }
            if (((1U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r)) 
                 & (0U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)))) {
                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__active_osc_count_r 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r;
                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r = 0U;
            }
            if (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_w) 
                 & (8U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r)))) {
                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_trigger_r = 1U;
            }
        }
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_am_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_wds_reset_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_add_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_reset_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_acc_reset_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_load_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_load_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_osc_halt_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_unhalt_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_reset_req = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_update_req = 0U;
        if (vlSelfRef.clk_en_i) {
            if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r) {
                if ((4U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                    if ((2U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                        if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r = 0U;
                            if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_step_w) {
                                __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 1U;
                            }
                        } else {
                            if ((IData)(((4U == (6U 
                                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r))) 
                                         & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)))) {
                                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_reset_req = 1U;
                                if (((0x1fU > (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)) 
                                     & (~ vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                        [2U]))) {
                                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_am_req_r = 1U;
                                }
                            } else {
                                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_update_req = 1U;
                            }
                            __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                        }
                    } else if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_wds_reset_req_r = 1U;
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_osc_halt_req_r = 1U;
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                        if ((3U == (3U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r) 
                                          >> 1U)))) {
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_unhalt_req_r = 1U;
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_acc_reset_req_r = 1U;
                        } else if (((3U == (3U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                                  [1U] 
                                                  >> 1U))) 
                                    & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)))) {
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_osc_halt_req_r = 0U;
                        }
                    } else {
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__high_bit_w 
                            = ((7U == (7U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r) 
                                             >> 3U)))
                                ? 0xfU : (8U | (7U 
                                                & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r) 
                                                   >> 3U))));
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__overflow 
                            = (1U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_addr_r) 
                                     >> (0xfU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__high_bit_w)));
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__zero_byte_w 
                            = (0U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_a_r));
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_load_req_r = 1U;
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_load_req_r = 1U;
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r = 1U;
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 6U;
                        if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__zero_byte_w)))) {
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_hi_mask_w 
                                = (0xffU & ((IData)(0xffU) 
                                            << (7U 
                                                & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r) 
                                                   >> 3U))));
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_w 
                                = (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_hi_mask_w) 
                                    & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_a_r)) 
                                   << 8U);
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__addr_w 
                                = (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__overflow)
                                     ? 0U : (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_addr_r)) 
                                   | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_w));
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o = 1U;
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r 
                                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o 
                                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__addr_w;
                        }
                        if (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__overflow) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__zero_byte_w))) {
                            if ((IData)(((0U != (6U 
                                                 & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r))) 
                                         | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__zero_byte_w)))) {
                                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_reset_req_r = 1U;
                                if (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__zero_byte_w) 
                                     | (1U == (3U & 
                                               ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r) 
                                                >> 1U))))) {
                                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 5U;
                                }
                            } else {
                                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_reset_req_r = 0U;
                            }
                            if ((2U == (3U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r) 
                                              >> 1U)))) {
                                if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)))) {
                                    if ((1U & (~ vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                               [1U]))) {
                                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_acc_reset_req_r = 1U;
                                    }
                                }
                            }
                        }
                    }
                } else if ((2U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                    if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk2__DOT__shift_w 
                            = (0x1fU & (((IData)(9U) 
                                         + (7U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r))) 
                                        - (7U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r) 
                                                 >> 3U))));
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_addr_r 
                            = (0xffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_a_r 
                                          >> (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk2__DOT__shift_w)));
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 4U;
                    } else if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r))) {
                        if ((2U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r))) {
                            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_reset_req_r = 1U;
                        }
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_reset_req = 1U;
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 0U;
                    } else {
                        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_add_req_r = 1U;
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 3U;
                    }
                } else if ((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r))) {
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_load_req_r = 1U;
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_load_req_r = 1U;
                    __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 2U;
                } else {
                    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r = 0U;
                    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_step_w) {
                        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r = 1U;
                    }
                }
            }
        }
    } else {
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__ready_r = 0U;
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r = 0U;
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r = 0U;
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_reset_r = 1U;
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r = 0U;
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__active_osc_count_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r = 0x3eU;
        __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_am_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_wds_reset_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_add_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_reset_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_acc_reset_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_load_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_load_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_osc_halt_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_unhalt_req_r = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_reset_req = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_update_req = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r = 0U;
    }
    if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_en_r)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__i 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_addr_r;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__data_curr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r
            [(0x1fU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__i)];
        if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_en_r) {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT____Vlvbound_hd935d61a__0 = 1U;
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT____Vlvbound_h3d041937__0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
            if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_index))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT____Vlvbound_hd935d61a__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1 = 1U;
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT____Vlvbound_h3d041937__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0 = 1U;
            }
        } else {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_a_r 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
        }
    }
    if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_en_r)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__i 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_addr_r;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__data_curr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r
            [(0x1fU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__i)];
        if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_en_r) {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT____Vlvbound_hd935d61a__0 = 1U;
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT____Vlvbound_h3d041937__0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
            if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_index))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT____Vlvbound_hd935d61a__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1 = 1U;
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT____Vlvbound_h3d041937__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0 = 1U;
            }
        } else {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
        }
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r) {
        vlSelfRef.doc5503_harness__DOT__dbg_vol = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_a_r;
        vlSelfRef.doc5503_harness__DOT__dbg_wds = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_a_r;
        vlSelfRef.doc5503_harness__DOT__dbg_cycle = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
        vlSelfRef.doc5503_harness__DOT__dbg_acc = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_a_r;
        vlSelfRef.doc5503_harness__DOT__dbg_control 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r;
    }
    if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__i 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__data_curr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r
            [(0x1fU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__i)];
        if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r) {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT____Vlvbound_h8a8ade32__0 = 1U;
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT____Vlvbound_hdf6ad119__0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
            if ((2U >= (3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT____Vlvbound_h8a8ade32__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3 
                    = (3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3 = 1U;
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT____Vlvbound_hdf6ad119__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0 
                    = (3U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0 = 1U;
            }
        } else {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
        }
    }
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r) {
        vlSelfRef.doc5503_harness__DOT__dbg_output 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_r;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
        [3U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
        [3U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r[0U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r[0U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r[0U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r[0U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r[0U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r[0U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r[0U] = 0U;
    if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r)))) {
        if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_en_r) {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT____Vlvbound_hd935d61a__0 = 1U;
            if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_index))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT____Vlvbound_hd935d61a__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1 = 1U;
            }
        }
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r[0U] = 0U;
    if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r)))) {
        if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_en_r) {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT____Vlvbound_hd935d61a__0 = 1U;
            if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_index))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT____Vlvbound_hd935d61a__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1 = 1U;
            }
        }
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r[0U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r[0U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r__v1;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[0U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[1U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[2U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r__v3;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[0U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[1U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r__v2) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r__v2] = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[0U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[1U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[2U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r__v3;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[0U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[1U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[2U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r__v3;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r__v0;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[0U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[1U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[2U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[3U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r__v4) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r__v4] = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[0U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[1U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[2U] = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[3U] = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r__v4) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r__v4] = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r__v1] = 0U;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v1) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__channel_r__v1] = 0U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
           [3U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
           [3U]);
    vlSelfRef.dbg_osc_state_o = vlSelfRef.doc5503_harness__DOT__dbg_osc_state;
    vlSelfRef.dbg_vol_o = vlSelfRef.doc5503_harness__DOT__dbg_vol;
    vlSelfRef.dbg_wds_o = vlSelfRef.doc5503_harness__DOT__dbg_wds;
    vlSelfRef.dbg_cycle_o = vlSelfRef.doc5503_harness__DOT__dbg_cycle;
    vlSelfRef.dbg_output_o = vlSelfRef.doc5503_harness__DOT__dbg_output;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__data_w 
        = (0x80U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_a_r));
    if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__i 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_addr_r;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__data_curr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r
            [(0x1fU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__i)];
        if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_en_r) {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT____Vlvbound_h3d041937__0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
            if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_index))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT____Vlvbound_h3d041937__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0 = 1U;
            }
        } else {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_a_r 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
        }
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__vol_s 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_a_r;
    if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__i 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_addr_r;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__data_curr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r
            [(0x1fU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__i)];
        if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_en_r) {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT____Vlvbound_h3d041937__0 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
            if ((0U >= (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_index))) {
                __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0 
                    = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT____Vlvbound_h3d041937__0;
                __VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0 
                    = (1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_index);
                __VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0 = 1U;
            }
        } else {
            vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_a_r 
                = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
        }
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_r 
        = (0xffffU & VL_MULS_III(16, (0xffffU & VL_EXTENDS_II(16,8, (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__data_w))), 
                                 (0xffffU & VL_EXTENDS_II(16,9, (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__vol_s)))));
    vlSelfRef.mono_mix_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r;
    vlSelfRef.left_mix_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r;
    vlSelfRef.right_mix_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_b_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r
        [(0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r)))];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_b_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r
        [(0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r)))];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xfU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xfU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xeU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xeU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xdU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xdU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xcU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xcU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xbU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xbU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0xaU] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0xaU];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[9U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [9U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[8U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [8U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[7U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [7U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[6U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [6U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[5U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [5U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[4U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [4U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[3U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [3U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[2U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r__v0;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__req_cs_w 
        = ((~ (IData)(vlSelfRef.cs_n_i)) & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__prev_cs_n_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__ready_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_addr_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_addr_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[2U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_r;
    vlSelfRef.wave_rd_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o;
    vlSelfRef.wave_address_o = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[2U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_am_req_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_reset_req;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[2U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_update_req;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_add_req_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_reset_req_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[2U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_acc_reset_req_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_load_req_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i[2U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_load_req_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_wds_reset_req_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_osc_halt_req_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[2U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_unhalt_req_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[0U] 
        = (1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r)));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[0U] 
        = (1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r)));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[0U] 
        = (1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r)));
    if ((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_en_r)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__i 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__data_curr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r
            [(0x1fU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__i)];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_a_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__data_curr_r;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_index = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r__v0;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
           [0U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_index = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r__v0;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_en_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_en_r = 1U;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r__v0;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_index = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r__v0;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_en_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_en_r = 1U;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r__v0;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_index = 0U;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r__v0;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
           [3U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
           [1U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
           [2U]);
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w 
        = ((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r)) 
           & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
           [3U]);
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r__v0;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r 
        = __Vdly__doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r__v0;
    }
    if (__VdlySet__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[__VdlyDim0__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0] 
            = __VdlyVal__doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r__v0;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w[1U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[1U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[2U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[0U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[1U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[2U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_data_i[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_a_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[1U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[2U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[1U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[2U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[3U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[1U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[2U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[3U] 
        = (((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
            & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
           | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[0U] 
        = (0xffffffU & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_a_r 
                        + (((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_a_r) 
                            << 8U) | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_a_r))));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_addr_i
            [0U];
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_w 
        = ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r) 
           == (0x1fU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r) 
                        >> 1U)));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[2U] 
        = (1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[2U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i[1U] 
        = (1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i[2U] 
        = (0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_addr_i[0U] 
        = (0x1fU & ((IData)(1U) + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[2U] 
        = (1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[1U] 
        = (1U | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[0U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r
        [0U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[1U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r
        [1U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[2U] 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r
        [2U];
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_step_w 
        = ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_w)
            ? (0x17U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r))
            : (7U == (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r)));
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w
         [1U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i
            [1U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w
         [2U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index = 2U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i
            [2U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w
         [1U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i
            [1U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w
         [2U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index = 2U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i
            [2U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w
         [1U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i
            [1U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w
         [2U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index = 2U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i
            [2U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r;
    if (((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w
          [1U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__found))) 
         & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_index = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_addr_i
            [1U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w
         [1U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i
            [1U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w
         [2U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index = 2U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i
            [2U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r;
    if (((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w
          [3U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found))) 
         & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index = 3U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i
            [3U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index = 0U;
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r = 0U;
    if (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w
        [0U]) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index = 0U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i
            [0U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w
         [1U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i
            [1U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    if ((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w
         [2U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index = 2U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i
            [2U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en 
        = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r;
    if (((vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w
          [3U] & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found))) 
         & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en)))) {
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r = 1U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index = 3U;
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r 
            = vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i
            [3U];
        vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found = 1U;
    }
    vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[2U] 
        = (0xfeU & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
           [1U]);
    vlSelfRef.data_o = ((0U == (7U & ((IData)(vlSelfRef.addr_i) 
                                      >> 5U))) ? vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o
                        [0U] : ((1U == (7U & ((IData)(vlSelfRef.addr_i) 
                                              >> 5U)))
                                 ? vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o
                                [0U] : ((2U == (7U 
                                                & ((IData)(vlSelfRef.addr_i) 
                                                   >> 5U)))
                                         ? vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o
                                        [0U] : ((3U 
                                                 == 
                                                 (7U 
                                                  & ((IData)(vlSelfRef.addr_i) 
                                                     >> 5U)))
                                                 ? 
                                                vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o
                                                [0U]
                                                 : 
                                                ((4U 
                                                  == 
                                                  (7U 
                                                   & ((IData)(vlSelfRef.addr_i) 
                                                      >> 5U)))
                                                  ? 
                                                 vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o
                                                 [0U]
                                                  : 
                                                 ((5U 
                                                   == 
                                                   (7U 
                                                    & ((IData)(vlSelfRef.addr_i) 
                                                       >> 5U)))
                                                   ? 
                                                  vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                                  [0U]
                                                   : 
                                                  ((6U 
                                                    == 
                                                    (7U 
                                                     & ((IData)(vlSelfRef.addr_i) 
                                                        >> 5U)))
                                                    ? 
                                                   vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o
                                                   [0U]
                                                    : 
                                                   ((0xe0U 
                                                     == (IData)(vlSelfRef.addr_i))
                                                     ? (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r)
                                                     : 
                                                    ((0xe1U 
                                                      == (IData)(vlSelfRef.addr_i))
                                                      ? (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r)
                                                      : 0U)))))))));
}

void Vdoc5503_harness___024root___eval_triggers__act(Vdoc5503_harness___024root* vlSelf);

bool Vdoc5503_harness___024root___eval_phase__act(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_phase__act\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<1> __VpreTriggered;
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
            VL_FATAL_MT("harness/doc5503_harness.sv", 4, "", "Input combinational region did not converge.");
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
            VL_FATAL_MT("harness/doc5503_harness.sv", 4, "", "NBA region did not converge.");
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
                VL_FATAL_MT("harness/doc5503_harness.sv", 4, "", "Active region did not converge.");
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
