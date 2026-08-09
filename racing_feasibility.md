# XNA 4 Racing Game Kit → CNA feasibility audit

Audit date: 2026-08-09

## Executive summary

**Verdict: FEASIBLE WITH SIGNIFICANT CNA WORK. Confidence: medium.**

The game is a realistic C++/CNA target, but it is not a mechanical source translation. Its game rules, track representation, physics, camera, replay, screens, and UI are conventional and finite. The difficult portion is the XNA-era rendering/content contract: 57 legacy `.x` models, a custom `ModelProcessor`, embedded custom material metadata, ten DX9 `.fx` files, a multi-render-target post-processing pipeline, and an authored XACT project.

CNA is substantially more capable than older `cna-samples` documentation suggests. The current integration code has partial XNB reading, a real binary model reader, texture/cube readers, programmable custom shaders on selected backends, generic vertex layouts, 32-bit indices, and a nontrivial XACT implementation. Focused tests for binary models, texture cubes, XACT cue variables/RPCs/categories, and MS-ADPCM passed. Therefore, “CNA cannot read XNB” and “CNA has no XACT” are false as blanket claims.

There are nevertheless concrete gaps. Directly consuming the original custom-effect XNB model output is blocked because general compiled `Effect` payloads are intentionally unsupported. The old `.fx` technique/pass/state model is not implemented by CNA's portable `ShaderEffect` abstraction. `GraphicsAdapter` can select `Rgba64` even though shared texture/render-target validation rejects it. The supplied uncompressed normalization cube DDS is outside the loose cube loader's supported DXT formats. Some non-reference backends cannot bind or compile the shader set adequately.

The recommended first implementation route is:

1. pin a stable post-integration CNA commit;
2. target Linux `OPENGL33` (internally EasyGL) only;
3. build a deterministic Racing-specific offline asset package preserving hierarchy, pivots, material values, bounds, and processor technique IDs;
4. port shader semantics explicitly into small `ShaderEffect` programs and C++ render passes;
5. use authentic generated XACT banks if they can be lawfully/tool-reproducibly obtained, otherwise make the raw-WAV mixer fallback an explicit product decision;
6. establish visual/telemetry/audio references before broad translation.

The central realistic estimate is roughly **1,100 engineering hours**, with a useful planning range of **900–1,250 hours** for one feature-complete reference backend. Of that, approximately **750–950 hours** are Racing Game port/tooling work, **140–240 hours** are CNA fixes and framework tests, and **40–60 hours** are the completed/repeated analysis baseline. Additional backends are excluded.

## Evidence convention and source baselines

Statements marked **Fact** were observed directly in source, project/content files, assets, or tests. **Inference** means the conclusion follows from those facts but has not been demonstrated by a running full game. **Unknown** identifies the experiment needed to decide.

Primary Racing source:

`/rv/tmp/XNAGameStudio/Samples/XNA-4-Racing-Game-Kit-master`

Important subtrees:

- runtime: `RacingGameWindows1/RacingGame/RacingGame/`
- active XNA 4 content project: `RacingGameWindows1/RacingGame/RacingGameContent/`
- custom processor: `RacingGameWindows1/RacingGameContentPipelineExtension/`

CNA baselines:

- **Forward-looking implementation inspected:** `../cnaintegration`, commit `4ac696c748fb`, branch `integration/post-audit-phase1`, dated 2026-08-09.
- **Older requested checkout:** `../cna`, commit `ac3aaaeb2a5b`, branch `develop`, dated about three weeks earlier.

**Fact:** the integration branch was actively absorbing 21 post-audit feature branches. Conclusions about it are a snapshot, not a promise about the eventual merge result. The future port must pin a commit and rerun probes. No CNA file was modified during this audit.

### Audit diagnostics performed

- Static inventories used the actual C# files, both content projects, project references, processor source, all `.fx`/`.x`/texture/audio/data assets, and runtime call sites; namespace or assembly presence alone was not counted as use.
- CNA guidance and status material reviewed included `CLAUDE.md`, `NEXT.md`, `CHECKLIST.md`, `AUDIT.md`, `docs/xna-4-api-coverage.md`, `docs/graphics-backend-feature-matrix.md`, `docs/xnb-content-pipeline-support.md`, `docs/model-content-pipeline-support.md`, and relevant graphics/content/audio plans and audits. Source/tests overruled stale documentation, notably for the now-real binary Model reader and the fixed SpriteBatch/3D ordering case.
- A selected ASAN test run in the integration worktree executed 35 model/XNB/texture-cube/XACT tests covering the binary model reader, cube readers, cue variables/RPCs, audio-category instance behavior, WaveBank handling, and MS-ADPCM; all 35 passed.
- Three focused graphics tests covering adapter format queries and unsupported `Rgba64` construction passed. This confirms CNA's current tested behavior, including the inconsistency that a queried/selected format can still be rejected at construction; it does not validate the Racing render-target chain.
- The original tree has no compiled game, XNBs, or XACT banks, and this environment has no `msbuild`, `xbuild`, or `dotnet` toolchain for that VS2010/XNA project. The source was therefore not repaired or executed.
- Documentation claims were checked against current headers/implementations/examples/tests for every high-risk conclusion. In particular, older model-XNB and SpriteBatch/3D statements were not accepted as authoritative when current code/tests contradicted them.

