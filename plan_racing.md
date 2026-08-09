# Future implementation plan: XNA 4 Racing Game Kit on CNA

## Purpose

This is the execution plan for a future session. It deliberately contains no translated C++ and no converted assets. Its evidence base is [`racing_feasibility.md`](racing_feasibility.md), the detailed [`racing_api_matrix.md`](racing_api_matrix.md), and the current framework issues in [`missing.md`](missing.md).

The plan assumes one correct Linux desktop reference implementation on CNA `OPENGL33` before any additional backend. It must be rebased onto a pinned, stable commit after the active CNA post-audit integration finishes.

## Goals

1. Produce a maintainable C++23 port with behavior and presentation traceable to the XNA 4 source.
2. Reach a complete three-lap race with all original screens, cars, tracks, ghost/replay, settings/high scores, UI, audio, shadows and post-processing.
3. Preserve important source-asset semantics: node hierarchy/pivots, material values, per-part technique selection, tangents, bounds, track XML and height data.
4. Use CNA's XNA-style public API where it is correct and useful, while using explicit idiomatic C++ ownership for game code.
5. Distinguish game adaptations from generalized CNA defects; contribute minimized framework fixes/tests only after reproduction.
6. Build an objective reference corpus for physics, content, rendering, audio and persistence.

## Non-goals for the first implementation

- Do not make every CNA backend work simultaneously.
- Do not implement a general XNA Content Pipeline, general `.x` importer, or general DX9 FX runtime merely to start the game.
- Do not preserve original custom-effect XNB as the primary deployment format.
- Do not mechanically reproduce garbage-collector ownership, global statics, background GPU creation, Windows Forms, or GamerServices UI.
- Do not redesign the physics, controls, game rules or visual style before a measurable reference exists.
- Do not copy/redistribute source assets until provenance and redistribution terms are resolved.
- Do not fix unrelated CNA issues encountered while browsing; minimize and file only failures that block a current validation gate.
- Do not enable a second graphics backend until the reference backend passes the feature-complete regression corpus.

## Required preconditions

Before Phase 1 begins, the implementing session must:

1. choose a stable CNA integration commit after the 21 feature branches settle and record its full hash;
2. re-read CNA's then-current `AGENTS.md`/`CLAUDE.md`, backend matrix, XNB/content/model/audio documents and relevant tests;
3. diff the files named in `missing.md` against audit commit `4ac696c748fb`;
4. rerun the focused model/XNB/cube/XACT/custom-layout/render-target tests;
5. record current dirty state of this repository and CNA, preserving unrelated user work;
6. decide whether implementation can use the original assets locally under a research-only workflow while the public redistribution question is resolved;
7. check for an obtainable/runnable original XNA build and authentic XACT build tool/output.

A failed precondition is useful evidence. Do not repair the original C# project as a side quest.

## Architecture decisions

### Baseline and backend

- First public CNA selector: `CNA_GRAPHICS_BACKEND=OPENGL33` (internally EasyGL).
- Pin one CNA commit in build documentation and CI; do not track a moving integration worktree silently.
- Update this repository's stale `EASYGL`/`easy-gl` CMake assumptions as part of Phase 1 only. Use current CNA-exported target conventions rather than deriving obsolete target names.
- Keep backend-specific shader files behind a small renderer boundary. Do not introduce a universal abstraction beyond needs already demonstrated by Racing.

### Separation of simulation and presentation

- `RaceSession` owns race state, player, car physics, checkpoints, laps, ghost and timing.
- `World` owns parsed track/height/scenery instances, but not GPU resources.
- `RacingRenderer` consumes immutable world/render descriptions plus frame state. It owns render passes/targets and queues but not gameplay objects.
- `AudioDirector` consumes telemetry/events rather than reading arbitrary globals.
- Screens own navigation/UI state and invoke explicit application services.
- Preserve the source's variable-step runtime behavior initially; add a fixed-delta deterministic test path. Do not silently switch the shipping physics to a fixed step until telemetry proves the effect acceptable.

### Ownership and lifetime

Use RAII and explicit destruction ordering:

