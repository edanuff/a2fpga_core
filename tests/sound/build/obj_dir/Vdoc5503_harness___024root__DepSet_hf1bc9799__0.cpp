// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vdoc5503_harness.h for the primary calling header

#include "Vdoc5503_harness__pch.h"
#include "Vdoc5503_harness__Syms.h"
#include "Vdoc5503_harness___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_harness___024root___dump_triggers__ico(Vdoc5503_harness___024root* vlSelf);
#endif  // VL_DEBUG

void Vdoc5503_harness___024root___eval_triggers__ico(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_triggers__ico\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered.set(0U, (IData)(vlSelfRef.__VicoFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vdoc5503_harness___024root___dump_triggers__ico(vlSelf);
    }
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_harness___024root___dump_triggers__act(Vdoc5503_harness___024root* vlSelf);
#endif  // VL_DEBUG

void Vdoc5503_harness___024root___eval_triggers__act(Vdoc5503_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_harness___024root___eval_triggers__act\n"); );
    Vdoc5503_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered.set(0U, ((IData)(vlSelfRef.clk) 
                                       & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__0))));
    vlSelfRef.__VactTriggered.set(1U, ((IData)(vlSelfRef.clk_i) 
                                       & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk_i__0))));
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__clk_i__0 = vlSelfRef.clk_i;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vdoc5503_harness___024root___dump_triggers__act(vlSelf);
    }
#endif
}
