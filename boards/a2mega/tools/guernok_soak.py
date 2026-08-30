#!/usr/bin/env python3
"""
guernok_soak.py — automated DP soak station (08-29).

Watches two things at once and writes one merged, timestamped log:

  1. SCREEN TRUTH: frames from the HDMI capture device (Guermok USB3
     Video, UVC) via ffmpeg/avfoundation, classified every frame as
     COLORBARS / WHITE / BLACK / OTHER against the dp_test pattern
     (8 vertical bands: white, yellow, cyan, green, magenta, red, blue,
     black). Classification TRANSITIONS are logged with timestamps and
     an anomaly snapshot (PPM) is saved for every departure from
     COLORBARS — this is the automated blink detector.

  2. LINK TRUTH: the board's telnet telemetry polled every POLL_S
     seconds — D2 (C: lock state), D3 (K:/W:/T:), D6 (SE0:/SE1: symbol
     error counters). Any change in SE0/SE1 or T: is flagged loudly.

Usage:
  python3 guernok_soak.py --ip 192.168.232.127 [--device "Guermok"]
        [--out soak_YYYYmmdd] [--fps 10]
  Ctrl-C to stop; prints a summary (uptime, transitions, error deltas).

The frame check is resolution-independent (frames are scaled to 480x270
by ffmpeg). Band means are sampled from the middle 60% of each band to
tolerate scaling/overscan edges.
"""
import argparse, datetime, os, socket, subprocess, sys, threading, time

EXPECTED = [  # (r,g,b) per band, dp_test colorbars
    (255,255,255),(255,255,0),(0,255,255),(0,255,0),
    (255,0,255),(255,0,0),(0,0,255),(0,0,0),
]
W, H = 480, 270
TOL = 60          # per-channel tolerance on band means
POLL_S = 30       # telemetry poll period

stop_flag = False
log_lock = threading.Lock()

def now():
    return datetime.datetime.now().strftime("%H:%M:%S.%f")[:-3]

def log(f, line, echo=True):
    with log_lock:
        f.write(f"{now()} {line}\n"); f.flush()
    if echo:
        print(f"{now()} {line}", flush=True)

def classify(frame):
    """frame: W*H*3 RGB bytes -> 'COLORBARS'|'WHITE'|'BLACK'|'OTHER'."""
    band_w = W // 8
    means = []
    rows = range(H//4, 3*H//4, 8)           # middle half, subsampled
    for b in range(8):
        x0 = b*band_w + band_w//5           # middle 60% of the band
        x1 = (b+1)*band_w - band_w//5
        rs = gs = bs = n = 0
        for y in rows:
            base = (y*W)*3
            for x in range(x0, x1, 6):
                i = base + x*3
                rs += frame[i]; gs += frame[i+1]; bs += frame[i+2]; n += 1
        means.append((rs//n, gs//n, bs//n))
    if all(all(abs(m[c]-e[c]) <= TOL for c in range(3))
           for m, e in zip(means, EXPECTED)):
        return "COLORBARS", means
    flat = [sum(m)//3 for m in means]
    if all(v > 200 for v in flat): return "WHITE", means
    if all(v < 40 for v in flat):  return "BLACK", means
    return "OTHER", means

def save_ppm(frame, path):
    with open(path, "wb") as f:
        f.write(b"P6\n%d %d\n255\n" % (W, H)); f.write(frame)

def telnet_thread(ip, f):
    last = {}
    while not stop_flag:
        lines = {}
        try:
            s = socket.create_connection((ip, 23), timeout=8)
            s.settimeout(8)
            time.sleep(1.0); s.sendall(b"r\n"); time.sleep(2.0)
            buf = b""
            try:
                while True:
                    d = s.recv(4096)
                    if not d: break
                    buf += d
                    if len(buf) > 65536: break
            except socket.timeout:
                pass
            s.close()
            for ln in buf.decode("ascii", "replace").splitlines():
                ln = ln.strip()
                for tag in ("D2 ", "D3 ", "D6 "):
                    if ln.startswith(tag):
                        lines[tag.strip()] = ln
        except OSError as e:
            log(f, f"TEL  poll failed: {e}", echo=False)
        for tag in ("D2", "D3", "D6"):
            if tag in lines and lines[tag] != last.get(tag):
                loud = tag == "D6" or "T:" in lines[tag]
                log(f, f"TEL  {lines[tag]}" + ("   <-- CHANGED" if tag in last else ""),
                    echo=loud or tag not in last)
                last[tag] = lines[tag]
        for _ in range(POLL_S):
            if stop_flag: return
            time.sleep(1)

def main():
    global stop_flag
    ap = argparse.ArgumentParser()
    ap.add_argument("--ip", required=True)
    ap.add_argument("--device", default="Guermok")
    ap.add_argument("--out", default=None)
    ap.add_argument("--fps", type=int, default=10)
    a = ap.parse_args()
    out = a.out or ("soak_" + datetime.datetime.now().strftime("%Y%m%d_%H%M%S"))
    os.makedirs(out, exist_ok=True)
    logf = open(os.path.join(out, "soak.log"), "a")
    log(logf, f"SOAK start: device~'{a.device}' ip={a.ip} fps={a.fps} out={out}")

    cmd = ["ffmpeg", "-hide_banner", "-loglevel", "error",
           "-f", "avfoundation", "-framerate", str(a.fps),
           "-pixel_format", "uyvy422",
           "-i", a.device,
           "-vf", f"scale={W}:{H}", "-pix_fmt", "rgb24",
           "-f", "rawvideo", "-"]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE, bufsize=W*H*3*4)
    tt = threading.Thread(target=telnet_thread, args=(a.ip, logf), daemon=True)
    tt.start()

    fsz = W*H*3
    state, state_t0 = None, time.time()
    frames = transitions = anomalies = 0
    last_beat = time.time()
    try:
        while True:
            frame = proc.stdout.read(fsz)
            if len(frame) < fsz:
                err = proc.stderr.read(2000).decode("utf-8", "replace")
                log(logf, f"CAPTURE ended/failed: {err.strip()[:300]}")
                break
            frames += 1
            cls, means = classify(frame)
            if cls != state:
                dur = time.time() - state_t0
                if state is not None:
                    log(logf, f"SCRN {state} -> {cls} (prev held {dur:.2f}s)")
                    transitions += 1
                    if cls != "COLORBARS":
                        anomalies += 1
                        p = os.path.join(out, f"anom_{frames:08d}_{cls}.ppm")
                        save_ppm(frame, p)
                        log(logf, f"SCRN anomaly frame saved: {p}  bands={means}")
                else:
                    log(logf, f"SCRN initial state: {cls}")
                state, state_t0 = cls, time.time()
            if time.time() - last_beat >= 60:
                log(logf, f"BEAT frames={frames} state={state} "
                          f"transitions={transitions} anomalies={anomalies}", echo=False)
                last_beat = time.time()
    except KeyboardInterrupt:
        pass
    finally:
        stop_flag = True
        proc.kill()
        log(logf, f"SOAK end: frames={frames} transitions={transitions} "
                  f"anomalies={anomalies} final={state}")
        logf.close()

if __name__ == "__main__":
    main()