1. `RacingGameApp` (the CNA `Game`) uniquely owns the graphics manager, application services, screen coordinator and asset registry.
2. `RaceSession` uniquely owns mutable per-race gameplay objects.
3. `AssetRegistry` owns immutable CPU asset records and GPU resources. Expose stable typed handles/IDs; use `shared_ptr<const Asset>` only where true shared lifetime is necessary.
4. Render queues store stable handles plus per-instance transforms, never owning raw pointers into movable vectors and never extending asset lifetime accidentally.
5. Screens use `unique_ptr` or values and explicit transitions; they do not form shared cycles.
6. GPU creation, upload, render-target resize and destruction occur on the render/main thread. Worker jobs may read, parse or decompress CPU data and then submit an upload description.
7. Audio cues are RAII wrappers and are stopped/disposed before banks, banks before `AudioEngine`.
8. Render passes/targets die before the graphics device/application host.
9. Avoid namespace-level mutable state. A narrow read-only `GameServices` reference may be passed where constructor injection becomes noisy.

### Error model

- Content/package errors include asset ID, source checksum, schema version and field context and fail before a race starts.
- Shader compilation/binding errors include program, stage, parameter and backend.
- Runtime gameplay code avoids exception-driven control flow.
- A missing optional asset may show a diagnostic placeholder only in development; release validation treats it as failure.

## Proposed future directory/module structure

No part of this tree is created by this planning session. The future structure should be close to:

```text
samples/RacingGame/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── App/
│   │   ├── Program.cpp
│   │   ├── RacingGameApp.{hpp,cpp}
│   │   └── GameServices.hpp
│   ├── Core/
│   │   ├── AssetId.hpp
│   │   ├── Diagnostics.{hpp,cpp}
│   │   └── TimeControl.{hpp,cpp}
│   ├── Content/
│   │   ├── AssetRegistry.{hpp,cpp}
│   │   ├── RacingPackage.{hpp,cpp}
│   │   ├── TrackReader.{hpp,cpp}
│   │   ├── CombiModelReader.{hpp,cpp}
│   │   └── HeightFieldReader.{hpp,cpp}
│   ├── Gameplay/
│   │   ├── RaceSession.{hpp,cpp}
│   │   ├── CarPhysics.{hpp,cpp}
│   │   ├── Player.{hpp,cpp}
│   │   ├── Replay.{hpp,cpp}
│   │   └── RaceRules.{hpp,cpp}
│   ├── World/
│   │   ├── Track.{hpp,cpp}
│   │   ├── TrackLine.{hpp,cpp}
│   │   ├── Landscape.{hpp,cpp}
│   │   └── Collision.{hpp,cpp}
│   ├── Camera/
│   │   └── ChaseCamera.{hpp,cpp}
│   ├── Rendering/
│   │   ├── RacingRenderer.{hpp,cpp}
│   │   ├── RenderGraph.{hpp,cpp}
│   │   ├── ModelAsset.{hpp,cpp}
│   │   ├── MeshQueue.{hpp,cpp}
│   │   ├── Material.hpp
│   │   ├── Passes/
│   │   │   ├── ScenePass.*
│   │   │   ├── ShadowPass.*
│   │   │   ├── BlurPass.*
│   │   │   ├── GlowPass.*
│   │   │   └── CompositePass.*
│   │   └── Shaders/
│   │       ├── ShaderBindings.*
│   │       └── opengl33/*.{vert,frag}
│   ├── Audio/
│   │   └── AudioDirector.{hpp,cpp}
│   ├── UI/
│   │   ├── ScreenCoordinator.*
│   │   ├── Screens/*.cpp
│   │   └── UiRenderer.*
│   └── Persistence/
│       ├── SettingsStore.*
│       ├── HighScoreStore.*
│       └── ReplayStore.*
├── tests/
│   ├── unit/
│   ├── fixtures/
│   └── reference/
└── generated/                 # build output or ignored local content, not presumed redistributable

tools/racing-content/          # deterministic offline converter, if repository convention permits
```

For a project of this size, normal `.hpp/.cpp` modules are preferable to forcing the smaller samples' header-only pattern.

## Content strategy

### Primary strategy: a deterministic Racing package

Create an offline game-specific conversion tool only after the Phase 2 proof defines the minimum semantics. It may use an existing `.x` parser, Assimp or glTF as an internal geometry bridge, but its output contract must be Racing-owned and testable.

Minimum package records:

- magic, schema version, converter version and source file checksum;
- coordinate-system/unit conversion declaration;
- node IDs, names, parent IDs and local transforms;
- mesh IDs, node attachment, vertex/index ranges and index width;
- position, UV0, normal and tangent attributes with explicit format/stride/handedness;
- mesh-part/material association;
- local/model bounds and processor-comparable bounding spheres;
- material type and Racing technique enum;
- ambient/diffuse/specular colors, shininess, alpha, car hue/shadow values;
- diffuse/normal/reflection/normalization texture IDs;
- flags for transparency, double-sided/cull behavior and any alpha-test policy;
- optional source diagnostics preserving original mesh/material names.

