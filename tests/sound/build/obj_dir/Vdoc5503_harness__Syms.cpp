// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vdoc5503_harness__pch.h"
#include "Vdoc5503_harness.h"
#include "Vdoc5503_harness___024root.h"

// FUNCTIONS
Vdoc5503_harness__Syms::~Vdoc5503_harness__Syms()
{
}

Vdoc5503_harness__Syms::Vdoc5503_harness__Syms(VerilatedContext* contextp, const char* namep, Vdoc5503_harness* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
        // Check resources
        Verilated::stackCheck(279);
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-9);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
}
