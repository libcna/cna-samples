# VertexLighting — port notes

## Current requalification — 2026-09-20

`SAMPLE-036` was rebuilt against active `libcna/cna` (`95b7e14a2`) and
`libcna/sharp-runtime` (`cb8fd7f8`). The physical Microsoft sample is
byte-identical to `xna4-original/`. The unchanged XNA 4.0 Debug build succeeds
for Windows/Reach, Windows/HiDef and Xbox 360/HiDef, using only stock importers
and processors. All seven new Windows XNBs are byte-identical to the checked-in
Content. The original Windows executable and each of its ten static scene
states run under WineD3D on the isolated Xvfb display.

The current `EffectPassCollection` indexer returns a pointer. Its two old
value-style `Apply()` call sites, in the mesh and reference-grid loops, were
corrected to pointer calls. Those are faithful C++ API spelling corrections,
not renderer workarounds. `SampleCamera` and `SampleGrid` remain identical to
SAMPLE-035's port apart from namespace spelling. The grid's `SetData` is
present in the original C# source. The FX-122 vertex-COLOR clamp is already
part of active CNA; no new CNA or sharp-runtime fix, stub, asset substitute or
intentional behavior deviation was needed.

Fresh Release OPENGLES3 and non-threaded WEBGL2 builds used no more than four
parallel compiler jobs. The original and native products were each captured
in ten matching lighting/mesh states at 800×480. Every comparison covers all
384,000 pixels with **zero exclusions**: 381,012–381,018 pixels
(99.222–99.223%) agree within eight channel levels, and mean absolute channel
error is 0.790–0.794/255. On all four `FlatShaded` captures the exactly-equal
count equals the within-eight count, leaving edge/grid coverage rather than a
flat-fill color error. Both engines return to a byte-identical initial frame
after the complete effect/mesh cycle. Additional original and native captures
respond to zoom, camera orbit and mesh rotation, and both close on Escape.

The fresh web build and its byte-identical local-gallery copy each pass the
system-Chrome WEBGL2 gate: blue vertex shading versus flat white, all five
meshes, green reference grid, full effect/mesh cycle, zoom, camera orbit, mesh
rotation and the original document title. All four game files return HTTP 200;
there are no game-asset HTTP errors, JavaScript exceptions or fatal renderer
errors. Chrome's unrelated `/favicon.ico` request returns 404 on the isolated
test server. The exact upstream `Lambertian.png`, `VertexLightingSample.PNG`
and `Game.ico` are restored outside Content; the original HTML document can
resolve its illustration. The local gallery now has 35 cards on 12/12/11
pages. Publication and artifact pruning were not requested in this task.

Reproduction scripts, build logs, whole-frame comparisons, captures and Chrome
results live under
`/rv/tmp/samples/SAMPLE-036-VertexLightingSample_4_0/{scripts,evidence/requal-20260920}/`.
The refreshed retained native and web products are in that root's
`cna-native-opengles3/` and `cna-web-webgl2/` directories.

## Historical port record

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
