#!/usr/bin/env bash
# S7b: the accelerator broker (gs_accel_broker + gs_mmu_replica) with the
# unchanged socket PHY and a 65C816 core, against the behavioural IIgs of the
# S2-S6 bench extended with the golden machine model.
# See boards/a2mega/docs/gs_socket_s7_accelerator_scoping.md 2.1-2.4, 6.2, 9
# and the header of tb_gs_broker.sv.
#
# Usage: boards/a2mega/hdl/twgs/sim/broker/run_gs_broker_sim.sh [program|rom|all]
#   program   broker bench on the built test ROM (broker_test.s): every mode,
#             108 MHz / 4 clks per step and 100 MHz / 2 clks per step, IRQ,
#             RDY stalls, a BE pause, a warm reset, expansion RAM, local slot
#             firmware, the F65C816 core; plus the same program in native, paced and
#             fast mode with the step hash and the write stream compared
#   rom       end-to-end: a ROM 01 image booted in native (S6), paced and
#             fast mode; all runs must reach the same point (step hash) and
#             show the machine the same writes to I/O, Mega II and shadowed
#             ranges, in the same order.  Needs ROM=<boot1.rom>.
#   all       both (default; the rom suite is skipped with a notice if ROM
#             is not set)
# Env: OUT   work dir, default ${TMPDIR:-/tmp}/gs_broker_sim
#      ROM   path to a ROM 01 image (128 KB), e.g. the MiSTer vsim/boot1.rom
#            built by Apple-IIgs_MiSTer/roms/Makefile; Apple ROM content is
#            never committed
#      STEPS core steps for the rom suite (default 3000000)
# Requires Verilator >= 5 (timing support), ca65/ld65.
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SIM="$(cd "$HERE/.." && pwd)"
TWGS="$(cd "$SIM/.." && pwd)"
CORE="$TWGS/65C816"
FAST="$TWGS/fast"
OUT="${OUT:-${TMPDIR:-/tmp}/gs_broker_sim}"
SUITE="${1:-all}"
STEPS="${STEPS:-3000000}"
mkdir -p "$OUT"

# ---------------------------------------------------------------- test ROM
ca65 --cpu 65816 -o "$OUT/broker_test.o" -l "$OUT/broker_test.lst" "$HERE/broker_test.s" || exit 1
ld65 -C "$HERE/broker_rom.cfg" -o "$OUT/broker_test.rom" -m "$OUT/broker_test.map" "$OUT/broker_test.o" || exit 1
if [[ "$(wc -c < "$OUT/broker_test.rom" | tr -d ' ')" != "131072" ]]; then
    echo "test ROM is not 131072 bytes"; exit 1
fi

# ---------------------------------------------------------------- builds
STOCK=( "$CORE/P65C816_pkg.sv" "$CORE/mcode.sv" "$CORE/bit_adder.v" "$CORE/adder4.v"
        "$CORE/BCDAdder.v" "$CORE/AddSubBCD.sv" "$CORE/ALU.sv" "$CORE/AddrGen.sv" "$CORE/P65C816.sv" )
FASTF=( "$FAST/F65C816_pkg.sv" "$FAST/mcode.sv" "$FAST/AddSubBCD.sv" "$FAST/ALU.sv" "$FAST/AddrGen.sv"
        "$FAST/AddrBus.sv" "$FAST/F65C816.sv" )
DUT=( "$TWGS/gs_socket_phy.sv" "$TWGS/gs_mmu_decode.sv" "$TWGS/gs_mmu_replica.sv" "$TWGS/gs_accel_broker.sv"
      "$HERE/tb_gs_broker.sv" "$SIM/mmu/iigs_machine_dpi.cpp" )