Do not encode technique semantics by appending digits to names in the new runtime format. Read that convention during conversion and store a typed field.

### Proof asset set

The converter proof must include, in order:

1. `Cube.x` or another stock/simple model to prove axes, winding and scale;
2. one car including body, glass and wheel pivots/hierarchy;
3. one windmill with an animated named blade mesh;
4. one Alpha foliage model for transparency/cull behavior;
5. one ordinary normal-mapped scenery model;
6. sky and normalization cubes;
7. representative premultiplied and explicitly non-premultiplied textures.

Batch conversion of all 57 models is forbidden until these pass hierarchy/material/tangent/image gates.

### Non-model data

- Parse `.Track` and `.CombiModel` using explicit versioned XML readers, not reflection/`XmlSerializer` emulation.
- Validate all required elements, counts, finite numbers and model IDs; reject unknown required schema rather than silently defaulting.
- Read the height field as exactly 257×257 bytes and assert size/checksum.
- Normalize logical asset IDs independently from platform paths. Keep a case-collision test for Linux.
- Generated assets should live in the build tree or an ignored local directory until licensing permits distribution.

### Selective XNB use

XNB may be retained selectively for a SpriteFont, stock Cube, Texture2D/cube or SoundEffect proof. Register CNA's built-in XNB readers once during bootstrap. Do not make the entire game depend on a legacy XNA build unless Phase 2 establishes that this is both reproducible and redistributable.

## Shader and rendering strategy

### Shader representation

- Treat the `.fx` files as behavioral specifications, not directly compilable assets.
- For `OPENGL33`, author explicit GLSL 3.3 vertex/fragment program pairs through CNA `ShaderEffect`.
- Preserve original parameter names in typed binding structures where practical, but never rely on runtime `EffectParameter` reflection.
- Represent technique choice with a C++ enum and explicit program/material variant. Consolidate variants only when generated shader output and reference images prove equivalence.
- Represent each multi-pass effect as a C++ pass graph with explicit input/output target, viewport, blend, depth, rasterizer and sampler state.
- Set all relevant state at each pass boundary; do not depend on FX state blocks or previous-pass leakage.
- Start with `SurfaceFormat::Color` only as a documented bootstrap fallback if the `Rgba64` contract remains broken. The fidelity gate decides whether CNA must gain high-precision targets.

Suggested program/pass decomposition:

- `Line2D`, `Line3D`;
- `SkyCube`;
- normal-mapped diffuse/specular/reflection/road/car/transparent material variants;
- landscape normal/detail material variants;
- glass/reflection;
- `ShadowGenerate`, `ShadowApply`;
- horizontal/vertical blur;
- menu downsample/blur/composite;
- gameplay glow/radial/downsample/blur/composite.

### Render graph and targets

`RacingRenderer` should expose named frame stages matching the audited order. A target pool owns size-dependent full/half/quarter targets and shadow targets. Resizing invalidates and recreates them on the render thread. Every stage supports development capture to a deterministic file name so intermediate comparison is routine rather than emergency debugging.

### State and geometry validation

Before real shaders proliferate, prove:

- exact 44-byte tangent declaration;
- 32-bit 393,216-index terrain-sized draw;
- 16-bit 10×10 screen grid;
- 2D/cube textures and all sampler modes used;
- separate color/alpha blend factors;
- depth write/read transitions and culling/front-face convention;
- full→quarter→blur→compose target sequence;
- SpriteBatch/3D layering;
- backbuffer readback orientation.

## Audio strategy

### Preferred route

Use CNA's XACT runtime with genuine PC `.xgs`, `.xsb` and `.xwb` outputs generated from `RacingGame.xap`. Record the XACT tool version, source checksums, output filenames and any compression settings. Build a standalone bank probe before integrating `Sound.cs` behavior.

The probe must enumerate/load every cue and verify:

- Music and Gears category maximum-instance/replacement behavior;
- 500 ms Music crossfade;
- `Pitch` variable clamp and RPC result at -12, 0 and +12 plus out-of-range game inputs;
- Gear1–Gear5 loops/transitions, brake/crash/checkpoint/win/loss cues;
- random choice of two game music tracks and fixed menu music;
- loop duration/termination and MS-ADPCM decode duration.

Then implement `AudioDirector` as a state machine driven by RPM, gear, braking, collision and screen/race events. It must not read arbitrary renderer/game globals.

### Fallback route

