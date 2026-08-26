# PerPixelLighting — port notes

Upstream: `PerPixelLightingSample_4_0` (SAMPLE-035). Ported whole — all three source files, both
effects, all five techniques, all five meshes and every key binding. Nothing is missing, stubbed
or simplified.

## Content

Eight assets, every one through a **stock** importer/processor pair — unlike SAMPLE-032 and
SAMPLE-034, this sample ships no pipeline extension. Built by the official XNA 4.0 Content
Pipeline from the unmodified original for all three of the solution's targets (Windows/Reach,
Windows/HiDef, Xbox 360/HiDef). Both `.fx` files compile in **Debug**; `scripts/build-original.sh`
does the whole build.

Linking the original also needs `Microsoft.Xna.Framework.Storage` and
`Microsoft.Xna.Framework.Avatar` on the compiler line — `SampleCamera.cs` and `SampleGrid.cs` both
carry a `using Microsoft.Xna.Framework.Storage`, which no earlier sample in this campaign did.

## Framework work in `cnanext`

**One defect, and two API members C# gets for free.**

`plans/plan_fx.md` **FX-122 — a compiled effect's vertex COLOR output was not clamped on the GLSL
ES profiles.** Direct3D 9 clamps `oD0`/`oD1` to [0,1] *before* interpolating them; MojoShader's
desktop GLSL path inherits that by writing `gl_FrontColor`, which GL clamps, but the ES profiles
fall through to a plain varying that nothing clamps. `VertexLighting.fx` sums ambient
(DarkSlateGray) + diffuse (CornflowerBlue) + specular, so blue reaches ~1.24 at a lit vertex —
D3D9 interpolates from 1.0 down, CNA interpolated from 1.24 and saturated per fragment, which is
**flat where D3D9 still has slope**. On the low-poly cone that was a solid saturated wedge over
4.8 % of the frame.

What identified it: **red and green matched the original exactly, pixel for pixel, and only blue
differed — only where blue would exceed 1.** Nothing but a missing clamp produces that.

Two members were added to the math layer, both `CNAEXT` in spelling and XNA in behavior:

- `Matrix::operator*=` (matrix and scalar). C# synthesises `*=` from `op_Multiply`; C++ does not,
  and this sample's `world *= Matrix.CreateFromAxisAngle(...)` needs it. Same reasoning as
  SAMPLE-024's `Vector2` compound operators.
- `Quaternion()`. C# gives every struct a parameterless constructor that zeroes its fields, so
  `SampleArcBallCamera`'s undeclared `orientation` field starts at all zeros. `Vector3` and
  `Matrix` already had one.

## Comparison against the original

The scene is fully static — world, camera and both specular values change only on input — so no
freeze hook is needed. `scripts/capture-original.sh` and `scripts/capture-cna-native.sh` drive
the same ten frames: the five effect/technique combinations Space cycles, the cycle back to the
first, and the four meshes Tab reaches.

| Frame | Within 8 levels |
|-------|-----------------|
| PerVertexDiffuse | 99.40 % |
| PerPixelDiffuse | 99.40 % |
| PerVertexDiffuseAndPhong | 99.40 % |
| PerVertexDiffuseAndPerPixelPhong | 99.40 % |
| PerPixelDiffuseAndPhong | 99.40 % |
| back to PerVertexDiffuse | 99.40 % |
| SphereLowPoly | 99.37 % |
| Cylinder | 99.37 % |
| Cone | 99.40 % |
| Cube | 99.39 % |

Before FX-122 the three low-poly frames sat at 95.2 %, 97.9 % and 98.1 %; the fix brought them
onto the same line as the rest. The residue is the mesh's 1-pixel silhouette and the far-field
grid lines — line and edge rasterization, identical in shape on every frame.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the scene
renders, the LimeGreen reference grid is drawn (BasicEffect's line route still runs beside a
custom effect), the three lines of debug text are drawn (so does SpriteBatch), all five
combinations differ, Space cycles back to the first, Tab really swaps the mesh, the document
title is the original's `Per-Pixel Lighting`, and — as the regression pin for FX-122 — that the
cone's mesh pixels are not driven into saturation.

## Deviations

None in behavior. The C++ shapes are the campaign's usual ones: `getXProperty()`/`setXProperty()`
for C# properties, `std::optional` for reference-type fields that are null until `LoadContent`,
`Game::Update`/`Game::Draw` for `base.Update`/`base.Draw`, and `Matrix::getIdentityProperty()`
for `Matrix.Identity`.

Two null guards the original raises `ArgumentNullException` for — `HandleDefaultKeyboardControls`
and `HandleDefaultGamepadControls` checking `gameTime` — have nothing to check here, because a
C++ reference cannot be null. `SampleGrid`'s finalizer becomes the C++ destructor, which runs at
the same point.
