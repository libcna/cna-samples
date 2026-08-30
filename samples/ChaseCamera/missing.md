# Missing / Differences from XNA 4.0 original

**Current status: complete (fresh audit 2026-08-30).** The port now follows the original stock
XNA content and rendering path with no raw-model loader, converted sidecars, culling override,
invented overlay or omitted platform branch. The 2026-07-10 report is retained at the end under an
explicitly superseded heading because it documents why the removed workaround once existed.

Artifact root: `/rv/tmp/samples/SAMPLE-058-ChaseCamera_4_0/`.

## Fresh source and project audit

The complete upstream directory was copied unchanged to `xna4-original/`. Every C# file, Windows
and Windows Phone project, solution, content declaration, source asset, HTML topic, icon and
license was reviewed against the port. The old port diverged in five material ways:

- it replaced `Content.Load<Model>("Ship")` and `Content.Load<Model>("Ground")` with
  `RawModel.hpp` and loose JSON/PNG/vertex/index sidecars;
- it changed rasterizer state around the converted ground to compensate for converted winding;
- it loaded and displayed an invented F1 help image;
- it flattened `ChaseCamera` C# properties into public fields and changed several member names;
- it omitted the Windows Phone configuration and returned the wrong logical game type name.

All five differences are removed. `ChaseCamera.hpp/.cpp`, `Ship.hpp/.cpp` and
`ChaseCameraGame.hpp/.cpp` preserve the original decomposition, member order and behavior.
`ChaseCamera` uses CNA property naming for every original C# property, including the computed
desired position/look-at getters. Its defaults and spring equation remain 1800 stiffness, 600
damping, 50 mass and `force = -stiffness * stretch - damping * velocity`. `Ship` retains the four
original public fields, its separately sampled mouse-region helpers, local-axis steering,
re-orthonormalization, 24000 thrust, 0.97 drag and 350 minimum altitude.

The game retains the exact initialization/update/draw order, 853x480 desktop backbuffer, all
keyboard/game-pad/mouse controls, spring toggle and reset behavior, model traversal, default
lighting, draw states and shadowed HUD string. Its `WINDOWS_PHONE` branch is again present with a
480x800 backbuffer, 333333-tick target elapsed time, fullscreen mode and empty keyboard state.
The logical type name is `ChaseCameraSample.ChaseCameraGame` and the assembly title remains
`ChaseCameraSample`.

## Exact official content

The unchanged XNA 4.0 content project builds successfully for Windows Reach, Windows HiDef and
Windows Phone Reach. The active Windows project declares Reach, so the five checked-in files are
byte-identical to that official output:

| asset | SHA-256 |
|---|---|
| `Checker_0.xnb` | `e76157fed0e09dfa5e9c646f49adad29d611d5985cb62699a8c18a799a8efb9d` |
| `Ground.xnb` | `4deb1f9fafbe411a030f694f6757dbdcbc5f0f14f9f6003e78bf1b363e4fd6a3` |
| `Ship.xnb` | `c2da35cd00e14c4da47c74a4a7b3200949ff0c7c3c57e02c2bfa46cd5bb701de` |
| `ShipDiffuse_0.xnb` | `743a40c047a783cb92cf335ef38e2c3001584a47a29a7dc17583b372b6ded006` |
| `gameFont.xnb` | `29b6764c1fb48aa7a315d840999bf7f048ff0494ed1f1ab290a2534b76b7abc8` |

Reader-table evidence records the stock `ModelReader`, vertex declaration/buffer, index buffer and
`BasicEffectReader` graphs for both models. The ship has five shared resources and the ground
three; the two texture XNBs arrive from material references even though the content project lists
only the font and two models. The old loose model/font/texture/buffer substitutes are gone.
Historical `help.png` is retained at the sample root and is neither packaged nor loaded.

## Original execution and input

The unchanged Windows Reach source compiles to `xna4-build/bin/ChaseCamera.exe` and runs under the
campaign Wine prefix with WineD3D. The audit harness captures the initial scene, then exercises
held Space thrust, Space+Left steering, `A` spring disable, `R` reset, center mouse-button thrust
and clean Escape exit. The official ship and ground are textured, lit, correctly wound and drawn
through the original model traversal without a special rasterizer state.