If authentic banks cannot be generated, used, or redistributed, pause and make an explicit product decision. A raw-WAV mixer can reproduce cue/category/RPC behavior game-side, but this is extra work and may diverge from authored compression/loop metadata. Do not silently switch strategies and do not implement a general XACT authoring pipeline in this repository.

## Persistence and platform strategy

- Use CNA's storage root/container behavior with explicit application name.
- Replace reflection-based settings XML with a small versioned schema and safe defaults.
- Define replay/high-score binary formats explicitly: magic/version, fixed endianness, count limits and corruption handling. Add a legacy reader only if original-file compatibility is required.
- Write temporary then replace where the platform API supports it; never leave a partially written settings file after interruption.
- Use portable path components and create the screenshot directory inside the actual storage root.
- Replace Windows Forms errors/window hiding with CNA/SDL window state and structured logging.
- Keep Xbox-only branches out unless a concrete desktop behavior depends on them.

## Staged implementation milestones

### Milestone 0 — Freeze evidence and legal/tooling gates

- **Objective:** create a reproducible implementation baseline without porting code.
- **Source areas:** project/content files, XAP, license/readme/header evidence.
- **CNA features:** version/build/test inventory only.
- **Visible result:** none; a signed-off baseline record with hashes and available reference artifacts.
- **Validation gate:** pinned CNA builds its own selected tests; exact original assets/tool outputs and redistribution status are recorded as available/unavailable.
- **Likely risks:** integration continues moving; no original executable; unclear asset rights.
- **Prerequisites:** completion of this audit.
- **Must not include:** C# translation, asset batch conversion, framework fixes.
- **Effort:** 6–12 hours.

### Milestone 1 — Empty CNA application and diagnostic harness

- **Objective:** one buildable/runnable `RacingGame` target on pinned `OPENGL33` with clean lifetime and capture/test hooks.
- **Source areas:** `Program.cs`, minimal lifecycle shape from `BaseGame.cs`/`RacingGameManager.cs`; no gameplay logic.
- **CNA features:** `Game`, `GraphicsDeviceManager`, clear/present, window resize/fullscreen smoke, keyboard exit, optional XNB reader registration, CMake backend linkage.
- **Visible result:** deterministic solid-color window whose backbuffer can be captured and whose build prints CNA/backend/asset-schema versions.
- **Validation gate:** debug and sanitizer builds; launch/resize/exit repeatedly; pixel-check clear color; no leaks/errors; CI command documented.
- **Likely risks:** this repository's stale `EASYGL` target assumptions and active integration target names.
- **Prerequisites:** Milestone 0.
- **Must not include:** models, Racing assets, shaders, audio, physics, screens beyond a diagnostic screen.
- **Effort:** 10–24 port hours; 0–8 CNA hours if a real baseline defect appears.

### Milestone 2 — Asset conversion and resource-format proof

- **Objective:** prove the proposed package can preserve the difficult semantics before scaling it.
- **Source areas:** `RacingGameModelProcessor.cs`, `Model.cs`, `CarModel.cs`, representative `.x`/textures/cubes/materials.
- **CNA features:** Texture2D/TextureCube, vertex/index buffers, declarations, basic/custom diagnostic shader, optional stock-model XNB reader.
- **Visible result:** one representative model with hierarchy/pivot debug axes, normals/tangents, texture and cube-map diagnostic modes.
- **Validation gate:** exact node/material inventory; car wheels and windmill pivots match source transforms; bounds/winding/UV/tangent handedness verified; deterministic package byte/checksum; alpha asset and both cubes tested.
- **Likely risks:** `.x` importer behavior, instance/node flattening, tangent mismatch, normalization cube format, licensing.
- **Prerequisites:** Milestone 1 and permission for local test assets.
- **Must not include:** batch conversion, final normal-mapping shader, track/world loading, gameplay.
- **Effort:** 55–110 port/tool hours; 5–30 CNA hours only for minimized resource defects.

### Milestone 3 — Shader/material foundation

- **Objective:** render the proof models with one faithful material route and explicit states.
- **Source areas:** `ShaderEffect.cs`, `LineRendering.fx`, `NormalMapping.fx`, `ReflectionSimpleGlass.fx`, relevant `.x` material values.
- **CNA features:** `ShaderEffect`, named values/textures/cubes, custom vertex layout, blend/depth/raster/sampler state.
- **Visible result:** controlled car/scenery/glass turntable plus debug lines under a fixed camera/light.
- **Validation gate:** reference image or independently calculated values; no missing parameter/material; alpha/cull and cube orientation correct; shader compile errors are actionable.
- **Likely risks:** old HLSL semantics, tangent basis, reflection orientation, under-specified source blend state.
- **Prerequisites:** Milestone 2.
- **Must not include:** post-processing, shadows, broad material optimization.
- **Effort:** 45–90 port hours; 10–30 CNA hours.

