# Graphics3D — port notes

Upstream: `Graphics3DSample_4_0` (SAMPLE-046). **Re-ported from scratch.** Ported whole — the
spaceship model, the three directional lights, the per-pixel-lighting toggle, the starfield
background, the sprite-sheet explosion animation, all four touch checkboxes and both gestures.

Artifact root: `/rv/tmp/samples/SAMPLE-046-Graphics3DSample_4_0/`.

## What the 2026-07-09 pass recorded, and what survives

The previous port was header-only, shipped a hand-converted model, and opened with three findings.
**None of the three survives the official content pipeline and the current `cnanext`.**

| The old note said | Measured now |
|---|---|
| `spaceship.fbx` is binary FBX 6000, "unreadable by this repo's normal tools"; converted by hand through `ufbx` → `.obj` → `obj2model.py` | The official **`FbxImporter` reads it directly**, first try, and pulls in the material's `Models\enemy.tga` as a tenth asset. `evidence/build-original.log`. The port now loads `Models/spaceship.xnb` from the real pipeline. |
| The ship does not render — "the pre-existing EasyGL near-plane-clipping framework bug", isolated over a session | **The ship renders.** `evidence/cna-native-opengles3/`. Same class of claim as SAMPLE-041's "near-plane clipping" terrain, and it dissolves the same way: it was the hand-converted asset, not the renderer. |
| `Game::DoInitialize()` subscribes `ComponentAdded` *after* `Initialize()`, so a component added from inside `Initialize()` is never initialized; worked around with an `AddComponent()` helper | **No gap.** CNA matches FNA (`Game.cs:791`) *and* XNA here: components are added before `base.Initialize()`, and `Game::Initialize()`'s own loop initializes everything in `Components`. The upstream sample adds all four checkboxes before calling `base.Initialize()`, which is exactly why it works. The helper is gone; the port calls `getComponentsProperty().Add(...)` like the original. |
| `GraphicsDevice::Clear(Color)` never clears depth | **Already fixed** (Task 928): the single-argument overload forwards `Target \| DepthBuffer \| Stencil` at `Viewport.MaxDepth`, matching FNA verbatim. |

The old port also added an F1 help overlay, an Escape exit and a `help.png` the original has no
control table for. None of that is upstream, and none of it is here.

## Two upstream defects, reproduced rather than repaired

- **`Program.cs` does not compile.** Inside `#if WINDOWS || XBOX` it constructs a type named
  `Sample3DGraphics`; the game class is `Graphics3DSampleGame`. The project only ever built for
  Windows Phone, so that block was never compiled. `Buttons/Button.cs` is the other half of the same
  abandoned rename — it still declares `namespace Sample3DGraphics`, and the `.csproj` does not list
  it at all. The audit links `xna4-build/generated/Program.cs` instead, the same way SAMPLE-021 does
  for a phone project with no `Program.cs`; `xna4-original/` is untouched and `Button.cs` stays
  excluded because the project excludes it. The C++ `Program.cpp` is the entry point that block was
  meant to be.
- Only `Buttons/Clickable.cs`, `Buttons/Checkbox.cs`, `Animation/Animation.cs`,
  `Models/Spaceship.cs`, `GameMain.cs` and `Properties/AssemblyInfo.cs` are compiled, matching the
  `.csproj` exactly.

## Content

Ten `.xnb` files from the official pipeline — nine listed assets plus `enemy_0.xnb`, which the FBX
material asks for. `AnimationDef.xml` is **not** compiled: the content project lists it as
`<None Include ... CopyToOutputDirectory="PreserveNewest">`, so it is copied beside the content and
the game parses it at run time with `XDocument.Load("Content/AnimationDef.xml")`. sharp-runtime
already provides `System::Xml::Linq`; the sample links `SharpRuntime::Xml.Linq`, mirroring the
project's own `<Reference Include="System.Xml.Linq" />`.

## Touch

**Owner-approved deviation — see [`diff.md`](diff.md).** Upstream this is a touch-only Windows Phone title: the four checkboxes read `TouchPanel.GetState()`
and the camera is driven by `FreeDrag` and `Pinch`. The port opts into CNA's
`TouchPanel::setMouseTouchEmulationEnabledEXT(true)` — the one `CNAEXT` line in the sample, the same
one SAMPLE-021 uses — so the pointer feeds the same `TouchPanel` and the same gesture recognizer.
The game logic is untouched, and CNA's `GestureDetector` produces the real `FreeDrag`/`Pinch`
samples, so nothing here is a substitute input scheme.

## Two framework defects, both in the same place, both the built-in twin of an already-fixed one

The sample's whole state space is four checkboxes and nothing moves on its own, so the frame is
static from the first draw and the two engines can be compared directly. `../../../cna-diag/` and
`../../../xna4-diag/` add a hook per checkbox (`CNA_LIGHTS`, `CNA_PERPIXEL`, `CNA_BACKGROUND`,
`CNA_ANIMFRAME`) to both engines; `scripts/compare-frozen.sh` drives them.

### FX-123 — the per-vertex-lit colour was never saturated

`plans/plan_fx.md` FX-122 fixed a Direct3D 9 semantic in MojoShader's compiled-effect path: D3D9
clamps a vertex shader's colour output registers (`oD0`/`oD1`) to [0,1] **before** the rasterizer
interpolates them. The **built-in** effect path had the same hole. EasyGL's per-vertex-lit programs
carry the lit and specular RGB in plain `out vec3` varyings, which nothing clamps, so a per-vertex
sum above 1 interpolated unclamped and the model came out brighter — a different gradient, not a
rounding difference.

