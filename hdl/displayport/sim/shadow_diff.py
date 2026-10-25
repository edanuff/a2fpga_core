#!/usr/bin/env python3
"""shadow_diff.py — cycle-exact differential check of the AUX ladder against a
git revision (timing campaign round 2, 138B clk100 ladder, 2026-10-24).

Timing fixes in the ladder must not change behaviour on any cycle. This tool
builds the proof: it takes the AUX modules (channel_managemnt / aux_channel /
dp_aux_messages / aux_interface) from a reference git revision, renames them
*_ref, and rewrites the three FSM benches (tb_train_recover, tb_polite_attach,
tb_link_observe) so that a second channel_management built from the reference
modules runs beside the DUT on the SAME inputs (the benches' forced pulses are
mirrored onto it) while every output of the two is compared on every clock.
The sink model answers the DUT; the first cycle the reference would have
behaved differently is a DIFF line and a bench error.

Usage (repo root):
    python3 hdl/displayport/sim/shadow_diff.py <ref-rev> <outdir>
    sh <outdir>/run.sh            # compiles and runs all six scenarios
PASS lines come from the benches themselves; DIFFCHECK reports the compared
cycle count and the error total (diffs are counted as bench errors).
Requires iverilog/vvp.
"""
import os, re, subprocess, sys

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
AUX = 'hdl/displayport/auxch'
MODS = {'channel_managemnt.v': 'channel_management', 'aux_channel.v': 'aux_channel',
        'dp_aux_messages.v': 'dp_aux_messages', 'aux_interface.v': 'aux_interface'}
WIDTHS = {'auxch_out': '', 'auxch_tri': '', 'tx_clock_train': '', 'tx_align_train': '', 'tx_link_established': '',
 'tx_powerup_channel': '[3:0] ', 'debug': '[7:0] ', 'debug_gate': '[7:0] ', 'debug_teardown': '[15:0] ',
 'debug_aux_err': '[23:0] ', 'debug_err_detail': '[27:0] ', 'debug_snapshot': '[31:0] ', 'wedge_suspect': '',
 'adjust_evt': '', 'debug_adjust': '[15:0] ', 'debug_chstate': '[23:0] ', 'errcnt0': '[15:0] ', 'errcnt1': '[15:0] ',
 'debug_esi': '[15:0] ', 'debug_defer': '[6:0] ', 'debug_auxif': '[23:0] ', 'debug_rx': '[15:0] ', 'debug_locks': '[3:0] ',
 'debug_sink': '[7:0] ', 'hpd_present_out': '', 'tx_preemp_0p0': '', 'tx_preemp_3p5': '', 'tx_preemp_6p0': '',
 'tx_swing_0p4': '', 'tx_swing_0p6': '', 'tx_swing_0p8': '', 'debug_caps': '[7:0] ', 'debug_sink_caps': '[15:0] ', 'debug_obs': '[103:0] '}
INPUTS = {'clk100', 'hpd', 'auxch_in', 'afe_busy', 'train_set_byte', 'stream_channel_count', 'source_channel_count', 'tx_running'}
# debug_snapshot is compared against the reference delayed one cycle: the
# 2026-10-24 change latches the same value one cycle later by design.
BENCHES = [('tb_train_recover', ['', '+silent', '+exhaust'], False),
           ('tb_polite_attach', ['', '+giveup'], True),
           ('tb_link_observe', [''], True)]

def ref_modules(rev, out):
    for f, m in MODS.items():
        src = subprocess.check_output(['git', 'show', f'{rev}:{AUX}/{f}'], cwd=REPO, text=True)
        src = re.sub(rf'^module {m} ', f'module {m}_ref ', src, flags=re.M)
        for other in MODS.values():
            src = re.sub(rf'^{other} #\(', f'{other}_ref #(', src, flags=re.M)
        open(os.path.join(out, f.replace('.v', '_ref.v')), 'w').write(src)