## Project inventory

### Source-code scope

| Measure | Result |
|---|---:|
| C# files, including assembly metadata and processor | 60 |
| Runtime project C# files, including `AssemblyInfo.cs` | 58 |
| Runtime implementation files excluding `AssemblyInfo.cs` | 57 |
| Custom processor implementation files excluding its `AssemblyInfo.cs` | 1 |
| Raw physical lines, runtime project | 23,428 |
| Raw physical lines, processor project | 176 |
| `cloc` C# code lines, all 60 files | 13,952 |
| `cloc` comments | 7,360 |
| `cloc` blank lines | 2,296 |
| Runtime code lines excluding assembly metadata | 13,836 |

Subsystem distribution of the 57 runtime implementation files:

| Subsystem | Files | C# code lines | Main responsibilities |
|---|---:|---:|---|
| Game logic | 8 | 2,333 | player, physics, camera, replay/race behavior |
| Screens/menus | 10 | 1,668 | loading, splash, selections, options, help, highscores, gameplay |
| Graphics | 13 | 3,760 | game host, model wrappers, batching, targets, UI, screenshots |
| Helpers | 6 | 531 | settings, interpolation, logging and utilities |
| Landscape | 1 | 778 | height mesh, object population, collision data |
| Shader wrappers | 8 | 1,654 | effects, post chains, shadow, sky |
| Sound | 1 | 324 | XACT cue/category state machine |
| Tracks | 7 | 2,159 | XML track data, spline/road helpers, collision/race positions |
| Root classes | 3 | 629 | entry point and game manager |

**Inference:** roughly 40–45% of the implementation is domain/game behavior and 55–60% is XNA/rendering/content/UI/platform integration. These are judgment ranges, not generated metrics, because many files mix both.

Largest files by raw physical lines:

| File | Lines | Why it is difficult |
|---|---:|---|
| `Graphics/BaseGame.cs` | 1,663 | lifecycle, states, drawing helpers, UI batches, device setup |
| `Tracks/Track.cs` | 1,475 | XML data, road geometry, track position and collision |
| `Landscapes/Landscape.cs` | 1,347 | height mesh, material/model population, visibility/collision |
| `GameLogic/CarPhysics.cs` | 1,257 | custom frame-sensitive vehicle simulation |
| `Helpers/Input.cs` | 966 | keyboard/mouse/gamepad state and action logic |
| `Graphics/UIRenderer.cs` | 944 | complete 2D presentation layer |
| `Shaders/ShaderEffect.cs` | 864 | common effect/material parameter orchestration |
| `Graphics/Model.cs` | 807 | mesh parts, materials, effects, transforms, queues |
| `Tracks/TrackLine.cs` | 693 | spline/road helper calculations |
| `Sounds/Sound.cs` | 552 | authored cue/category behavior |

### Assets and projects

**Fact:** the inspected content tree is approximately 273 MiB and contains:

| Format | Count | Role |
|---|---:|---|
| `.x` | 57 | ASCII DirectX models |
| `.tga` | 129 | model, landscape, UI and material textures |
| `.png` | 13 | UI/other textures |
| `.dds` | 2 | sky and normalization cube maps |
| `.fx` | 10 | custom DX9-era effects |
| `.wav` | 28 | source audio |
| `.xap` | 1 | XACT authoring project |
| `.Track` | 3 | XML track definitions |
| `.CombiModel` | 10 | XML object/model transforms |
| `.data` | 1 | 257×257 byte height field |
| `.contentproj` | 2 | XNA 4 active and older/legacy content definitions |
| `.csproj` | 3 | runtime, content pipeline extension, related project material |

No generated `.xnb`, `.xgs`, `.xsb`, `.xwb`, executable, or DLL output is present.

### Inheritance and ownership shape

- `BaseGame : Microsoft.Xna.Framework.Game`; `RacingGameManager : BaseGame`.
- `BasePlayer -> CarPhysics -> ChaseCamera -> Player` combines simulation and presentation concerns in one chain.
- `Track : TrackLine, IDisposable` owns generated road/collision/render data.
- `ShaderEffect` is the base for `PostScreenMenu`; `PostScreenGlow` derives from that. `ShadowMapBlur`, `ShadowMapShader`, and `PreScreenSkyCubeMapping` also derive from `ShaderEffect`.
- `RenderToTexture : Texture` wraps render-target allocation.
- Ten screen classes implement `IGameScreen`.
- `ScreenshotCapturer : GameComponent`.
- `RacingGameModelProcessor : ModelProcessor`.
- `TangentVertex : IVertexType` is the main custom vertex type.

**Fact:** the runtime is static-heavy. `RacingGameManager` exposes global resources/state, and a background `Thread` loads resources, including objects that create GPU resources. C++ should not reproduce that ownership or cross-thread graphics behavior literally.

## Runtime architecture

### Game and screen lifecycle

`Program` starts `RacingGameManager`, catches `NoSuitableGraphicsDeviceException`/`NotSupportedException`, and displays Windows Forms message boxes. The manager installs `GamerServicesComponent`, owns the current `IGameScreen`, initializes static resources, and starts a loader thread. Screens cover loading, splash, main menu, car selection, track selection, options, help, highscores, and the race.