### Milestone 4 — Static terrain and track scene

- **Objective:** load raw height/track data and display one static track/terrain with a controllable debug camera.
- **Source areas:** `Landscape.cs`, `Track.cs`, `TrackLine.cs`, XML data and height file.
- **CNA features:** 44-byte declaration, 32-bit index buffer, draw calls, matrices/frustum/bounds, textures.
- **Visible result:** one correctly oriented terrain and road/track guide in a free/deterministic camera.
- **Validation gate:** exactly 66,049 vertices/393,216 indices; sampled heights/track positions match C# calculations; no winding/culling/index corruption; fixed-camera image.
- **Likely risks:** coordinate conversions, XML defaults, large-buffer backend behavior.
- **Prerequisites:** Milestones 2–3.
- **Must not include:** car physics, all scenery, postprocessing.
- **Effort:** 35–75 port hours; 0–15 CNA hours.

### Milestone 5 — Car, bones and chase camera

- **Objective:** display one car on the track with wheel/pivot transforms and source-equivalent chase camera interpolation.
- **Source areas:** `CarModel.cs`, `Model.cs`, `ChaseCamera.cs`, minimal `Player` state.
- **CNA features:** matrices, model hierarchy/package, render queue/material binding.
- **Visible result:** stationary/scripted-moving car with correct wheel/body/glass geometry and camera.
- **Validation gate:** bone absolute transforms and wheel centers match exported references; deterministic camera matrices; bounds/culling stable.
- **Likely risks:** C# reference identity, transform multiplication order, converter hierarchy loss.
- **Prerequisites:** Milestone 4.
- **Must not include:** real driving physics, collision, race rules.
- **Effort:** 25–50 port hours.

### Milestone 6 — Basic driving and telemetry parity

- **Objective:** translate input/player/car physics sufficiently to drive one car on one track.
- **Source areas:** `Input.cs`, `BasePlayer.cs`, `CarPhysics.cs`, `Player.cs`, camera update.
- **CNA features:** keyboard/gamepad/mouse, time/math.
- **Visible result:** accelerate, brake, steer, reverse/gears and chase camera on a flat/track surface.
- **Validation gate:** fixed scripted input produces bounded-difference position/orientation/velocity/RPM/gear telemetry at checkpoints; no NaN/divergence; multiple frame-rate runs characterized.
- **Likely risks:** variable timestep, float/order differences, hidden static state, controller dead zones.
- **Prerequisites:** Milestone 5 and reference telemetry method.
- **Must not include:** complete lap rules, scenery, UI/audio polish.
- **Effort:** 45–95 port hours; math/input CNA fixes only with minimized proof.

### Milestone 7 — Collision, checkpoints, laps and ghost

- **Objective:** complete the deterministic gameplay core for one track.
- **Source areas:** `Track.cs`, collision helpers, `Replay.cs`, `Player.cs`, race manager logic.
- **CNA features:** math/ray/plane/bounds; storage only for a later sub-gate.
- **Visible result:** guard-rail/road collision, checkpoint ordering, three laps, finish result, replay ghost.
- **Validation gate:** track-position/collision fixtures; fixed input crosses identical checkpoints/laps; replay sample cadence/interpolation and deep copy are correct; source replay-load defect decision documented.
- **Likely risks:** spline boundary cases, variable time, corrupt/out-of-range replay data.
- **Prerequisites:** Milestone 6.
- **Must not include:** all screens, final high-score persistence, shadows/post effects.
- **Effort:** 45–90 port hours.

### Milestone 8 — Complete world population and batching

- **Objective:** render all scenery/material categories efficiently and correctly.
- **Source areas:** `Landscape.cs`, `CombiModel.cs`, `Model.cs`, `MeshRenderManager.cs`, all model/material content.
- **CNA features:** buffers, queues, states, alpha rendering, texture/cube sampling.
- **Visible result:** all three track environments with car, alpha foliage, animated windmills and transparent glass.
- **Validation gate:** every logical asset/material resolves; no case collisions; object transforms/counts match XML/source; opaque/transparent order and culling captured; no batch dangling handles.
- **Likely risks:** dynamic aliases, material variants, overdraw/performance, alpha-test source ambiguity.
- **Prerequisites:** Milestones 2–7; batch conversion allowed only now.
- **Must not include:** shadows/glow as a way to hide base-material errors.
- **Effort:** 45–90 port hours.

