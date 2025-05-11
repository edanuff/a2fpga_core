// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vdoc5503_reg_harness__pch.h"
#include "verilated_vcd_c.h"

//============================================================
// Constructors

Vdoc5503_reg_harness::Vdoc5503_reg_harness(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vdoc5503_reg_harness__Syms(contextp(), _vcname__, this)}
    , clk_i{vlSymsp->TOP.clk_i}
    , reset_n_i{vlSymsp->TOP.reset_n_i}
    , clk_en_i{vlSymsp->TOP.clk_en_i}
    , cs_n_i{vlSymsp->TOP.cs_n_i}
    , we_n_i{vlSymsp->TOP.we_n_i}
    , addr_i{vlSymsp->TOP.addr_i}
    , data_i{vlSymsp->TOP.data_i}
    , data_o{vlSymsp->TOP.data_o}
    , wave_rd_o{vlSymsp->TOP.wave_rd_o}
    , wave_data_ready_i{vlSymsp->TOP.wave_data_ready_i}
    , wave_data_i{vlSymsp->TOP.wave_data_i}
    , ready_o{vlSymsp->TOP.ready_o}
    , wave_address_o{vlSymsp->TOP.wave_address_o}
    , mono_mix_o{vlSymsp->TOP.mono_mix_o}
    , left_mix_o{vlSymsp->TOP.left_mix_o}
    , right_mix_o{vlSymsp->TOP.right_mix_o}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
    contextp()->traceBaseModelCbAdd(
        [this](VerilatedTraceBaseC* tfp, int levels, int options) { traceBaseModel(tfp, levels, options); });
}

Vdoc5503_reg_harness::Vdoc5503_reg_harness(const char* _vcname__)
    : Vdoc5503_reg_harness(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vdoc5503_reg_harness::~Vdoc5503_reg_harness() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vdoc5503_reg_harness___024root___eval_debug_assertions(Vdoc5503_reg_harness___024root* vlSelf);
#endif  // VL_DEBUG
void Vdoc5503_reg_harness___024root___eval_static(Vdoc5503_reg_harness___024root* vlSelf);
void Vdoc5503_reg_harness___024root___eval_initial(Vdoc5503_reg_harness___024root* vlSelf);
void Vdoc5503_reg_harness___024root___eval_settle(Vdoc5503_reg_harness___024root* vlSelf);
void Vdoc5503_reg_harness___024root___eval(Vdoc5503_reg_harness___024root* vlSelf);

void Vdoc5503_reg_harness::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vdoc5503_reg_harness::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vdoc5503_reg_harness___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_activity = true;
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vdoc5503_reg_harness___024root___eval_static(&(vlSymsp->TOP));
        Vdoc5503_reg_harness___024root___eval_initial(&(vlSymsp->TOP));
        Vdoc5503_reg_harness___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vdoc5503_reg_harness___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vdoc5503_reg_harness::eventsPending() { return false; }

uint64_t Vdoc5503_reg_harness::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vdoc5503_reg_harness::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vdoc5503_reg_harness___024root___eval_final(Vdoc5503_reg_harness___024root* vlSelf);

VL_ATTR_COLD void Vdoc5503_reg_harness::final() {
    Vdoc5503_reg_harness___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vdoc5503_reg_harness::hierName() const { return vlSymsp->name(); }
const char* Vdoc5503_reg_harness::modelName() const { return "Vdoc5503_reg_harness"; }
unsigned Vdoc5503_reg_harness::threads() const { return 1; }
void Vdoc5503_reg_harness::prepareClone() const { contextp()->prepareClone(); }
void Vdoc5503_reg_harness::atClone() const {
    contextp()->threadPoolpOnClone();
}
std::unique_ptr<VerilatedTraceConfig> Vdoc5503_reg_harness::traceConfig() const {
    return std::unique_ptr<VerilatedTraceConfig>{new VerilatedTraceConfig{false, false, false}};
};

//============================================================
// Trace configuration

void Vdoc5503_reg_harness___024root__trace_decl_types(VerilatedVcd* tracep);

void Vdoc5503_reg_harness___024root__trace_init_top(Vdoc5503_reg_harness___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD static void trace_init(void* voidSelf, VerilatedVcd* tracep, uint32_t code) {
    // Callback from tracep->open()
    Vdoc5503_reg_harness___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vdoc5503_reg_harness___024root*>(voidSelf);
    Vdoc5503_reg_harness__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (!vlSymsp->_vm_contextp__->calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
            "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->pushPrefix(std::string{vlSymsp->name()}, VerilatedTracePrefixType::SCOPE_MODULE);
    Vdoc5503_reg_harness___024root__trace_decl_types(tracep);
    Vdoc5503_reg_harness___024root__trace_init_top(vlSelf, tracep);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vdoc5503_reg_harness___024root__trace_register(Vdoc5503_reg_harness___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD void Vdoc5503_reg_harness::traceBaseModel(VerilatedTraceBaseC* tfp, int levels, int options) {
    (void)levels; (void)options;
    VerilatedVcdC* const stfp = dynamic_cast<VerilatedVcdC*>(tfp);
    if (VL_UNLIKELY(!stfp)) {
        vl_fatal(__FILE__, __LINE__, __FILE__,"'Vdoc5503_reg_harness::trace()' called on non-VerilatedVcdC object;"
            " use --trace-fst with VerilatedFst object, and --trace with VerilatedVcd object");
    }
    stfp->spTrace()->addModel(this);
    stfp->spTrace()->addInitCb(&trace_init, &(vlSymsp->TOP));
    Vdoc5503_reg_harness___024root__trace_register(&(vlSymsp->TOP), stfp->spTrace());
}