`BaseGame` disables a fixed time step. It maintains two SpriteBatches, begins them before delegating to the current screen, permits 3D drawing in the screen, and ends the batches afterward. Older CNA documentation reports a SpriteBatch-before-3D defect; current integration includes a regression example stating that issue has been fixed. This must be rechecked on the pinned build, not treated as an open blocker.

### Race, physics, and replay

The game implements its own vehicle dynamics rather than using a physics library. `CarPhysics` integrates acceleration, braking, gears, friction, steering, orientation, and velocity; `Player` couples this to input and race state. `Track`/`TrackLine` provide spline helpers, road widths, checkpoints, guard rails, and nearest-track calculations. `ChaseCamera` interpolates the view behind the car.

A race is three laps with checkpoints and high scores. Replay/ghost data samples car matrices at 0.2-second intervals and interpolates them with `Matrix.Lerp`. Because simulation uses variable frame time and several hand-tuned thresholds, translation correctness should be measured through telemetry, not feel alone.

**Fact/source-quality caveats:**

- replay storage appears to leave `replayFileFound` false after a successful load, causing fallback/default samples to be appended;
- mouse movement detection checks only a positive delta in one path;
- some XNA 3→4 blend-state conversion code initializes alpha factors without fully spelling out color blending;
- alpha-test calls are commented out in the XNA 4 conversion;
- screenshot path construction appears to mix a `Screenshots\` subpath with container handling inconsistently.

These make the source an imperfect oracle. Future work must decide whether to preserve observable behavior or correct an apparent conversion bug and document the choice.

### Landscape and track data

`LandscapeHeights.data` is exactly 66,049 bytes, matching a 257×257 unsigned-byte height grid. The generated terrain has 66,049 tangent vertices and 393,216 32-bit indices (`256 × 256 × 6`). The three `.Track` XML files store spline/control points, widths, road helpers, neutral objects, and related race data. Ten `.CombiModel` XML files store a model name and full transform matrix for grouped scenery.

Landscape code dynamically maps names and aliases and loads broad model sets. Examples include OilWell→OilPump, PalmSmall/AlphaPalm4→AlphaPalmSmall, Casino→Casino01, and Combi→CombiPalms. Linux packaging therefore needs deterministic case and separator normalization rather than assuming Windows asset lookup behavior.

## Actual XNA API surface

The detailed member-level mapping is in [`racing_api_matrix.md`](racing_api_matrix.md). The important actual namespaces are core Game/math, Graphics, Content, Input, Audio/XACT, Storage, and limited GamerServices. Referenced Video, Avatar, and networking assemblies are not used by game code and should not influence scope. No actual LINQ query use was found despite framework references.

Critical API groups include:

- `Game`, `GameTime`, `GameComponent`, services and `GraphicsDeviceManager`;
- vectors/matrices/quaternions/planes/rays/bounds and `MathHelper`;
- `GraphicsDevice`, viewport, presentation settings, clears and render-target switching;
- `BlendState`, `DepthStencilState`, `RasterizerState`, and per-slot `SamplerState`;
- `RenderTarget2D`, Texture2D, TextureCube, SpriteFont and SpriteBatch;
- vertex/index buffers, 16/32-bit indices, declarations/elements, `DrawIndexedPrimitives` and `DrawUserPrimitives`;
- `Model`, bones, meshes, mesh parts, bounds, effect replacement and absolute transforms;
- `Effect`, named parameters, techniques, passes and pass application;
- keyboard, mouse recentering/buttons/wheel, and one gamepad;
- XACT `AudioEngine`, `WaveBank`, `SoundBank`, `AudioCategory`, and `Cue`;
- `StorageDevice`/`StorageContainer` Begin/End APIs and stream serialization;
- `TitleContainer.OpenStream` for raw XML/binary data.

## Rendering architecture

### Gameplay frame order

The gameplay screen's high-level order is:

1. prepare and render the shadow map;
2. bind the full-resolution scene/glow target;
3. render the sky cube and SpriteBatch lens flares;
4. render landscape and track scenery;
5. enqueue the car;
6. flush shared model/mesh queues;
7. render the ghost/replay car;
8. project/use the blurred shadow overlay;
9. run the glow/post-processing chain and compose to the backbuffer;
10. update audio state;
11. render HUD/UI; `BaseGame` finally ends its outer SpriteBatches.

Car selection uses a smaller version of the shadow and menu post-processing path. Screens reuse full-, half-, and quarter-size targets. Shadow maps are 1024 or 2048 pixels. Depth is encoded into a color target, so the game does not require sampling a hardware depth texture.

### Passes and visible elements

- **Shadow:** generate manual depth, blur horizontally/vertically at reduced size, sample approximately ten taps while applying the car shadow.
- **Menu post effect:** full scene → quarter-size/downsample → two blur passes → composition; four authored passes.
- **Gameplay glow:** radial/full-scene processing, downsample, two blur passes, composition; five authored passes. Pass 0 uses a 10×10 indexed screen grid.
- **Sky:** cube model with cube-map sampling.
- **Lens flare:** additive SpriteBatch sprites; hardware occlusion-query code is commented out, so no occlusion query is required.
- **Landscape:** generated tangent mesh, 32-bit indices, normal/detail mapping and queued scenery models.
- **Cars/models:** shared mesh render manager, per-part techniques/materials, cube reflections and custom bone/pivot use.
- **Transparent content:** authored Alpha models render with no culling and shader alpha; source alpha-test behavior is ambiguous because conversion-era calls are commented.
- **UI:** two SpriteBatch layers for text/icons/background/overlays.

### Custom vertex formats

The principal `TangentVertex` has stride 44:

| Offset | Semantic | Format |
|---:|---|---|
| 0 | Position0 | `Vector3` |
| 12 | TextureCoordinate0 | `Vector2` |
| 20 | Normal0 | `Vector3` |
| 32 | Tangent0 | `Vector3` |

The terrain uses the same layout. Other paths use `VertexPositionTexture`, a 10×10 indexed post grid, and `VertexPositionColor` debug/track lines. Current OpenGL/EasyGL code and tests support generic declarations; an exact 44-byte/large-32-bit-index live proof is still required.

### Custom effects

| Effect source | Approx. lines | Purpose and inputs | Techniques/passes | Main compatibility concern |
|---|---:|---|---|---|
| `Shaders/NormalMapping.fx` | 1,107 | diffuse/normal maps, reflection and normalization cubes, lighting/specular/car hue | 15 one-pass variants, including `*20`, road, transparent and car variants | many material variants; old semantics/profiles; cube and tangent basis |
| `Shaders/LandscapeNormalMapping.fx` | 822 | terrain diffuse/normal/detail textures, lighting | 11 one-pass variants | 32-bit terrain, detail sampling, tangent parity |
| `Shaders/LightingShader.fx` | 286 | diffuse/specular per-pixel and shadow-car color | 6 one-pass variants | parameter/state equivalence |
| `Shaders/LineRendering.fx` | 69 | colored 2D/3D lines | two techniques | simplest early shader proof |
| `Shaders/ReflectionSimpleGlass.fx` | 251 | glass/reflection cube | two techniques | alpha/blend/cull and cube coordinate fidelity |
| `Shaders/PreScreenSkyCubeMapping.fx` | 75 | sky cube | one pass | cube face orientation and depth/cull state |
| `Shaders/ShadowMap.fx` | 418 | shadow depth generation and use, PCF | generation/use plus `20` variants | color-encoded depth precision and bias |
| `Shaders/PostScreenShadowBlur.fx` | 308 | separable blur | two passes | texel offsets, RT sampling/origin |
| `Shaders/PostScreenMenu.fx` | 664 | menu downsample/blur/compose | four passes | explicit pass graph, precision/blending |
| `Shaders/PostScreenGlow.fx` | 738 | gameplay glow/radial/downsample/blur/compose | five passes | explicit pass graph, 10×10 grid, precision |

All effects are old DirectX HLSL using shader model 1.1/2.0 declarations and FX technique/pass state. Runtime wrappers explicitly choose the `20` techniques and depend on names, pass order/count, parameter names, textures/samplers, and authored state. Automatic source translation is not a safe plan. Each visual formula and state must be represented explicitly and checked against captures.

## Content pipeline analysis

### Active content project

The XNA 4 runtime project references the active `RacingGameContent.contentproj`, not merely the older `ContentWindows` definition. It declares 238 compile items:

| Importer → processor | Items |
|---|---:|
| `XactImporter` → `XactProcessor` | 1 |
| `WavImporter` → `SoundEffectProcessor` | 28 |
| `TextureImporter` → `TextureProcessor` | 142 |
| `XImporter` → `RacingGameModelProcessor` | 56 |
| `XImporter` → stock `ModelProcessor` (`Cube`) | 1 |
| `EffectImporter` → `EffectProcessor` | 10 |

All 56 custom-processed models request tangent generation and disable vertex-color and texture-alpha premultiplication. Five models request `DefaultEffect=AlphaTestEffect`: `AlphaDeadTree`, `AlphaPalm`, `AlphaPalm2`, `AlphaPalm3`, and `AlphaPalmSmall`. Four textures disable premultiplied alpha: `NormalizeCubeMap`, `RacerCar`, `RacerCar2`, and `RacerCar3`; the three RacerCar textures additionally use `TextureFormat=NoChange`.

The `.Track`, `.CombiModel`, and `LandscapeHeights.data` items are not processed object content: they are copied as raw files and opened at runtime through `TitleContainer`, XML deserialization, and byte-stream reads. Any replacement pipeline must preserve that distinction or deliberately replace it with an explicitly versioned game format.

### What `RacingGameModelProcessor` actually does

**Fact:** the custom processor is small and does exactly these relevant transformations:

1. calls tangent-frame generation from `TextureCoordinate0` into `Tangent0`; it does not generate/store a binormal;
2. assigns a missing node name from its parent;
3. reads each geometry's material `OpaqueData["technique"]` integer and appends it to the mesh name;
4. delegates to the stock `ModelProcessor`.

It does **not** populate a custom `Model.Tag`. A comment mentions alpha, but no separate alpha metadata is written by this processor.

Runtime code relies on more than geometry:

- the complete bone hierarchy and `CopyAbsoluteBoneTransformsTo`;
- each mesh's parent-bone index;
- bounding-sphere radius;
- initial effect/material parameters embedded in each `.x` material;
- technique collection order/names and the processor-appended per-part technique code;
- car wheel/body hierarchy and named windmill blade meshes;
- stable mesh-part/effect identity for render batching.

The `.x` files contain 82 effect-material instances across the custom models. Common values include light direction, ambient/diffuse/specular colors, shininess, alpha, diffuse/normal/reflection/normalization textures, car/shadow colors, hue change, and technique. Observed technique integer values are 0, 1, 4, 6, and 8, with 4/6/8 dominant. The car specifically combines reflection glass and normal-mapped body/wheels, while runtime selects `SpecularWithReflectionForCar20` for the body path.

### CNA XNB reality

Current source and tests show:

- `ContentManager` attempts XNB and requires built-in XNB reader registration;
- uncompressed and LZX-compressed XNB are supported;
- common primitives/math, Color/DXT Texture2D, TextureCube, SoundEffect, SpriteFont, stock effects, vertex/index/declaration objects, and binary Model are implemented;
- binary Model reconstructs bones, mesh parents, bounds, buffers, declarations and supported effects;
- non-null model tags are rejected;
- general custom compiled `EffectReader` and `Effect(effectCode)` are intentionally unsupported.

This means stock/simple XNB content can be useful as an oracle or selective runtime path, but original custom-model XNB output is not a complete solution.

### Strategy comparison

| Strategy | Advantages | Problems | Recommendation |
|---|---|---|---|
| Preserve original XNA XNB output | closest stock processor semantics; CNA already reads meaningful subsets | no built outputs; legacy Windows XNA toolchain required; custom effects block model graph; no CNA producer; weak portability | Keep as an oracle/experiment, not primary deployment format |
| Load `.x` directly at runtime | avoids generated assets | CNA has no verified direct `.x` route; parsing/triangulation/tangents/materials at startup; hard to make deterministic | Reject for first port |
| Convert `.x` to ordinary glTF and use CNA runtime glTF | leverages a capable existing importer and PBR/static/skinned work | current import simplifies unskinned node graph/instances; stock materials; loses Racing technique/effect metadata and may lose wheel/windmill pivots | Useful geometry experiment only, not sufficient by itself |
| Racing-specific offline conversion/package | can preserve exact hierarchy, names, bounds, pivots, material fields and technique IDs; deterministic and testable; shaders remain explicit | new game-specific tool/schema; tangent and material validation effort; licensing of generated output still matters | **Recommended primary strategy** |
| Extend generic CNA content pipeline/importers | potentially benefits other games | broad scope, delays visible proof, risks solving speculative cases | Do only for minimized reusable gaps discovered by the package proof |

The prospective package should contain versioned geometry/index buffers, the exact 44-byte tangent attributes or an explicitly versioned equivalent, node hierarchy/local transforms, mesh-part ranges, bounds, material records, texture/cube references, technique enum, and provenance/source checksum. It should never silently flatten nodes or infer material behavior at runtime.

## Audio analysis

### Actual authored behavior

XACT is genuinely used at runtime. `Sounds/Sound.cs` creates:

- `AudioEngine` from `Content\Audio\RacingGameManager.xgs`;
- `WaveBank` from `Wave Bank.xwb`;
- `SoundBank` from `Sound Bank.xsb`.

The source tree contains `Content/Audio/RacingGame.xap` and WAVs but no compiled banks. The XAP declares Windows outputs below `Win\`, while runtime resolves the three bank filenames directly below `Content\Audio`; the actual XNA content-build copy/flattening behavior must therefore be reproduced with a real XACT/content build rather than guessed.

Authored categories include Global, Default, Music, and Gears. Music and Gears have maximum-instance/replacement policies; Music includes a 500 ms crossfade. Cues cover UI effects, two game-music alternatives, menu music, gear loops/transitions, braking, crashes, checkpoints, and win/loss behavior. Music loops are authored up to 255; “new variation on loop” is disabled. The game replaces/starts/stops cues based on RPM, gear, braking and collision timers.

A cue-scoped `Pitch` variable is authored from -12 to +12 and drives an RPC from -1200 to +1200 cents. Game code passes a much wider computed range and therefore depends on authored clamping. PC output uses PCM/MS-ADPCM sources with 128/256 block sizes for relevant assets. No `Apply3D` or positional audio is used.

### CNA comparison

CNA's current XACT code is not a stub. It implements the core engine/bank/cue/category model, cue variables, RPC curves, loops/random variations, instance limits/behaviors, crossfade/stop handling, and PCM/MS-ADPCM. Focused integration tests for these areas passed. Unsupported XMA/WMA paths do not affect the authored Windows PC build. A known variation-reselection limitation does not appear relevant because this project disables new variation per loop. No evidence showed a required DSP/reverb path.

**Classification: LIKELY READY, not READY.** Genuine compiled Racing banks are absent, so exact parsing, cue transitions, pitch clamping, crossfades, durations, and long-loop behavior are unproven. The best next evidence is an authentic `.xgs/.xsb/.xwb` probe plus cue-state and decoded-output comparisons. If those banks cannot be produced or redistributed, an explicit raw-WAV mixer is a viable port-side fallback but increases fidelity effort; it should not be mistaken for proof that CNA XACT is inadequate.

## C# to C++ translation risks

### Patterns present

- Delegates/events and anonymous delegates: load completion, callbacks and screen behavior. Use CNA `EventHandler` where API compatibility matters and `std::function` for game-owned callbacks.
- Properties/indexers and collection-heavy code: use clear accessors and STL containers rather than mechanical macro emulation.
- `IDisposable`, `using`, `try/finally`, finalizers and `GC.SuppressFinalize`: replace with RAII and explicit GPU/audio shutdown order.
- Static fields/singletons: high risk for initialization/destruction order and hidden ownership. Centralize into an application/context object.
- C# reference identity: model mesh parts/effects are dictionary keys and shared across queues. Use stable asset IDs/handles or stable owning allocations; do not retain pointers into moving vectors.
- Serialization attributes/`XmlSerializer`: replace with explicit schema-aware XML readers/writers.
- `Thread`, `ThreadPool`, `ManualResetEvent`: use standard C++ concurrency, but create/upload/destroy graphics objects on the render thread.
- Exceptions: retain at application/content boundaries; ordinary update/draw behavior should not depend on exceptions.
- `ICloneable`/memberwise replay copying: implement an explicit deep copy with tests.

### Patterns not found as meaningful port risks

No runtime reliance was found on LINQ queries, reflection-driven gameplay, unsafe code, P/Invoke, `dynamic`, iterator/yield machinery, async/await, covariance/contravariance tricks, or a third-party physics library. Generics are ordinary framework collections and loaders. This keeps the language conversion manageable once rendering/content contracts are decided.

Sharp Runtime can provide familiar System-style types where CNA APIs expose them, but game-owned logic should prefer idiomatic C++ when that makes lifetime, ownership, and error behavior clearer.

## Platform dependencies

| Dependency | Actual use | Classification and action |
|---|---|---|
| Windows Forms / `[STAThread]` | error message boxes and hiding a `Form` obtained from the game window handle | Active Windows-only integration; replace with CNA/SDL window handling and logging |
| GamerServices | component plus `Guide.IsVisible` coordination around storage UI | Limited; desktop port can encapsulate/remove overlay behavior while keeping storage |
| Storage selector/container | settings, highscores, replay and screenshots | Active; CNA implementation appears sufficient but needs round-trip test |
| Filesystem/path conventions | backslashes, dynamic asset names/case, screenshot subdirectory | Active portability work; normalize and version paths |
| XACT tooling | `.xap` exists, generated banks do not | Active build/provenance dependency; obtain legal/reproducible bank generation route |
| Graphics adapter/fullscreen | resolution list, fullscreen toggle, HiDef profile, x86 Windows project | Active; target desktop programmable backend and validate mode changes |
| Mouse recenter/hide/confine | camera/UI input behavior | Active; test window focus/relative input on modern Linux/Windows |
| Xbox branches | conditional storage/display/input leftovers | Mostly compile-time legacy; omit unless behavior affects Windows path |
| Screenshot JPEG | backbuffer readback into container path | Active optional feature; test late and correct path logic |
| Registry | none found | Not a dependency |

## CNA compatibility summary

The full matrix is separate because the actual surface is large. The highest-value classification is:

| Requirement | Status | Evidence/consequence |
|---|---|---|
| Core game/math/lifecycle | LIKELY READY | implemented and broadly tested; physics needs differential telemetry |
| Keyboard/mouse/gamepad | LIKELY READY | current implementations/tests; manual focus/recenter/controller test needed |
| SpriteBatch/UI/texture loading | LIKELY READY | implemented; exact game layering/premultiplication needs captures |
| Generic custom vertex declarations and 32-bit indices | BACKEND-SPECIFIC / LIKELY READY on OPENGL33 | current EasyGL generic layout code/test; exact 44-byte terrain proof pending |
| Render targets and state objects | PARTIAL | ordinary paths exist; `Rgba64` query/construction is inconsistent |
| Binary XNB Model | PARTIAL | real and tested; custom effect dependency blocks original custom-model output |
| Original custom compiled Effects/XNB | BLOCKER for direct-XNB route | intentional throwing implementation |
| Portable custom shader source | BACKEND-SPECIFIC | `ShaderEffect` strong on OPENGL33; technique/pass graph must be game-owned |
| Legacy `.x` plus custom processor semantics | CONTENT-PIPELINE GAP | no direct complete route; offline Racing package recommended |
| XACT behavior | LIKELY READY | substantial implementation/tests; genuine game banks absent |
| Storage | LIKELY READY | API exists; game-shaped round trip pending |
| Multiple graphics backends | PARTIAL | OpenGL33 is credible; Vulkan/Bgfx and 2D backends are not initial targets |

Concrete CNA shortcomings and file locations are kept in [`missing.md`](missing.md) so they can be re-audited independently.

## Graphics backend recommendation

### Primary reference backend: `OPENGL33`

Use public CNA selector `CNA_GRAPHICS_BACKEND=OPENGL33`, implemented internally by EasyGL. It currently offers the strongest evidence for the exact needed combination: desktop programmable 3D, GLSL `ShaderEffect`, named values and 2D/cube texture bindings, generic custom vertex layouts, render targets, SpriteBatch/3D ordering fixes, and focused bloom/custom-shader examples. Linux also matches the available audit environment.

The current repository's root `CMakeLists.txt` still defaults to the obsolete public name `EASYGL` and derives an obsolete `easy-gl` target. That is a future build-configuration update, not evidence of missing CNA functionality, and it was deliberately not changed in this planning session.

### Other backend tiers

- **Expected only after shader variants and validation:** D3D11/D3D12. D3D9 may be a useful Windows/XNA-era visual oracle, but it does not bypass shared resource-format issues.
- **Potential later:** `OPENGLES`, OpenGL4, Sokol, Diligent, SDL_GPU, WebGPU, LLGL and other newly integrated 3D paths. Make no promise before running the reference render corpus.
- **Needs significant backend fixes/tooling:** Vulkan's current custom-effect binding ABI is too constrained; Bgfx expects precompiled backend shaders rather than compiling the Racing sources through the present path.
- **Unsuitable:** SDL_Renderer, Direct2D, Canvas, HTML DOM, Skia, ASCII, GDI and other 2D renderers, plus fixed-function paths such as OpenGL ES 1. They cannot faithfully implement this programmable multipass 3D game.

The first release target should be one correct `OPENGL33` build, not simultaneous backend parity.

## Licensing and provenance review

This is a preliminary technical provenance review, not legal advice.

Observed facts:

- the XNA 4 conversion repository's README says it brings an original exDream.com Racing Game project to XNA 4 and fixes bugs;
- no `LICENSE`, `NOTICE`, or `COPYING` file was found in that source root;
- many code headers identify the Microsoft XNA Community Game Platform and state “Copyright Microsoft Corporation. All rights reserved”;
- the original HTML documentation states “© 2008 Microsoft Corporation. All rights reserved”;
- an assembly metadata file contains a 2010 copyright line without a clear licensor;
- explicit redistribution terms for the models, textures, music, WAVs, XAP project, tracks, and conversion changes were not located;
- this `cna-samplesracing` repository's general `LICENSE`/`NOTICE` claims cannot automatically establish rights for a separately obtained conversion and its assets.

Unresolved before public redistribution:

1. identify the exact original Microsoft/exDream release and its governing license/EULA;
2. identify the XNA 4 conversion author and license for their changes;
3. establish asset-by-asset redistribution and derivative/conversion rights, especially music, vehicle/scenery art, and generated banks;
4. determine required notices and whether converted/generated packages may be hosted;
5. record provenance/checksums for every copied or transformed source asset.

Public availability on GitHub is not equivalent to permission to redistribute. Until resolved, implementation can be structured to require users to provide/build assets locally, but even that approach should receive appropriate legal review before publication.

## Feasibility verdict

**FEASIBLE WITH SIGNIFICANT CNA WORK — medium confidence.**

Why not simply FEASIBLE: one confirmed shared render-target contract defect exists; the original custom-effect XNB path is blocked; portable FX techniques/passes do not exist; some backend shader contracts are insufficient; and exact Racing XACT/model content has not been demonstrated. A faithful result will likely expose additional framework bugs.

Why not “not currently practical”: CNA already provides the core application, math, input, buffers, render targets, textures/cubes, SpriteBatch, storage, model graph, custom shader primitives, and XACT foundations. A narrow OpenGL33 route with explicit shaders and an offline package fits the architecture; no fundamental need to replace CNA was found.

Why confidence is medium rather than high:

- the active CNA integration is moving rapidly;
- no original compiled content or runnable XNA reference build is present;
- old model/shader semantics must be reconstructed from source assets;
- the game has not yet exercised CNA as one integrated workload;
- licensing may constrain the distributable result even if engineering succeeds.

## AI-assisted effort estimates

These are supervised engineering hours with Codex/Claude Code assistance, not unattended wall-clock time. They exclude legal review, acquiring proprietary tools, and hardware procurement. Optimistic assumes a stable CNA baseline and usable reference assets; pessimistic assumes subtle visual/content/backend defects. Avoid interpreting the figures as commitments.

### Work packages

Rows 2–10, 15 and 17 are approximately additive but will overlap in practice. Row 16 summarizes CNA work already distributed across those rows and must **not** be added again. Row 18 is per additional backend and excluded from first-backend totals.

| # | Work package | Racing port/tooling O / R / P | CNA framework O / R / P | Notes |
|---:|---|---:|---:|---|
| 1 | Analysis completed by this task | 35 / 50 / 75 total analysis | — | Human-equivalent audit effort; no implementation/modification |
| 2 | Project skeleton/build conversion | 10 / 16 / 28 | 0 / 2 / 8 | pin integration, OPENGL33, diagnostics, test harness |
| 3 | Mechanical C#→C++ translation | 55 / 95 / 170 | 0 / 0 / 0 | declarations/control flow only; excludes correctness work |
| 4 | Core game logic | 40 / 75 / 140 | 0 / 5 / 20 | race/replay/camera/physics parity and math findings |
| 5 | Graphics/render architecture | 55 / 100 / 190 | 10 / 30 / 90 | buffers, states, queues, targets, model binding |
| 6 | Shaders/effects | 80 / 145 / 280 | 15 / 45 / 150 | ten FX semantics, explicit passes, shader backend defects |
| 7 | Content conversion/pipeline | 70 / 130 / 250 | 10 / 35 / 120 | converter, package schema, hierarchy/material/tangent tests |
| 8 | Audio | 20 / 45 / 100 | 0 / 20 / 80 | authentic banks, cue/RPC/category validation or fallback |
| 9 | Menus/UI/input | 30 / 55 / 110 | 0 / 5 / 25 | screens, SpriteBatch, font and device/focus behavior |
| 10 | Platform/persistence cleanup | 15 / 35 / 70 | 0 / 10 / 35 | settings, replay, screenshots, paths, WinForms removal |
| 15 | Visual/audio fidelity work | 50 / 110 / 230 | 5 / 20 / 80 | image/audio comparison, tuning, source ambiguities |
| 16 | CNA fixes discovered by the port | — | 40 / 120 / 400 | aggregate/overlapping envelope, not an extra sum |
| 17 | Testing/regression work | 35 / 85 / 180 | 15 / 40 / 140 | game tests plus minimized CNA tests |
| 18 | Each additional graphics backend | 35 / 90 / 220 | 30 / 120 / 400 | shader variants and backend fixes; excluded from total |

### Observable checkpoint estimates

These are cumulative engineering hours after implementation starts and include both game and CNA work. They overlap the work packages above and are not additive.

| # | Checkpoint | Optimistic | Realistic | Pessimistic |
|---:|---|---:|---:|---:|
| 11 | First correctly rendered representative scene | 150 | 320 | 700 |
| 12 | First drivable car on a track | 240 | 500 | 1,050 |
| 13 | First complete race loop | 340 | 680 | 1,400 |
| 14 | Feature-complete reference-backend game | 500 | 950 | 2,000 |

### Planning totals for one reference backend

| Scenario | Completed/repeated analysis | Racing port/tooling | CNA framework | Total |
|---|---:|---:|---:|---:|
| Optimistic | 30–45 | 430–520 | 40–90 | 500–655 |
| Realistic | 40–60 | 750–950 | 140–240 | **930–1,250** |
| Pessimistic | 60–85 | 1,500–2,100 | 450–750 | 2,010–2,935 |

Use approximately **1,100 hours** as the central realistic planning number. Parallel AI generation can shorten calendar time, but review, reference capture, debugging, and integration gates remain human-supervised.

## Testing strategy summary

Game-level tests should include:

- obtain/run the original XNA 4 Windows build if possible without repairing it; record tool/runtime versions;
- deterministic camera transforms and scene screenshots at fixed resolutions;
- captures of every intermediate render target for shadow/menu/glow passes;
- representative material/model captures: car body/glass/wheels, road, alpha palm, windmill, sky cube;
- fixed input or replay sequences and per-frame car telemetry (position, basis, velocity, RPM, gear, checkpoint/lap);
- track nearest-position/collision/checkpoint fixtures from raw XML and height data;
- XACT cue-state logs plus decoded/mixed audio comparisons at known RPM/brake/crash transitions;
- settings/high-score/replay round trips and screenshot verification;
- ASAN/UBSAN, leak/lifetime tests, and debug validation layers where applicable;
- after OpenGL33 passes, compare identical scenes on one backend at a time.

CNA-level tests should be added only for minimized genuine framework defects: query-then-create render target, exact custom layout, effect binding, XACT bank edge case, storage behavior, or backend state transition. Game-specific shader formulas and asset schema belong in the Racing repository tests, not CNA.

## Unresolved questions and decisive experiments

1. **Which CNA commit will be the implementation baseline after all 21 branches integrate?** Pin it, diff cited files, rerun focused tests.
2. **Can an authentic original XNA build be run?** If yes, capture reference frames, intermediate targets, physics telemetry, and audio events before code changes.
3. **Can genuine Windows XACT banks be generated and redistributed?** Build the `.xap`, record tool/version/output names, and load all banks/cues in CNA.
4. **Can a converter preserve car/windmill node transforms from `.x`?** Prove one car, one windmill, one alpha model and one stock cube before batch conversion.
5. **What tangent basis did the XNA processor emit?** Compare a normal-mapped reference under controlled light/camera; do not infer from format alone.
6. **Is `Color` precision visually adequate for initial post-processing?** Capture identical menu/glow/shadow chains before deciding whether `Rgba64` is release-critical.
7. **Does the exact 44-byte layout plus 393,216 32-bit indices render correctly on OPENGL33?** Make this an early focused test.
8. **What are the legal terms for code, conversion changes, and each asset class?** Resolve before copying or publishing assets.
9. **Which original-source defects should be fixed versus preserved?** Record decisions for replay loading, input delta, alpha testing/blending and screenshot paths.
10. **Are all material parameter defaults embedded in `.x` reproducible?** Export a complete machine-readable inventory and assert no material/technique is silently dropped.

The future implementation plan, with small validation gates and explicit non-goals, is in [`plan_racing.md`](plan_racing.md).
