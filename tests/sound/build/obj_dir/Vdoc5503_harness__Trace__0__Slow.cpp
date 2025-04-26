// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vdoc5503_harness__Syms.h"


VL_ATTR_COLD void Vdoc5503_harness___024root__trace_init_sub__TOP__0(Vdoc5503_harness___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root__trace_init_sub__TOP__0\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    const int c = vlSymsp->__Vm_baseCode;
    // Body
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+898,0,"reset_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+899,0,"clk_en_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+900,0,"irq_n_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+901,0,"cs_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+902,0,"we_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+903,0,"addr_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+904,0,"data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+905,0,"data_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+906,0,"wave_address_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+907,0,"wave_rd_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+908,0,"wave_data_ready_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+909,0,"wave_data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+910,0,"mono_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+911,0,"left_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+912,0,"right_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->pushPrefix("doc5503_harness", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+898,0,"reset_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+899,0,"clk_en_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+900,0,"irq_n_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+901,0,"cs_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+902,0,"we_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+903,0,"addr_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+904,0,"data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+905,0,"data_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+906,0,"wave_address_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+907,0,"wave_rd_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+908,0,"wave_data_ready_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+909,0,"wave_data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+910,0,"mono_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+911,0,"left_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+912,0,"right_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+213,0,"dbg_cycle",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+214,0,"dbg_cycle_max",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+215,0,"dbg_osc_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+216,0,"dbg_osc_state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+217,0,"dbg_ready",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+218,0,"dbg_output",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+219,0,"dbg_wds",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+220,0,"dbg_vol",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+221,0,"dbg_control",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+222,0,"dbg_acc",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->pushPrefix("doc5503_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+913,0,"OUTPUT_CHANNEL_MIX",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+914,0,"NUM_CHANNELS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"OUTPUT_MONO_MIX",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"OUTPUT_STEREO_MIX",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+915,0,"CHANNEL_MAX",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+898,0,"reset_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+899,0,"clk_en_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+900,0,"irq_n_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+901,0,"cs_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+902,0,"we_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+903,0,"addr_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+904,0,"data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+905,0,"data_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+223,0,"wave_address_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+224,0,"wave_rd_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+908,0,"wave_data_ready_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+909,0,"wave_data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+910,0,"mono_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+911,0,"left_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+912,0,"right_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+225,0,"osc_1_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->pushPrefix("channel_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 16; ++i) {
        tracep->declBus(c+226+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+215,0,"osc_en_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+242,0,"osc_int_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+215,0,"osc_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+243,0,"f_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+244,0,"vol_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+245,0,"wds_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+246,0,"wtp_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+247,0,"rts_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+248,0,"control_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+249,0,"partner_control_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+250,0,"next_control_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+251,0,"fl_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+252,0,"fh_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+253,0,"vol_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+254,0,"wds_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+255,0,"wtp_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+256,0,"control_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+257,0,"rts_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+258,0,"odd_osc_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+259,0,"even_osc_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+260,0,"partner_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+261,0,"wts_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+262,0,"res_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+263,0,"halt_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+264,0,"mode_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+265,0,"partner_halt_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+266,0,"partner_mode_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+267,0,"next_halt_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+268,0,"next_mode_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+269,0,"acc_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+270,0,"wave_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+916,0,"MODE_FREE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+917,0,"MODE_ONESHOT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+918,0,"MODE_SYNC_AM",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+919,0,"MODE_SWAP",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+271,0,"cycle_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+272,0,"cycle_step_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+273,0,"osc_en_num_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+273,0,"cycle_max_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+274,0,"mixer_cycle_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+275,0,"mixer_control_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+276,0,"mixer_output_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+277,0,"last_osc_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+278,0,"ready_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+279,0,"init_cycle_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+280,0,"last_cycle_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"last_cycle_step_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"inhibit_host_writes_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+283,0,"next_am_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+284,0,"current_wds_reset_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+285,0,"current_osc_halt_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+286,0,"partner_unhalt_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+287,0,"partner_control_load_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+288,0,"next_control_load_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+289,0,"current_acc_add_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+290,0,"current_acc_reset_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+291,0,"partner_acc_reset_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+292,0,"output_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+293,0,"output_reset_req",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+294,0,"output_update_req",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+295,0,"prev_cs_n_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+45,0,"req_cs_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+920,0,"OSC_IDLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+921,0,"OSC_START",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+922,0,"OSC_ACC",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+923,0,"OSC_ADDR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+924,0,"OSC_READ_DATA",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+925,0,"OSC_HALT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+926,0,"OSC_OUT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+296,0,"osc_state_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+914,0,"MIXER_SUM_RESOLUTION",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+297,0,"mono_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+298,0,"next_mono_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+299,0,"left_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+300,0,"next_left_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+301,0,"right_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+302,0,"next_right_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+225,0,"osc_1_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->pushPrefix("channel_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 16; ++i) {
        tracep->declBus(c+303+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("next_channel_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 16; ++i) {
        tracep->declBus(c+319+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+916,0,"MIXER_INIT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+917,0,"MIXER_ZERO",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+918,0,"MIXER_ADD",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+919,0,"MIXER_OUTPUT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+335,0,"mixer_state_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+336,0,"mixer_channel_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->pushPrefix("osc_acc_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+927,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+929,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+930,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+337+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+340+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+343+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 23,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+1+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+2+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+3+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 23,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+271,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+269,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+931,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+932,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+346+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+349+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+933+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+934+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+935+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 23,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+352+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 23,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+384,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+385,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+386,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+936,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+937,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+271,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+938,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+269,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+939,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+936,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+937,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+387,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+940,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+936,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+388,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+389,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+390,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+391,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+392,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+393,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+394,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+395,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+396,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+397,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+398,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+399,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+400,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+401,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+402,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+403,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[2]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+404,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+405,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+406,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+407,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+408,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+409,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+410,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+411,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+412,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+413,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+414,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+415,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_control_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+941,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+927,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+929,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+942,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBit(c+46+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBus(c+50+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBus(c+54+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+58+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+61+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+416+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+271,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+248,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+419,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+275,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBit(c+64+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBit(c+420+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+68+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+424+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+427+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+430+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+71,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+72,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+73,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+74,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+75,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+76,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+938,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+248,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+275,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+77,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+462,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+78,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+463,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+464,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+465,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+79,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+466,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+80,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+467,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+81,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+468,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+469,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+470,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+82,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+471,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_port_loop[2]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+83,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+472,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+84,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+473,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+474,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+475,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+85,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+476,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+86,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+940,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+87,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+943,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+88,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+89,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+477,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+90,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+478,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+479,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+480,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+91,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+481,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[2]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+92,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+482,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+93,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+483,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+484,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+485,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+94,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+486,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[3]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+95,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+487,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+96,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+488,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+489,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+490,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+97,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+491,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+492,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+493,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+494,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+495,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_fh_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+913,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+929,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+942,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+7+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+8+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+9+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+10+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+11+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+496+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+271,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+497,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+931,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+944,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+98+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+498+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+99+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+499+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+500+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+501+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+100,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+101,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+102,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+103,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+104,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+105,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+938,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+497,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+945,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+12,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+533,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+106,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+534,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+535,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+536,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+107,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+537,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+108,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+946,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+109,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+946,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+936,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+13,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+538,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+110,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+539,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+540,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+541,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+111,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+542,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+543,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+544,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+545,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+546,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_fl_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+913,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+929,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+942,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+14+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+15+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+16+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+17+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+18+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+547+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+271,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+548,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+931,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+944,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+112+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+549+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+113+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+550+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+551+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+552+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+114,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+115,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+116,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+117,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+118,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+119,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+938,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+548,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+947,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+19,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+584,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+120,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+585,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+586,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+587,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+121,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+588,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+122,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+946,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+123,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+946,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+936,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+20,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+589,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+124,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+590,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+591,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+592,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+125,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+593,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+594,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+595,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+596,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+597,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_output_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+927,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+929,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+914,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+598+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+601+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+604+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+4+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+5+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+6+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+931,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+948,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+419,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+276,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+607+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+610+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+949+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+950+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+951+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+613+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+645,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+646,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+647,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+936,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+937,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+931,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+952,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+953,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+276,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+936,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+937,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+648,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+940,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+936,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+649,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+650,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+651,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+652,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+653,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+654,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+655,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+656,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[2]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+657,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+658,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+659,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+660,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+661,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+662,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+663,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+664,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+665,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+666,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_rts_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+913,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+929,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+942,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+21+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+22+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+23+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+24+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+25+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+667+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+271,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+247,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+931,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+944,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+126+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+668+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+127+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+669+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+670+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+671+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+128,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+129,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+130,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+131,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+132,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+133,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+938,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+247,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+954,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+26,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+703,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+134,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+704,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+705,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+706,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+135,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+707,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+136,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+946,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+137,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+946,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+936,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+27,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+708,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+138,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+709,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+710,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+711,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+139,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+712,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+713,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+714,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+715,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+716,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_vol_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+955,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+929,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+942,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 2; ++i) {
        tracep->declBit(c+140+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 2; ++i) {
        tracep->declBus(c+142+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 2; ++i) {
        tracep->declBus(c+144+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+28+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+29+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+717+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+271,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+244,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+931,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+944,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 2; ++i) {
        tracep->declBit(c+146+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 2; ++i) {
        tracep->declBit(c+718+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+148+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+720+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+721+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+722+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+149,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+150,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+151,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+152,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+153,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+154,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+938,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+244,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+956,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+30,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+754,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+155,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+755,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+756,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+757,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+156,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+758,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+157,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+946,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+158,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+957,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+159,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+160,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+759,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+161,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+760,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+761,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+762,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+162,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+763,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+163,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+764,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+164,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+765,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+766,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+767,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+165,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+768,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+769,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+770,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+771,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+772,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_wds_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+941,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+929,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+942,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBit(c+166+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBus(c+170+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBus(c+31+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+35+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+36+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+773+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+271,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+245,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+931,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+944,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBit(c+174+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBit(c+774+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+178+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+778+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+779+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+780+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+179,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+180,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+181,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+182,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+183,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+184,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+938,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+245,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+958,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+37,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+812,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+185,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+813,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+814,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+815,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+186,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+816,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+187,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+946,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+188,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+943,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+189,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+190,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+817,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+191,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+818,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+819,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+820,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+192,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+821,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[2]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+193,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+822,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+194,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+823,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+824,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+825,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+195,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+826,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[3]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+196,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+827,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+197,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+828,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+829,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+830,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+198,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+831,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+832,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+833,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+834,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+835,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_wtp_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+913,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+913,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+928,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+929,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+942,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+897,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+38+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+39+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+40+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+41+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+42+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+836+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+271,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+246,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+931,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+944,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+199+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+837+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+200+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+838+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+839+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+840+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+201,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+202,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+203,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+204,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+205,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+206,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+938,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+246,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+959,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+43,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+872,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+207,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+873,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+874,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+875,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+208,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+876,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+209,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+946,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+210,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+946,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+936,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+897,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+44,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+877,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+211,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+878,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+879,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+880,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+212,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+881,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+882,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+883,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+884,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+885,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+886,0,"data_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+887,0,"vol_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+888,0,"mult_result",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 17,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+889,0,"shift_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+890,0,"high_bit_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+891,0,"overflow",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+892,0,"zero_byte_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+893,0,"ptr_hi_mask_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+894,0,"ptr_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+895,0,"addr_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+896,0,"ca",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
}

VL_ATTR_COLD void Vdoc5503_harness___024root__trace_init_top(Vdoc5503_harness___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root__trace_init_top\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vdoc5503_harness___024root__trace_init_sub__TOP__0(vlSelf, tracep);
}

VL_ATTR_COLD void Vdoc5503_harness___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
VL_ATTR_COLD void Vdoc5503_harness___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vdoc5503_harness___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vdoc5503_harness___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/);

VL_ATTR_COLD void Vdoc5503_harness___024root__trace_register(Vdoc5503_harness___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root__trace_register\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    tracep->addConstCb(&Vdoc5503_harness___024root__trace_const_0, 0U, vlSelf);
    tracep->addFullCb(&Vdoc5503_harness___024root__trace_full_0, 0U, vlSelf);
    tracep->addChgCb(&Vdoc5503_harness___024root__trace_chg_0, 0U, vlSelf);
    tracep->addCleanupCb(&Vdoc5503_harness___024root__trace_cleanup, vlSelf);
}

VL_ATTR_COLD void Vdoc5503_harness___024root__trace_const_0_sub_0(Vdoc5503_harness___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vdoc5503_harness___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root__trace_const_0\n"); );
    // Init
    Vdoc5503_harness___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vdoc5503_harness___024root*>(voidSelf);
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vdoc5503_harness___024root__trace_const_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vdoc5503_harness___024root__trace_const_0_sub_0(Vdoc5503_harness___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root__trace_const_0_sub_0\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullIData(oldp+913,(1U),32);
    bufp->fullIData(oldp+914,(0x10U),32);
    bufp->fullIData(oldp+915,(0xfU),32);
    bufp->fullCData(oldp+916,(0U),2);
    bufp->fullCData(oldp+917,(1U),2);
    bufp->fullCData(oldp+918,(2U),2);
    bufp->fullCData(oldp+919,(3U),2);
    bufp->fullCData(oldp+920,(0U),3);
    bufp->fullCData(oldp+921,(1U),3);
    bufp->fullCData(oldp+922,(2U),3);
    bufp->fullCData(oldp+923,(3U),3);
    bufp->fullCData(oldp+924,(4U),3);
    bufp->fullCData(oldp+925,(5U),3);
    bufp->fullCData(oldp+926,(6U),3);
    bufp->fullIData(oldp+927,(3U),32);
    bufp->fullIData(oldp+928,(0U),32);
    bufp->fullIData(oldp+929,(5U),32);
    bufp->fullIData(oldp+930,(0x18U),32);
    bufp->fullCData(oldp+931,(0U),5);
    bufp->fullIData(oldp+932,(0U),24);
    bufp->fullBit(oldp+933,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+934,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullIData(oldp+935,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_read_data_r[0]),24);
    bufp->fullBit(oldp+936,(0U));
    bufp->fullIData(oldp+937,(0U),32);
    bufp->fullBit(oldp+938,(1U));
    bufp->fullIData(oldp+939,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_b_r),24);
    bufp->fullIData(oldp+940,(3U),32);
    bufp->fullIData(oldp+941,(4U),32);
    bufp->fullIData(oldp+942,(8U),32);
    bufp->fullIData(oldp+943,(4U),32);
    bufp->fullCData(oldp+944,(0U),8);
    bufp->fullCData(oldp+945,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_b_r),8);
    bufp->fullIData(oldp+946,(1U),32);
    bufp->fullCData(oldp+947,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_b_r),8);
    bufp->fullSData(oldp+948,(0U),16);
    bufp->fullBit(oldp+949,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+950,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullSData(oldp+951,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_read_data_r[0]),16);
    bufp->fullBit(oldp+952,(0U));
    bufp->fullSData(oldp+953,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_a_r),16);
    bufp->fullCData(oldp+954,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_b_r),8);
    bufp->fullIData(oldp+955,(2U),32);
    bufp->fullCData(oldp+956,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_b_r),8);
    bufp->fullIData(oldp+957,(2U),32);
    bufp->fullCData(oldp+958,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_b_r),8);
    bufp->fullCData(oldp+959,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_b_r),8);
}

VL_ATTR_COLD void Vdoc5503_harness___024root__trace_full_0_sub_0(Vdoc5503_harness___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vdoc5503_harness___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root__trace_full_0\n"); );
    // Init
    Vdoc5503_harness___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vdoc5503_harness___024root*>(voidSelf);
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vdoc5503_harness___024root__trace_full_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vdoc5503_harness___024root__trace_full_0_sub_0(Vdoc5503_harness___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root__trace_full_0_sub_0\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullBit(oldp+1,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_read_req_i[0]));
    bufp->fullCData(oldp+2,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_read_addr_i[0]),5);
    bufp->fullIData(oldp+3,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_read_data_o[0]),24);
    bufp->fullBit(oldp+4,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_read_req_i[0]));
    bufp->fullCData(oldp+5,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_read_addr_i[0]),5);
    bufp->fullSData(oldp+6,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_read_data_o[0]),16);
    bufp->fullBit(oldp+7,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_req_i[0]));
    bufp->fullCData(oldp+8,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+9,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_data_i[0]),8);
    bufp->fullBit(oldp+10,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_req_i[0]));
    bufp->fullCData(oldp+11,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_addr_i[0]),5);
    bufp->fullBit(oldp+12,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_read_req_i
                           [0U]));
    bufp->fullBit(oldp+13,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fh_r_inst__priority_write_req_i
                           [0U]));
    bufp->fullBit(oldp+14,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_req_i[0]));
    bufp->fullCData(oldp+15,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+16,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_data_i[0]),8);
    bufp->fullBit(oldp+17,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_req_i[0]));
    bufp->fullCData(oldp+18,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_addr_i[0]),5);
    bufp->fullBit(oldp+19,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_read_req_i
                           [0U]));
    bufp->fullBit(oldp+20,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_fl_r_inst__priority_write_req_i
                           [0U]));
    bufp->fullBit(oldp+21,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_req_i[0]));
    bufp->fullCData(oldp+22,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+23,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_data_i[0]),8);
    bufp->fullBit(oldp+24,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_req_i[0]));
    bufp->fullCData(oldp+25,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_addr_i[0]),5);
    bufp->fullBit(oldp+26,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_read_req_i
                           [0U]));
    bufp->fullBit(oldp+27,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_rts_r_inst__priority_write_req_i
                           [0U]));
    bufp->fullBit(oldp+28,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_req_i[0]));
    bufp->fullCData(oldp+29,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_addr_i[0]),5);
    bufp->fullBit(oldp+30,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_read_req_i
                           [0U]));
    bufp->fullCData(oldp+31,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i[0]),8);
    bufp->fullCData(oldp+32,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i[1]),8);
    bufp->fullCData(oldp+33,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i[2]),8);
    bufp->fullCData(oldp+34,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_data_i[3]),8);
    bufp->fullBit(oldp+35,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_req_i[0]));
    bufp->fullCData(oldp+36,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_addr_i[0]),5);
    bufp->fullBit(oldp+37,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_read_req_i
                           [0U]));
    bufp->fullBit(oldp+38,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_req_i[0]));
    bufp->fullCData(oldp+39,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+40,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_data_i[0]),8);
    bufp->fullBit(oldp+41,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_req_i[0]));
    bufp->fullCData(oldp+42,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_addr_i[0]),5);
    bufp->fullBit(oldp+43,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_read_req_i
                           [0U]));
    bufp->fullBit(oldp+44,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wtp_r_inst__priority_write_req_i
                           [0U]));
    bufp->fullBit(oldp+45,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__req_cs_w));
    bufp->fullBit(oldp+46,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[0]));
    bufp->fullBit(oldp+47,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[1]));
    bufp->fullBit(oldp+48,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[2]));
    bufp->fullBit(oldp+49,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i[3]));
    bufp->fullCData(oldp+50,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+51,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[1]),5);
    bufp->fullCData(oldp+52,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[2]),5);
    bufp->fullCData(oldp+53,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_addr_i[3]),5);
    bufp->fullCData(oldp+54,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[0]),8);
    bufp->fullCData(oldp+55,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[1]),8);
    bufp->fullCData(oldp+56,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[2]),8);
    bufp->fullCData(oldp+57,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_data_i[3]),8);
    bufp->fullBit(oldp+58,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i[0]));
    bufp->fullBit(oldp+59,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i[1]));
    bufp->fullBit(oldp+60,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i[2]));
    bufp->fullCData(oldp+61,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i[0]),5);
    bufp->fullCData(oldp+62,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i[1]),5);
    bufp->fullCData(oldp+63,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_addr_i[2]),5);
    bufp->fullBit(oldp+64,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+65,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[1]));
    bufp->fullBit(oldp+66,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[2]));
    bufp->fullBit(oldp+67,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_w[3]));
    bufp->fullBit(oldp+68,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+69,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[1]));
    bufp->fullBit(oldp+70,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_w[2]));
    bufp->fullBit(oldp+71,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+72,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+73,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+74,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_en_r));
    bufp->fullIData(oldp+75,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_index),32);
    bufp->fullCData(oldp+76,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_addr_r),5);
    bufp->fullBit(oldp+77,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
                           [0U]));
    bufp->fullBit(oldp+78,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                             & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                            | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
    bufp->fullBit(oldp+79,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+80,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
                           [1U]));
    bufp->fullBit(oldp+81,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_r_w)) 
                             & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_s_w)) 
                            | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__q_r))));
    bufp->fullBit(oldp+82,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+83,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_read_req_i
                           [2U]));
    bufp->fullBit(oldp+84,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_r_w)) 
                             & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_s_w)) 
                            | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__q_r))));
    bufp->fullBit(oldp+85,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+86,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ctrl__DOT__found));
    bufp->fullBit(oldp+87,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+88,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en));
    bufp->fullBit(oldp+89,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
                           [1U]));
    bufp->fullBit(oldp+90,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                             & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                            | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+91,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+92,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
                           [2U]));
    bufp->fullBit(oldp+93,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                             & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                            | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+94,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+95,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_control_r_inst__priority_write_req_i
                           [3U]));
    bufp->fullBit(oldp+96,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                             & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                            | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+97,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+98,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+99,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+100,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+101,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+102,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+103,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_en_r));
    bufp->fullIData(oldp+104,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_index),32);
    bufp->fullCData(oldp+105,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_addr_r),5);
    bufp->fullBit(oldp+106,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
    bufp->fullBit(oldp+107,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+108,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ctrl__DOT__found));
    bufp->fullBit(oldp+109,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+110,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+111,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+112,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+113,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+114,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+115,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+116,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+117,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_en_r));
    bufp->fullIData(oldp+118,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_index),32);
    bufp->fullCData(oldp+119,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_addr_r),5);
    bufp->fullBit(oldp+120,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
    bufp->fullBit(oldp+121,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+122,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ctrl__DOT__found));
    bufp->fullBit(oldp+123,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+124,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+125,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+126,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+127,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+128,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+129,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+130,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+131,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_en_r));
    bufp->fullIData(oldp+132,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_index),32);
    bufp->fullCData(oldp+133,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_addr_r),5);
    bufp->fullBit(oldp+134,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
    bufp->fullBit(oldp+135,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+136,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ctrl__DOT__found));
    bufp->fullBit(oldp+137,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+138,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+139,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+140,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i[0]));
    bufp->fullBit(oldp+141,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i[1]));
    bufp->fullCData(oldp+142,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+143,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_addr_i[1]),5);
    bufp->fullCData(oldp+144,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_data_i[0]),8);
    bufp->fullCData(oldp+145,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_data_i[1]),8);
    bufp->fullBit(oldp+146,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+147,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_w[1]));
    bufp->fullBit(oldp+148,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+149,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+150,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+151,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+152,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_en_r));
    bufp->fullIData(oldp+153,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_index),32);
    bufp->fullCData(oldp+154,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_addr_r),5);
    bufp->fullBit(oldp+155,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
    bufp->fullBit(oldp+156,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+157,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ctrl__DOT__found));
    bufp->fullBit(oldp+158,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+159,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en));
    bufp->fullBit(oldp+160,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i
                            [0U]));
    bufp->fullBit(oldp+161,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+162,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+163,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_vol_r_inst__priority_write_req_i
                            [1U]));
    bufp->fullBit(oldp+164,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+165,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+166,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[0]));
    bufp->fullBit(oldp+167,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[1]));
    bufp->fullBit(oldp+168,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[2]));
    bufp->fullBit(oldp+169,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i[3]));
    bufp->fullCData(oldp+170,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+171,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[1]),5);
    bufp->fullCData(oldp+172,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[2]),5);
    bufp->fullCData(oldp+173,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_addr_i[3]),5);
    bufp->fullBit(oldp+174,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+175,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[1]));
    bufp->fullBit(oldp+176,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[2]));
    bufp->fullBit(oldp+177,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_w[3]));
    bufp->fullBit(oldp+178,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+179,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+180,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+181,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+182,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_en_r));
    bufp->fullIData(oldp+183,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_index),32);
    bufp->fullCData(oldp+184,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_addr_r),5);
    bufp->fullBit(oldp+185,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
    bufp->fullBit(oldp+186,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+187,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ctrl__DOT__found));
    bufp->fullBit(oldp+188,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+189,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ctrl__DOT__unnamedblk1__DOT__unnamedblk2__DOT__inhibit_en));
    bufp->fullBit(oldp+190,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
                            [1U]));
    bufp->fullBit(oldp+191,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+192,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+193,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
                            [2U]));
    bufp->fullBit(oldp+194,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+195,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+196,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_wds_r_inst__priority_write_req_i
                            [3U]));
    bufp->fullBit(oldp+197,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+198,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+199,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+200,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+201,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+202,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+203,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+204,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_en_r));
    bufp->fullIData(oldp+205,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_index),32);
    bufp->fullCData(oldp+206,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_addr_r),5);
    bufp->fullBit(oldp+207,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r))));
    bufp->fullBit(oldp+208,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+209,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ctrl__DOT__found));
    bufp->fullBit(oldp+210,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+211,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+212,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullCData(oldp+213,(vlSelfRef.doc5503_harness__DOT__dbg_cycle),5);
    bufp->fullCData(oldp+214,(vlSelfRef.doc5503_harness__DOT__dbg_cycle_max),5);
    bufp->fullCData(oldp+215,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r),8);
    bufp->fullCData(oldp+216,(vlSelfRef.doc5503_harness__DOT__dbg_osc_state),3);
    bufp->fullBit(oldp+217,(vlSelfRef.doc5503_harness__DOT__dbg_ready));
    bufp->fullSData(oldp+218,(vlSelfRef.doc5503_harness__DOT__dbg_output),16);
    bufp->fullCData(oldp+219,(vlSelfRef.doc5503_harness__DOT__dbg_wds),8);
    bufp->fullCData(oldp+220,(vlSelfRef.doc5503_harness__DOT__dbg_vol),8);
    bufp->fullCData(oldp+221,(vlSelfRef.doc5503_harness__DOT__dbg_control),8);
    bufp->fullIData(oldp+222,(vlSelfRef.doc5503_harness__DOT__dbg_acc),24);
    bufp->fullSData(oldp+223,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_address_o),16);
    bufp->fullBit(oldp+224,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_rd_o));
    bufp->fullSData(oldp+225,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_1_r),16);
    bufp->fullSData(oldp+226,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0]),16);
    bufp->fullSData(oldp+227,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[1]),16);
    bufp->fullSData(oldp+228,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[2]),16);
    bufp->fullSData(oldp+229,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[3]),16);
    bufp->fullSData(oldp+230,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[4]),16);
    bufp->fullSData(oldp+231,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[5]),16);
    bufp->fullSData(oldp+232,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[6]),16);
    bufp->fullSData(oldp+233,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[7]),16);
    bufp->fullSData(oldp+234,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[8]),16);
    bufp->fullSData(oldp+235,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[9]),16);
    bufp->fullSData(oldp+236,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[10]),16);
    bufp->fullSData(oldp+237,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[11]),16);
    bufp->fullSData(oldp+238,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[12]),16);
    bufp->fullSData(oldp+239,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[13]),16);
    bufp->fullSData(oldp+240,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[14]),16);
    bufp->fullSData(oldp+241,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[15]),16);
    bufp->fullCData(oldp+242,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r),8);
    bufp->fullSData(oldp+243,((((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_a_r) 
                                << 8U) | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_a_r))),16);
    bufp->fullCData(oldp+244,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_a_r),8);
    bufp->fullCData(oldp+245,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_a_r),8);
    bufp->fullCData(oldp+246,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_a_r),8);
    bufp->fullCData(oldp+247,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r),8);
    bufp->fullCData(oldp+248,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r),8);
    bufp->fullCData(oldp+249,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                              [1U]),8);
    bufp->fullCData(oldp+250,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                              [2U]),8);
    bufp->fullCData(oldp+251,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+252,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+253,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+254,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+255,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+256,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+257,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullBit(oldp+258,((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r))));
    bufp->fullBit(oldp+259,((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)))));
    bufp->fullCData(oldp+260,((1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r))),5);
    bufp->fullCData(oldp+261,((7U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r) 
                                     >> 3U))),3);
    bufp->fullCData(oldp+262,((7U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r))),3);
    bufp->fullBit(oldp+263,((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r))));
    bufp->fullCData(oldp+264,((3U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r) 
                                     >> 1U))),2);
    bufp->fullBit(oldp+265,((1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                             [1U])));
    bufp->fullCData(oldp+266,((3U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                     [1U] >> 1U))),2);
    bufp->fullBit(oldp+267,((1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                             [2U])));
    bufp->fullCData(oldp+268,((3U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                     [2U] >> 1U))),2);
    bufp->fullIData(oldp+269,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_a_r),24);
    bufp->fullSData(oldp+270,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_addr_r),16);
    bufp->fullCData(oldp+271,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r),5);
    bufp->fullCData(oldp+272,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r),5);
    bufp->fullCData(oldp+273,((0x1fU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r) 
                                        >> 1U))),5);
    bufp->fullCData(oldp+274,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r),6);
    bufp->fullCData(oldp+275,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_b_r),8);
    bufp->fullSData(oldp+276,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_b_r),16);
    bufp->fullCData(oldp+277,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r),5);
    bufp->fullBit(oldp+278,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r));
    bufp->fullCData(oldp+279,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r),5);
    bufp->fullBit(oldp+280,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_w));
    bufp->fullBit(oldp+281,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_step_w));
    bufp->fullBit(oldp+282,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r));
    bufp->fullBit(oldp+283,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_am_req_r));
    bufp->fullBit(oldp+284,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_wds_reset_req_r));
    bufp->fullBit(oldp+285,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_osc_halt_req_r));
    bufp->fullBit(oldp+286,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_unhalt_req_r));
    bufp->fullBit(oldp+287,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_load_req_r));
    bufp->fullBit(oldp+288,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_load_req_r));
    bufp->fullBit(oldp+289,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_add_req_r));
    bufp->fullBit(oldp+290,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_reset_req_r));
    bufp->fullBit(oldp+291,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_acc_reset_req_r));
    bufp->fullSData(oldp+292,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_r),16);
    bufp->fullBit(oldp+293,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_reset_req));
    bufp->fullBit(oldp+294,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_update_req));
    bufp->fullBit(oldp+295,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__prev_cs_n_r));
    bufp->fullCData(oldp+296,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r),3);
    bufp->fullSData(oldp+297,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r),16);
    bufp->fullSData(oldp+298,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r),16);
    bufp->fullSData(oldp+299,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r),16);
    bufp->fullSData(oldp+300,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r),16);
    bufp->fullSData(oldp+301,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r),16);
    bufp->fullSData(oldp+302,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r),16);
    bufp->fullSData(oldp+303,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[0]),16);
    bufp->fullSData(oldp+304,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[1]),16);
    bufp->fullSData(oldp+305,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[2]),16);
    bufp->fullSData(oldp+306,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[3]),16);
    bufp->fullSData(oldp+307,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[4]),16);
    bufp->fullSData(oldp+308,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[5]),16);
    bufp->fullSData(oldp+309,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[6]),16);
    bufp->fullSData(oldp+310,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[7]),16);
    bufp->fullSData(oldp+311,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[8]),16);
    bufp->fullSData(oldp+312,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[9]),16);
    bufp->fullSData(oldp+313,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[10]),16);
    bufp->fullSData(oldp+314,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[11]),16);
    bufp->fullSData(oldp+315,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[12]),16);
    bufp->fullSData(oldp+316,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[13]),16);
    bufp->fullSData(oldp+317,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[14]),16);
    bufp->fullSData(oldp+318,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[15]),16);
    bufp->fullSData(oldp+319,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[0]),16);
    bufp->fullSData(oldp+320,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[1]),16);
    bufp->fullSData(oldp+321,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[2]),16);
    bufp->fullSData(oldp+322,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[3]),16);
    bufp->fullSData(oldp+323,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[4]),16);
    bufp->fullSData(oldp+324,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[5]),16);
    bufp->fullSData(oldp+325,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[6]),16);
    bufp->fullSData(oldp+326,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[7]),16);
    bufp->fullSData(oldp+327,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[8]),16);
    bufp->fullSData(oldp+328,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[9]),16);
    bufp->fullSData(oldp+329,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[10]),16);
    bufp->fullSData(oldp+330,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[11]),16);
    bufp->fullSData(oldp+331,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[12]),16);
    bufp->fullSData(oldp+332,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[13]),16);
    bufp->fullSData(oldp+333,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[14]),16);
    bufp->fullSData(oldp+334,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[15]),16);
    bufp->fullCData(oldp+335,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r),2);
    bufp->fullCData(oldp+336,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r),4);
    bufp->fullBit(oldp+337,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[0]));
    bufp->fullBit(oldp+338,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[1]));
    bufp->fullBit(oldp+339,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[2]));
    bufp->fullCData(oldp+340,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+341,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[1]),5);
    bufp->fullCData(oldp+342,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[2]),5);
    bufp->fullIData(oldp+343,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[0]),24);
    bufp->fullIData(oldp+344,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[1]),24);
    bufp->fullIData(oldp+345,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[2]),24);
    bufp->fullBit(oldp+346,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+347,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[1]));
    bufp->fullBit(oldp+348,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[2]));
    bufp->fullBit(oldp+349,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+350,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[1]));
    bufp->fullBit(oldp+351,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[2]));
    bufp->fullIData(oldp+352,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[0]),24);
    bufp->fullIData(oldp+353,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[1]),24);
    bufp->fullIData(oldp+354,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[2]),24);
    bufp->fullIData(oldp+355,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[3]),24);
    bufp->fullIData(oldp+356,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[4]),24);
    bufp->fullIData(oldp+357,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[5]),24);
    bufp->fullIData(oldp+358,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[6]),24);
    bufp->fullIData(oldp+359,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[7]),24);
    bufp->fullIData(oldp+360,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[8]),24);
    bufp->fullIData(oldp+361,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[9]),24);
    bufp->fullIData(oldp+362,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[10]),24);
    bufp->fullIData(oldp+363,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[11]),24);
    bufp->fullIData(oldp+364,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[12]),24);
    bufp->fullIData(oldp+365,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[13]),24);
    bufp->fullIData(oldp+366,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[14]),24);
    bufp->fullIData(oldp+367,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[15]),24);
    bufp->fullIData(oldp+368,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[16]),24);
    bufp->fullIData(oldp+369,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[17]),24);
    bufp->fullIData(oldp+370,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[18]),24);
    bufp->fullIData(oldp+371,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[19]),24);
    bufp->fullIData(oldp+372,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[20]),24);
    bufp->fullIData(oldp+373,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[21]),24);
    bufp->fullIData(oldp+374,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[22]),24);
    bufp->fullIData(oldp+375,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[23]),24);
    bufp->fullIData(oldp+376,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[24]),24);
    bufp->fullIData(oldp+377,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[25]),24);
    bufp->fullIData(oldp+378,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[26]),24);
    bufp->fullIData(oldp+379,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[27]),24);
    bufp->fullIData(oldp+380,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[28]),24);
    bufp->fullIData(oldp+381,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[29]),24);
    bufp->fullIData(oldp+382,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[30]),24);
    bufp->fullIData(oldp+383,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[31]),24);
    bufp->fullBit(oldp+384,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+385,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+386,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+387,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+388,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                            [0U]));
    bufp->fullBit(oldp+389,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+390,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+391,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+392,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+393,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+394,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+395,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+396,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                            [1U]));
    bufp->fullBit(oldp+397,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                            [1U]));
    bufp->fullBit(oldp+398,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+399,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+400,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+401,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+402,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+403,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+404,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                            [2U]));
    bufp->fullBit(oldp+405,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                            [2U]));
    bufp->fullBit(oldp+406,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+407,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+408,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+409,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+410,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+411,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+412,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+413,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+414,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullIData(oldp+415,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__data_curr_r),24);
    bufp->fullCData(oldp+416,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[0]),8);
    bufp->fullCData(oldp+417,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[1]),8);
    bufp->fullCData(oldp+418,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[2]),8);
    bufp->fullCData(oldp+419,((0x1fU & ((IData)(1U) 
                                        + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r)))),5);
    bufp->fullBit(oldp+420,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+421,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[1]));
    bufp->fullBit(oldp+422,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[2]));
    bufp->fullBit(oldp+423,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[3]));
    bufp->fullBit(oldp+424,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullBit(oldp+425,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[1]));
    bufp->fullBit(oldp+426,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[2]));
    bufp->fullCData(oldp+427,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+428,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[1]),8);
    bufp->fullCData(oldp+429,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[2]),8);
    bufp->fullCData(oldp+430,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+431,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+432,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+433,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+434,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+435,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+436,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+437,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+438,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+439,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+440,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+441,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+442,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+443,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+444,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+445,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+446,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+447,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+448,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+449,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+450,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+451,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+452,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+453,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+454,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+455,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+456,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+457,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+458,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+459,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+460,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+461,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+462,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+463,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+464,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+465,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+466,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+467,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                            [1U]));
    bufp->fullBit(oldp+468,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+469,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+470,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+471,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+472,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                            [2U]));
    bufp->fullBit(oldp+473,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+474,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+475,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+476,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+477,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                            [1U]));
    bufp->fullBit(oldp+478,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+479,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+480,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+481,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+482,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                            [2U]));
    bufp->fullBit(oldp+483,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+484,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+485,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+486,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+487,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                            [3U]));
    bufp->fullBit(oldp+488,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+489,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+490,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+491,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+492,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+493,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+494,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+495,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+496,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o[0]),8);
    bufp->fullCData(oldp+497,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_a_r),8);
    bufp->fullBit(oldp+498,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+499,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+500,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+501,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+502,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+503,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+504,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+505,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+506,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+507,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+508,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+509,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+510,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+511,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+512,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+513,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+514,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+515,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+516,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+517,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+518,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+519,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+520,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+521,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+522,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+523,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+524,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+525,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+526,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+527,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+528,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+529,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+530,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+531,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+532,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+533,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+534,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+535,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+536,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+537,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+538,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+539,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+540,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+541,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+542,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+543,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+544,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+545,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+546,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+547,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o[0]),8);
    bufp->fullCData(oldp+548,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_a_r),8);
    bufp->fullBit(oldp+549,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+550,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+551,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+552,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+553,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+554,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+555,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+556,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+557,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+558,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+559,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+560,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+561,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+562,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+563,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+564,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+565,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+566,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+567,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+568,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+569,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+570,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+571,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+572,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+573,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+574,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+575,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+576,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+577,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+578,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+579,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+580,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+581,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+582,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+583,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+584,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+585,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+586,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+587,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+588,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+589,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+590,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+591,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+592,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+593,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+594,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+595,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+596,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+597,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullBit(oldp+598,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[0]));
    bufp->fullBit(oldp+599,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[1]));
    bufp->fullBit(oldp+600,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[2]));
    bufp->fullCData(oldp+601,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+602,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[1]),5);
    bufp->fullCData(oldp+603,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[2]),5);
    bufp->fullSData(oldp+604,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[0]),16);
    bufp->fullSData(oldp+605,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[1]),16);
    bufp->fullSData(oldp+606,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[2]),16);
    bufp->fullBit(oldp+607,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+608,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[1]));
    bufp->fullBit(oldp+609,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[2]));
    bufp->fullBit(oldp+610,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+611,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[1]));
    bufp->fullBit(oldp+612,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[2]));
    bufp->fullSData(oldp+613,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[0]),16);
    bufp->fullSData(oldp+614,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[1]),16);
    bufp->fullSData(oldp+615,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[2]),16);
    bufp->fullSData(oldp+616,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[3]),16);
    bufp->fullSData(oldp+617,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[4]),16);
    bufp->fullSData(oldp+618,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[5]),16);
    bufp->fullSData(oldp+619,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[6]),16);
    bufp->fullSData(oldp+620,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[7]),16);
    bufp->fullSData(oldp+621,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[8]),16);
    bufp->fullSData(oldp+622,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[9]),16);
    bufp->fullSData(oldp+623,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[10]),16);
    bufp->fullSData(oldp+624,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[11]),16);
    bufp->fullSData(oldp+625,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[12]),16);
    bufp->fullSData(oldp+626,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[13]),16);
    bufp->fullSData(oldp+627,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[14]),16);
    bufp->fullSData(oldp+628,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[15]),16);
    bufp->fullSData(oldp+629,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[16]),16);
    bufp->fullSData(oldp+630,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[17]),16);
    bufp->fullSData(oldp+631,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[18]),16);
    bufp->fullSData(oldp+632,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[19]),16);
    bufp->fullSData(oldp+633,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[20]),16);
    bufp->fullSData(oldp+634,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[21]),16);
    bufp->fullSData(oldp+635,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[22]),16);
    bufp->fullSData(oldp+636,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[23]),16);
    bufp->fullSData(oldp+637,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[24]),16);
    bufp->fullSData(oldp+638,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[25]),16);
    bufp->fullSData(oldp+639,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[26]),16);
    bufp->fullSData(oldp+640,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[27]),16);
    bufp->fullSData(oldp+641,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[28]),16);
    bufp->fullSData(oldp+642,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[29]),16);
    bufp->fullSData(oldp+643,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[30]),16);
    bufp->fullSData(oldp+644,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[31]),16);
    bufp->fullBit(oldp+645,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+646,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+647,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+648,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+649,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
                            [1U]));
    bufp->fullBit(oldp+650,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
                            [1U]));
    bufp->fullBit(oldp+651,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+652,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+653,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+654,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+655,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+656,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+657,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
                            [2U]));
    bufp->fullBit(oldp+658,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
                            [2U]));
    bufp->fullBit(oldp+659,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+660,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+661,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+662,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+663,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+664,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+665,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+666,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullCData(oldp+667,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o[0]),8);
    bufp->fullBit(oldp+668,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+669,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+670,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+671,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+672,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+673,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+674,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+675,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+676,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+677,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+678,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+679,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+680,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+681,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+682,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+683,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+684,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+685,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+686,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+687,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+688,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+689,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+690,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+691,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+692,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+693,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+694,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+695,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+696,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+697,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+698,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+699,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+700,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+701,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+702,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+703,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+704,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+705,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+706,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+707,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+708,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+709,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+710,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+711,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+712,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+713,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+714,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+715,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+716,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+717,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o[0]),8);
    bufp->fullBit(oldp+718,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+719,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[1]));
    bufp->fullBit(oldp+720,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+721,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+722,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+723,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+724,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+725,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+726,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+727,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+728,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+729,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+730,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+731,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+732,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+733,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+734,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+735,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+736,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+737,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+738,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+739,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+740,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+741,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+742,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+743,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+744,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+745,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+746,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+747,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+748,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+749,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+750,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+751,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+752,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+753,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+754,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+755,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+756,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+757,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+758,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+759,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+760,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+761,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+762,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+763,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+764,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
                            [1U]));
    bufp->fullBit(oldp+765,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+766,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+767,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+768,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+769,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+770,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+771,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+772,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+773,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o[0]),8);
    bufp->fullBit(oldp+774,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+775,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[1]));
    bufp->fullBit(oldp+776,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[2]));
    bufp->fullBit(oldp+777,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[3]));
    bufp->fullBit(oldp+778,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+779,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+780,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+781,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+782,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+783,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+784,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+785,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+786,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+787,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+788,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+789,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+790,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+791,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+792,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+793,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+794,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+795,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+796,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+797,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+798,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+799,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+800,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+801,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+802,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+803,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+804,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+805,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+806,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+807,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+808,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+809,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+810,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+811,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+812,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+813,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+814,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+815,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+816,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+817,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                            [1U]));
    bufp->fullBit(oldp+818,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+819,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+820,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+821,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+822,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                            [2U]));
    bufp->fullBit(oldp+823,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+824,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+825,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+826,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+827,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                            [3U]));
    bufp->fullBit(oldp+828,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+829,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+830,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+831,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+832,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+833,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+834,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+835,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+836,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o[0]),8);
    bufp->fullBit(oldp+837,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+838,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+839,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+840,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+841,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+842,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+843,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+844,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+845,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+846,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+847,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+848,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+849,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+850,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+851,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+852,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+853,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+854,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+855,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+856,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+857,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+858,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+859,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+860,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+861,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+862,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+863,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+864,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+865,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+866,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+867,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+868,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+869,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+870,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+871,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+872,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+873,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+874,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+875,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+876,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+877,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+878,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+879,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+880,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+881,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+882,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+883,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+884,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+885,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullSData(oldp+886,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__data_w),9);
    bufp->fullSData(oldp+887,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__vol_s),9);
    bufp->fullIData(oldp+888,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__mult_result),18);
    bufp->fullCData(oldp+889,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk2__DOT__shift_w),5);
    bufp->fullIData(oldp+890,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__high_bit_w),32);
    bufp->fullBit(oldp+891,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__overflow));
    bufp->fullBit(oldp+892,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__zero_byte_w));
    bufp->fullCData(oldp+893,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_hi_mask_w),8);
    bufp->fullSData(oldp+894,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_w),16);
    bufp->fullSData(oldp+895,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__addr_w),16);
    bufp->fullCData(oldp+896,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk5__DOT__ca),4);
    bufp->fullBit(oldp+897,(vlSelfRef.clk_i));
    bufp->fullBit(oldp+898,(vlSelfRef.reset_n_i));
    bufp->fullBit(oldp+899,(vlSelfRef.clk_en_i));
    bufp->fullBit(oldp+900,(vlSelfRef.irq_n_o));
    bufp->fullBit(oldp+901,(vlSelfRef.cs_n_i));
    bufp->fullBit(oldp+902,(vlSelfRef.we_n_i));
    bufp->fullCData(oldp+903,(vlSelfRef.addr_i),8);
    bufp->fullCData(oldp+904,(vlSelfRef.data_i),8);
    bufp->fullCData(oldp+905,(vlSelfRef.data_o),8);
    bufp->fullSData(oldp+906,(vlSelfRef.wave_address_o),16);
    bufp->fullBit(oldp+907,(vlSelfRef.wave_rd_o));
    bufp->fullBit(oldp+908,(vlSelfRef.wave_data_ready_i));
    bufp->fullCData(oldp+909,(vlSelfRef.wave_data_i),8);
    bufp->fullSData(oldp+910,(vlSelfRef.mono_mix_o),16);
    bufp->fullSData(oldp+911,(vlSelfRef.left_mix_o),16);
    bufp->fullSData(oldp+912,(vlSelfRef.right_mix_o),16);
}
