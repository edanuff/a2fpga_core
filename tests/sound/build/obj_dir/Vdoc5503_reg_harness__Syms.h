// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VDOC5503_REG_HARNESS__SYMS_H_
#define VERILATED_VDOC5503_REG_HARNESS__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vdoc5503_reg_harness.h"

// INCLUDE MODULE CLASSES
#include "Vdoc5503_reg_harness___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vdoc5503_reg_harness__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vdoc5503_reg_harness* const __Vm_modelp;
    bool __Vm_activity = false;  ///< Used by trace routines to determine change occurred
    uint32_t __Vm_baseCode = 0;  ///< Used by trace routines when tracing multiple models
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vdoc5503_reg_harness___024root TOP;

    // CONSTRUCTORS
    Vdoc5503_reg_harness__Syms(VerilatedContext* contextp, const char* namep, Vdoc5503_reg_harness* modelp);
    ~Vdoc5503_reg_harness__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
