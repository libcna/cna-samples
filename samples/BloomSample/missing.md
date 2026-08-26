# BloomSample — SAMPLE-031 audit record

Upstream: `BloomSample_4_0` (`BloomPostprocess`), ported against the unchanged XNA 4.0 sources
snapshotted at `/rv/tmp/samples/SAMPLE-031-BloomSample_4_0/xna4-original`, per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## 1. What was ported

The whole sample, all 671 lines of C# across four files, as `.hpp`/`.cpp` pairs mirroring the
original's own layout. There was no port before this one: the sample was a placeholder holding a
help image and a `missing.md`.

| Original | Port |
|---|---|
| `BloomSettings.cs` | `src/BloomSettings.{hpp,cpp}` |
| `BloomComponent.cs` | `src/BloomComponent.{hpp,cpp}` |
| `Game.cs` | `src/BloomPostprocessGame.{hpp,cpp}` + `src/Program.cpp` |
| `Properties/AssemblyInfo.cs` | `src/Properties/AssemblyInfo.cpp` |

## 2. What the previous record claimed, and what is actually true

| Old claim | Finding |
|---|---|
| "HLSL `.fx` shaders must be rewritten as GLSL `.shader.json` — DEFERRED.md item #11" | **Stale.** CNA loads compiled XNA Effect bytecode through MojoShader on EasyGL. All three `.fx` files are the sample's own, compiled by the official pipeline; nothing was rewritten. |
| "`tank.fbx` must be converted to `.model.json` — DEFERRED.md item #6" | **Stale.** `FbxImporter`/`ModelProcessor` builds it and the port loads the resulting `Model` XNB, with its two textures. |
| "`DrawableGameComponent` / `Game.Components` — not yet in CNA" | **False.** Both exist. `BloomComponent` is a real `DrawableGameComponent` in `Game.Components`, with `Visible` driving both its `Draw` and its own `BeginDraw`. |
| "`RenderTarget2D` is supported in CNA; no blocker there" | True, and it stayed true. |

## 3. Three framework defects, all found by this sample

Every one was measured against a live XNA 4.0 build of the unchanged original running under
Wine, not inferred. All three are fixed in `cnanext`; nothing is worked around here.

### 3.1 A rendered source was corrected twice and came out upside down

The first CNA run drew the entire scene **vertically mirrored** — sky at the bottom, tank
inverted — while the text overlay, which is drawn straight to the backbuffer after the
postprocess, was the right way up.

EasyGL corrects a render target's bottom-up row order in two different places. The stock sprite
path mirrors V in the sprite's own vertex data; the compiled-Effect path binds a row-reversed
**copy** of the source, per sampler slot, because MojoShader's generated GLSL carries none of
this renderer's own sampling-time correction. A `SpriteBatch` draw of a rendered source through
a compiled `.fx` went through both, and two corrections are no correction.

It had never shown up because the contract that pins the compiled route's row order drives it
through `DrawUserPrimitives`, where the game supplies its own vertices. This sample is the other
shape: four fullscreen `SpriteBatch` quads, each a compiled `.fx` reading the render target the
previous quad wrote.

Fixed by suppressing the sprite's V mirror for a batch that flushes through the compiled route,
leaving the per-slot copy as the single correction — which is also the only one of the two that
can correct a slot the sprite quad does not own, such as the base image `BloomCombine.fx` reads
from `GraphicsDevice.Textures[1]`. Pinned by a new shared contract
(`RunCompiledEffectSpriteBatchRenderTargetSourceContract`: one hop, two hops, and a plain
`Texture2D` that must stay untouched), verified to fail without the fix, wired into all four
backends that run the compiled-effect suite. `plans/plan_fx.md` `FX-118`.

### 3.2 The first glyph of a line took the wrong left side bearing

With the flip fixed, 1542 pixels still differed by more than 64 levels, and 97.9 % of them were
inside the three-line overlay. The glyph bitmaps were byte-identical; lines 1 and 3 were one
pixel to the right of XNA's and line 2 was exact.

The discriminator is the first character: `A` and `X` have `kerning.X = -1` in this font, `B`
has `+1`. Decoding `hudFont.xnb` (`scripts/dump-spritefont-xnb.py`) gives the exact values, and
six of its 95 characters have a negative left side bearing.

FNA advances a line's first glyph by `Math.Abs(cKern.X)` — "always push the width rightward,
even if the kerning pushes the character to the left". **XNA does not.** Measured directly:
`MeasureString` on the live build returns 11 for `"A"` (`0 + 11 + 0`) and 10 for `"B"`
(`1 + 8 + 1`), so a negative bearing is *clamped away*, not reflected; FNA's rule would make
`"A"` 12. Rebuilding the same font with `<Spacing>` 3 and -2 returns the same three
single-character widths, so `Spacing` is not applied to a line's first glyph either. The rule is
`curOffset.X += Max(cKern.X, 0)`, with no `Spacing` term.

Fixed in both `SpriteBatch::DrawString` and `SpriteFont::MeasureString`. All 27 probe
measurements across the three fonts now match XNA exactly
(`evidence/measurestring-comparison.txt`), and the overlay text goes from 1542 differing pixels
to **zero**.

