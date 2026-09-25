# PickingSample — port notes

Upstream: `PickingSample_4_0` (SAMPLE-047). **Re-ported from scratch.** Ported whole — the table and
four models, the turning camera, the cursor, the bounding-sphere wireframes, the ray/sphere picking
and the projected name labels.

Artifact root: `/rv/tmp/samples/SAMPLE-047-PickingSample_4_0/`.

## Requalification on 2026-09-25 — no known differences

The unchanged physical upstream `PickingSample_4_0` is a Windows/HiDef game with a
Phone/Reach project. Its 28 files match `xna4-original/` path for path and SHA-256.
`scripts/build-original.sh` rebuilt and ran the Windows game under the XNA 4.0
Wine prefix with `WINEDLLOVERRIDES=d3d9=b`. The stock pipeline made ten XNBs
from seven content rows; **all ten** match both checked-in `Content/` and the
fresh OPENGLES3 product byte for byte. The port now also retains the upstream
license, icon, thumbnail and Phone background beside its existing `Picking.htm`.
The Phone 30 fps/fullscreen constructor branch and the Phone touch/Xbox gamepad
cursor selection are preserved; both inactive variants passed C++ syntax
compilation. `GeometricPrimitive.cs` remains unported because neither upstream
project compiles it.

The previous FX-126 conclusion below is **superseded**. CNA commit `a63d0a739`
published SpriteBatch's effective sampler into public
`GraphicsDevice.SamplerStates[0]`. The original's `LinearClamp` therefore
continues into the subsequent table draw; CNA no longer restores `LinearWrap`
for the table's out-of-range UVs. The earlier proposed normal-sign cause was
incorrect. With the original and CNA diagnostic sources in isolated builds,
the camera and cursor pinned to identical states, the fresh comparison is:

| Camera and cursor | Within 8 | Within 32 | Mean absolute difference / 255 | After 4px blur, within 8 |
|---|---:|---:|---:|---:|
| 185°, over models | 99.98% | 99.98% | 0.060 | 100.00% |
| 185°, away | 99.98% | 99.98% | 0.060 | 100.00% |
| 240°, over models | 99.98% | 99.99% | 0.072 | 100.00% |
| 300°, over models | 99.99% | 99.99% | 0.054 | 100.00% |

At 240° the model coverage is 47.821% in XNA and 47.823% in CNA and both
centroids round to `(346.82, 332.68)`. At 185°, the picked-name white pixels
in the HUD region are **176 in both engines** with the cursor over the models,
and **zero in both** with it away. These are matched diagnostic frames; the
unchanged original and normal port were also freshly run. The real native
pointer produced the expected names and a WM_DELETE_WINDOW request closed the
game with exit code zero. Synthetic Escape key events under this private Xvfb
did not close either the XNA or CNA process, so the clean-exit gate uses the
window-manager protocol instead of attributing that input-injection result to
the port. Original XNA captures with physical mouse injection were likewise
unreliable after Wine moved its window; the pinned diagnostic proves the
original's cursor-to-name path, and real pointer input is separately verified
in CNA native and Chrome.

The fresh WEBGL2 bundle ran over HTTP in system Chrome with a textured wood
table, a lit vertex-coloured sphere, moving camera and pointer-triggered names.
Its gate reported no runtime exception, HTTP error or fatal console message.
The **byte-identical four-file gallery copy** passed that same Chrome gate.
The gallery now contains the 46th card, detail page, full screenshot and
thumbnail; both pages were visually checked in Chrome. The `.wasm` has no
DWARF `debug_info` marker and the `.js` has no pthread/shared-memory marker.
No new CNA or sharp-runtime source change was required; FX-125 and FX-126 are
already in CNA `next` at `c74569ae5`, with sharp-runtime `41b918c9`.

Reproduction: `scripts/build-original.sh`, `scripts/build-current.sh native|web`,
`scripts/build-original-diag.sh`, `scripts/compare-frozen.sh` (isolated source
shadow), `scripts/capture-original.sh`, `scripts/capture-cna-native.sh` and
`scripts/capture-web.sh`. Current logs, frames, comparisons, Chrome results,
hashes and branch syntax checks are under `evidence/requal-20260925/`; its
`provenance.json` names the exact products and pre-commit repository heads.
The artifact `MANIFEST.md` explains how to recreate the reusable trees.

## Historical 2026-08 comparison (superseded by the requalification above)

## What the 2026-07-09 pass recorded, and what survives

That pass was header-only over five hand-converted `.model.json` models and recorded four findings.
**None of them survives.**

| The old note said | Measured now |
|---|---|
| CNA's `.model.json` schema has no texture field, so no mesh can be textured — "flat, fully-saturated white shapes", filed as an addendum to DEFERRED item #6 | The official `ModelProcessor` binds the textures itself. `wood.tga`, `cat.tga` and `wedge_p2_diff_v1.tga` are **not rows in the content project at all** — the `FbxImporter` resolves them from each FBX's own material — and the build emits **10 `.xnb` from 7 listed assets**. The table is wood, the box is a cat. |
| The flat-white look is "a direct consequence of the missing per-mesh texture support" | Correct as a diagnosis of the *old port*, and it evaporates with the real pipeline. |
| `Content.Load<Model>("Table")` needs the asset renamed to `Table.*` because Linux is case-sensitive | **No.** `ContentManager::ResolveExistingAssetPath` already matches a path component case-insensitively, and refuses a match that is ambiguous. `Load<Model>("Table")` finds `table.xnb` unchanged. |
| A near-plane-clipping bug renders a model as a thin line at one camera angle | Not reproduced at any of the three camera angles compared here. |