### Milestone 9 — Complete race loop, screens, UI and input

- **Objective:** make the game navigable from splash/menu through car/track selection, race, results, options/help/high scores and restart.
- **Source areas:** all `GameScreens`, `UIRenderer.cs`, `BaseGame.cs`, input/settings interfaces.
- **CNA features:** SpriteBatch, SpriteFont, textures, blend/sampler states, keyboard/mouse/gamepad, resize/fullscreen.
- **Visible result:** complete playable race loop with original screen flow and HUD.
- **Validation gate:** scripted navigation and input-edge tests; fixed-resolution screenshots; UI safe areas/layout; device disconnect/focus/fullscreen smoke; no SpriteBatch/3D ordering issue.
- **Likely risks:** static screen state, text/font asset availability, premultiplied alpha, focus/recenter behavior.
- **Prerequisites:** Milestones 7–8.
- **Must not include:** audio/shadow/postprocessing as gate requirements.
- **Effort:** 45–85 port hours; up to 15 CNA hours if a minimized UI/input defect appears.

### Milestone 10 — XACT audio

- **Objective:** restore authored menu/music/engine/gear/brake/crash/checkpoint/result behavior.
- **Source areas:** `Sound.cs`, `RacingGame.xap`, WAV sources.
- **CNA features:** AudioEngine/WaveBank/SoundBank/Cue/AudioCategory, RPC, MS-ADPCM.
- **Visible/audible result:** music categories and all event/vehicle cues react correctly.
- **Validation gate:** standalone bank probe passes first; cue-state logs and pitch checkpoints; category replacement/crossfade; long-run loop and stop; resource teardown clean.
- **Likely risks:** no generated banks/tool/license, subtle XACT policy differences, timing/mixer fidelity.
- **Prerequisites:** authentic banks or approved fallback decision; Milestone 6 telemetry/events.
- **Must not include:** generalized XACT authoring implementation.
- **Effort:** 20–60 port hours; 0–50 CNA hours depending on genuine failures.

### Milestone 11 — Shadows

- **Objective:** reproduce car shadow generation, blur and projection.
- **Source areas:** `ShadowMapShader.cs`, `ShadowMapBlur.cs`, `ShadowMap.fx`, `PostScreenShadowBlur.fx`.
- **CNA features:** shadow RT, depth buffer, explicit two-pass blur, custom blend/depth/sampler state.
- **Visible result:** stable blurred car shadow on track/landscape.
- **Validation gate:** capture raw shadow, horizontal/vertical blur and final overlay; bias/PCF/edge behavior; no state leakage.
- **Likely risks:** render-target origin, encoded depth precision, blend semantics, resize.
- **Prerequisites:** base scene images pass; `Rgba64` decision recorded.
- **Must not include:** menu/glow postprocessing.
- **Effort:** 25–55 port hours; 5–20 CNA hours.

### Milestone 12 — Sky, lens flare and post-processing

- **Objective:** complete sky/cube mapping, lens flare, menu blur and gameplay glow chains.
- **Source areas:** sky/lens flare code, `PostScreenMenu.cs`, `PostScreenGlow.cs`, corresponding `.fx` files.
- **CNA features:** cube texture, additive SpriteBatch, full/half/quarter targets, 10×10 grid, 4/5 explicit passes.
- **Visible result:** final menu and gameplay presentation on the reference backend.
- **Validation gate:** every intermediate target captured; pass-by-pass image thresholds; `Color` versus high-precision decision; viewport/texel offsets and state reset verified.
- **Likely risks:** precision, UV origin/half-texel history, source blend defects, cumulative image differences.
- **Prerequisites:** Milestones 9 and 11.
- **Must not include:** another backend.
- **Effort:** 45–100 port hours; 10–45 CNA hours.

### Milestone 13 — Persistence and platform completion

- **Objective:** settings, highscores, replay files, screenshots and desktop behavior survive restarts safely.
- **Source areas:** `GameSettings.cs`, `Highscores.cs`, `Replay.cs`, `ScreenshotCapturer.cs`, `Program.cs`.
- **CNA features:** StorageDevice/StorageContainer, streams, backbuffer readback/JPEG, window settings.
- **Visible result:** options/high scores/replay persist; screenshots are valid; errors are portable.
- **Validation gate:** fresh/corrupt/old/new round trips; interrupted write behavior; path containment/case; screenshot pixel orientation; sanitizer shutdown.
- **Likely risks:** source path bugs, implicit XML defaults, backend readback differences.
- **Prerequisites:** Milestone 9; screenshots after final render chain.
- **Must not include:** cloud/platform services not used by desktop game.
- **Effort:** 20–50 port hours; 0–20 CNA hours.