build() {   # name, extra verilator args...
    local name="$1"; shift
    verilator --binary --timing -Wno-fatal -Wno-lint -Wno-style -Wno-MULTIDRIVEN -O3 \
        --top-module tb_gs_broker --Mdir "$OUT/obj_$name" -o Vtb_gs_broker \
        -CFLAGS "-O2 -I$SIM/mmu" "$@" "${STOCK[@]}" "${DUT[@]}" > "$OUT/verilate_$name.log" 2>&1
}
build b108_4                                  & p1=$!
build b100_2 +define+CLK100 -GSTEP_CLKS=2     & p2=$!
build b108_4f +define+FAST_CORE "${FASTF[@]}" & p3=$!
# data-late step (broker launches the next step's read ahead, F65C816.NA_OUT): three clks of
# 108 MHz = 36 MHz steps with two clks to decode the address, and two clks of 100 MHz = 50 MHz
build b108_3d +define+FAST_CORE -GDATA_LATE=1 -GSTEP_CLKS=3 -GADDR_CLKS=2 "${FASTF[@]}" & p4=$!
build b100_2d +define+FAST_CORE +define+CLK100 -GDATA_LATE=1 -GSTEP_CLKS=2 -GADDR_CLKS=1 "${FASTF[@]}" & p5=$!
bfail=0
wait $p1 || { echo "verilator FAILED (b108_4) - see $OUT/verilate_b108_4.log"; grep -E "%Error|error:" "$OUT/verilate_b108_4.log" | head -20; bfail=1; }
wait $p2 || { echo "verilator FAILED (b100_2) - see $OUT/verilate_b100_2.log"; grep -E "%Error|error:" "$OUT/verilate_b100_2.log" | head -20; bfail=1; }
wait $p3 || { echo "verilator FAILED (b108_4f, F65C816) - see $OUT/verilate_b108_4f.log"; grep -E "%Error|error:" "$OUT/verilate_b108_4f.log" | head -20; bfail=1; }
wait $p4 || { echo "verilator FAILED (b108_3d) - see $OUT/verilate_b108_3d.log"; grep -E "%Error|error:" "$OUT/verilate_b108_3d.log" | head -20; bfail=1; }
wait $p5 || { echo "verilator FAILED (b100_2d) - see $OUT/verilate_b100_2d.log"; grep -E "%Error|error:" "$OUT/verilate_b100_2d.log" | head -20; bfail=1; }
[[ $bfail -ne 0 ]] && exit 1

# ---------------------------------------------------------------- runs
declare -a NAMES=()
run() {     # name, build, args...
    local name="$1" bld="$2"; shift 2
    NAMES+=( "$name" )
    mkdir -p "$OUT/$name"
    ( cd "$OUT/$name" && "$OUT/obj_$bld/Vtb_gs_broker" "$@" > sim.log 2>&1 ) &
}
T="+rom=$OUT/broker_test.rom"
if [[ "$SUITE" == "program" || "$SUITE" == "all" ]]; then
    # (b) every mode, with interrupts, RDY stalls and a BE pause
    run p_native    b108_4  $T +mode=native +expect +irq
    run p_paced     b108_4  $T +mode=paced  +expect +irq
    run p_fast      b108_4  $T +mode=fast   +expect +irq
    run p_toggle    b108_4  $T +mode=toggle +expect +irq +steps=120000
    run p_fast_exp  b108_4  $T +mode=fast   +expect +irq +exp=2
    run p_fast_mb   b108_4  $T +mode=fast   +expect +irq +exp=1 +mbexp=4
    run p_fast_cx   b108_4  $T +mode=fast   +expect +irq +cxrom=1
    run p_tog_cx    b108_4  $T +mode=toggle +expect +irq +cxrom=1 +exp=2 +steps=120000
    run p_fast_1mhz b108_4  $T +mode=fast   +expect +irq +noturbo
    run p_rst_nat   b108_4  $T +mode=native +expect +irq +reset
    run p_rst_fast  b108_4  $T +mode=fast   +expect +irq +reset
    run p_rst_tog   b108_4  $T +mode=toggle +expect +irq +reset +steps=120000
    run p_f816      b108_4f $T +mode=fast   +expect +irq
    run p_f816_nat  b108_4f $T +mode=native +expect +irq
    # data-late broker: every mode again, at 108/3 and 100/2
    run d_native    b108_3d $T +mode=native +expect +irq
    run d_paced     b108_3d $T +mode=paced  +expect +irq
    run d_fast      b108_3d $T +mode=fast   +expect +irq
    run d_toggle    b108_3d $T +mode=toggle +expect +irq +steps=120000
    run d_fast_exp  b108_3d $T +mode=fast   +expect +irq +exp=2
    run d_tog_cx    b108_3d $T +mode=toggle +expect +irq +cxrom=1 +exp=2 +steps=120000
    run d_fast_1mhz b108_3d $T +mode=fast   +expect +irq +noturbo
    run d_rst_fast  b108_3d $T +mode=fast   +expect +irq +reset
    run d_rst_tog   b108_3d $T +mode=toggle +expect +irq +reset +steps=120000
    run d2_fast     b100_2d $T +mode=fast   +expect +irq
    run d2_toggle   b100_2d $T +mode=toggle +expect +irq +steps=120000
    run q_native    b100_2  $T +mode=native +expect +irq
    run q_fast      b100_2  $T +mode=fast   +expect +irq
    run q_toggle    b100_2  $T +mode=toggle +expect +irq +steps=120000
    # the same program, no interrupts: native = paced = fast, step for step
    run e_native    b108_4  $T +mode=native +expect +wlog=wstream.txt
    run e_paced     b108_4  $T +mode=paced  +expect +wlog=wstream.txt
    run e_fast      b108_4  $T +mode=fast   +expect +wlog=wstream.txt
    run e_fast100   b100_2  $T +mode=fast   +expect +wlog=wstream.txt
    run e_dl108     b108_3d $T +mode=fast   +expect +wlog=wstream.txt
    run e_dl100     b100_2d $T +mode=fast   +expect +wlog=wstream.txt
