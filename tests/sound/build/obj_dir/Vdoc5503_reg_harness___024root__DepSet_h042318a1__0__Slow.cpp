// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vdoc5503_reg_harness.h for the primary calling header

#include "Vdoc5503_reg_harness__pch.h"
#include "Vdoc5503_reg_harness__Syms.h"
#include "Vdoc5503_reg_harness___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vdoc5503_reg_harness___024root___dump_triggers__stl(Vdoc5503_reg_harness___024root* vlSelf);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vdoc5503_reg_harness___024root___eval_triggers__stl(Vdoc5503_reg_harness___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vdoc5503_reg_harness___024root___eval_triggers__stl\n"); );
    Vdoc5503_reg_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered.set(0U, (IData)(vlSelfRef.__VstlFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vdoc5503_reg_harness___024root___dump_triggers__stl(vlSelf);
    }
#endif
}
