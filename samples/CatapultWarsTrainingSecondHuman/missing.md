# SAMPLE-068 — Catapult Wars Training: Second Human Player

## Owner decision — cancelled, 2026-09-26

After confirming that SAMPLE-068 is a training kit of six staged exercises
and one advanced variant of Catapult Wars, the owner explicitly directed
that this row remain `⛔ cancelled`. The seven previously translated products,
exact content and original/native/browser evidence remain preserved as
history. Their 2026-09-07 qualification is not a current-head completion
claim. The requalification and gallery plan in the analysis below is
superseded by this decision; no further port, rebuild or publication work is
scheduled for SAMPLE-068. SAMPLE-067 remains the separately qualified
Catapult Wars endpoint.

## Current-head analysis — 2026-09-26

This product's old `✅` records the 2026-09-07 qualification, not a fresh
run on `cna/next 8c917a6d7` and `sharp-runtime/next d86adb65`. The unchanged
469-file upstream snapshot still matches the physical training kit; all
**34** checked-in content files match this product's retained official XNA
pipeline output byte for byte (251 files across all seven products). No sample
source has changed since 2026-08-31. A targeted scan found no backend calls,
`NOXNA` bypass or sample-side workaround. This original Phone game remains
touch-only: unlike owner-approved SAMPLE-067, it does not enable CNA's optional
mouse-to-touch emulation; the old native capture used an external SDL touch
shim. The prior original build is a compiled Phone DLL, not a visual run.

The retained native executable dates from September 9 and embeds a RUNPATH
into the retired `openeggbert/cnanext` checkout; the WEBGL2 bundle dates from
September 7 and its ~116 MB WASM still carries ~81 MB of name/DWARF debug
sections. Neither is a current-head qualification. The pruned artifact's
`MANIFEST.md` names the retired `openeggbert/cna-samples` source path, and
`capture-native.sh` defaults to a build-tree name no longer retained. The
website currently has no gallery entry for any of the seven products.

Before restoring `✅`, repair reproduction commands, rebuild all seven
products against the active repository chain, repeat native and real
Chrome/Firefox touch, branch, audio and texture checks, produce lean WEBGL2
bundles, and add gameplay captures and launchers to the gallery. Keep this
product `🔎` until that evidence exists. No code or artifact was changed in
this read-only audit.

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

Evidence: `/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/evidence/CatapultWarsTrainingSecondHuman/`
-- `firefox-run1..2/`, `chrome-run1..2/` and `cna-native-opengles3-release-qualified/`.

## Result

This is the complete second exercise from `CatapultWarsTrainingKit_4_0`, kept as a separate runnable
product to match its own upstream solution, entry point and content project. It preserves the
common Catapult Wars screen/gameplay implementation while replacing the AI side with the original
second `Human` player; turn ownership, touch input, prompts and both human firing paths remain
distinct.

The exact 469-file source kit is retained at
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. Its unchanged C# Phone/Reach
project compiles with XNA 4.0, producing `CatapultGame.dll` with SHA-256
`54c211d5a0a1f03836606afc7558e31fd36e87d2685aa8a7d839560bcab7fd54`. The build-only diagnostic
haptics shim supplies the unavailable Phone assembly; a local Phone 7 host is unavailable, so the
original is not claimed as visually executed.

## Content

The checked-in content is the official output of the unchanged XNA 4.0 content project: 33 XNBs
and `Textures/Catapults/AnimationsDef.xml` (34 files). Its recursive byte comparison with the
retained pipeline output is empty. The upstream tutorial DOCX is byte-identical, SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`.

## Qualification

Native Debug and Release OPENGLES3 builds pass. The Release binary runs on OpenGL ES 3.2 through
menu, instructions, both human turns and shots, pause, return and clean exit; the result is exit 0
with no console error. Evidence is under
`evidence/CatapultWarsTrainingSecondHuman/cna-native-opengles3-release-qualified/` in the artifact
root.

The threaded WEBGL2 bundle runs in real Chrome with WebGL 2. Real browser touch completes the same
two-human branch and both turns, then pause/return; the post-interaction 600-frame canary reports no
exceptions, rejections or HTTP errors. Evidence is under
`evidence/CatapultWarsTrainingSecondHuman/cna-web-webgl2-qualified/`.

No CNA or sharp-runtime change was required. There is no sample workaround and no known active
behavioral difference; necessary C++/AOT mechanics are listed in `diff.md`.
