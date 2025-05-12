// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vdoc5503_verilog_test.h for the primary calling header

#include "Vdoc5503_verilog_test__pch.h"
#include "Vdoc5503_verilog_test__Syms.h"
#include "Vdoc5503_verilog_test___024root.h"

void Vdoc5503_verilog_test___024root___ctor_var_reset(Vdoc5503_verilog_test___024root* vlSelf);

Vdoc5503_verilog_test___024root::Vdoc5503_verilog_test___024root(Vdoc5503_verilog_test__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vdoc5503_verilog_test___024root___ctor_var_reset(this);
}

void Vdoc5503_verilog_test___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vdoc5503_verilog_test___024root::~Vdoc5503_verilog_test___024root() {
}
