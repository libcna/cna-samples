# SAMPLE-068 — Catapult Wars Training: Health Bar

## Re-gated on 2026-09-07 after the SAMPLE-067 findings

Nothing was wrong with this port. Its bundle was simply built on 2026-08-31, which means Emscripten
6.0.3 and the pre-fix EasyGL thread-context lease, and like every product in this kit it starts a
background loading thread. It therefore carried both defects SAMPLE-067 uncovered, and the browser
evidence recorded here was Chrome-only, so neither could have been seen:

- **Emscripten 6.0.3 aborts on the first proxied GL call a loading pthread makes** in any browser
  without `Atomics.waitAsync` -- Firefox before 145. The notification to the main thread's mailbox
  goes by `postMessage`, and 6.0.3's `worker.onmessage` asserts on a message addressed to the main
  thread instead of processing it (upstream #27018, fixed by #27336 in release 6.0.4).
- **The renderer's thread-context lease was a no-op on Emscripten**, so a content load and a frame
  issued GL concurrently into the one proxied browser-thread context and background-loaded textures
  came out empty, silently (fixed in `cnanext cde325ecd`).

The bundle was rebuilt on emsdk **6.0.9** and that fixed renderer, and re-gated in both browsers.

**Firefox** (new; `scripts/capture-web-firefox.sh` + `firefox-smoke.mjs`, headed on a private Xvfb
display and driven over WebDriver BiDi, because Firefox 140 ESR starts BiDi rather than CDP and its
CDP shim has no input command): 2 runs, both green -- menu, instructions, the background load,
gameplay, a real touch drag that fires, 600 further frames, `moduleAbort: null`, no page error, no
`Aborted(`, `crossOriginIsolated: true`, WebGL 2, canvas 800x480, and `atomicsWaitAsync:
"undefined"` recorded in each, i.e. exactly the configuration that used to abort.

**Chrome**: 2 further runs of the existing touch gate, both green, with no exception, rejection or
HTTP error.

**The empty-texture defect cannot be seen by any of those checks** -- every asset reports loaded, no
GL error is raised and the gate passes while textures are black -- so each gameplay frame is also
compared by pixel. `scripts/check-missing-textures.py` divides the frame into 50x48 patches, takes
the brightest mean each product achieved across its runs, and flags a run whose patch is near-black
where another run lit it. The instrument is calibrated on a known-defective frame from SAMPLE-067,
which it flags at 111 patches (266 400 px). Result for this product: **0 of 4 gameplay frames
defective** (2 Firefox, 2 Chrome).

**Native** Release OPENGLES3 was rebuilt on the fixed renderer and re-captured through the same
interactive flow, ending in a clean exit (`exit=0`).

Evidence: `/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/evidence/CatapultWarsTrainingHealthBar/`
-- `firefox-run1..2/`, `chrome-run1..2/` and `cna-native-opengles3-release-qualified/`.

## Result

This is the complete first exercise from `CatapultWarsTrainingKit_4_0`, retained as its own
runnable product because upstream gives it a separate solution, `Program.cs` and content project.
The port preserves the screen stack, human/AI turns, catapult/projectile state machines and the
exercise's three-zone damage calculation and animated health-bar display. No feature is borrowed
from a later exercise.

The entire 469-file upstream training kit is retained at
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. The unchanged C# Phone/Reach
project compiles with XNA 4.0; `CatapultGame.dll` has SHA-256
`30fd73cb0afe39eb80746d0455cc4c4e81f70f716b4b03e1227f7eb7963c6691`. A labelled diagnostic shim
supplies only the unavailable `Microsoft.Devices.Haptic` assembly. There is no local Windows Phone
7 application host, so no visual run of the original XAP is claimed.

## Content

The official XNA 4.0 pipeline built the unchanged original content project. All 34 resulting files
(33 XNBs plus `Textures/Catapults/AnimationsDef.xml`) are checked in, and `diff -qr` between this
`Content/` directory and the retained pipeline output is empty. The original tutorial DOCX is also
retained byte-identical (SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`).

## Qualification

Native Debug and Release builds pass with `CNA_GRAPHICS_RENDERER=OPENGLES3`. The Release run uses a
real OpenGL ES 3.2 context and completes
`menu -> instructions -> gameplay -> drag/fire -> damage/health update -> pause -> menu -> exit`
with exit code 0 and no console error. Captures and the run log are under
`evidence/CatapultWarsTrainingHealthBar/cna-native-opengles3-release-qualified/` in the artifact
root above.

The threaded WEBGL2 bundle runs in real Chrome on an actual WebGL 2 context. Browser touch reaches
gameplay, fires a shot, pauses and returns to the menu; a 600-requestAnimationFrame canary then
reports zero exceptions, unhandled promise rejections and HTTP errors. Evidence is under
`evidence/CatapultWarsTrainingHealthBar/cna-web-webgl2-qualified/`.

No CNA or sharp-runtime workaround is present or was required. `diff.md` records only necessary
C++/AOT representation changes. There are no known active behavioral differences.