### Milestone 14 — Fidelity, performance and feature-complete gate

- **Objective:** close known differences and meet the definition of done on OPENGL33.
- **Source areas:** all, guided by captures/telemetry rather than broad rewrites.
- **CNA features:** complete exercised subset.
- **Visible result:** feature-complete three-car/three-track game with stable frame pacing.
- **Validation gate:** full regression corpus; no missing assets/materials/cues/screens; performance budget; ASAN/UBSAN; repeated load/race/restart; licensing packaging gate.
- **Likely risks:** cumulative shader/content differences, dynamic-view-distance behavior, rare race/replay bugs.
- **Prerequisites:** Milestones 1–13.
- **Must not include:** speculative refactors or backend expansion.
- **Effort:** 60–140 port hours; 10–50 CNA hours.

### Milestone 15 — Additional backend enablement

- **Objective:** enable one selected backend using the frozen reference corpus.
- **Source areas:** backend shader variants and only necessary renderer boundaries.
- **CNA features:** chosen backend's effect bindings, buffers, targets, states, textures and readback.
- **Visible result:** same game and captures within approved backend tolerances.
- **Validation gate:** all reference scenes/passes/gameplay/storage/audio tests; no OPENGL33 regression.
- **Likely risks:** shader language/toolchain, coordinate conventions, unsupported bindings/formats, immature backend.
- **Prerequisites:** Milestone 14.
- **Must not include:** multiple new backends in one milestone.
- **Effort per backend:** 65–210 combined port/CNA hours in the normal case; Vulkan/Bgfx may exceed this materially.

## Validation gates and evidence artifacts

Every milestone ends with checked-in test code/metadata where licensing permits, plus locally reproducible commands. A green build without the specified visible/numeric artifact does not pass.

Reference artifacts should include:

- source/CNA/tool hashes and graphics/audio device metadata;
- original-XNA screenshots if executable, otherwise clearly labeled source-derived expectations;
- fixed camera/view/projection matrices;
- intermediate render-target PNGs and final screenshots;
- asset manifests, node transforms, bounds, material values and converter checksums;
- fixed input/replay event streams;
- CSV/JSON telemetry for car position/orientation/velocity/RPM/gear/checkpoint/lap;
- XACT cue/category/variable event logs and optional normalized audio captures;
- settings/high-score/replay binary/XML fixtures with versions;
- sanitizer/test output and backend comparison tolerances.

Image thresholds should combine exact checks for deterministic diagnostic buffers with perceptual/error metrics for final shader output. Never accept a final image alone when an intermediate pass can localize the difference.

## Game tests versus CNA tests

### Tests owned by this repository

- content converter/package golden tests;
- XML/height/alias/material inventory tests;
- physics/race/replay/camera telemetry;
- shader formulas, pass ordering and reference images;
- screen navigation, UI layout and input actions;
- audio director event decisions and Racing bank behavior;
- save/replay/high-score formats;
- end-to-end races and performance budgets.

### Tests owned by CNA

Only add a CNA test after reproducing a framework defect with the smallest public-API case. Likely candidates are:

- query a render-target format and construct exactly the returned format;
- the exact 44-byte declaration/32-bit-index draw if generic layout fails;
- backend-independent named custom-effect value/texture binding;
- a minimized genuine XACT bank/RPC/category parser case;
- storage/readback/state behavior that fails outside Racing-specific code.

Do not put Racing asset schemas, material rules or shader equations into CNA tests.

## Likely CNA framework work

Priority is evidence-driven:

1. Fix the confirmed `Rgba64` query/construction contradiction or document/return a constructible fallback.
2. Add one integrated OpenGL33 render-target/custom-layout/state proof if existing tests do not cover the exact sequence.
3. Fix only genuine XACT issues exposed by authentic banks.
4. Improve loose uncompressed DDS cube loading only if offline conversion is rejected or broader value is established.
5. Consider richer glTF node/material preservation only if the Racing converter chooses generic CNA glTF as its runtime contract.
6. Do not require general compiled FX/XNB support for the reference port. Treat it as a separate large CNA feature proposal.
7. Defer Vulkan/Bgfx/other backend shader work until Milestone 15.

All CNA changes require separate authorization/work in the CNA repository; this plan does not grant it.

## Dependencies

