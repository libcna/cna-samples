# SAMPLE-068 — Catapult Wars Training: Shot Angle

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

Evidence: `/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/evidence/CatapultWarsTrainingShotAngle/`
-- `firefox-run1..2/`, `chrome-run1..2/` and `cna-native-opengles3-release-qualified/`.

## Result

This is the complete third exercise from `CatapultWarsTrainingKit_4_0`, with its own target because
the original has an independent solution, entry point and content project. It retains the common
game plus the exercise's minimum/maximum shot velocity, pullback-derived launch angle, aim state
and angle-sensitive ballistic launch behavior.

The complete 469-file upstream snapshot is retained under
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. The unchanged C# Phone/Reach
project compiles with XNA 4.0; its `CatapultGame.dll` SHA-256 is
`2e8a3511003ecd3c4dc3ed2f9007551f9987b94390c414dfa1579b5adba7436a`. The diagnostic haptics shim
only supplies the missing Phone assembly. No local Phone 7 host exists, so original visual
execution is not claimed.

## Content

The official XNA 4.0 pipeline output is used verbatim: 33 XNBs and the original catapult animation
XML, 34 files total. `diff -qr` against the retained unchanged-project output is empty. The tutorial
DOCX is also byte-identical (SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`).

## Qualification

Debug and Release native OPENGLES3 builds pass. A Release OpenGL ES 3.2 run traverses menu and
instructions, performs the original pullback/angle shot, advances the turn, pauses, returns and
exits with status 0 and no console error. Evidence is under
`evidence/CatapultWarsTrainingShotAngle/cna-native-opengles3-release-qualified/` in the artifact
root.

The threaded WEBGL2 build runs in real Chrome on WebGL 2. Browser touch exercises the angle/shot
path and pause/return flow; 600 subsequent animation frames complete with zero page exceptions,
unhandled rejections and asset errors. Evidence is under
`evidence/CatapultWarsTrainingShotAngle/cna-web-webgl2-qualified/`.

No framework/runtime fix or sample workaround was needed. `diff.md` contains only lossless C++/AOT
adaptations, and there are no known active behavioral differences.
