# Continuous Integration

> Status (2026-11-20): written with the initial CI workflow; not yet validated by a
> real runner run — the first PR's matrix run is the smoke test.

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
