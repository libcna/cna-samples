# NonPhotoRealistic — SAMPLE-033 audit record

Upstream: `NonPhotoRealisticSample_4_0`, ported against the unchanged XNA 4.0 sources snapshotted
at `/rv/tmp/samples/SAMPLE-033-NonPhotoRealisticSample_4_0/xna4-original`, per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

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
both builds carry a `CNA_FREEZE` hook (`xna4-diag/` and `cna-diag/`, each with the diff against
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