**The isolation is the finding**, and it is the reason this sample was the one to expose it:

| lights on | within 0 | within 8 | signed CNA−XNA over the model | after 4 px blur |
|---|---|---|---|---|
| none | 95.99 % | **99.99 %** | −0.01 / +0.00 / +0.05 | 100.00 % |
| light 0 only | 95.76 % | **99.99 %** | +0.01 / +0.00 / +0.05 | 100.00 % |
| light 1 only | 94.90 % | **99.99 %** | +0.01 / −0.01 / +0.07 | 100.00 % |
| light 2 only | 95.64 % | **99.99 %** | +0.01 / −0.01 / +0.06 | 100.00 % |
| **all three** | 88.72 % | **90.31 %** | **+11.18 / +14.29 / +8.88** | 89.67 % |

Any one light is exact. Only the accumulated sum crosses 1, which is precisely when a missing clamp
can matter — and the gap grew with the lit value and collapsed to zero in whichever channel XNA had
already saturated. Geometry was never in question: coverage matched to 15.042 % on both sides, the
centroid to two decimals, the row extent exactly, and the four button sprites were **100.00 %
pixel-identical** throughout.

### FX-124 — the per-pixel-lit fragment stage ran at `mediump`

`plans/plan_fx.md` FX-121 raised MojoShader's GLSL ES fragment output to `highp` because `mediump`
guarantees only fp16 *range*. The built-in path had that hole too: four EasyGL fragment shaders
normalize a world-space view vector, `normalize(uEyePosition - vWorldPos)`, while declaring
`precision mediump float;`. `normalize()` computes `dot(v, v)` first, and this sample's camera sits
3500 units out.

| per-pixel lighting, one light | within 8 levels | signed CNA−XNA |
|---|---|---|
| `mediump` | 90.62 % | **−20.12 / −20.13 / −20.10** |
| `highp` | **99.99 %** | −0.01 / −0.02 / +0.04 |

A signed error that is *uniform across all three channels* is a lost grey additive term, which is
what specular is here. With all three lights the diffuse saturates and hides most of it (92.40 %),
so the **single-light** leg is the one that names the defect. Mesa does honour the qualifier — that
was measured, not assumed. `EnsureEnvMapped3DProgram` normalizes in its *vertex* stage and passes a
unit vector, so it is correct at `mediump` and was left alone.

Both fixes are pinned by tests that were confirmed to fail when the fix is removed:
`easygl_basiceffect_vertex_color_clamp_test` (the sampled pixel reads 102 clamped and 153 unclamped
— a **grey**, not white, texture is what keeps the two answers apart) and
`easygl_basiceffect_world_scale_precision_test` (scale invariance: Blinn-Phong depends only on
directions, so multiplying every length by 1000 must change nothing; it also asserts both legs
actually have a highlight, so it cannot pass by comparing two black frames).

## Agreement with real XNA 4.0

Every state the sample has, native `OPENGLES3` against the original under Wine, both full screen on
the same 800×480 display:

| state | within 0 | within 8 | after 4 px blur |
|---|---|---|---|
| **default** (3 lights, per-vertex) | 94.96 % | **99.99 %** | **100.00 %** |
| per-pixel lighting | 96.00 % | **99.99 %** | **100.00 %** |
| starfield background | 94.96 % | **99.99 %** | **100.00 %** |
| explosion animation, frame 0 | 95.84 % | **99.99 %** | **100.00 %** |
| explosion animation, frame 3 | 98.53 % | **99.99 %** | **100.00 %** |

Default state: mean absolute difference **0.030 of 255**, median 0, coverage 15.042 % on both sides,
centroids 0.01 px apart.

## A capture note worth keeping

`IsFullScreen = true` is set in the constructor. CNA maps XNA's `IsFullScreen` to SDL's **exclusive**
fullscreen (`Sdl3Window::SetFullscreenMode`), where FNA asks for the desktop mode; with no window
manager on Xvfb the mode switch is never confirmed, SDL waits out two ~5 s timeouts, logs
`Time out elapsed after mode switch ... reverting` and carries on windowed. An 8 s capture landed
inside that stall and produced a **solid black frame that looks exactly like "the sample draws
nothing"** — the same shape as the old port's report. `scripts/capture-cna-native.sh` waits past it.
The divergence from FNA is real but is a fullscreen-policy question rather than anything this sample
exercises, and it is not filed as a fix here.

## Web

`WEBGL2` built under a real Emscripten toolchain and driven in real Google Chrome
(`scripts/capture-web.sh`, `scripts/chrome-smoke.mjs`), with the mouse dispatched as touch:

- the model, the four button sprites and the sky are all present;
- **`shipMeanLuminance` is calibrated against the native frames** — XNA reads 162.74 and native
  OPENGLES3 162.76, the browser 159.82, and the tolerance is 6, so the pre-FX-123 value of ~176
  fails it. The gate measures the thing that was fixed;
- two captures with no input are hash-identical, because nothing here animates by itself;
- each tap changes the frame, turning a lamp off **darkens the ship** (159.82 → 136.97), the
  background toggle covers the sky (84.36 % → 0.00 %), and a drag rotates the model;
- no runtime exceptions, no HTTP errors, no fatal console messages, `WEBGL2` renderer logged.

## Evidence

- `evidence/build-original.log` — the official pipeline, including the FBX import.
- `evidence/xna-original-800x480/`, `evidence/cna-native-opengles3/` — the compared frames.
- `evidence/frozen/<state>/{xna,cna}/` — one directory per hooked state.
- `evidence/cna-web-webgl2/browser-result.json` — the browser gate's own numbers.