## Native XNA/CNA fidelity

The Release OPENGLES3 target builds and runs against the exact XNBs. It exercises the same six
states and exits cleanly without a fatal log. The stable initial 853x480 frame compares as follows:

| comparison | exact pixels | within 8 | after 4 px blur, within 8 |
|---|---:|---:|---:|
| XNA vs CNA OPENGLES3 | 37.56% | **99.09%** | **100.00%** |
| XNA vs CNA WEBGL2 | 41.80% | **99.90%** | **100.00%** |
| OPENGLES3 vs WEBGL2 | 52.90% | **99.00%** | **100.00%** |

The frames visibly agree on ship geometry and texture, checker scale and winding, horizon,
lighting, camera pose and every HUD glyph. The unblurred residue is sub-pixel texture filtering and
lighting, not omitted geometry. Dynamic screenshots are retained as interaction evidence rather
than treated as fixed-time comparisons because the harness holds real inputs for wall-clock
intervals and each engine may advance a different number of frames.

## Web

The complete Release Emscripten `WEBGL2` bundle runs in the system Google Chrome over local HTTP.
The gate verifies a real WebGL 2 context and 853x480 canvas, original title and renderer log, and
drives thrust, steering, spring disable, reset and mouse thrust. All six state hashes differ as
expected. The `.html`, `.js`, `.wasm` and `.data` requests return successfully, with no promise
rejection, runtime exception, relevant HTTP failure or fatal console message.

## Framework and runtime result

No CNA, SharpRuntime, EasyGL or MetaGL repair was needed. CNA's current XNB `ModelReader` loads and
draws both official models and their shared textures directly. This supersedes the old JSON reader
finding rather than preserving its sample-side workaround: `RawModel.hpp`, every loose sidecar and
the converted-ground `CullNone` toggle were deleted. The official `Ground.xnb` renders with the
unchanged default rasterizer state, proving the old winding issue belonged to the lossy
`.x` -> OBJ -> JSON conversion path, not to the original asset or CNA's XNA content path.

## Intentional C++ mappings

- C# reference ownership maps to `std::unique_ptr` and `std::optional` while preserving object
  lifetimes and initialization order.
- C# properties map to `getXProperty()`/`setXProperty()`; the original `Ship` public fields remain
  fields because they are fields in C# too.
- The C# typed `foreach (BasicEffect effect ...)` maps to checked `dynamic_cast` and throws
  `System::InvalidCastException` on a mismatch.
- The C# `float` API surface uses `SharpRuntime::Single`; `Math.Max` uses `System::Math::Max`.
- `GameTime` is a C++ reference, so the C# null guard has no representable null case.
- `static void Main()` maps to `int main()`.

These are lossless language mappings. There is no owner-approved behavioral addition and no
`diff.md` is needed.

## Documentation and evidence

`ChaseCamera.htm` and `Microsoft Permissive License.rtf` are byte-identical to upstream. Important
artifact paths are:

- `xna4-original/`: complete untouched upstream snapshot;
- `xna4-build/`: unchanged executable and Windows Reach/HiDef plus Phone Reach pipeline products;
- `cna-native-opengles3/` and `cna-web-webgl2/`: reusable native and browser build trees;
- `scripts/`: pipeline/original build and original/native/browser capture helpers;
- `evidence/xna-original/`, `evidence/cna-native-opengles3/` and
  `evidence/cna-web-webgl2/`: six-state captures, logs and browser result;
- `evidence/{xnb-reader-tables,xnb-sha256,pixel-comparison}.txt`: content and image measurements.

There is no remaining SAMPLE-058 blocker, omission, substitute or sample-side workaround.

## Superseded 2026-07-10 audit (historical evidence only)

Source: `/rv/tmp/XNAGameStudio/Samples/ChaseCamera_4_0/ChaseCamera/{ChaseCamera.cs,
ChaseCameraGame.cs, Ship.cs}` plus `ChaseCameraContent/{Ship.fbx, Ground.x, ShipDiffuse.tga,
Checker.bmp, gameFont.spritefont}`. The sample demonstrates a spring-physics chase camera
(`ChaseCamera.cs`, pure `Vector3`/`Matrix` math) following a ship (`Ship.cs`, simple flight
physics) flying over a large checkered ground plane.