`GeometricPrimitive.cs` sits beside the other sources but is listed by **neither** `.csproj`, so it
is dead code and is not ported — the same shape as SAMPLE-046's `Buttons/Button.cs`. Unlike that
sample, this one's `Program.cs` compiles, so no entry point is generated.

## Content and profile

Seven listed assets plus the three FBX-implicit textures = 10 `.xnb`. The Windows project declares
`<XnaProfile>HiDef</XnaProfile>` (the phone project declares Reach), so the executable's embedded
`RuntimeProfile` resource says HiDef — SAMPLE-039 established that a Reach resource makes the game
die in `LoadContent`.

## The framework defect this sample found — FX-125

`Sphere01` carries a **colour channel**: its vertex stride is 36 (Position + Normal + Color +
TextureCoordinate) and the `ModelProcessor` sets `BasicEffect.VertexColorEnabled` accordingly. Both
engines agree on that — the material dump is byte-identical on both sides, all 11 meshes, every
channel — so the XNB reader was never in question.

EasyGL's `SelectStockProgramShape` dispatched on stride alone, with cases for 20, 24 and 32. Stride
36 matched none of them and fell through to the **unlit** `prog_colored_`, so the sphere rendered as
a flat green disc with no shading and no specular at all.

**The isolation is the evidence:**

| leg | sphere agreement | signed CNA−XNA |
|---|---|---|
| vertex colour switched **off** in both engines | **99.76 %** | +0.04 / −0.05 / −0.05 |
| specular only (ambient and diffuse zeroed) | 99.88 % | +0.08 / −0.02 / −0.02 |
| **as the sample ships** | **46.94 %** | **−41.50 / −74.93 / −21.81** |

Two things had to be right and only measuring separated them. The stride case is one. The other is
that **an attribute's location is the element's index in that program's own input table** —
`ConfigureDeclarationForStockProgramEXT` binds `inputs[i]` to location `i` — so declaring `aColor` at
5, copying the skinned program's slot, left it at its generic default while every summary number
stayed *byte-identical*. That is worth remembering: a change that moves a number not at all is a
signal, not a null result.

After the fix the sphere agrees on **99.76 %** of pixels and the whole frame on **97.35 %** within 8
levels, **99.98 % within 32**. Fixed in CNA as `plans/plan_fx.md` FX-125, pinned by
`easygl_basiceffect_lit_vertex_color_test`.

## Historical agreement with real XNA 4.0, before FX-126

The camera turns continuously and the cursor decides which names are drawn, so neither is
comparable without hooks. `../../../cna-diag/` and `../../../xna4-diag/` add `CNA_ROTATION` and
`CNA_CURSOR` to both engines; `scripts/compare-frozen.sh` drives them.

| camera | within 0 | within 8 | within 32 |
|---|---|---|---|
| **185° — the angle the sample opens at** | 77.56 % | **97.35 %** | **99.98 %** |
| 240° | 75.93 % | 92.41 % | 93.11 % |
| 300° | 77.26 % | 92.20 % | 93.15 % |

At 185° the residue is ordinary boundary noise: 2.65 % of pixels differ, median 15 levels, and they
are *less* edge-prone than the frame average — text antialiasing and the wireframe circles.

**At 240° and 300° the old builds had a divergence, since fixed by FX-126.**
Those angles dip the camera below the tabletop and reveal its underside.
XNA shades that surface essentially black — (4,2,1) — and CNA shades it dark brown — (68,21,4).
Everything else about it agrees:

- coverage and centroid are identical to the digit (47.820 % / 47.821 %, centroid to two decimals),
  so it is the same surface, not different geometry;
- with **ambient light only** the two frames agree on **99.99 %** of pixels, so the geometry, the
  texture, the UVs and the ambient term are all exact;
- with **one directional light** CNA still lights it and XNA still does not, so it is not a
  particular light;
- the near plane (the original's `.01f` against a far plane of 1000) changes nothing;
- forcing `CullNone` or `CullCounterClockwise` in both engines changes nothing;
- the world-matrix determinants are identical and positive in both engines, so no mirroring flips
  a normal;
- CNA's `uNormalMatrix` is the proper inverse-transpose, computed by cofactors.

The old normal-sign hypothesis was disproved by the later FX-126 isolation.
The actual cause was the sampler-state publication described above.

## Web

`WEBGL2` built under a real Emscripten toolchain and driven in real Google Chrome:

- the table is drawn **and textured** (brown ≥ 20 % of the frame — the wood texture is the one that
  is not a content-project row, so this pins the implicit-texture path);
- **`sphereGreenStd` is calibrated against the native frames** and is the FX-125 pin: the standard
  deviation of the sphere's green channel is 22.00 when the mesh is drawn unlit, 58.56 once it is
  lit, and 58.55 in real XNA. The gate demands > 45, so it fails for exactly the defect that was
  fixed. Chrome measured 56.24;
- two captures with no input differ, because the camera turns by itself;
- moving the pointer onto the row of models makes a name appear — white pixels 85 → 398 — which is
  the picking path (`Viewport.Unproject` → `Ray` → `BoundingSphere.Intersects`) working end to end;
- no runtime exceptions, no HTTP errors, no fatal console messages, `WEBGL2` renderer logged.

## Evidence

- `evidence/build-original.log` — the official pipeline, including the three implicit FBX textures.
- `evidence/frozen/<leg>/{xna,cna}/` — one directory per hooked leg, including every isolation leg
  named above.
- `evidence/cna-web-webgl2/browser-result.json` — the browser gate's own numbers.
