// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vdoc5503_verilog_test.h for the primary calling header

#include "Vdoc5503_verilog_test__pch.h"
#include "Vdoc5503_verilog_test__Syms.h"
#include "Vdoc5503_verilog_test___024root.h"

extern const VlWide<8>/*255:0*/ Vdoc5503_verilog_test__ConstPool__CONST_h7fef1898_0;

VL_INLINE_OPT VlCoroutine Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__1(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ doc5503_verilog_test__DOT__unnamedblk1_2__DOT____Vrepeat1;
    doc5503_verilog_test__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__0__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__0__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__0__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__0__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__1__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__1__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__1__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__1__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__2__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__2__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__2__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__2__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__3__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__3__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__3__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__3__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__4__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__4__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__4__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__4__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__5__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__5__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__5__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__5__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__6__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__6__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__6__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__6__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__7__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__7__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__7__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__7__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__8__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__8__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__8__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__8__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__9__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__9__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__9__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__9__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__10__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__10__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__10__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__10__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__11__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__11__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__11__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__11__reg_data = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__12__reg_addr;
    __Vtask_doc5503_verilog_test__DOT__write_register__12__reg_addr = 0;
    CData/*7:0*/ __Vtask_doc5503_verilog_test__DOT__write_register__12__reg_data;
    __Vtask_doc5503_verilog_test__DOT__write_register__12__reg_data = 0;
    // Body
    if (VL_TESTPLUSARGS_I(std::string{"trace"})) {
        vlSymsp->_vm_contextp__->dumpfile(VL_CVT_PACK_STR_NW(8, Vdoc5503_verilog_test__ConstPool__CONST_h7fef1898_0));
        vlSymsp->_traceDumpOpen();
    }
    VL_WRITEF_NX("=== DOC5503 Verilog SWAP MODE Test ===\nTesting SWAP mode functionality\n",0);
    vlSelfRef.doc5503_verilog_test__DOT__reset_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__wave_data_ready = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__wave_data = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__cycle_count = 0U;
    VL_WRITEF_NX("Resetting DOC5503...\n",0);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         91);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         91);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         91);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         91);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         91);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         91);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         91);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         91);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         91);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         91);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__reset_n = 1U;
    VL_WRITEF_NX("Reset released\nWaiting for DOC5503 to be ready...\n",0);
    while ((0U == (IData)(vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__cycle_state_r))) {
        co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge doc5503_verilog_test.clk)", 
                                                             "doc5503_verilog_swap_test.sv", 
                                                             100);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
    }
    VL_WRITEF_NX("DOC5503 is ready after %0# cycles\n\nEnabling oscillators 0 and 1 (0xE1 = 0x02)\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count);
    __Vtask_doc5503_verilog_test__DOT__write_register__0__reg_data = 2U;
    __Vtask_doc5503_verilog_test__DOT__write_register__0__reg_addr = 0xe1U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__0__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__0__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__0__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__0__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n\nConfiguring oscillator 0 registers\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__1__reg_data = 0U;
    __Vtask_doc5503_verilog_test__DOT__write_register__1__reg_addr = 0U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__1__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__1__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__1__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__1__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__2__reg_data = 1U;
    __Vtask_doc5503_verilog_test__DOT__write_register__2__reg_addr = 0x20U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__2__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__2__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__2__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__2__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__3__reg_data = 0xffU;
    __Vtask_doc5503_verilog_test__DOT__write_register__3__reg_addr = 0x40U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__3__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__3__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__3__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__3__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__4__reg_data = 0U;
    __Vtask_doc5503_verilog_test__DOT__write_register__4__reg_addr = 0x80U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__4__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__4__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__4__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__4__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__5__reg_data = 6U;
    __Vtask_doc5503_verilog_test__DOT__write_register__5__reg_addr = 0xa0U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__5__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__5__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__5__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__5__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__6__reg_data = 0U;
    __Vtask_doc5503_verilog_test__DOT__write_register__6__reg_addr = 0xc0U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__6__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__6__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__6__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__6__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n\nConfiguring oscillator 1 registers\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__7__reg_data = 0U;
    __Vtask_doc5503_verilog_test__DOT__write_register__7__reg_addr = 1U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__7__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__7__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__7__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__7__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__8__reg_data = 1U;
    __Vtask_doc5503_verilog_test__DOT__write_register__8__reg_addr = 0x21U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__8__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__8__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__8__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__8__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__9__reg_data = 0xffU;
    __Vtask_doc5503_verilog_test__DOT__write_register__9__reg_addr = 0x41U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__9__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__9__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__9__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__9__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__10__reg_data = 0U;
    __Vtask_doc5503_verilog_test__DOT__write_register__10__reg_addr = 0x81U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__10__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__10__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__10__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__10__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__11__reg_data = 7U;
    __Vtask_doc5503_verilog_test__DOT__write_register__11__reg_addr = 0xa1U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__11__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__11__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__11__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__11__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    __Vtask_doc5503_verilog_test__DOT__write_register__12__reg_data = 0U;
    __Vtask_doc5503_verilog_test__DOT__write_register__12__reg_addr = 0xc1U;
    VL_WRITEF_NX("  Writing register 0x%02x = 0x%02x\n",0,
                 8,__Vtask_doc5503_verilog_test__DOT__write_register__12__reg_addr,
                 8,(IData)(__Vtask_doc5503_verilog_test__DOT__write_register__12__reg_data));
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__addr = __Vtask_doc5503_verilog_test__DOT__write_register__12__reg_addr;
    vlSelfRef.doc5503_verilog_test__DOT__data_in = __Vtask_doc5503_verilog_test__DOT__write_register__12__reg_data;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         190);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 0U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 0U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         199);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    vlSelfRef.doc5503_verilog_test__DOT__cs_n = 1U;
    vlSelfRef.doc5503_verilog_test__DOT__we_n = 1U;
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge doc5503_verilog_test.clk)", 
                                                         "doc5503_verilog_swap_test.sv", 
                                                         209);
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    VL_WRITEF_NX("    Cycle %0#: CS=%0# WE=%0# CLK_EN=%0# OSC_EN=0x%02x\n\nConfiguration completed\nRunning simulation to observe SWAP behavior\n",0,
                 32,vlSelfRef.doc5503_verilog_test__DOT__cycle_count,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__cs_n),
                 1,vlSelfRef.doc5503_verilog_test__DOT__we_n,
                 1,(IData)(vlSelfRef.doc5503_verilog_test__DOT__clk_en),
                 8,vlSelfRef.doc5503_verilog_test__DOT__dut__DOT__osc_en_r);
    doc5503_verilog_test__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0x7a120U;
    while (VL_LTS_III(32, 0U, doc5503_verilog_test__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        co_await vlSelfRef.__VtrigSched_h49264d22__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge doc5503_verilog_test.clk)", 
                                                             "doc5503_verilog_swap_test.sv", 
                                                             157);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
        doc5503_verilog_test__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (doc5503_verilog_test__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    VL_FINISH_MT("doc5503_verilog_swap_test.sv", 159, "");
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_verilog_test___024root___dump_triggers__act(Vdoc5503_verilog_test___024root* vlSelf);
#endif  // VL_DEBUG

void Vdoc5503_verilog_test___024root___eval_triggers__act(Vdoc5503_verilog_test___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_verilog_test___024root___eval_triggers__act\n"); );
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered.set(0U, ((IData)(vlSelfRef.doc5503_verilog_test__DOT__clk) 
                                       & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__doc5503_verilog_test__DOT__clk__0))));
    vlSelfRef.__VactTriggered.set(1U, vlSelfRef.__VdlySched.awaitingCurrentTime());
    vlSelfRef.__Vtrigprevexpr___TOP__doc5503_verilog_test__DOT__clk__0 
        = vlSelfRef.doc5503_verilog_test__DOT__clk;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vdoc5503_verilog_test___024root___dump_triggers__act(vlSelf);
    }
#endif
}