## Major finding: independent confirmation of DEFERRED.md item #26 (`ModelTypeReader` vertex-corruption bug), on two more assets

**XNA behaviour:** `ChaseCameraGame.cs`'s `DrawModel()` helper calls `Content.Load<Model>
("Ship")`/`Content.Load<Model>("Ground")` in `LoadContent()` and draws both models every
frame via the standard `Model.Meshes`/`BasicEffect.EnableDefaultLighting()` pattern used by
every other lit-`BasicEffect` sample in this repo.

**CNA port behaviour:** Empirically tested, per this task's own brief, **before** assuming
the item #26 bug applied here. A temporary test build used `Content.Load<Model>("Ship")`/
`Content.Load<Model>("Ground")` (`Ship.fbx` converted directly via `tools/fbx_ascii2model.py`;
`Ground.x` converted via `assimp export Ground.x Ground.obj` + `tools/obj2model.py`, per this
task's own suggested pipeline — both produce ordinary stride-32 `.model.json` files, built and
loaded without any error) plus the exact `DrawModel()`/`BoneIndexOf()` pattern already
established by PickingSample/TrianglePicking/HeightmapCollision. Screenshot-confirmed: **a
solid CornflowerBlue screen with only the 2D HUD text visible — neither the ship nor the
ground rendered at all**, across two screenshots 3 seconds apart, no crash. This sample's own
initial camera distance is `sqrt(2000^2 + 3500^2) ≈ 4031` units (`DesiredPositionOffset =
(0, 2000, 3500)`) — even farther than Graphics3D's ~3523-unit spaceship, which showed the
identical "fully invisible" symptom.

This matches DEFERRED.md item #26 exactly (see `samples/InverseKinematics/missing.md` for the
full root-cause writeup): every CNA vertex struct now inherits from the polymorphic
`IVertexType`, inflating its `sizeof()` past the "clean" XNA size (16/20/24/32) every
conversion tool in this repo declares as `"vertexStride"`. `"vertexStride": 32` (used by both
`Ship.model.json` and `Ground.model.json` here, like every other `Content.Load<Model>`-based
sample in this repo) *accidentally* equals the inflated `sizeof(VertexPositionTexture)` (also
32), so `ModelTypeReader::Read()` always dispatches to the wrong typed `SetData` overload and
`reinterpret_cast`s the raw, vtable-free file bytes as vtable-shifted `VertexPositionTexture`
objects — reading Position/TextureCoordinate from the wrong byte offsets.

**This is a THIRD and FOURTH independent confirmation of item #26's hypothesis** (the first
was InverseKinematics' 418-vertex `cylinder.model.json`), on two more independently-converted
assets — one FBX (`Ship_p1_wedge_geo1`: 32458 vertices, 16118 triangles — two orders of
magnitude larger than the cylinder) and one `.x`-file (`Ground`: 6 vertices, 2 triangles — the
*smallest* mesh yet tested through this bug) — **at both a much larger and a much smaller
vertex count than InverseKinematics' cylinder**, further reinforcing that this bug is a
structural reader defect, not something correlated with any particular mesh size, complexity,
or source format. Saying so explicitly, as this task's brief requested: **item #26's
hypothesis is confirmed again here, independently, for a fourth and fifth (Ship + Ground)
`.model.json` asset.**

**Workaround used in this port:** `src/RawModel.hpp` (NOXNA) — the same bypass shape
established by InverseKinematics' `CylinderModel.hpp` and HeightmapCollision's/
GeneratedGeometry's `Terrain.hpp`: reads the already-converted `Ship_p1_wedge_geo1_verts.bin`/
`_idx.bin` and `Ground_verts.bin`/`_idx.bin` (produced once, offline, by the *unchanged*
`tools/fbx_ascii2model.py`/`tools/obj2model.py`) directly and constructs real,
normally-initialized C++ `VertexPositionNormalTexture` objects (field-by-field, not a
`reinterpret_cast` on a raw byte blob), then uploads them through the same typed
`VertexBuffer::SetData(const VertexPositionNormalTexture*, count)` overload `ModelTypeReader`
was trying (and failing) to reach. Confirmed live via screenshot: both the ship (fully
textured with `ShipDiffuse.tga`→`.png`, visibly shaded) and the ground (fully textured with
`Checker.bmp`→`.png`, visibly shaded) render correctly — generalized from `CylinderModel.hpp`
to also bind a real `Texture2D` directly to the `BasicEffect` (the same side benefit
`Terrain.hpp` already established), since `.model.json` has no per-mesh texture field
(DEFERRED.md item #6's addendum) — so, unlike PickingSample/TrianglePicking/HeightmapCollision's
sphere, neither model in this sample hits the "flat white, no shading" finding at all.

**Tracked in:** DEFERRED.md item #26 (existing item, this session adds a confirmation note,
not a new item).

## Ground.x's converted winding needs `RasterizerState::CullNone`

**XNA behaviour:** `Ground.x`'s single `Mesh` block (a 4-vertex, 2-triangle quad, 65536 units
on a side, textured with `Checker.bmp` tiled 32×32) renders normally through the stock content
pipeline and `BasicEffect`'s default `RasterizerState.CullCounterClockwise`.

**CNA port behaviour:** After switching to `RawModel.hpp` (above), the ship rendered correctly
immediately, but the ground still did not appear at all — confirmed live (screenshot: ship
visible, floating over a plain CornflowerBlue background, no checker plane anywhere). Isolated
by temporarily setting `RasterizerState::CullNone` around only the ground's draw call:
confirmed live this alone made the full checkered ground plane appear, textured and shaded,
with no other change. Root cause: `assimp export Ground.x Ground.obj` (this task's own
suggested `.x`→`.obj` conversion path, since neither `tools/obj2model.py` nor
`tools/fbx_ascii2model.py` reads `.x` directly) re-emits the quad's two triangles wound the
opposite way from what CNA's default `RasterizerState::CullCounterClockwise` expects, so every
triangle was being back-face-culled. This is the same per-asset winding adjustment
`HeightmapCollision`'s/`GeneratedGeometry`'s own `Terrain.hpp` already needed for their
runtime-built terrain meshes (see those samples' `missing.md`), not a new class of bug — just
the first time it's been seen on an `assimp`-round-tripped `.x` asset specifically rather than
a hand-built runtime mesh. `Ship.fbx` (converted directly via `tools/fbx_ascii2model.py`, no
`assimp` round-trip) needed no such adjustment — its winding survives conversion correctly,
matching every other FBX-sourced model in this repo.

**Root cause:** `assimp export`'s re-triangulation/re-emission of `Ground.x`'s two faces uses
the opposite winding convention from CNA's default `RasterizerState::CullCounterClockwise`.

**CNA port behaviour (workaround):** `ChaseCameraGame::Draw()` sets
`RasterizerState::CullNone` around only the ground's draw call (restoring
`CullCounterClockwise` immediately after, for the ship and any future geometry), rather than
disabling culling globally.

**Tracked in:** not a CNA gap — a per-asset conversion-pipeline quirk of the `assimp export`
step this task's own brief suggested for `.x` files. Not filed as a new DEFERRED.md item
(same root cause/fix shape as the pre-existing `Terrain.hpp` precedent, just a different
trigger).

## Windows-only control scheme (matches every other desktop port in this repo)

**XNA behaviour:** `ChaseCameraGame.cs` has a `#if WINDOWS_PHONE` branch (480×800 portrait
back buffer, fixed 30 Hz `TargetElapsedTime`, full-screen) and a Windows/`#else` branch (853×480
landscape back buffer) — plus full `GamePad` support and `MouseState`-based "touch" helpers
(`TouchLeft`/`TouchRight`/`TouchUp`/`TouchDown` in `Ship.cs`, `touchTopLeft` in
`ChaseCameraGame.cs`) that predate real XNA touch APIs and already use mouse-as-touch directly,
even when compiled for Windows.