### 3.3 `DrawString` still quantised glyph destinations

A separate deviation of the same shape as one already fixed for `Draw`: `DrawString` rounded each
glyph's destination to whole pixels. Drawing the overlay at (64.5, 64.5) shows it plainly — XNA
filters each glyph **across** the half pixel, the row through its `A` reading
121, 162, 174, 174, 162, 121, where CNA produced a crisp 255, 255, 255, 255 snapped to the pixel
grid. The same is visible at a fractional scale.

Fixed by carrying the glyph destination through in floating point, as `Draw` already does, and
the now-unused rounding helper is gone. This one does **not** change this sample's own output —
every advance in it is integral — so it is pinned by its own tests rather than by a frame.

## 4. Content

All **8** XNBs are byte-identical to this sample's own official pipeline output for the Windows
(Reach) target (`cmp`), hashes in `evidence/content-sha256.txt`. Two of them —
`engine_diff_tex_0` and `turret_alt_diff_tex_0` — are textures the `ModelProcessor` pulls in
itself; they are not listed in the content project.

The sample also ships an Xbox 360 solution, which is HiDef rather than Reach. That content
builds here too, and **every one of the eight files differs** from its Windows counterpart —
the three effects are compiled for a different shader target and are roughly half the size, and
the font, texture and model files are byte-swapped for the console's endianness. Windows is the
audited configuration; the Xbox 360 build is kept only so the difference is a measurement rather
than an assumption. There is no Windows Phone project for this sample.

## 5. Verification

Both builds are EasyGL: native `OPENGLES3` and web `WEBGL2` under Emscripten. Both windows are
titled `Bloom Postprocess`, from the ported `AssemblyInfo.cs`.

The model spins with wall time, so neither engine is ever twice in the same place. Both builds
therefore carry a one-line `CNA_FREEZE` hook (`xna4-diag/` and `cna-diag/`, each with the diff
against the shipped source) that pins the rotation angle. Every figure below is a whole-frame
comparison at the same angle — 384000 px, nothing excluded (`evidence/comparison.txt`):

| Frame | within 8 levels | worst channel delta | clusters >40 levels |
|---|---|---|---|
| FinalResult | **100.0 %** (383876) | 212 | 53, largest **2 px** |
| PreBloom | 99.9 % (383798) | **47** | 2, largest 4 px |
| BlurredHorizontally | **100.0 %** (384000) | **6** | none |
| BlurredBothWays | **100.0 %** (384000) | **1** | none |
| bloom off | **100.0 %** (383872) | 252 | 61, largest **2 px** |

The two blur buffers have no pixel differing by more than 8 levels anywhere in the frame, and
the second one's worst single-channel difference over 384000 pixels is 1. In the composited
frames the largest connected disagreement is two pixels — scattered silhouette and specular
edges where a D3D9 and a GL rasterizer round a triangle edge differently — with nothing left in
the text.

The browser gate (`scripts/capture-web.sh`, `scripts/chrome-smoke.mjs`) asserts the postprocess
rather than merely that something rendered. Its thresholds come from the frozen XNA measurement,
and every cross-capture comparison uses the top eighth of the frame — sunset sky the tank never
reaches — because the browser run is not frozen and the whole-frame mean drifts with the
rotation:

| Assertion | XNA | CNA on WEBGL2 |
|---|---|---|
| scene is upright (top band vs bottom band) | 133.2 vs 16.3 | 131.7 vs 21.8 |
| bloom brightens the sky (on vs off) | 133.2 vs 97.5 | 131.7 vs 97.4 |
| PreBloom is the extract, not the scene | 46.0 | 46.1 |
| both blur passes keep the extract's level | 46.0 / 46.7 | 46.1 / 46.7 |
| `X` cycles back to FinalResult, `B` toggles back | — | exact to 0.0 |
| `A` selects "Soft", whose threshold is 0 | — | 156.4, up from 131.7 |

together with an 800×480 WebGL2 canvas, `CNA: graphics renderer: WEBGL2`,
`document.title == "Bloom Postprocess"`, and no rejections, runtime exceptions, HTTP errors or
fatal console messages. Full record in `evidence/cna-web-webgl2/browser-result.json`.

## 6. C++ mapping notes — not deviations

- `BloomSettings`'s fields are C# `readonly`, which in C++ would delete the copy assignment the
  `Settings` property needs. They are plain fields here; nothing in the sample writes one after
  construction, and the property still rebinds the whole object as the original does.
- `BloomComponent`'s constructor throws `ArgumentNullException` for a null game. A C++ reference
  cannot be null, so the check has nothing left to test.
- `bloom.ShowBuffer++` on an enum becomes an explicit cast through `int`, and
  `IntermediateBuffer.ToString()` a four-name table, because C++ has no reflection.
- `foreach (BasicEffect effect in mesh.Effects)` is a per-element cast in C#; the port uses
  `dynamic_cast` and, like the original, does not guard the result — a mesh carrying another
  effect type is a content error in both.

## 7. Deviations

None.
