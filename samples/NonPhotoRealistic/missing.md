# NonPhotoRealistic — SAMPLE-033 audit record

Upstream: `NonPhotoRealisticSample_4_0`, ported against the unchanged XNA 4.0 sources snapshotted
at `/rv/tmp/samples/SAMPLE-033-NonPhotoRealisticSample_4_0/xna4-original`, per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## Current requalification — 2026-09-20

The physical upstream still matches that retained snapshot exactly. Its unchanged Windows
executable and Windows/Reach **Release content** were rebuilt with the official XNA 4.0 tools,
alongside the Xbox/HiDef content. The executable uses the original Windows project's Debug
defines; Release applies to the effect-processing configuration. All **six** checked-in Windows
XNBs are again byte-identical to the fresh
official outputs, including both compiled effects and the model's auto-imported texture. The
original was run and all six presets captured. Debug remains unsuitable for the original
`PostprocessEffect.Fx` because the unoptimised `ps_2_0` program needs 65 of 64 slots.

Fresh Release `OPENGLES3` and non-threaded `WEBGL2` builds use the active `libcna/cna` and
`libcna/sharp-runtime` checkouts, compiled effects and at most four compiler jobs. The old
`FX-120` retained-buffer fix is present in current CNA. No new framework/runtime fix or source
change was needed. The shipped sample still loads the six official XNBs through `Content.Load`,
with no gameplay workaround, stub, shader rewrite or intentional behavioral deviation. The only
port additions are byte-exact original non-Content documentation artwork: the three PNG figures
referenced by `NonPhotoRealistic.htm`, plus `NonPhotoRealistic/NonPhotoRealistic.png` and
`NonPhotoRealistic/Game.ico`.

The real XNA game and the fresh native port were both captured through all six presets. For a
whole-frame comparison, separate diagnostic copies under `xna4-diag/` and `diag-project/` pin
the spinning model's rotation **and** the sketch's random jitter; the two hooks are absent from
the shipped port. The exact diagnostic-source diff and all seven 800×480 comparisons, with no
pixels excluded, are in `evidence/requal-20260920/`:

| Preset | Pixels within 8/255 | Pixels over 64/255 |
|---|---:|---:|
| Cartoon | 383932 / 384000 | 28 |
| Pencil | 383932 / 384000 | 16 |
| Chunky Monochrome | 383870 / 384000 | 8 |
| Colored Hatching | 383935 / 384000 | 21 |
| Subtle Edge Enhancement | 383932 / 384000 | 29 |
| Nothing Special | 383968 / 384000 | 17 |
| Return to Cartoon | 383932 / 384000 | 28 |

Both engines return to a pixel-identical first frame after the full preset cycle. Agreement is
99.966–99.992% within eight levels across the seven frames, materially better than the historical
measurement in §4; the changed framework/toolchain and capture conditions are not isolated here
to a single causal fix. The ordinary fresh and stripped retained native executables both draw all
six presets and exit cleanly on Escape.

The old Chrome test's whole-frame mean-brightness check for returning to Cartoon failed by 3.12
levels with a three-level threshold, solely because the ship continues to rotate during the
cycle. The browser gate now hashes the fixed HUD region containing the preset name: all six
labels differ, and the final Cartoon HUD is byte-identical to the first. Fresh work, retained and
byte-identical local gallery WEBGL2 bundles pass that gate, scene/outline/sketch assertions,
800×480 WebGL2, title and renderer checks, and have no runtime, promise, relevant HTTP, fatal
console or WebGL-driver errors. The only 404 is Chrome's optional `favicon.ico`; WebGL performance
warnings concern screenshot readback. The gallery has 32 cards on 12/12/8 pages.
Samples commit `6f7f727` and gallery commit `9f9d48e` were pushed; GitHub Pages
deployment `35506659448` succeeded. The public detail, page-3 card, both images
and all four WEBGL2 files return HTTP 200, with all four public game files
SHA-256-identical to the retained bundle. The owner-authorized prune removed
only the three dated work trees and `xna4-build/obj/` and
`xna4-build/pipeline-runner/`, freeing 367.8 MB including stripping and
deduplication. Original source, scripts, evidence, official XNBs, original
executable, stripped native executable and full web bundle remain; their
product hashes are unchanged. `MANIFEST.md` in the artifact root records the
exact removed paths and active-checkout restoration commands with at most four
compile jobs.

## Original port and audit record

## 1. What was ported

The whole sample, all 539 lines of C#, as `.hpp`/`.cpp` pairs mirroring the original's own layout.
There was no port before this one.

| Original | Port |
|---|---|
| `NonPhotoRealisticSettings.cs` | `src/NonPhotoRealisticSettings.{hpp,cpp}` |
| `Game.cs` | `src/NonPhotoRealisticGame.{hpp,cpp}` + `src/Program.cpp` |
| `Properties/AssemblyInfo.cs` | `src/Properties/AssemblyInfo.cpp` |

All six presets are ported, and with them every technique the two effects declare: `Toon` and
`Lambert` and `NormalDepth` from `CartoonEffect.Fx`, and `EdgeDetect`, `EdgeDetectMonoSketch`,
`EdgeDetectColorSketch`, `MonoSketch` and `ColorSketch` from `PostprocessEffect.Fx`.

## 2. Content — and one property of the original

Every asset goes through a stock XNA importer and processor; this sample ships no pipeline
extension. `ShipDiffuse.tga` is not listed in the content project — the `ModelProcessor` pulls it
in itself as `Ship.fbx`'s material texture.

