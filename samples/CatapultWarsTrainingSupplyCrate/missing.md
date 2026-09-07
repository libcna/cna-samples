# SAMPLE-068 — Catapult Wars Training: Supply Crate

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

Evidence: `/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/evidence/CatapultWarsTrainingSupplyCrate/`
-- `firefox-run1..2/`, `chrome-run1..2/` and `cna-native-opengles3-release-qualified/`.

## Result

This is the complete fifth exercise from `CatapultWarsTrainingKit_4_0`, kept separate because its
upstream solution owns an entry point and content project. It preserves the expanded projectile
state model, animated supply crates, crate collision/destruction, split-ammunition award, weapon
HUD, `SplitProjectile` creation and the two-projectile split behavior.

The entire 469-file upstream kit is retained under
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. The unchanged C# Phone/Reach
source compiles against XNA 4.0; `CatapultGame.dll` has SHA-256
`edc28eb3a22e9420da051b93c3e9159d94f538f752d0377b02d4b0370450b450`. The labelled haptics shim is
only a diagnostic substitute for the missing Phone assembly. A Phone 7 host is unavailable, so a
visual original run is not claimed.

## Content

The unchanged content project was built by the official XNA 4.0 pipeline. The checked-in 39 files
(37 XNBs and two animation XML copy items) compare byte-for-byte with that retained output. They
include the original crate animation, normal/split ammo, weapon HUD and projectile textures. The
tutorial DOCX is retained byte-identical, SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`.

## Qualification

Native Debug and Release OPENGLES3 builds pass. The real OpenGL ES 3.2 Release run reaches
gameplay, hits and destroys the enemy supply crate, observes split ammunition in the HUD, fires a
later split projectile, pauses, returns and exits cleanly. `after-first-turn.png` and
`split-projectile.png` plus exit-0/no-error logs are under
`evidence/CatapultWarsTrainingSupplyCrate/cna-native-opengles3-release-qualified/` in the artifact
root.

The threaded WEBGL2 build also runs in real Chrome on WebGL 2. Browser touch exercises normal
gameplay and the menu/pause lifecycle, and the 600-frame post-interaction canary has no exceptions,
unhandled rejections or HTTP errors. The deterministic crate-hit/split-ammo branch is established
by the targeted native run rather than claimed from the random-wind browser capture. Browser
evidence is under `evidence/CatapultWarsTrainingSupplyCrate/cna-web-webgl2-qualified/`.

No CNA or sharp-runtime workaround was needed. Shared ownership in the C++ active-projectile list
preserves C# reference identity; it is a representation detail, not a behavior change. There are
no known active behavioral differences.
