# RimLighting — port notes

## Current requalification — 2026-09-20

`SAMPLE-037` was audited against the byte-identical physical upstream and the
active `libcna/cna` (`95b7e14a2`) and `libcna/sharp-runtime` (`cb8fd7f8`)
checkouts. The unchanged Windows Phone/Reach sources, including the inactive
desktop `Program.cs` branch, both cameras and all three UI classes, were
reviewed with the content project, project/phone manifests and non-Content
media. The existing generated-entry-point method builds the original XNA 4.0
game with `WINDOWS_PHONE` defined. The official pipeline rebuilt the four
declared assets for Windows/Reach, Windows/HiDef and Xbox/HiDef; its five new
Windows XNBs are byte-identical to `samples/RimLighting/Content/`:

| Content file | SHA-256 |
|---|---|
| `Font.xnb` | `ba753a7e86b9b3b8bf00ef6ae7188b7fc3f768e4e59ffa94d468a835a200b476` |
| `Head_Diff_0.xnb` | `acfe57906451ed8a4723ee37dd611648f3b0e244e0cc061477ef0037de6cc937` |
| `OutputCube.xnb` | `084e07283954c6a45ddea0d974ce0da623da51c461fd200aff679964d33f4ba1` |
| `blankTex.xnb` | `9bd241116f0053cb19a5973d88971e2611bd232e4ce0a8cdfad4e629dcf06a86` |
| `head.xnb` | `aaa2f083c2ff2867aa74cc4869a482a752518c93396bd321b9bc878f05c723ba` |

The sole stale effect call was corrected from `passes[i].Apply()` to
`passes[i]->Apply()` for the current pointer-returning collection. The UI
classes now preserve C#'s `sender == this` on both events by deriving from
`System::Object`; their logical type names are marked `CNAEXT`. The earlier
sample-local opt-in for mouse-as-touch was initially removed during this
requalification, then restored after the owner explicitly approved desktop
mouse emulation for SAMPLE-037. This is the existing one-line, off-by-default
CNA extension, not a sample input workaround; see [`diff.md`](diff.md). The original
`Background.png`, `GameThumbnail.png` and `Game.ico` were restored byte-for-byte
outside Content. There is no model, cube-map, font, shader, renderer or input
workaround and no CNA or sharp-runtime source change in this requalification.

Fresh Release OPENGLES3 and non-threaded Release WEBGL2 builds were configured
against those active checkouts with the shared ccache and at most four compiler
jobs. Original and native start frames are 480×800: 383,047 of all 384,000
pixels (99.752%) agree within eight channel levels, with zero excluded pixels
and 0.1122/255 channel MAE. All seven original pointer-attempt captures are
byte-identical because XNA's desktop host has no touch digitizer; the CNA
mouse-enabled start frame still has the same SHA-256 as before the opt-in.
Seven fresh native captures confirm that mouse drags move both sliders and
rotate the model/camera, while held clicks toggle the mode button in both
directions. The native game renders without a
fatal error and exits with status 0 on SDL's SIGINT quit event. `xdotool
windowclose` on bare Xvfb instead removes the X11 drawable without a window
manager and can provoke a `BadDrawable`; that destructive diagnostic is not
the game's Back-button input and is not used as a completion gate.

The final, byte-identical local-gallery WEBGL2 bundle passes a real system
Chrome gate with six `touchstart`, 16 `touchmove` and six `touchend` events.
Both sliders change the effect (warm rim pixels increase 12,130 → 14,466 when
Amount rises); the head rotates in world mode, the button switches to camera
mode and back, and a second drag rotates the camera. A separate held-left-mouse
slider drag changes the amount and a mouse click switches modes. The scene,
480×800 canvas, title,
WebGL2 context and all four HTTP assets pass, with no runtime exception,
rejected promise or asset HTTP error. The local gallery has 36 cards on
12/12/12 pages. Publication and artifact pruning were not requested.

The stable artifact root is
`/rv/tmp/samples/SAMPLE-037-RimLighting_4_0/`. Rebuild scripts and the exact
upstream snapshot are under `scripts/` and `xna4-original/`; fresh comparison,
exit and Chrome evidence is under `evidence/requal-20260920/`. The retained
original executable, native OPENGLES3 executable/content and complete WEBGL2
bundle are respectively under `xna4-build/bin/`, `cna-native-opengles3/` and
`cna-web-webgl2/`. Rebuildable 2026-09-20 work trees remain unpruned.

Reproduction commands (all paths are absolute so the current directory does
not affect source selection):

```bash
root=/rv/tmp/samples/SAMPLE-037-RimLighting_4_0
samples=/rv/data/development/github.com/libcna/cna-samples
cna=/rv/data/development/github.com/libcna/cna
sharp=/rv/data/development/github.com/libcna/sharp-runtime
export CCACHE_DIR=/home/robertvokac/.cache/ccache CCACHE_BASEDIR=/rv
"$root/scripts/build-original.sh"
cmake -S "$samples" -B "$root/work-native-opengles3-20260920" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCNA_GRAPHICS_RENDERER=OPENGLES3 \
  -DCNA_SAMPLES_CNA_ROOT="$cna" -DCNA_SHARP_RUNTIME_ROOT="$sharp" \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
