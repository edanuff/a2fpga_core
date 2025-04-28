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
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+932,0,"reset_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+933,0,"clk_en_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+934,0,"irq_n_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+935,0,"cs_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+936,0,"we_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+937,0,"addr_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+938,0,"data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+939,0,"data_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+940,0,"wave_address_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+941,0,"wave_rd_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+942,0,"wave_data_ready_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+943,0,"wave_data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+944,0,"mono_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+945,0,"left_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+946,0,"right_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+947,0,"dbg_cycle_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+948,0,"dbg_osc_state_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+949,0,"dbg_vol_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+950,0,"dbg_wds_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+951,0,"dbg_output_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->pushPrefix("doc5503_harness", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+932,0,"reset_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+933,0,"clk_en_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+934,0,"irq_n_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+935,0,"cs_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+936,0,"we_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+937,0,"addr_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+938,0,"data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+939,0,"data_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+940,0,"wave_address_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+941,0,"wave_rd_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+942,0,"wave_data_ready_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+943,0,"wave_data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+944,0,"mono_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+945,0,"left_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+946,0,"right_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+947,0,"dbg_cycle_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+948,0,"dbg_osc_state_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+949,0,"dbg_vol_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+950,0,"dbg_wds_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+951,0,"dbg_output_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
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
    tracep->declBus(c+953,0,"OUTPUT_CHANNEL_MIX",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+954,0,"NUM_CHANNELS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"OUTPUT_MONO_MIX",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"OUTPUT_STEREO_MIX",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"OUTPUT_OSC_DIRECT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"DIRECT_OUTPUT_OSC_NUM",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+956,0,"CHANNEL_MAX",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+932,0,"reset_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+933,0,"clk_en_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+934,0,"irq_n_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+935,0,"cs_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+936,0,"we_n_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+937,0,"addr_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+938,0,"data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+939,0,"data_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+223,0,"wave_address_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+224,0,"wave_rd_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+942,0,"wave_data_ready_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+943,0,"wave_data_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+944,0,"mono_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+945,0,"left_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+946,0,"right_mix_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->pushPrefix("channel_o", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 16; ++i) {
        tracep->declBus(c+225+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+215,0,"osc_en_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+241,0,"osc_int_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+215,0,"osc_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+242,0,"f_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+243,0,"vol_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+244,0,"wds_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+245,0,"wtp_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+246,0,"rts_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+247,0,"control_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+248,0,"partner_control_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+249,0,"next_control_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+250,0,"fl_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+251,0,"fh_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+252,0,"vol_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+253,0,"wds_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+254,0,"wtp_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+255,0,"control_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+256,0,"rts_o_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+257,0,"odd_osc_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+258,0,"even_osc_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+259,0,"partner_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+260,0,"wts_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+261,0,"res_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+262,0,"halt_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+263,0,"mode_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+264,0,"partner_halt_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+265,0,"partner_mode_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+266,0,"next_halt_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+267,0,"next_mode_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+268,0,"acc_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+269,0,"wave_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+957,0,"MODE_FREE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+958,0,"MODE_ONESHOT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+959,0,"MODE_SYNC_AM",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+960,0,"MODE_SWAP",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+270,0,"cycle_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+271,0,"cycle_step_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+272,0,"osc_en_num_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+272,0,"cycle_max_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+273,0,"mixer_cycle_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+274,0,"mixer_control_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+275,0,"mixer_output_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+276,0,"last_osc_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+277,0,"ready_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+278,0,"init_cycle_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+279,0,"last_cycle_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+280,0,"last_cycle_step_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"inhibit_host_writes_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+282,0,"next_am_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+283,0,"current_wds_reset_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+284,0,"current_osc_halt_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+285,0,"partner_unhalt_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+286,0,"partner_control_load_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+287,0,"next_control_load_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+288,0,"current_acc_add_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+289,0,"current_acc_reset_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+290,0,"partner_acc_reset_req_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+291,0,"output_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+292,0,"output_reset_req",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+293,0,"output_update_req",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+294,0,"current_active_osc_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+295,0,"active_osc_count_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+296,0,"prev_cs_n_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+45,0,"req_cs_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+961,0,"OSC_IDLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+962,0,"OSC_START",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+963,0,"OSC_ACC",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+964,0,"OSC_ADDR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+965,0,"OSC_READ_DATA",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+966,0,"OSC_HALT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+967,0,"OSC_OUT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+297,0,"osc_state_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+968,0,"MIXER_SUM_RESOLUTION",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+969,0,"TOP_BIT_OFFSET",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+956,0,"WINDOW_SIZE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declDouble(c+970,0,"COMPRESSOR_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+972,0,"COMPRESSOR_ATTACK",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+974,0,"COMPRESSOR_RELEASE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+976,0,"COMPRESSOR_THRESHOLD",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+978,0,"COMPRESSOR_RATIO",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+980,0,"COMPRESSOR_KNEE_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+982,0,"COMPRESSOR_MAKEUP_GAIN",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declBus(c+298,0,"osc_out_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+299,0,"mono_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+300,0,"next_mono_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+301,0,"left_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+302,0,"compressed_left_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+303,0,"next_left_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+304,0,"right_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+305,0,"compressed_right_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+306,0,"next_right_mix_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->pushPrefix("channel_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 16; ++i) {
        tracep->declBus(c+307+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("next_channel_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 16; ++i) {
        tracep->declBus(c+323+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 23,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+957,0,"MIXER_INIT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+958,0,"MIXER_ZERO",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+959,0,"MIXER_ADD",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+960,0,"MIXER_OUTPUT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+339,0,"mixer_state_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+340,0,"mixer_channel_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->pushPrefix("compressor_block", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("compressor", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+968,0,"INPUT_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+954,0,"OUTPUT_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declDouble(c+972,0,"ATTACK_RATE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+974,0,"RELEASE_RATE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+976,0,"THRESHOLD",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+978,0,"RATIO",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+980,0,"KNEE_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declDouble(c+982,0,"MAKEUP_GAIN",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::DOUBLE, false,-1);
    tracep->declBit(c+984,0,"PROTECT_SIGN_BIT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::BIT, false,-1);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+952,0,"reset_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+985,0,"enable_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+341,0,"audio_in_l",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+342,0,"audio_in_r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+302,0,"audio_out_l",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+305,0,"audio_out_r",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+954,0,"FP_BITS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+986,0,"FP_ONE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+987,0,"FP_ATTACK",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+988,0,"FP_RELEASE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+989,0,"FP_THRESHOLD",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+990,0,"FP_KNEE_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+991,0,"FP_MAKEUP_GAIN",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+992,0,"FP_RATIO",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+993,0,"FP_RATIO_MINUS_ONE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+994,0,"MAX_INPUT_VALUE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+995,0,"MAX_OUTPUT_VALUE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declQuad(c+343,0,"envelope_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declQuad(c+345,0,"gain_reduction_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declBus(c+996,0,"SCALING_SHIFT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+347,0,"abs_left_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+348,0,"abs_right_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+349,0,"max_abs_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declQuad(c+350,0,"level_fp",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declQuad(c+352,0,"gain_reduction",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declQuad(c+354,0,"level_normalized",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declQuad(c+356,0,"threshold_min",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declQuad(c+358,0,"threshold_max",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declQuad(c+360,0,"knee_factor",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declQuad(c+362,0,"amount_over",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declQuad(c+997,0,"left_reduced",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declQuad(c+999,0,"right_reduced",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+364,0,"sign_bit_l",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+365,0,"abs_input_l",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+366,0,"abs_scaled_l",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+367,0,"final_l",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+368,0,"sign_bit_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+369,0,"abs_input_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+370,0,"abs_scaled_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+371,0,"final_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_acc_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1001,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1002,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+968,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+372+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+375+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+378+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 23,0);
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
    tracep->declBus(c+270,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+268,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+1003,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1004,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+381+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+384+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+1005+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+1006+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+1007+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 23,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+387+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 23,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+419,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+420,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+421,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+1008,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+1009,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+270,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1010,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+268,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBus(c+1011,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+1008,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1009,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+422,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1012,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+1008,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+423,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+424,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+425,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+426,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+427,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+428,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+429,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+430,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+431,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+432,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+433,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+434,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+435,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+436,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+437,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+438,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[2]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+439,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+440,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+441,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+443,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+444,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+445,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+446,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+447,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+448,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+449,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+450,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_control_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1013,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1001,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1002,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+996,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
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
        tracep->declBus(c+451+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+270,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+247,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+454,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+274,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBit(c+64+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBit(c+455+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+68+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+459+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+462+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+465+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+71,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+72,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+73,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+74,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+75,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+76,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1010,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+247,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+274,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+77,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+497,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+78,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+498,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+499,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+500,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+79,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+501,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+80,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+502,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+81,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+503,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+504,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+505,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+82,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+506,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_port_loop[2]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+83,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+507,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+84,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+508,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+509,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+510,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+85,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+511,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+86,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1012,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+87,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1014,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+88,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+89,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+512,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+90,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+513,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+514,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+515,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+91,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+516,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[2]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+92,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+517,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+93,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+518,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+519,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+520,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+94,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+521,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[3]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+95,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+522,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+96,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+523,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+524,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+525,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+97,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+526,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+527,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+528,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+529,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+530,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_fh_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+953,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1002,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+996,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
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
        tracep->declBus(c+531+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+270,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+532,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1003,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1015,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+98+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+533+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+99+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+534+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+535+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+536+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+100,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+101,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+102,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+103,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+104,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+105,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1010,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+532,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1016,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+12,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+568,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+106,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+569,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+570,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+571,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+107,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+572,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+108,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1017,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+109,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1017,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+1008,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+13,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+573,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+110,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+574,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+575,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+576,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+111,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+577,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+578,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+579,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+580,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+581,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_fl_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+953,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1002,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+996,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
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
        tracep->declBus(c+582+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+270,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+583,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1003,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1015,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+112+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+584+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+113+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+585+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+586+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+587+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+114,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+115,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+116,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+117,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+118,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+119,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1010,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+583,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1018,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+19,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+619,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+120,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+620,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+621,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+622,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+121,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+623,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+122,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1017,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+123,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1017,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+1008,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+20,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+624,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+124,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+625,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+626,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+627,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+125,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+628,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+629,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+630,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+631,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+632,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_output_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1001,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1002,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+954,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("priority_write_req_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+633+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_addr_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+636+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_data_i", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+639+i*1,0,"",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
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
    tracep->declBus(c+1003,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1019,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+454,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+275,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+642+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBit(c+645+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+1020+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+1021+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+1022+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+648+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+680,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+681,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+682,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+1008,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+1009,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1003,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1023,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+1024,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+275,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+1008,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1009,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+683,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1012,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+1008,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+684,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+685,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+686,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+687,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+688,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+689,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+690,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+691,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[2]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+692,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+693,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+694,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+695,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+696,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+697,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+698,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+699,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+700,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+701,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_rts_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+953,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1002,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+996,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
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
        tracep->declBus(c+702+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+270,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+246,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1003,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1015,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+126+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+703+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+127+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+704+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+705+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+706+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+128,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+129,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+130,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+131,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+132,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+133,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1010,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+246,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1025,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+26,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+738,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+134,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+739,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+740,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+741,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+135,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+742,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+136,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1017,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+137,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1017,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+1008,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+27,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+743,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+138,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+744,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+745,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+746,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+139,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+747,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+748,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+749,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+750,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+751,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_vol_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1026,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1002,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+996,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
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
        tracep->declBus(c+752+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+270,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+243,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1003,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1015,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 2; ++i) {
        tracep->declBit(c+146+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 2; ++i) {
        tracep->declBit(c+753+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+148+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+755+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+756+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+757+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+149,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+150,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+151,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+152,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+153,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+154,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1010,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+243,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1027,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+30,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+789,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+155,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+790,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+791,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+792,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+156,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+793,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+157,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1017,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+158,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1028,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+159,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+160,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+794,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+161,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+795,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+796,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+797,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+162,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+798,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+163,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+799,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+164,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+800,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+801,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+802,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+165,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+803,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+804,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+805,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+806,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+807,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_wds_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1013,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1002,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+996,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
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
        tracep->declBus(c+808+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+270,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+244,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1003,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1015,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBit(c+174+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 4; ++i) {
        tracep->declBit(c+809+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+178+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+813+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+814+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+815+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+179,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+180,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+181,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+182,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+183,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+184,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1010,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+244,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1029,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+37,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+847,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+185,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+848,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+849,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+850,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+186,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+851,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+187,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1017,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+188,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1014,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+189,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[1]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+190,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+852,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+191,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+853,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+854,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+855,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+192,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+856,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[2]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+193,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+857,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+194,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+858,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+859,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+860,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+195,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+861,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[3]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+196,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+862,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+197,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+863,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+864,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+865,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+198,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+866,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+867,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+868,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+869,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+870,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("osc_wtp_r_inst", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+953,0,"PRIORITY_WRITE_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PRIORITY_READ_PORTS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"LAST_PRIORITY_WRITE_PORT_INHIBITABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+953,0,"PORT_A_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+955,0,"PORT_B_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+1002,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+996,0,"DATA_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+931,0,"clk_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+281,0,"inhibit_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
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
        tracep->declBus(c+871+i*1,0,"",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+270,0,"addr_a_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+245,0,"data_a_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1003,0,"addr_b_i",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1015,0,"data_b_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("priority_write_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+199+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+872+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_w", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+200+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ack_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBit(c+873+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0));
    }
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 1; ++i) {
        tracep->declBus(c+874+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("data_r", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 32; ++i) {
        tracep->declBus(c+875+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+201,0,"priority_write_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+202,0,"priority_write_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+203,0,"priority_write_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+204,0,"priority_read_en_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+205,0,"priority_read_index",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+206,0,"priority_read_addr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1010,0,"SP_READ_ENABLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 0,0);
    tracep->declBus(c+245,0,"data_a_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+1030,0,"data_b_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("gen_read_ports", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("priority_read_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("read_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+43,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+907,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+207,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+908,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+909,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+910,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+208,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+911,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_read_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+209,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1017,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_ctrl", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+210,0,"found",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+1017,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+1008,0,"inhibit_en",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("priority_write_port_loop[0]", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("gen_edge_triggered", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->pushPrefix("write_ff", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+931,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+44,0,"s",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+912,0,"r",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+211,0,"q",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+913,0,"prev_s_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+914,0,"prev_r_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+915,0,"q_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+212,0,"edge_s_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+916,0,"edge_r_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+917,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+918,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk6", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+919,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBus(c+920,0,"data_curr_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+921,0,"data_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+922,0,"vol_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk2", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+923,0,"shift_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk3", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+924,0,"high_bit_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INT, false,-1, 31,0);
    tracep->declBit(c+925,0,"overflow",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+926,0,"zero_byte_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk4", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+927,0,"ptr_hi_mask_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+928,0,"ptr_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+929,0,"addr_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("unnamedblk5", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+930,0,"ca",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
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
    bufp->fullIData(oldp+953,(1U),32);
    bufp->fullIData(oldp+954,(0x10U),32);
    bufp->fullIData(oldp+955,(0U),32);
    bufp->fullIData(oldp+956,(0xfU),32);
    bufp->fullCData(oldp+957,(0U),2);
    bufp->fullCData(oldp+958,(1U),2);
    bufp->fullCData(oldp+959,(2U),2);
    bufp->fullCData(oldp+960,(3U),2);
    bufp->fullCData(oldp+961,(0U),3);
    bufp->fullCData(oldp+962,(1U),3);
    bufp->fullCData(oldp+963,(2U),3);
    bufp->fullCData(oldp+964,(3U),3);
    bufp->fullCData(oldp+965,(4U),3);
    bufp->fullCData(oldp+966,(5U),3);
    bufp->fullCData(oldp+967,(6U),3);
    bufp->fullIData(oldp+968,(0x18U),32);
    bufp->fullIData(oldp+969,(6U),32);
    bufp->fullDouble(oldp+970,(1.0));
    bufp->fullDouble(oldp+972,(1.00000000000000002e-02));
    bufp->fullDouble(oldp+974,(1.00000000000000002e-03));
    bufp->fullDouble(oldp+976,(6.99999999999999956e-01));
    bufp->fullDouble(oldp+978,(4.0));
    bufp->fullDouble(oldp+980,(2.00000000000000011e-01));
    bufp->fullDouble(oldp+982,(1.19999999999999996e+00));
    bufp->fullBit(oldp+984,(1U));
    bufp->fullBit(oldp+985,(1U));
    bufp->fullIData(oldp+986,(0x10000U),32);
    bufp->fullIData(oldp+987,(0x28fU),32);
    bufp->fullIData(oldp+988,(0x42U),32);
    bufp->fullIData(oldp+989,(0xb333U),32);
    bufp->fullIData(oldp+990,(0x3333U),32);
    bufp->fullIData(oldp+991,(0x13333U),32);
    bufp->fullIData(oldp+992,(0x40000U),32);
    bufp->fullIData(oldp+993,(0x30000U),32);
    bufp->fullIData(oldp+994,(0x7fffffU),32);
    bufp->fullIData(oldp+995,(0x7fffU),32);
    bufp->fullIData(oldp+996,(8U),32);
    bufp->fullQData(oldp+997,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__left_reduced),40);
    bufp->fullQData(oldp+999,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__right_reduced),40);
    bufp->fullIData(oldp+1001,(3U),32);
    bufp->fullIData(oldp+1002,(5U),32);
    bufp->fullCData(oldp+1003,(0U),5);
    bufp->fullIData(oldp+1004,(0U),24);
    bufp->fullBit(oldp+1005,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+1006,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullIData(oldp+1007,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_read_data_r[0]),24);
    bufp->fullBit(oldp+1008,(0U));
    bufp->fullIData(oldp+1009,(0U),32);
    bufp->fullBit(oldp+1010,(1U));
    bufp->fullIData(oldp+1011,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_b_r),24);
    bufp->fullIData(oldp+1012,(3U),32);
    bufp->fullIData(oldp+1013,(4U),32);
    bufp->fullIData(oldp+1014,(4U),32);
    bufp->fullCData(oldp+1015,(0U),8);
    bufp->fullCData(oldp+1016,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_b_r),8);
    bufp->fullIData(oldp+1017,(1U),32);
    bufp->fullCData(oldp+1018,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_b_r),8);
    bufp->fullSData(oldp+1019,(0U),16);
    bufp->fullBit(oldp+1020,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_read_w[0]));
    bufp->fullBit(oldp+1021,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullSData(oldp+1022,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_read_data_r[0]),16);
    bufp->fullBit(oldp+1023,(0U));
    bufp->fullSData(oldp+1024,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_a_r),16);
    bufp->fullCData(oldp+1025,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_b_r),8);
    bufp->fullIData(oldp+1026,(2U),32);
    bufp->fullCData(oldp+1027,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_b_r),8);
    bufp->fullIData(oldp+1028,(2U),32);
    bufp->fullCData(oldp+1029,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_b_r),8);
    bufp->fullCData(oldp+1030,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_b_r),8);
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
    bufp->fullSData(oldp+225,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[0]),16);
    bufp->fullSData(oldp+226,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[1]),16);
    bufp->fullSData(oldp+227,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[2]),16);
    bufp->fullSData(oldp+228,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[3]),16);
    bufp->fullSData(oldp+229,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[4]),16);
    bufp->fullSData(oldp+230,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[5]),16);
    bufp->fullSData(oldp+231,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[6]),16);
    bufp->fullSData(oldp+232,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[7]),16);
    bufp->fullSData(oldp+233,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[8]),16);
    bufp->fullSData(oldp+234,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[9]),16);
    bufp->fullSData(oldp+235,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[10]),16);
    bufp->fullSData(oldp+236,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[11]),16);
    bufp->fullSData(oldp+237,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[12]),16);
    bufp->fullSData(oldp+238,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[13]),16);
    bufp->fullSData(oldp+239,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[14]),16);
    bufp->fullSData(oldp+240,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_o[15]),16);
    bufp->fullCData(oldp+241,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_int_r),8);
    bufp->fullSData(oldp+242,((((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_a_r) 
                                << 8U) | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_a_r))),16);
    bufp->fullCData(oldp+243,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_a_r),8);
    bufp->fullCData(oldp+244,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_a_r),8);
    bufp->fullCData(oldp+245,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_a_r),8);
    bufp->fullCData(oldp+246,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r),8);
    bufp->fullCData(oldp+247,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r),8);
    bufp->fullCData(oldp+248,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                              [1U]),8);
    bufp->fullCData(oldp+249,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                              [2U]),8);
    bufp->fullCData(oldp+250,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+251,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+252,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+253,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+254,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+255,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullCData(oldp+256,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o
                              [0U]),8);
    bufp->fullBit(oldp+257,((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r))));
    bufp->fullBit(oldp+258,((1U & (~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r)))));
    bufp->fullCData(oldp+259,((1U ^ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r))),5);
    bufp->fullCData(oldp+260,((7U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r) 
                                     >> 3U))),3);
    bufp->fullCData(oldp+261,((7U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_a_r))),3);
    bufp->fullBit(oldp+262,((1U & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r))));
    bufp->fullCData(oldp+263,((3U & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_a_r) 
                                     >> 1U))),2);
    bufp->fullBit(oldp+264,((1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                             [1U])));
    bufp->fullCData(oldp+265,((3U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                     [1U] >> 1U))),2);
    bufp->fullBit(oldp+266,((1U & vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                             [2U])));
    bufp->fullCData(oldp+267,((3U & (vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o
                                     [2U] >> 1U))),2);
    bufp->fullIData(oldp+268,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_a_r),24);
    bufp->fullSData(oldp+269,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__wave_addr_r),16);
    bufp->fullCData(oldp+270,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_r),5);
    bufp->fullCData(oldp+271,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__cycle_step_r),5);
    bufp->fullCData(oldp+272,((0x1fU & ((IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_en_r) 
                                        >> 1U))),5);
    bufp->fullCData(oldp+273,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r),6);
    bufp->fullCData(oldp+274,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_b_r),8);
    bufp->fullSData(oldp+275,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_b_r),16);
    bufp->fullCData(oldp+276,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_osc_r),5);
    bufp->fullBit(oldp+277,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__ready_r));
    bufp->fullCData(oldp+278,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__init_cycle_r),5);
    bufp->fullBit(oldp+279,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_w));
    bufp->fullBit(oldp+280,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__last_cycle_step_w));
    bufp->fullBit(oldp+281,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__inhibit_host_writes_r));
    bufp->fullBit(oldp+282,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_am_req_r));
    bufp->fullBit(oldp+283,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_wds_reset_req_r));
    bufp->fullBit(oldp+284,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_osc_halt_req_r));
    bufp->fullBit(oldp+285,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_unhalt_req_r));
    bufp->fullBit(oldp+286,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_control_load_req_r));
    bufp->fullBit(oldp+287,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_control_load_req_r));
    bufp->fullBit(oldp+288,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_add_req_r));
    bufp->fullBit(oldp+289,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_acc_reset_req_r));
    bufp->fullBit(oldp+290,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__partner_acc_reset_req_r));
    bufp->fullSData(oldp+291,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_r),16);
    bufp->fullBit(oldp+292,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_reset_req));
    bufp->fullBit(oldp+293,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__output_update_req));
    bufp->fullCData(oldp+294,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__current_active_osc_r),5);
    bufp->fullCData(oldp+295,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__active_osc_count_r),5);
    bufp->fullBit(oldp+296,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__prev_cs_n_r));
    bufp->fullCData(oldp+297,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_state_r),3);
    bufp->fullSData(oldp+298,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_out_r),16);
    bufp->fullSData(oldp+299,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mono_mix_r),16);
    bufp->fullIData(oldp+300,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_mono_mix_r),24);
    bufp->fullSData(oldp+301,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__left_mix_r),16);
    bufp->fullSData(oldp+302,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressed_left_mix_r),16);
    bufp->fullIData(oldp+303,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_left_mix_r),24);
    bufp->fullSData(oldp+304,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__right_mix_r),16);
    bufp->fullSData(oldp+305,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressed_right_mix_r),16);
    bufp->fullIData(oldp+306,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_right_mix_r),24);
    bufp->fullSData(oldp+307,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[0]),16);
    bufp->fullSData(oldp+308,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[1]),16);
    bufp->fullSData(oldp+309,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[2]),16);
    bufp->fullSData(oldp+310,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[3]),16);
    bufp->fullSData(oldp+311,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[4]),16);
    bufp->fullSData(oldp+312,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[5]),16);
    bufp->fullSData(oldp+313,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[6]),16);
    bufp->fullSData(oldp+314,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[7]),16);
    bufp->fullSData(oldp+315,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[8]),16);
    bufp->fullSData(oldp+316,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[9]),16);
    bufp->fullSData(oldp+317,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[10]),16);
    bufp->fullSData(oldp+318,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[11]),16);
    bufp->fullSData(oldp+319,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[12]),16);
    bufp->fullSData(oldp+320,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[13]),16);
    bufp->fullSData(oldp+321,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[14]),16);
    bufp->fullSData(oldp+322,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__channel_r[15]),16);
    bufp->fullIData(oldp+323,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[0]),24);
    bufp->fullIData(oldp+324,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[1]),24);
    bufp->fullIData(oldp+325,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[2]),24);
    bufp->fullIData(oldp+326,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[3]),24);
    bufp->fullIData(oldp+327,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[4]),24);
    bufp->fullIData(oldp+328,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[5]),24);
    bufp->fullIData(oldp+329,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[6]),24);
    bufp->fullIData(oldp+330,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[7]),24);
    bufp->fullIData(oldp+331,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[8]),24);
    bufp->fullIData(oldp+332,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[9]),24);
    bufp->fullIData(oldp+333,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[10]),24);
    bufp->fullIData(oldp+334,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[11]),24);
    bufp->fullIData(oldp+335,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[12]),24);
    bufp->fullIData(oldp+336,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[13]),24);
    bufp->fullIData(oldp+337,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[14]),24);
    bufp->fullIData(oldp+338,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__next_channel_r[15]),24);
    bufp->fullCData(oldp+339,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_state_r),2);
    bufp->fullCData(oldp+340,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_channel_r),4);
    bufp->fullIData(oldp+341,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__compressor_block__DOT__compressor__audio_in_l),24);
    bufp->fullIData(oldp+342,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__compressor_block__DOT__compressor__audio_in_r),24);
    bufp->fullQData(oldp+343,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__envelope_r),40);
    bufp->fullQData(oldp+345,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__gain_reduction_r),40);
    bufp->fullIData(oldp+347,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__abs_left_r),24);
    bufp->fullIData(oldp+348,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__abs_right_r),24);
    bufp->fullIData(oldp+349,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__max_abs_r),24);
    bufp->fullQData(oldp+350,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__level_fp),40);
    bufp->fullQData(oldp+352,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__gain_reduction),40);
    bufp->fullQData(oldp+354,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__level_normalized),40);
    bufp->fullQData(oldp+356,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__threshold_min),40);
    bufp->fullQData(oldp+358,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__threshold_max),40);
    bufp->fullQData(oldp+360,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__knee_factor),40);
    bufp->fullQData(oldp+362,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__amount_over),40);
    bufp->fullBit(oldp+364,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__unnamedblk1__DOT__sign_bit_l));
    bufp->fullIData(oldp+365,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__unnamedblk1__DOT__abs_input_l),32);
    bufp->fullIData(oldp+366,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__unnamedblk1__DOT__abs_scaled_l),32);
    bufp->fullSData(oldp+367,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__unnamedblk1__DOT__final_l),16);
    bufp->fullBit(oldp+368,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__unnamedblk2__DOT__sign_bit_r));
    bufp->fullIData(oldp+369,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__unnamedblk2__DOT__abs_input_r),32);
    bufp->fullIData(oldp+370,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__unnamedblk2__DOT__abs_scaled_r),32);
    bufp->fullSData(oldp+371,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__compressor_block__DOT__compressor__DOT__unnamedblk2__DOT__final_r),16);
    bufp->fullBit(oldp+372,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[0]));
    bufp->fullBit(oldp+373,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[1]));
    bufp->fullBit(oldp+374,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i[2]));
    bufp->fullCData(oldp+375,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+376,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[1]),5);
    bufp->fullCData(oldp+377,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_addr_i[2]),5);
    bufp->fullIData(oldp+378,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[0]),24);
    bufp->fullIData(oldp+379,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[1]),24);
    bufp->fullIData(oldp+380,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_data_i[2]),24);
    bufp->fullBit(oldp+381,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+382,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[1]));
    bufp->fullBit(oldp+383,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_w[2]));
    bufp->fullBit(oldp+384,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+385,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[1]));
    bufp->fullBit(oldp+386,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r[2]));
    bufp->fullIData(oldp+387,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[0]),24);
    bufp->fullIData(oldp+388,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[1]),24);
    bufp->fullIData(oldp+389,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[2]),24);
    bufp->fullIData(oldp+390,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[3]),24);
    bufp->fullIData(oldp+391,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[4]),24);
    bufp->fullIData(oldp+392,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[5]),24);
    bufp->fullIData(oldp+393,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[6]),24);
    bufp->fullIData(oldp+394,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[7]),24);
    bufp->fullIData(oldp+395,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[8]),24);
    bufp->fullIData(oldp+396,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[9]),24);
    bufp->fullIData(oldp+397,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[10]),24);
    bufp->fullIData(oldp+398,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[11]),24);
    bufp->fullIData(oldp+399,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[12]),24);
    bufp->fullIData(oldp+400,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[13]),24);
    bufp->fullIData(oldp+401,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[14]),24);
    bufp->fullIData(oldp+402,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[15]),24);
    bufp->fullIData(oldp+403,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[16]),24);
    bufp->fullIData(oldp+404,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[17]),24);
    bufp->fullIData(oldp+405,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[18]),24);
    bufp->fullIData(oldp+406,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[19]),24);
    bufp->fullIData(oldp+407,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[20]),24);
    bufp->fullIData(oldp+408,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[21]),24);
    bufp->fullIData(oldp+409,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[22]),24);
    bufp->fullIData(oldp+410,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[23]),24);
    bufp->fullIData(oldp+411,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[24]),24);
    bufp->fullIData(oldp+412,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[25]),24);
    bufp->fullIData(oldp+413,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[26]),24);
    bufp->fullIData(oldp+414,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[27]),24);
    bufp->fullIData(oldp+415,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[28]),24);
    bufp->fullIData(oldp+416,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[29]),24);
    bufp->fullIData(oldp+417,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[30]),24);
    bufp->fullIData(oldp+418,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__data_r[31]),24);
    bufp->fullBit(oldp+419,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+420,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+421,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+422,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+423,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                            [0U]));
    bufp->fullBit(oldp+424,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+425,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+426,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+427,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+428,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+429,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+430,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+431,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                            [1U]));
    bufp->fullBit(oldp+432,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                            [1U]));
    bufp->fullBit(oldp+433,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+434,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+435,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+436,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+437,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+438,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+439,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_acc_r_inst__priority_write_req_i
                            [2U]));
    bufp->fullBit(oldp+440,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_ack_r
                            [2U]));
    bufp->fullBit(oldp+441,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+442,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+443,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+444,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+445,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+446,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+447,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+448,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+449,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullIData(oldp+450,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_acc_r_inst__DOT__unnamedblk6__DOT__data_curr_r),24);
    bufp->fullCData(oldp+451,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[0]),8);
    bufp->fullCData(oldp+452,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[1]),8);
    bufp->fullCData(oldp+453,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_control_r_inst__priority_read_data_o[2]),8);
    bufp->fullCData(oldp+454,((0x1fU & ((IData)(1U) 
                                        + (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__mixer_cycle_r)))),5);
    bufp->fullBit(oldp+455,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+456,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[1]));
    bufp->fullBit(oldp+457,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[2]));
    bufp->fullBit(oldp+458,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r[3]));
    bufp->fullBit(oldp+459,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullBit(oldp+460,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[1]));
    bufp->fullBit(oldp+461,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r[2]));
    bufp->fullCData(oldp+462,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+463,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[1]),8);
    bufp->fullCData(oldp+464,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_data_r[2]),8);
    bufp->fullCData(oldp+465,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+466,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+467,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+468,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+469,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+470,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+471,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+472,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+473,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+474,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+475,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+476,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+477,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+478,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+479,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+480,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+481,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+482,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+483,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+484,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+485,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+486,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+487,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+488,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+489,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+490,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+491,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+492,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+493,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+494,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+495,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+496,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+497,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+498,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+499,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+500,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+501,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+502,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                            [1U]));
    bufp->fullBit(oldp+503,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+504,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+505,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+506,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__1__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+507,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_read_ack_r
                            [2U]));
    bufp->fullBit(oldp+508,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+509,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+510,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+511,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__2__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+512,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                            [1U]));
    bufp->fullBit(oldp+513,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+514,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+515,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+516,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+517,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                            [2U]));
    bufp->fullBit(oldp+518,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+519,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+520,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+521,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+522,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_ack_r
                            [3U]));
    bufp->fullBit(oldp+523,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+524,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+525,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+526,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+527,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+528,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+529,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+530,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_control_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+531,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fh_r_inst__priority_read_data_o[0]),8);
    bufp->fullCData(oldp+532,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_a_r),8);
    bufp->fullBit(oldp+533,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+534,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+535,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+536,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+537,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+538,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+539,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+540,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+541,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+542,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+543,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+544,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+545,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+546,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+547,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+548,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+549,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+550,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+551,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+552,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+553,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+554,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+555,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+556,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+557,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+558,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+559,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+560,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+561,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+562,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+563,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+564,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+565,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+566,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+567,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+568,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+569,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+570,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+571,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+572,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+573,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+574,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+575,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+576,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+577,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+578,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+579,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+580,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+581,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fh_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+582,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_fl_r_inst__priority_read_data_o[0]),8);
    bufp->fullCData(oldp+583,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_a_r),8);
    bufp->fullBit(oldp+584,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+585,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+586,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+587,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+588,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+589,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+590,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+591,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+592,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+593,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+594,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+595,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+596,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+597,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+598,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+599,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+600,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+601,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+602,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+603,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+604,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+605,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+606,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+607,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+608,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+609,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+610,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+611,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+612,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+613,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+614,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+615,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+616,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+617,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+618,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+619,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+620,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+621,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+622,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+623,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+624,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+625,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+626,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+627,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+628,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+629,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+630,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+631,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+632,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_fl_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullBit(oldp+633,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[0]));
    bufp->fullBit(oldp+634,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[1]));
    bufp->fullBit(oldp+635,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i[2]));
    bufp->fullCData(oldp+636,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[0]),5);
    bufp->fullCData(oldp+637,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[1]),5);
    bufp->fullCData(oldp+638,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_addr_i[2]),5);
    bufp->fullSData(oldp+639,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[0]),16);
    bufp->fullSData(oldp+640,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[1]),16);
    bufp->fullSData(oldp+641,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_data_i[2]),16);
    bufp->fullBit(oldp+642,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[0]));
    bufp->fullBit(oldp+643,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[1]));
    bufp->fullBit(oldp+644,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_w[2]));
    bufp->fullBit(oldp+645,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+646,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[1]));
    bufp->fullBit(oldp+647,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r[2]));
    bufp->fullSData(oldp+648,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[0]),16);
    bufp->fullSData(oldp+649,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[1]),16);
    bufp->fullSData(oldp+650,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[2]),16);
    bufp->fullSData(oldp+651,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[3]),16);
    bufp->fullSData(oldp+652,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[4]),16);
    bufp->fullSData(oldp+653,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[5]),16);
    bufp->fullSData(oldp+654,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[6]),16);
    bufp->fullSData(oldp+655,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[7]),16);
    bufp->fullSData(oldp+656,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[8]),16);
    bufp->fullSData(oldp+657,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[9]),16);
    bufp->fullSData(oldp+658,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[10]),16);
    bufp->fullSData(oldp+659,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[11]),16);
    bufp->fullSData(oldp+660,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[12]),16);
    bufp->fullSData(oldp+661,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[13]),16);
    bufp->fullSData(oldp+662,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[14]),16);
    bufp->fullSData(oldp+663,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[15]),16);
    bufp->fullSData(oldp+664,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[16]),16);
    bufp->fullSData(oldp+665,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[17]),16);
    bufp->fullSData(oldp+666,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[18]),16);
    bufp->fullSData(oldp+667,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[19]),16);
    bufp->fullSData(oldp+668,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[20]),16);
    bufp->fullSData(oldp+669,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[21]),16);
    bufp->fullSData(oldp+670,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[22]),16);
    bufp->fullSData(oldp+671,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[23]),16);
    bufp->fullSData(oldp+672,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[24]),16);
    bufp->fullSData(oldp+673,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[25]),16);
    bufp->fullSData(oldp+674,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[26]),16);
    bufp->fullSData(oldp+675,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[27]),16);
    bufp->fullSData(oldp+676,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[28]),16);
    bufp->fullSData(oldp+677,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[29]),16);
    bufp->fullSData(oldp+678,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[30]),16);
    bufp->fullSData(oldp+679,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__data_r[31]),16);
    bufp->fullBit(oldp+680,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_en_r));
    bufp->fullIData(oldp+681,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_index),32);
    bufp->fullCData(oldp+682,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_addr_r),5);
    bufp->fullBit(oldp+683,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ctrl__DOT__found));
    bufp->fullBit(oldp+684,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
                            [1U]));
    bufp->fullBit(oldp+685,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
                            [1U]));
    bufp->fullBit(oldp+686,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+687,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+688,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+689,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+690,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+691,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+692,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellinp__osc_output_r_inst__priority_write_req_i
                            [2U]));
    bufp->fullBit(oldp+693,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_ack_r
                            [2U]));
    bufp->fullBit(oldp+694,((((~ (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w)) 
                              & (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w)) 
                             | (IData)(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r))));
    bufp->fullBit(oldp+695,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+696,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+697,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+698,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_s_w));
    bufp->fullBit(oldp+699,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+700,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+701,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_output_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullCData(oldp+702,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_rts_r_inst__priority_read_data_o[0]),8);
    bufp->fullBit(oldp+703,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+704,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+705,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+706,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+707,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+708,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+709,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+710,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+711,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+712,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+713,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+714,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+715,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+716,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+717,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+718,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+719,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+720,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+721,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+722,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+723,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+724,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+725,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+726,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+727,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+728,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+729,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+730,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+731,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+732,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+733,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+734,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+735,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+736,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+737,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+738,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+739,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+740,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+741,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+742,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+743,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+744,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+745,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+746,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+747,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+748,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+749,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+750,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+751,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_rts_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+752,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_vol_r_inst__priority_read_data_o[0]),8);
    bufp->fullBit(oldp+753,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+754,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r[1]));
    bufp->fullBit(oldp+755,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+756,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+757,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+758,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+759,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+760,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+761,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+762,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+763,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+764,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+765,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+766,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+767,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+768,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+769,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+770,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+771,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+772,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+773,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+774,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+775,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+776,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+777,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+778,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+779,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+780,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+781,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+782,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+783,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+784,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+785,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+786,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+787,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+788,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+789,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+790,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+791,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+792,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+793,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+794,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+795,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+796,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+797,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+798,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+799,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_ack_r
                            [1U]));
    bufp->fullBit(oldp+800,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+801,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+802,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+803,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+804,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+805,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+806,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+807,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_vol_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+808,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wds_r_inst__priority_read_data_o[0]),8);
    bufp->fullBit(oldp+809,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+810,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[1]));
    bufp->fullBit(oldp+811,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[2]));
    bufp->fullBit(oldp+812,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r[3]));
    bufp->fullBit(oldp+813,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+814,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+815,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+816,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+817,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+818,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+819,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+820,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+821,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+822,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+823,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+824,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+825,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+826,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+827,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+828,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+829,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+830,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+831,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+832,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+833,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+834,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+835,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+836,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+837,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+838,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+839,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+840,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+841,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+842,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+843,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+844,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+845,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+846,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+847,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+848,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+849,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+850,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+851,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+852,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                            [1U]));
    bufp->fullBit(oldp+853,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+854,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+855,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+856,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__1__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+857,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                            [2U]));
    bufp->fullBit(oldp+858,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+859,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+860,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+861,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__2__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+862,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_ack_r
                            [3U]));
    bufp->fullBit(oldp+863,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+864,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+865,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+866,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__priority_write_port_loop__BRA__3__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+867,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+868,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+869,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+870,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wds_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+871,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT____Vcellout__osc_wtp_r_inst__priority_read_data_o[0]),8);
    bufp->fullBit(oldp+872,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r[0]));
    bufp->fullBit(oldp+873,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r[0]));
    bufp->fullCData(oldp+874,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_data_r[0]),8);
    bufp->fullCData(oldp+875,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[0]),8);
    bufp->fullCData(oldp+876,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[1]),8);
    bufp->fullCData(oldp+877,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[2]),8);
    bufp->fullCData(oldp+878,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[3]),8);
    bufp->fullCData(oldp+879,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[4]),8);
    bufp->fullCData(oldp+880,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[5]),8);
    bufp->fullCData(oldp+881,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[6]),8);
    bufp->fullCData(oldp+882,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[7]),8);
    bufp->fullCData(oldp+883,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[8]),8);
    bufp->fullCData(oldp+884,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[9]),8);
    bufp->fullCData(oldp+885,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[10]),8);
    bufp->fullCData(oldp+886,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[11]),8);
    bufp->fullCData(oldp+887,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[12]),8);
    bufp->fullCData(oldp+888,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[13]),8);
    bufp->fullCData(oldp+889,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[14]),8);
    bufp->fullCData(oldp+890,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[15]),8);
    bufp->fullCData(oldp+891,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[16]),8);
    bufp->fullCData(oldp+892,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[17]),8);
    bufp->fullCData(oldp+893,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[18]),8);
    bufp->fullCData(oldp+894,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[19]),8);
    bufp->fullCData(oldp+895,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[20]),8);
    bufp->fullCData(oldp+896,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[21]),8);
    bufp->fullCData(oldp+897,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[22]),8);
    bufp->fullCData(oldp+898,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[23]),8);
    bufp->fullCData(oldp+899,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[24]),8);
    bufp->fullCData(oldp+900,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[25]),8);
    bufp->fullCData(oldp+901,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[26]),8);
    bufp->fullCData(oldp+902,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[27]),8);
    bufp->fullCData(oldp+903,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[28]),8);
    bufp->fullCData(oldp+904,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[29]),8);
    bufp->fullCData(oldp+905,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[30]),8);
    bufp->fullCData(oldp+906,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__data_r[31]),8);
    bufp->fullBit(oldp+907,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_read_ack_r
                            [0U]));
    bufp->fullBit(oldp+908,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+909,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+910,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__q_r));
    bufp->fullBit(oldp+911,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__gen_read_ports__DOT__priority_read_port_loop__BRA__0__KET____DOT__read_ff__DOT__edge_r_w));
    bufp->fullBit(oldp+912,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_ack_r
                            [0U]));
    bufp->fullBit(oldp+913,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_s_r));
    bufp->fullBit(oldp+914,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__prev_r_r));
    bufp->fullBit(oldp+915,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__q_r));
    bufp->fullBit(oldp+916,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__priority_write_port_loop__BRA__0__KET____DOT__gen_edge_triggered__DOT__write_ff__DOT__edge_r_w));
    bufp->fullIData(oldp+917,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk4__DOT__i),32);
    bufp->fullIData(oldp+918,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk5__DOT__i),32);
    bufp->fullIData(oldp+919,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__i),32);
    bufp->fullCData(oldp+920,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__osc_wtp_r_inst__DOT__unnamedblk6__DOT__data_curr_r),8);
    bufp->fullCData(oldp+921,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__data_w),8);
    bufp->fullSData(oldp+922,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk1__DOT__vol_s),9);
    bufp->fullCData(oldp+923,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk2__DOT__shift_w),5);
    bufp->fullIData(oldp+924,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__high_bit_w),32);
    bufp->fullBit(oldp+925,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__overflow));
    bufp->fullBit(oldp+926,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__zero_byte_w));
    bufp->fullCData(oldp+927,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_hi_mask_w),8);
    bufp->fullSData(oldp+928,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__ptr_w),16);
    bufp->fullSData(oldp+929,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk3__DOT__unnamedblk4__DOT__addr_w),16);
    bufp->fullCData(oldp+930,(vlSelfRef.doc5503_harness__DOT__doc5503_inst__DOT__unnamedblk5__DOT__ca),4);
    bufp->fullBit(oldp+931,(vlSelfRef.clk_i));
    bufp->fullBit(oldp+932,(vlSelfRef.reset_n_i));
    bufp->fullBit(oldp+933,(vlSelfRef.clk_en_i));
    bufp->fullBit(oldp+934,(vlSelfRef.irq_n_o));
    bufp->fullBit(oldp+935,(vlSelfRef.cs_n_i));
    bufp->fullBit(oldp+936,(vlSelfRef.we_n_i));
    bufp->fullCData(oldp+937,(vlSelfRef.addr_i),8);
    bufp->fullCData(oldp+938,(vlSelfRef.data_i),8);
    bufp->fullCData(oldp+939,(vlSelfRef.data_o),8);
    bufp->fullSData(oldp+940,(vlSelfRef.wave_address_o),16);
    bufp->fullBit(oldp+941,(vlSelfRef.wave_rd_o));
    bufp->fullBit(oldp+942,(vlSelfRef.wave_data_ready_i));
    bufp->fullCData(oldp+943,(vlSelfRef.wave_data_i),8);
    bufp->fullSData(oldp+944,(vlSelfRef.mono_mix_o),16);
    bufp->fullSData(oldp+945,(vlSelfRef.left_mix_o),16);
    bufp->fullSData(oldp+946,(vlSelfRef.right_mix_o),16);
    bufp->fullCData(oldp+947,(vlSelfRef.dbg_cycle_o),5);
    bufp->fullCData(oldp+948,(vlSelfRef.dbg_osc_state_o),3);
    bufp->fullCData(oldp+949,(vlSelfRef.dbg_vol_o),8);
    bufp->fullCData(oldp+950,(vlSelfRef.dbg_wds_o),8);
    bufp->fullSData(oldp+951,(vlSelfRef.dbg_output_o),16);
    bufp->fullBit(oldp+952,((1U & (~ (IData)(vlSelfRef.reset_n_i)))));
}