def shadow(src, dst):
    s = open(src).read()
    m = re.search(r'\n    channel_management #\((.*?)\) dut \((.*?)\n    \);\n', s, re.S)
    assert m, src
    params, ports = m.group(1), m.group(2)
    conn, order = {}, []
    for line in ports.strip('\n').split('\n'):
        mm = re.match(r'\s*\.(\w+)\s*\((.*?)\),?\s*$', line)
        if mm: conn[mm.group(1)] = mm.group(2).strip(); order.append(mm.group(1))
    outs = list(WIDTHS)
    decl = []
    for o in outs:
        if o not in conn: order.append(o); conn[o] = ''
        if conn[o] == '':
            conn[o] = f'd_{o}'; decl.append(f'    wire {WIDTHS[o]}d_{o};')
        decl.append(f'    wire {WIDTHS[o]}r_{o};')
    dut_ports = [f'        .{n:<22}({conn[n]})' for n in order]
    ref_ports = [f'        .{n:<22}({"r_"+n if n in outs else (conn[n] if n in INPUTS else "")})' for n in order]
    cmp = []
    for o in outs:
        d = conn[o]
        if o == 'debug_snapshot':
            cmp.append(f'        if (r_snap_d !== {d}) begin errors = errors + 1; if (errors < 10) $display("DIFF debug_snapshot t=%0t new=%h ref(d1)=%h", $time, {d}, r_snap_d); end')
        else:
            cmp.append(f'        if (r_{o} !== {d}) begin errors = errors + 1; if (errors < 10) $display("DIFF {o} t=%0t new=%h ref=%h (state %02x / %02x)", $time, {d}, r_{o}, dut.i_aux_channel.state, ref0.i_aux_channel.state); end')
    block = '\n'.join(decl) + f'''
    reg [31:0] r_snap_d = 32'd0;
    always @(posedge clk100) r_snap_d <= r_debug_snapshot;

    channel_management_ref #({params}) ref0 (
''' + ',\n'.join(ref_ports) + '''
    );
    integer diff_checks = 0;
    always @(posedge clk100) begin
        diff_checks = diff_checks + 1;
''' + '\n'.join(cmp) + '''
    end
'''
    new = s[:m.start()] + '\n    channel_management #(' + params + ') dut (\n' + ',\n'.join(dut_ports) + '\n    );\n' + block + s[m.end():]
    name = re.search(r'module (tb_\w+);', s).group(1)
    new = new.replace(f'module {name};', f'module {name}_diff;', 1)
    new = re.sub(r'(\s*)(force|release) dut\.i_aux_channel\.(\w+)( = [^;]+)?;',
                 lambda mm: f"{mm.group(1)}{mm.group(2)} dut.i_aux_channel.{mm.group(3)}{mm.group(4) or ''};"
                            f"{mm.group(1)}{mm.group(2)} ref0.i_aux_channel.{mm.group(3)}{mm.group(4) or ''};", new)
    new = new.replace('\nendmodule', '\n    final $display("DIFFCHECK: %0d cycles compared, %0d bench errors incl. diffs", diff_checks, errors);\nendmodule')
    open(dst, 'w').write(new)

def main():
    rev, out = sys.argv[1], os.path.abspath(sys.argv[2])
    os.makedirs(out, exist_ok=True)
    ref_modules(rev, out)
    full = ' '.join(f'{AUX}/{f}' for f in ['channel_managemnt.v', 'aux_channel.v', 'dp_aux_messages.v', 'aux_interface.v',
                                            'link_signal_mgmt.v', 'edid_decode.v', 'dp_register_decode.v', 'hotplug_decode.v'])
    refs = ' '.join(os.path.join(out, f.replace('.v', '_ref.v')) for f in MODS)
    run = [f'#!/bin/sh\n# generated by shadow_diff.py against {rev}\ncd "{REPO}" || exit 1\nset -e\n']
    for tb, args, afe in BENCHES:
        shadow(os.path.join(REPO, 'hdl/displayport/sim', tb + '.v'), os.path.join(out, tb + '_diff.v'))
        extra = ' hdl/displayport/gowin/afe_adjust_seq.v' if afe else ''
        run.append(f'iverilog -g2012 -o "{out}/{tb}.vvp" "{out}/{tb}_diff.v" {full} {refs}{extra}\n')
        for a in args:
            run.append(f'echo "--- {tb} {a}"; vvp "{out}/{tb}.vvp" {a} | grep -E "DIFF|PASS|FAIL" | head -8\n')
    open(os.path.join(out, 'run.sh'), 'w').write(''.join(run))
    print(f'wrote {out}/run.sh')

if __name__ == '__main__':
    main()
