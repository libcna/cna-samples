# SAMPLE-068 — Catapult Wars Training: Scrolling Screen

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

Evidence: `/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/evidence/CatapultWarsTrainingScrollingScreen/`
-- `firefox-run1..2/`, `chrome-run1..2/` and `cna-native-opengles3-release-qualified/`.

## Result

This is the complete independent advanced product from `CatapultWarsTrainingKit_4_0`. Its separate
upstream solution, entry point and content project are preserved as their own CNA sample. The port
retains the larger world, draw scale/offset, clamped camera, direct pan, flick/inertia, pinch zoom,
catapult hit testing in transformed coordinates and automatic projectile-follow camera.

The complete 469-file source kit is retained at
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. Its unchanged C# Phone/Reach
project compiles with XNA 4.0 and yields `CatapultGame.dll` SHA-256
`caeaf095204514e430ebcde234f16158a86bf18afc991cd44c8f32c1943e0d32`. A clearly labelled build shim
only supplies the unavailable Phone haptics assembly. No local Phone 7 application host exists, so
original visual execution is not claimed.

## Content

The official XNA 4.0 pipeline output is checked in exactly: 33 XNBs plus the unchanged catapult
animation XML, 34 files total. A recursive byte comparison with the retained unchanged-project
output is empty. The upstream tutorial DOCX remains byte-identical, SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`.

## Qualification

Native Debug and Release OPENGLES3 builds pass. The real OpenGL ES 3.2 Release run exercises manual
camera movement, a transformed catapult drag/fire, projectile-follow across the world, return to
the next turn, pause/menu and clean exit. The result is exit 0 with no console error; captures
including `camera-pan.png`, `camera-following-projectile.png` and `after-first-turn.png` are under
`evidence/CatapultWarsTrainingScrollingScreen/cna-native-opengles3-release-qualified/` in the
artifact root.

The threaded WEBGL2 bundle runs in real Chrome on WebGL 2. Real browser touch exercises the
scrolling-camera path and pause/return lifecycle, then 600 animation frames complete without page
exceptions, unhandled rejections or HTTP errors. The specialized transformed catapult hit,
fire and follow sequence is proven by the targeted native run rather than inferred from the
browser camera-pan capture. Browser evidence is under
`evidence/CatapultWarsTrainingScrollingScreen/cna-web-webgl2-qualified/`.

No CNA/sharp-runtime fix or sample workaround was required. The C++ pointer back to the owning
gameplay screen represents the original C# object reference used by the catapult to control the
camera. There are no known active behavioral differences.