**CNA port behaviour:** Only the Windows (`#else`) branch is ported (853×480, matching every
other desktop-only port in this repo); `GamePad`/`Buttons`/`GamePadThumbSticks`/
`GamePadTriggers` code is kept (CNA supports `GamePad` fully) so a real gamepad still works
identically to the original. The mouse-based "touch" helpers port unchanged — no
mouse-substitutes-for-touch adaptation was needed here, since the C# original already used
`MouseState` + `GraphicsDevice.Viewport` thirds directly for this, not a real touch API.

**Root cause:** This repo targets desktop only; no Windows Phone target exists.

**Tracked in:** not planned.

## `graphics.SupportedOrientations = Portrait` has no visible effect on this desktop build

**XNA behaviour:** `ChaseCameraGame`'s constructor sets
`graphics.SupportedOrientations = DisplayOrientation.Portrait` unconditionally, even though the
non-Windows-Phone branch immediately afterward sets a landscape 853×480 back buffer. This
looks like a copy/paste leftover from the Windows-Phone branch (SupportedOrientations only
meaningfully restricts phone/tablet display rotation in real XNA); it has no effect on a
Windows desktop `.exe`'s window.

**CNA port behaviour:** Ported literally (`graphics_->setSupportedOrientationsProperty
(DisplayOrientation::Portrait)`), for source fidelity. Confirmed live this has no visible
effect on the sample's actual 853×480 landscape window (matches CNA's own previously-fixed
portrait-orientation-forcing bug, which is now platform-gated FNA-style and does not restrict
desktop windows).