fi
DO_ROM=0
if [[ "$SUITE" == "rom" || "$SUITE" == "all" ]]; then
    if [[ -n "${ROM:-}" && -f "${ROM:-}" ]]; then
        DO_ROM=1
        R="+rom=$ROM +steps=$STEPS +wlog=wstream.txt"
        run r_native    b108_4  $R +mode=native
        run r_paced     b108_4  $R +mode=paced
        run r_fast      b108_4  $R +mode=fast
        run r_fast_cx   b108_4  $R +mode=fast +cxrom=1
        run r_fast100   b100_2  $R +mode=fast
        run r_f816      b108_4f $R +mode=fast
        run r_dl108     b108_3d $R +mode=fast
        run r_dl100     b100_2d $R +mode=fast
    elif [[ "$SUITE" == "rom" ]]; then
        echo "rom suite: set ROM=<path to a 128 KB ROM 01 image>"; exit 1
    else
        echo "NOTE: ROM not set - the ROM 01 end-to-end suite is skipped"
    fi
fi
wait

# ---------------------------------------------------------------- results
fail=0
printf '%-12s %-5s %s\n' run result summary
for n in "${NAMES[@]}"; do
    log="$OUT/$n/sim.log"
    if grep -q '^PASS$' "$log"; then res=PASS; else res=FAIL; fail=1; fi
    printf '%-12s %-5s %s\n' "$n" "$res" "$(grep -E '^---- run:' "$log" | sed 's/^---- run: //')"
    if [[ $res == FAIL ]]; then grep -E "ERROR|FAIL|TIMEOUT|%Error" "$log" | head -8; fi
done

same() {    # label, reference run, other runs...: step hash and write stream must be identical
    local label="$1" ref="$2"; shift 2
    local h0 h t
    h0="$(grep -E '^---- point reached' "$OUT/$ref/sim.log")"
    t="$(grep -E '^---- time:' "$OUT/$ref/sim.log" | sed 's/^---- time: //')"
    echo "$label: $ref ${h0#---- }; write stream $(wc -l < "$OUT/$ref/wstream.txt" | tr -d ' ') writes"
    echo "  $ref: $t"
    for n in "$@"; do
        h="$(grep -E '^---- point reached' "$OUT/$n/sim.log")"
        t="$(grep -E '^---- time:' "$OUT/$n/sim.log" | sed 's/^---- time: //')"
        if [[ -z "$h0" || "$h" != "$h0" ]]; then
            echo "  $n: DIFFERENT POINT: ${h#---- }"; fail=1
        elif ! cmp -s "$OUT/$ref/wstream.txt" "$OUT/$n/wstream.txt"; then
            echo "  $n: same point, WRITE STREAM DIFFERS:"; fail=1
            diff "$OUT/$ref/wstream.txt" "$OUT/$n/wstream.txt" | head -6
        else
            echo "  $n: same point, identical write stream; $t"
        fi
    done
}
if [[ "$SUITE" == "program" || "$SUITE" == "all" ]]; then
    same "test program" e_native e_paced e_fast e_fast100 e_dl108 e_dl100
fi
if [[ $DO_ROM -eq 1 ]]; then
    same "ROM 01 boot ($STEPS steps)" r_native r_paced r_fast r_fast_cx r_fast100 r_f816 r_dl108 r_dl100
fi
for n in "${NAMES[@]}"; do
    case "$n" in d*|e_dl*|r_dl*) grep -E '^---- local reads' "$OUT/$n/sim.log" | sed "s/^---- /$n: /" ;; esac
done
for n in "${NAMES[@]}"; do
    grep -E '^---- socket timing' "$OUT/$n/sim.log" | sed "s/^---- /$n: /"
done > "$OUT/timing.txt"
echo "socket timing per run: $OUT/timing.txt"
if [[ $fail -eq 0 ]]; then echo "PASS"; else echo "FAIL"; fi
exit $fail
