# Continuous Integration

> Status (2026-11-21): first matrix run done 2026-11-20; its a2p25 and a2mega failures
> are root-caused and fixed in source where the design was at fault (see Toolchain variance).

GitHub Actions builds every active board on each PR and push to `main` that touches
`hdl/`, `boards/`, or `tools/build.sh`. Workflow: [`.github/workflows/fpga-build.yml`](../.github/workflows/fpga-build.yml).

## How it works

- **Toolchain:** Gowin EDA **Education edition** V1.9.11.03 for Linux — no license or
  registration required, and it builds all parts currently used by A2FPGA boards
  (GW1NR-9, GW2AR-18, GW5A-25, GW5AT-60).
- **Download:** the old `cdn.gowinsemi.com.cn` direct links are dead and the official
  download page now requires a login. CI fetches from the mirror host that the AUR
  packages use:
  `https://eda.gowinsemi.cn/Gowin_V1.9.11.03_Education_Linux.tar.gz` (~680 MB),
  verified against a pinned sha256 (same checksum the AUR `gowin-eda-edu` 1.9.11.03
  package pins). The tarball is cached with `actions/cache`, so the download happens
  only on cache misses.
- **Build & gate:** each matrix job runs `tools/build.sh <board>` — the same script
  used locally. It uses the pipe method (inviolable rule #1), detects `ERROR` lines
  (gw_sh exits 0 even on failure), rejects stale bitstreams, and **fails unless
  timing is clean** (0 setup + 0 hold violations), satisfying inviolable rule #2.
  The per-module resource report runs informationally (`RESOURCE_FAIL` unset).
  CI sets `BUILD_TIMEOUT=4800`: the script's 30-minute thrash watchdog is
  calibrated to a fast local machine, and hosted runners are slower; the job-level
  100-minute timeout still caps a wedged run.
- **Every matrix entry pins its `.gprj` explicitly** — boards carry extra test and
  probe projects, and first-alphabetical selection must not decide what CI builds.
  a2mega builds the 60K-die project (`a2mega.gprj`) only; the 138B-die and
  probe/test projects are not in CI.
- **Matrix (repo-aware):** a `plan` job picks the board list from the repo the
  workflow runs in, so the same file works upstream and in forks during the sync:
  - upstream (`a2fpga/a2fpga_core`): a2n20v2 + all three a2n20v2-Enhanced
    variants (standard, dualrate, fb)
  - forks (e.g. `edanuff/a2fpga_core`): those plus a2mega and a2p25
  - not built anywhere: the deprecated boards (a2n20v2-GS, a2n9, a2n20v1)

  `fail-fast` is off, so one board's failure doesn't cancel the others.
- **Artifacts:** each job uploads the freshly built `.fs` bitstream and the
  `*_tr_content.html` timing report (30-day retention). CI never commits build
  artifacts back — the tracked files in `impl/pnr/` are updated manually/locally
  on purpose (see repo conventions).

## Toolchain variance

The committed `impl/pnr/` artifacts of a2n20v2, a2mega and a2p25 were built locally
with **Gowin V1.9.12.01**; the newest license-free Linux build is **V1.9.11.03
Education**, so CI runs an older synthesis/PnR than the builds of record. The
Education edition is also installed on the development Mac, so every CI result can be
reproduced locally with the exact CI toolchain (`GW_SH=` the Education bundle's
`gw_sh`, see [tools/README.md](../tools/README.md#macos-setup)). What the two versions
do differently, established 2026-11-20/21:

- **Education 1.9.11.03 does build every chip in use** — GW2AR-18, GW5A-25 and
  GW5AT-60 all reach bitstream generation. Device support is not the problem.
- **The Education synthesizer writes an encrypted netlist** (`impl/gwsynthesis/*.vg`
  is a `pragma protect` block; 1.9.12 writes plaintext). Synthesis-generated instance
  names therefore cannot be read, and they differ between versions anyway, so physical
  constraints must name instances the HDL names itself. a2p25's `INS_LOC` pins for the
  audio IIR's DSP cells used inferred-multiplier names (`q_h0_0_s0`, …) and failed with
  CT1135 on CI; the filter now instantiates its MULT12X12 cells by name (`m_q_*`,
  `m_r_*`, [docs/audio.md](audio.md#filtering)). With that, **a2p25 builds clean on
  1.9.11.03** (0 setup / 0 hold on the first roll, 2026-11-21) and still clean on 1.9.12.01.
- **1.9.11.03 infers RAM16 (SSRAM) shift registers and distributed RAM on the GW5AT-60
  where 1.9.12 keeps flip-flops**: the a2mega Education build has 119–123 SSRAM cells and
  ~4,600 fewer registers than the committed one (aux-channel FIFOs, the ESP32 scratch RAM,
  the framebuffer read FIFO, DOC register banks, debug pipelines). Harmless for memories
  and pipelines, but it also converted the pixel CDC FIFO's two-deep gray-pointer
  synchronisers, turning the second stage into an asynchronous SSRAM read and routing the
  gray→binary cone through a cross-die RAM address net: that one path family was CI's
  "a2mega 7 setup violations" (clk_pix −0.96 ns). The synchronisers now carry
  `syn_srlstyle = "registers"` + `syn_preserve` (`hdl/displayport/video/pixel_cdc_fifo.v`);
  a2mega then closes setup on Education (clk_pix +0.66 ns). The documented inference rule
  (SUG949 §5.1.3, depth > 3) is not what 1.9.11.03 applies — any multi-bit chain is at
  risk, so mark synchronisers explicitly. A project-wide fallback exists if more turn up:
  a `.gsc` file (`add_file -type gsc`) containing `GLOBAL syn_srlstyle=registers`.
  The GW5A-25 (a2p25) showed no SSRAM inference under Education.
- **a2mega still fails hold under Education by 2–14 ps on three endpoints inside Gowin's
  DDR3 PHY IP** (`u_ddr_phy_cmd_io/oserdes_d_rr_* → cmd_oserdes_gen[*].u_cmd_gen/D*`,
  clk1x). The same paths are at +31…+81 ps in the committed 1.9.12.01 build; 1.9.12.01's
  release note lists updated GW5A(T)-60 delay data, so this is the timing model, not user
  logic, and nothing in the design can move it. a2mega therefore stays `experimental`
  in the matrix until CI runs a 1.9.12 Education release.
- **a2n20v2, a2n20v2-Enhanced, a2n20v2-Enhanced-dualrate**: build and close timing
  cleanly on CI. These jobs **block** the PR.
- **a2n20v2-Enhanced-fb** (11 setup violations on the first roll, clean on the
  second): roll-sensitive rather than broken (its committed build is even from 1.9.11.03
  Education on macOS). `experimental`.

When Gowin publishes a license-free Linux 1.9.12, switch `GOWIN_URL`/`GOWIN_SHA256`
and re-evaluate the `experimental` flags — matching the local version should make
these boards candidates for blocking status. a2p25 is a candidate for blocking status
now; promote it once a few CI runs confirm the first clean roll was not luck (its clk100
margin was +0.15 ns).

## Version pinning

CI is pinned to exactly V1.9.11.03 Education via URL + sha256. Some boards are
sensitive to the Gowin version (see [boards.md](boards.md) and the main README), so
**do not bump the CI toolchain casually** — change the `GOWIN_URL`/`GOWIN_SHA256`
pair in the workflow deliberately and watch a full matrix run.

## Known caveats

- If the mirror URL dies too, download the tarball from gowinsemi.com (login
  required, Education section) and host it somewhere CI can reach — a **private**
  location (private release asset, S3/R2 + token), since redistributing the Gowin
  installer publicly is questionable under its EULA.
- The Ubuntu runner needs a handful of X/font shared libraries even for headless
  CLI runs; the workflow installs them via apt. Gowin also bundles an old
  libfreetype in `IDE/lib` that breaks the system libfontconfig (`undefined
  symbol: FT_Done_MM_Var`), so the build step runs gw_sh with the system
  `libfreetype.so.6` in `LD_PRELOAD`. If gw_sh fails with another symbol lookup
  or missing-library error after an Ubuntu image update, compare `ldd $GW_SH`
  output against the apt list in the workflow.
- Runners are 2–4 cores; the GW5AT-60 (a2mega) PnR is the slowest job in the matrix.
  The job timeout is 90 minutes.