- stable CNA integration commit and matching sharp-runtime revision;
- CMake ≥3.20 and C++23 compiler supported by CNA;
- working OPENGL33 development/runtime environment;
- legal local access to original source assets;
- a deterministic `.x` geometry import/conversion component selected by proof, not assumption;
- image comparison and render-target capture tooling;
- optional Windows XNA 4 reference environment;
- XACT authoring/compiler capable of producing the PC banks, or an approved fallback;
- representative keyboard, mouse and SDL-compatible gamepad;
- CI runner capable of headless/virtual-display OpenGL plus sanitizer builds.

## Risk register

| Risk | Probability | Impact | Mitigation / decision gate |
|---|---|---|---|
| Asset/code redistribution rights remain unclear | High | Critical | resolve before public asset/code copy; local-user-supplied pipeline design |
| Converter loses hierarchy, pivots or material semantics | Medium-high | Critical | car/windmill/alpha proof before batch conversion; manifest assertions |
| Ten shaders diverge visually from DX9 originals | High | High | fixed captures, intermediate RTs, tangent/cube/state probes, one pass at a time |
| Moving CNA integration invalidates findings/build | High near-term | High | pin after merge, record hash, rerun focused tests, no floating baseline |
| Genuine Racing XACT banks unavailable or parse differently | Medium-high | High | early bank/tool/license probe; explicit fallback decision |
| `Rgba64`/render-target/state behavior breaks post chain | Medium | High | early target probe; `Color` bootstrap; minimized CNA fix |
| Original XNA build cannot serve as oracle | Medium-high | Medium-high | source-derived tests, exported asset invariants, multiple captures if obtainable |
| Variable-step C# physics diverges in C++ | Medium | High | fixed input/delta telemetry and frame-rate matrix; preserve operation order initially |
| Static/global C# structure causes lifetime bugs when copied | Medium | High | RAII architecture, stable handles, main-thread GPU ownership, sanitizers |
| Linux case/path/window/input differences | Medium | Medium | normalized IDs, case-collision tests, focus/recenter/controller smoke |
| Additional backend scope expands prematurely | Medium | High | hard Milestone 14 gate; one backend per later milestone |
| Source conversion bugs are mistaken for intended fidelity | High | Medium | maintain a documented preserve/fix decision ledger |

## Phase effort summary

| Milestones | Scope | Realistic combined hours |
|---|---|---:|
| 0–1 | baseline and empty application | 20–40 |
| 2–3 | content proof and material/shader foundation | 125–260 |
| 4–8 | world, car, physics, race, full scenery | 220–430 |
| 9–10 | screens/UI/input and audio | 85–180 |
| 11–13 | shadows, postprocessing and persistence | 115–270 |
| 14 | fidelity/performance/feature gate | 70–190 |
| 15 | each additional backend | 65–210 normally; substantially more for constrained backends |

These bands overlap with debugging/checkpoint estimates in the feasibility audit. Use the audit's 900–1,250-hour overall realistic planning range rather than summing every upper bound.

## Definition of done

The first reference implementation is done only when all of the following are true:

- builds reproducibly against a pinned CNA commit with `OPENGL33`;
- launches, resizes, toggles supported display settings and shuts down cleanly under sanitizers;
- all three cars and tracks load with complete validated hierarchy/material/texture data;
- all menus/screens and the three-lap race loop work with keyboard and gamepad, with mouse behavior documented/tested;
- driving, collision, checkpoints, lap timing, result, ghost/replay and high scores pass deterministic tests;
- landscape, road, scenery, alpha content, car/glass, sky, lens flare, shadow and menu/gameplay post-processing meet agreed reference tolerances;
- all authored required audio behaviors pass, or a formally approved/documented fallback meets its own acceptance criteria;
- settings, highscores, replay and screenshots survive restart/corruption tests;
- no known missing asset/material/technique/cue silently falls back;
- game-owned regressions pass and every CNA fix has a minimized CNA regression test;
- performance and frame pacing meet the agreed reference-hardware budget;
- README/build/content-generation instructions let a fresh session reproduce Phase 1 onward without rediscovering architecture;
- code/asset provenance and redistribution requirements are documented and satisfied for whatever is published;
- limitations and non-reference backends are stated honestly.

## Recommended next action

Do **Milestone 0**, then **Milestone 1 only**: freeze the post-integration CNA commit and build a minimal `OPENGL33` Racing target with a deterministic clear/capture/resize/input/sanitizer harness. Its purpose is to validate the build and runtime baseline, not to translate game code. Once that gate passes, begin the representative asset proof; never start with bulk C# translation or bulk asset conversion.