**`PostprocessEffect.Fx` does not compile in the Debug configuration**, by exactly one
instruction: *"Compiled shader code uses too many arithmetic instruction slots (65). Max. allowed
by the target (ps_2_0) is 64."* `EffectProcessor`'s `DebugMode` defaults to `Auto`, which skips
optimization for a Debug build. Release compiles it, and Release is therefore the audited
configuration. This is the second sample in a row with that property (SAMPLE-032's
`Distorters.fx` needed 73), so it is a property of the toolchain rather than of either sample.

All **6** XNBs are byte-identical to this sample's own official pipeline output for the Windows
(Reach) target (`cmp`), hashes in `evidence/content-sha256.txt`. The Xbox 360 (HiDef) target
builds too — and it builds `PostprocessEffect.Fx` even in Debug, because HiDef is not held to
`ps_2_0`.

## 3. One framework defect, found by this sample

Natively everything worked from the first run. On `WEBGL2` **five of the six presets rendered a
black frame** — every one that runs the postprocess. The sixth, `Nothing Special`, is the only
preset whose `Draw` takes neither branch: with both flags off it draws straight to the backbuffer
with no render target and no postprocess. It rendered correctly, and that contrast is what made
the shape of the defect visible.

What it was: `FlushBatchWithCompiledEffect` created a vertex buffer and an index buffer, drew with
them, and destroyed them, **every flush**. The compiled route records its geometry in one
long-lived vertex array object, and an element buffer binding is part of a VAO's own state — so
every flush left that array object holding a deleted name, and the next flush's draw read it.
Desktop GL tolerates that and draws anyway; WebGL 2 validates the binding and refuses the whole
draw with `GL_INVALID_OPERATION: glDrawElements: Insufficient buffer size`. The first flush of a
batch drew; every later one produced nothing.

Fixed by retaining the two buffers and re-uploading them, which also removes two buffer creations
and two deletions from every compiled-effect sprite flush. `plans/plan_fx.md` `FX-120`.

Two things had kept it invisible, and both are fixed as well:

- **The browser gate could not see WebGL errors.** Chrome delivers driver errors through the
  `Log` domain, not `Runtime.consoleAPICalled`. The gate now subscribes to both, and that is what
  produced the message above.
- **The draw route never asks for a GL error**, so the platform rendered black in silence. Worth
  knowing when reading any future "renders nothing" report against it.

## 4. Verification

Both builds are EasyGL: native `OPENGLES3` and web `WEBGL2` under Emscripten. Both windows are
titled `Non-Photorealistic`, from the ported `AssemblyInfo.cs` — hyphenated, and not the assembly
name.

The ship spins with wall time and two presets re-offset their sketch pattern from `Random`, so
diagnostic copies of both builds carry a `CNA_FREEZE` hook (`xna4-diag/` and `cna-diag/`, each with the diff against
the shipped source) that pins the rotation angle **and** the jitter offset. Both halves are
needed: with only the rotation frozen, `Pencil` compared at 0.2 % of pixels within 8 levels.

Whole-window, 384000 px, nothing excluded (`evidence/comparison.txt`):

| Preset | within 8 levels | >64 |
|---|---|---|
| Cartoon | 97.5 % | 1640 px |
| Pencil | **98.8 %** | 919 px |
| Chunky Monochrome | 98.2 % | 1006 px |
| Colored Hatching | 97.3 % | 1412 px |
| Subtle Edge Enhancement | 97.6 % | 1528 px |
| Nothing Special | 97.8 % | 887 px |
| back to Cartoon | 97.5 % | identical to Cartoon |

The differences are small clusters — 613 of them on Cartoon, largest 133 px, only 4 of 30 px or
more — lying along the ship's silhouette and on the boundaries between shading bands. Toon shading
quantises lighting into bands, so a sub-pixel difference in an interpolated normal flips a pixel a
whole band and shows up as a large delta. The pencil preset, which has no hard steps, has no
cluster bigger than 19 px. Zoomed crops of the two are indistinguishable.

The browser gate (`scripts/capture-web.sh`, `scripts/chrome-smoke.mjs`) walks all six presets and
asserts what each one is supposed to be, not merely that something rendered:

| Assertion | CNA on WEBGL2 |
|---|---|
| the ship is drawn against the CornflowerBlue clear | 88 % clear, mean 136.0 |
| toon shading plus edge detect paints far more near-black than plain Lambert | passes |
| both sketch presets replace the clear entirely (paper, not sky) | 0.000 clear, mean 210.4 / 176.6 |
| all six presets produce six different frames | passes |
| cycling all the way round returns to the first | mean back to 138.3 |

together with an 800×480 WebGL2 canvas, `CNA: graphics renderer: WEBGL2`,
`document.title == "Non-Photorealistic"`, and no rejections, runtime exceptions, HTTP errors,
fatal console messages **or WebGL errors**. Full record in
`evidence/cna-web-webgl2/browser-result.json`.

## 5. C++ mapping notes — not deviations

- `ChangeEffectUsedByModel` is `static` in C# and keeps its replacement effects alive through a
  local `Dictionary` that the model's mesh parts then reference. CNA's `ModelMeshPart` holds a
  **non-owning** `Effect*` and `Effect::Clone()` hands back an **owning** raw pointer, so the port
  makes it a member function and parks the clones in a `std::vector<std::unique_ptr<Effect>>` that
  outlives the model.
- `Settings` is a get-only property over `PresetSettings[settingsIndex]`, so it maps to
  `getSettingsProperty()` returning a reference rather than to a stored field.
- XNA's `Model`, `SpriteFont`, `Texture2D` and `Effect` are reference types whose fields start
  null; CNA loads the first three by value, so those are `std::optional`, and the effect is a
  `std::shared_ptr` as its content reader targets.

## 6. Deviations

None.
