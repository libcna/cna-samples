# RimLighting — port notes

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

**The original cannot be driven on this host, and that is a property of XNA, not a CNA gap.**
Every control in this sample reads `TouchPanel`, and XNA on Windows fills it from a real touch
digitizer only — there is none under Wine. Measured, not assumed: across a click on the button,
two slidebar drags and a drag on the head, the original's frames are **byte-identical to its own
start frame**, all seven of them. CNA responds to the same pointer script because the port opts
into the `CNAEXT` mouse-as-touch switch SAMPLE-021 added.

The comparison is therefore made through a diagnostic hook that pins both slidebar values in
both engines (`scripts/compare-frozen.sh`, `cna-diag/README.md`), which is also what produced
the table above. The residue at every setting is the 1–2 pixel rim contour, where the Fresnel
term crosses a quantization boundary.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the scene
renders over the flat grey clear, the rim is lit, a pointer reaches the controls, dragging the
slidebar grows the warm rim **without turning it white** (the FX-122-class regression pin), a
drag on the head rotates it, the UI overlay draws, and the document title is the original's
`RimLighting`.

## Deviations

- The port turns on `TouchPanel::setMouseTouchEmulationEnabledEXT(true)`, one `CNAEXT`-marked
  line, so a touch-only sample is playable without a digitizer. Off by default framework-wide;
  the precedent is SAMPLE-021.
- `Button.OnClick` and `Slidebar.OnValueChanged` are `System::EventHandler<System::EventArgs>`.
  The original declares its own single-argument delegates; the project-wide event type is the
  established mapping and its `EventArgs` is simply unused.
- `Slidebar`'s `static Texture2D texureBlank` becomes a function-local static, keeping the
  one-load-per-process behavior without a mutable namespace-scope global.
- `UIElement::WordWrap` reproduces C#'s `Split(char[], StringSplitOptions.None)`, which keeps
  empty entries, rather than collapsing runs of separators.
