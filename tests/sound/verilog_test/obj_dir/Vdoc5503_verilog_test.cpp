// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vdoc5503_verilog_test__pch.h"
#include "verilated_vcd_c.h"

//============================================================
// Constructors

Vdoc5503_verilog_test::Vdoc5503_verilog_test(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vdoc5503_verilog_test__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , s{vlSymsp->TOP.s}
    , r{vlSymsp->TOP.r}
    , q{vlSymsp->TOP.q}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
    contextp()->traceBaseModelCbAdd(
        [this](VerilatedTraceBaseC* tfp, int levels, int options) { traceBaseModel(tfp, levels, options); });
}

Vdoc5503_verilog_test::Vdoc5503_verilog_test(const char* _vcname__)
    : Vdoc5503_verilog_test(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vdoc5503_verilog_test::~Vdoc5503_verilog_test() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vdoc5503_verilog_test___024root___eval_debug_assertions(Vdoc5503_verilog_test___024root* vlSelf);
#endif  // VL_DEBUG
void Vdoc5503_verilog_test___024root___eval_static(Vdoc5503_verilog_test___024root* vlSelf);
void Vdoc5503_verilog_test___024root___eval_initial(Vdoc5503_verilog_test___024root* vlSelf);
void Vdoc5503_verilog_test___024root___eval_settle(Vdoc5503_verilog_test___024root* vlSelf);
void Vdoc5503_verilog_test___024root___eval(Vdoc5503_verilog_test___024root* vlSelf);

void Vdoc5503_verilog_test::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vdoc5503_verilog_test::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vdoc5503_verilog_test___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_activity = true;
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vdoc5503_verilog_test___024root___eval_static(&(vlSymsp->TOP));
        Vdoc5503_verilog_test___024root___eval_initial(&(vlSymsp->TOP));
        Vdoc5503_verilog_test___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vdoc5503_verilog_test___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

void Vdoc5503_verilog_test::eval_end_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+eval_end_step Vdoc5503_verilog_test::eval_end_step\n"); );
#ifdef VM_TRACE
    // Tracing
    if (VL_UNLIKELY(vlSymsp->__Vm_dumping)) vlSymsp->_traceDump();
#endif  // VM_TRACE
}

//============================================================
// Events and timing
bool Vdoc5503_verilog_test::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty(); }

uint64_t Vdoc5503_verilog_test::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vdoc5503_verilog_test::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vdoc5503_verilog_test___024root___eval_final(Vdoc5503_verilog_test___024root* vlSelf);

VL_ATTR_COLD void Vdoc5503_verilog_test::final() {
    Vdoc5503_verilog_test___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vdoc5503_verilog_test::hierName() const { return vlSymsp->name(); }
const char* Vdoc5503_verilog_test::modelName() const { return "Vdoc5503_verilog_test"; }
unsigned Vdoc5503_verilog_test::threads() const { return 1; }
void Vdoc5503_verilog_test::prepareClone() const { contextp()->prepareClone(); }
void Vdoc5503_verilog_test::atClone() const {
    contextp()->threadPoolpOnClone();
}
std::unique_ptr<VerilatedTraceConfig> Vdoc5503_verilog_test::traceConfig() const {
    return std::unique_ptr<VerilatedTraceConfig>{new VerilatedTraceConfig{false, false, false}};
};

//============================================================
// Trace configuration

void Vdoc5503_verilog_test___024root__trace_decl_types(VerilatedVcd* tracep);

void Vdoc5503_verilog_test___024root__trace_init_top(Vdoc5503_verilog_test___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD static void trace_init(void* voidSelf, VerilatedVcd* tracep, uint32_t code) {
    // Callback from tracep->open()
    Vdoc5503_verilog_test___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vdoc5503_verilog_test___024root*>(voidSelf);
    Vdoc5503_verilog_test__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (!vlSymsp->_vm_contextp__->calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
            "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->pushPrefix(std::string{vlSymsp->name()}, VerilatedTracePrefixType::SCOPE_MODULE);
    Vdoc5503_verilog_test___024root__trace_decl_types(tracep);
    Vdoc5503_verilog_test___024root__trace_init_top(vlSelf, tracep);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vdoc5503_verilog_test___024root__trace_register(Vdoc5503_verilog_test___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD void Vdoc5503_verilog_test::traceBaseModel(VerilatedTraceBaseC* tfp, int levels, int options) {
    (void)levels; (void)options;
    VerilatedVcdC* const stfp = dynamic_cast<VerilatedVcdC*>(tfp);
    if (VL_UNLIKELY(!stfp)) {
        vl_fatal(__FILE__, __LINE__, __FILE__,"'Vdoc5503_verilog_test::trace()' called on non-VerilatedVcdC object;"
            " use --trace-fst with VerilatedFst object, and --trace with VerilatedVcd object");
    }
    stfp->spTrace()->addModel(this);
    stfp->spTrace()->addInitCb(&trace_init, &(vlSymsp->TOP));
    Vdoc5503_verilog_test___024root__trace_register(&(vlSymsp->TOP), stfp->spTrace());
}
