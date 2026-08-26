# VertexLighting — port notes

Upstream: `VertexLightingSample_4_0` (SAMPLE-036). Ported whole — all three source files, both
effects, all five meshes and every key binding. Nothing is missing, stubbed or simplified.

## Relationship to SAMPLE-035

This is PerPixelLighting's sibling and shares two of its three source files. `SampleCamera.cs` and
`SampleGrid.cs` are byte-identical to SAMPLE-035's **except for the namespace line**, so the C++
ports of both were carried over with that one line changed and nothing else — each pair still
differs by exactly two lines, matching the originals. The game class is this sample's own: no
`SpriteBatch`, no debug text, no specular controls, and two effects toggled by Space rather than
five effect/technique combinations.

## Content

Seven assets — two effects and five models — every one through a **stock** importer/processor
pair; no pipeline extension. Built by the official XNA 4.0 Content Pipeline from the unmodified
original for all three of the solution's targets, in **Debug**, first try.

Linking the original needs `Microsoft.Xna.Framework.Storage` and `.Avatar` on the compiler line,
as SAMPLE-035 did — `SampleCamera.cs` and `SampleGrid.cs` both carry a
`using Microsoft.Xna.Framework.Storage`.

## Framework work in `cnanext`

**None.** This is the first compiled-effect sample in the campaign that needed no change to the
runtime at all, and that is worth recording rather than passing over: its `VertexLighting.fx`
sums `ambientColor` and `lightColor * N·L` into a `COLOR0` vertex output, which is exactly the
shape that FX-122 (SAMPLE-035, same session) had rendered wrong. The clamp landed first, so this
sample matched the original from its first capture.

## Comparison against the original

The scene is fully static — world and camera change only on input — so no freeze hook is needed.
Both capture scripts drive the same ten frames: both effects on the cube, the effect toggled on
the high-poly sphere, the three remaining meshes lit, the cone and cube flat-shaded, and the
toggle back to the start.

| Frame | Within 8 levels |
|-------|-----------------|
| VertexLighting, Cube | 99.39 % |
| FlatShaded, Cube | 99.39 % |
| FlatShaded, SphereHighPoly | 99.41 % |
| VertexLighting, SphereHighPoly | 99.40 % |
| VertexLighting, SphereLowPoly | 99.37 % |
| VertexLighting, Cylinder | 99.37 % |
| VertexLighting, Cone | 99.40 % |
| FlatShaded, Cone | 99.40 % |
| FlatShaded, Cube | 99.39 % |
| back to VertexLighting, Cube | 99.39 % |

The residue is the mesh's 1-pixel silhouette and the far-field grid lines, the same shape and the
same magnitude as SAMPLE-035's. On the four FlatShaded frames the exact and the within-8 counts
are **equal**, which is what a flat fill should give: every differing pixel is an edge, and there
is no gradient anywhere for filtering to disagree about.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the scene
renders, the LimeGreen reference grid is drawn, FlatShaded paints the mesh a single flat white
while VertexLighting shades it, Space toggles both ways, Tab really swaps the mesh, and the
document title is the original's `Vertex Lighting`.

## Deviations

None in behavior. The C++ shapes are the campaign's usual ones. Two worth naming:

- The original constructs `SampleGrid` and `SampleArcBallCamera` inside `LoadContent`, not in the
  constructor, so both are `std::optional` members emplaced there rather than plain members.
- `Initialize()` is overridden to do nothing but call the base, exactly as the original does.