cmake --build "$root/work-native-opengles3-20260920" \
  --target RimLighting_cna_samples --parallel 4
/home/robertvokac/emsdk/upstream/emscripten/emcmake cmake \
  -S "$samples" -B "$root/work-web-webgl2-20260920" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCNA_GRAPHICS_RENDERER=WEBGL2 \
  -DCNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF \
  -DCNA_SAMPLES_CNA_ROOT="$cna" -DCNA_SHARP_RUNTIME_ROOT="$sharp" \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
cmake --build "$root/work-web-webgl2-20260920" \
  --target RimLighting_cna_samples --parallel 4
CNA_RUN_DIR="$root/work-native-opengles3-20260920" \
  CNA_EVIDENCE_SUBDIR=requal-20260920/cna-native-mouse-approved-held-click \
  "$root/scripts/capture-cna-native.sh"
CNA_WEB_PRODUCT_DIR=/rv/data/development/github.com/libcna/samples.libcna.com/RimLighting \
  CNA_EVIDENCE_SUBDIR=requal-20260920/gallery-mouse-approved "$root/scripts/capture-web.sh"
```

## Historical port record

Upstream: `RimLighting_4_0` (SAMPLE-037). Ported whole — all seven source files, both cameras,
all three UI controls and the stock `EnvironmentMapEffect` draw path. Nothing is missing,
stubbed or simplified.

**This file replaces a 2026-07-10 version that documented two blockers as permanent.** Both were
stale: `Content.Load<TextureCube>` and `Content.Load<Model>` with `EnvironmentMapEffect`
materials both work in `cnanext` today, and the port now uses them. The hand-converted
`head.model.json` / `*_verts.bin` / `*_idx.bin` mesh, the six `envmap_*.png` cube faces, and the
invented `help.png` F1 overlay are all gone.

## The original is a Windows Phone project

`<XnaPlatform>Windows Phone</XnaPlatform>`, `<OutputType>Library</OutputType>`, and no usable
entry point: the `Program.cs` it ships is guarded by `#if WINDOWS || XBOX` and, inside that
guard, instantiates a class named `Game1` that does not exist anywhere in the sample — the game
class is `SampleGame`. On the platform the project targets, none of that is ever compiled.

`scripts/build-original.sh` therefore does what SAMPLE-021 PathDrawing established: defines
`WINDOWS_PHONE` rather than `WINDOWS`, so the sample's own guarded `Program.cs` stays out, and
generates the entry point the WP7 targets supply. Every one of the sample's own source files is
compiled unmodified.

## Content

Four assets, all stock importers, built for all three targets in Debug. One carries a processor
**parameter** that matters:

```xml
<Compile Include="head.fbx">
  <Processor>ModelProcessor</Processor>
  <ProcessorParameters_DefaultEffect>EnvironmentMapEffect</ProcessorParameters_DefaultEffect>
</Compile>
```

That is what makes the built model's materials `EnvironmentMapEffect` instead of the
ModelProcessor's `BasicEffect` default, and the game casts every mesh effect to it. The runner
passes it as `ProcessorParameters_DefaultEffect` metadata on the source asset; without it the
port would load a model whose effects are the wrong type. `head.xnb`'s reader list confirms it:
`ModelReader`, `VertexBufferReader`, `IndexBufferReader`, **`EnvironmentMapEffectReader`**.

