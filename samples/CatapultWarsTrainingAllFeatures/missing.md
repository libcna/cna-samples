# SAMPLE-068 — Catapult Wars Training: All Features

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

Evidence: `/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/evidence/CatapultWarsTrainingAllFeatures/`
-- `firefox-run1..2/`, `chrome-run1..2/` and `cna-native-opengles3-release-qualified/`.

## Result

This is the complete sixth exercise and combined endpoint of `CatapultWarsTrainingKit_4_0`. It is
an independent runnable product, matching its own upstream solution, `Program.cs` and content
project. The port combines the original one/two-human selection, health and damage zones,
pullback-derived angle, ballistic shot guide, supply crates, normal/split ammunition, weapon HUD
and split-projectile behavior without replacing the individual exercises.

The whole 469-file upstream directory is retained at
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. The unchanged C# Phone/Reach
project compiles with XNA 4.0; `CatapultGame.dll` has SHA-256
`a7498a49deb386972736b568f176f10aff17f7c696852dea865fd05a97cf0cbd`. Its diagnostic haptics shim
only supplies an unavailable Phone SDK assembly. There is no local Windows Phone 7 host, so no
visual execution of the original XAP is claimed.

## Content

The official XNA 4.0 pipeline built the unchanged original project. All 41 output files (39 XNBs
and two animation XML copy items) are checked in and recursively byte-identical to that retained
build. This includes the combined health, guide, crate and ammunition assets. The original tutorial
DOCX is byte-identical, SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`.

## Qualification

Native Debug and Release OPENGLES3 builds pass. The Release executable uses OpenGL ES 3.2 and
traverses menu, instructions, the two-human option, both turns/shots, combined HUD/guide behavior,
pause, return and clean exit. It finishes with status 0 and no console errors; evidence is under
`evidence/CatapultWarsTrainingAllFeatures/cna-native-opengles3-release-qualified/` in the artifact
root.

The threaded WEBGL2 product runs in real Chrome with a genuine WebGL 2 context. Browser touch
selects the two-human path, exercises both turns and pause/return, and completes 600 later animation
frames. The probe records zero page exceptions, unhandled rejections and HTTP errors. Evidence is
under `evidence/CatapultWarsTrainingAllFeatures/cna-web-webgl2-qualified/`.

No framework/runtime fix or sample workaround was required. `diff.md` describes only lossless
C++/AOT representations. There are no known active behavioral differences.