**Root cause:** Faithful reproduction of an apparent copy/paste artifact in the original
sample, not a CNA gap.

**Tracked in:** not planned (cosmetic, upstream-only, no observable behavior difference).

## Verification

Built `ChaseCamera_cna_samples` with 0 warnings/0 errors, verified via a from-scratch rebuild
of the changed translation unit (`cmake --build cmake-build-debug --target
ChaseCamera_cna_samples -j$(nproc)`, object file removed and rebuilt, output grepped for
"warning"/"error" — none found). Ran under `SDL_VIDEODRIVER=x11` for 8+ seconds across three
separate runs with no crash.

Screenshot-confirmed: the ship (`Ship_p1_wedge_geo1`, textured with `ShipDiffuse.png`) renders
correctly, fully lit and shaded, floating above a fully textured/shaded checkered ground plane
(`Ground`, textured with `Checker.png`) that extends to the horizon in every direction — both
via the `RawModel.hpp` bypass described above. The 2D HUD text (thrust/steer/spring-toggle
instructions, matching `DrawOverlayText()`'s exact C# string) renders correctly in the
top-left corner. F1 help overlay verified via this repo's established temporary
debug-auto-trigger pattern (`helpTimer_` forced to 10.0f, screenshotted, reverted before
commit): renders the `.htm`-table-derived semi-transparent panel correctly, centered on
screen, with the correct 4-row control table (Steer / Accelerate / Toggle camera spring
enabled / Exit the sample) extracted verbatim from `ChaseCamera.htm`'s own "Sample Controls"
table (no one-off `gen_help_png.py` column-selection variant needed — this `.htm`'s
"Keyboard Control (Windows)" column is already the tool's default column 1).

Ship movement and the chase camera's spring-damper physics were also verified live via a
second temporary debug-auto-trigger (`Ship::Update()`'s `thrustAmount` forced to `1.0f`,
reverted before commit): two screenshots taken 4 seconds apart show the ship's on-screen
position visibly shifting as it accelerates forward, with the chase camera visibly lagging
behind/around it (spring inertia) rather than staying perfectly locked — exactly the intended
"spring camera" behavior, and direct confirmation `ChaseCamera::Update()`'s ported spring-force
math (`force = -Stiffness*stretch - Damping*velocity`) is computing live, correct results, not
just static placeholder geometry.

Live mouse/keyboard-driven interaction (arrow-key steering, spacebar thrust, `A` to toggle the
camera spring, `R` to reset) was not separately exercised via synthetic input this session:
`xdotool getactivewindow` showed a different, real user window had focus throughout this
session (consistent with this repo's own documented shared-desktop `xdotool` reliability
caveat — NEXT.md section 5), so no keypresses were sent to avoid interfering with that window;
the debug-auto-trigger method above was used instead, per this repo's established fallback.

No known differences beyond the three documented above (the item #26 vertex-corruption
bypass, the `Ground.x` winding/`CullNone` adjustment, and the cosmetic
`SupportedOrientations`/Windows-Phone-branch omissions).