## Framework defect found and fixed in `cnanext`

**`EnvironmentMapEffect`'s fresnel term was not clamped, and Direct3D 9 clamps it.**

FNA's `Structures.fxh` declares `VSOutputTxEnvMap.Specular` as **`COLOR1`**, and
`EnvironmentMapEffect.fx` fills it with
`pow(max(1 - abs(viewAngle), 0), FresnelFactor) * EnvironmentMapAmount` — a value with no upper
bound, since `EnvironmentMapAmount` is an ordinary float property and this sample's own slidebar
reaches 5. The pixel shader then uses that register as the weight of
`lerp(color.rgb, envmap.rgb, ...)`. Direct3D 9 saturates a vertex shader's colour output
registers to [0,1] **before** interpolating them, so on the hardware the effect was written for
the weight is at most 1: the environment map can replace the base colour, never be extrapolated
past.

EasyGL implements the stock effect as its own GLSL, where the equivalent varying is an ordinary
float that nothing clamps. The rim therefore over-brightened as `EnvironmentMapAmount` rose —
the original's orange rim turned yellow-white. The measurement names it precisely, because
agreement tracks the amount and nothing else:

| EnvironmentMapAmount, FresnelFactor | before | after |
|---|---|---|
| 0, 0 | 99.83 % | 99.83 % |
| 1, 10 | 99.84 % | 99.84 % |
| 2.5, 6 (the sample's own defaults) | 98.94 % | **99.75 %** |
| 5, 2 | 95.44 % | **99.77 %** |

At amounts of 1 and below the fresnel never exceeds 1, so the clamp is a no-op and those two
rows do not move — which is what rules out "the fix simply darkened everything".

Two tests pin it (`modules/graphics/tests/.../EnvironmentMapFresnelClampTests.cpp`), and the
second one was **confirmed to fail with the fix reverted**: unclamped, its pixel reads 1 instead
of 128.

This is the same distinction `plans/plan_fx.md` FX-122 draws for *translated* effects, one
sample earlier. That one was MojoShader's GLSL ES output; this one is CNA's own hand-written
stock-effect GLSL. Both come from the same place: a D3D9 COLOR register is clamped and a GLSL
varying is not.

## Comparison against the original

**The original cannot be driven by a mouse on this host, and that is a property of XNA, not a CNA gap.**
Every control in this sample reads `TouchPanel`, and XNA on Windows fills it from a real touch
digitizer only — there is none under Wine. Measured, not assumed: across a click on the button,
two slidebar drags and a drag on the head, the original's frames are **byte-identical to its own
start frame**, all seven of them. The historical CNA OPENGLES3 port responded
to the same pointer script because it opted into the `CNAEXT` mouse-as-touch
switch SAMPLE-021 added. The 2026-09-20 requalification briefly removed that
opt-in; after explicit owner approval it was restored. The current CNA port
accepts mouse as touch, while the unchanged XNA original remains touch-only.

The comparison is therefore made through a diagnostic hook that pins both slidebar values in
both engines (`scripts/compare-frozen.sh`, `cna-diag/README.md`), which is also what produced
the table above. The residue at every setting is the 1–2 pixel rim contour, where the Fresnel
term crosses a quantization boundary.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The current gate asserts the scene
renders over the flat grey clear, the rim is lit, a real touch reaches the controls, dragging the
slidebar grows the warm rim **without turning it white** (the FX-122-class regression pin), a
drag on the head rotates it, the UI overlay draws, and the document title is the original's
`RimLighting`.

## Historical deviations and remaining C++ mappings

- The owner-approved mouse-as-touch opt-in is documented in [`diff.md`](diff.md).
  It is enabled by one marked line, off by default in CNA, and does not add a
  second input path to the sample.
- `Button.OnClick` and `Slidebar.OnValueChanged` are `System::EventHandler<System::EventArgs>`.
  The original declares its own single-argument delegates; the project-wide event type is the
  established mapping and its `EventArgs` is simply unused.
- `Slidebar`'s `static Texture2D texureBlank` becomes a function-local static, keeping the
  one-load-per-process behavior without a mutable namespace-scope global.
- `UIElement::WordWrap` reproduces C#'s `Split(char[], StringSplitOptions.None)`, which keeps
  empty entries, rather than collapsing runs of separators.
