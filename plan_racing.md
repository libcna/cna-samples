# Future implementation plan: Racing Game on CNA

## Status and governing rule

This is now the **active final sample plan**. Milestone 0 was completed on
2026-09-02 and is frozen in [`racing_baseline.md`](racing_baseline.md). No C++
gameplay has been translated yet; Milestone 1 is the current work.

> **NO RACING IMPLEMENTATION BEFORE CNA MODULARIZATION AND STABILIZATION.**

That gate is satisfied. The pinned implementation baseline is CNA
`51d61ef42d1105d97387feeba11eae91a2f3e2e9`, including the `FX-128` correction
found during this baseline. Continue with the minimal reference-backend harness;
do not start by translating game classes or processing all content.

## Source hierarchy

Use a hybrid hierarchy with one clear primary source:

1. `/rv/tmp/RacingGame` at
   `d8092633e4e43e014ff168d8e913a9373538b851` — **primary current implementation,
   content, shader, XACT, and runnable FNA reference**;
2. `/rv/tmp/XNAGameStudio/Samples/XNA-4-Racing-Game-Kit-master` — historical
   comparison, original `.x`/processor intent, and tie-breaker for modern changes;
3. original XNA-era notices/docs — behavioral and licensing provenance.

Do not update these inputs silently during implementation. Record source and CNA
SHAs in every reference capture and milestone report.

## Goals

- Preserve the complete race/gameplay/screen experience on one CNA reference
  backend before enabling more platforms.
- Keep simulation and authored semantics independent of graphics backend and input
  device.
- Consume the modern checked-in GLB, `.material`, raw, exact FNA `.efb`, and XACT-bank
  evidence without recreating solved pipeline work.
- Add only CNA capabilities that have a reusable framework boundary.
- Produce deterministic tests and FNA comparison evidence at every visible
  milestone.
- Qualify Windows, Android, and Web only after Linux `OPENGL33` is complete.

## Non-goals for the reference implementation

- Loading legacy `.x` models.
- Recreating XNA's Content Pipeline or `RacingGameModelProcessor`.
- Repacking all assets into a custom `RacingPackage`.
- Loading MonoGame `MGFX` or compiling HLSL/`.fx` source at runtime.
- Porting XNAssets or DigitalRiseModel wholesale.
- Supporting more than `OPENGL33` during the first feature-complete gate.
- Redesigning CNA beyond module boundaries directly evidenced by Racing.

## Superseded old-plan work

| Previous task | New evidence | Current disposition |
|---|---|---|
| Convert 57 `.x` files with a custom tool | 57 checked-in GLBs, exact basename coverage | **Remove**; validate and load GLB directly |
| Reconstruct hierarchy, pivots, tangents | GLBs retain named nodes/matrices and `_TANGENT`/`_BINORMAL` | **Replace** with a four-model semantic proof and a bounded CNA importer enhancement |
| Design a monolithic `RacingPackage` | 57 JSON sidecars bind effects, techniques, parameters, textures and ordered parts | **Remove**; add schema validation/optional manifest only |
| Recover material metadata from processor output | Metadata is explicit in sidecars | **Remove**; verify values and handle the known numeric `lightDir` quirk correctly |
| Acquire/generate `.xgs/.xsb/.xwb` | Runtime-used version-46 banks are checked in | **Remove acquisition**; keep CNA compatibility validation |
| Find or repair an original runnable oracle | User-confirmed Linux/FNA build runs | **Replace** with reproducible dependency pins and capture automation |
| Design a new portable Effect container and rewrite ten shaders | Live CNA executes XNA/FNA Effect Framework binaries on EasyGL; all ten Racing FNA `.efb` files create and every pass applies | **Remove**; retain the exact authored `.efb` files and qualify pixels/state during integration |
| Add a new node-preserving model API before inspecting live CNA | CNA now has a mature glTF/CNB `Model` path with nodes, parts, materials, bounds and import diagnostics | **Replace** with focused Racing proof assets and only repair demonstrated generic gaps |

## Milestone 0 baseline gates

These were the implementation preconditions. Their measured closure is in
[`racing_baseline.md`](racing_baseline.md):

1. exact stable CNA SHA, sharp-runtime SHA, graphics backend, compiler, SDL and
   relevant dependency versions;
2. clean focused tests for `OPENGL33` startup/clear/present/readback, resize/input,
   custom vertex buffers, indexed draws, texture/cube loading, render targets, and
   context/resource restoration;
3. a decision and small design note for the existing CNA compiled-Effect route;
4. a decision for the node-preserving model/glTF API and material binding hook;
5. a repeatable FNA Linux build/run recipe with dependency SHAs, launch directory,
   settings, and representative captures;
6. an asset/license inventory sufficient for development, with redistribution
   questions explicitly separated as a release gate;
7. checksum lock for canonical GLBs, material files, raw data, shaders, and XACT
   banks.

All seven are closed for starting Milestone 1. Two content defects are intentionally
carried into Milestone 2 rather than hidden: the supplied `Cube.glb` has an invalid,
unused `TEXCOORD_0` accessor range, and `NormalizeCubeMap.dds` is an uncompressed
RGB888 cube that CNA's loose DDS decoder currently refuses. Asset-level
redistribution provenance also remains a release gate; the repository-level Ms-PL
files are preserved but are not an itemized origin ledger for 301 MB of assets.

## Architecture decisions

### Module boundaries influenced by Racing

```text
Racing gameplay/screens
        |
        +-- RacingControls ---- desktop/gamepad/touch/tilt providers
        +-- RacingAssets ------ material schema + game asset registry
        +-- RacingRenderer ---- named Effect/model bindings + pass orchestration
        |
        v
CNA core/game + math
        |
        +-- graphics API -------- resources, states, render targets
        +-- effects/shaders ----- compiled Effect graph and named parameters
        +-- model/glTF ---------- nodes, transforms, ordered parts, bounds
        +-- raw content --------- paths, streams, cache, lifetime
        +-- XNB ----------------- optional, not Racing's primary content route
        +-- audio/XACT ---------- authored banks and playback semantics
        +-- input --------------- keyboard/mouse/gamepad/touch/sensors
        +-- storage/platform ---- saves, replay, screenshots, lifecycle
        |
        v
OPENGL33 / later OPENGLES and WEBGL2 backend modules
```

Do not put Racing technique names, material conventions, or control layouts in CNA.
Do not put backend shader handles or filesystem policy in gameplay classes.

### Simulation and presentation

- Translate the recognizable `CarPhysics`, `Player`, `Replay`, `TrackLine`, race,
  checkpoint, and lap logic with the same public class/constant names where useful.
- Drive simulation with a game-owned logical input snapshot, not direct platform
  polling inside physics.
- Keep render interpolation/camera/post-processing outside deterministic physics
  tests.
- Preserve fixed/variable timing behavior only after measuring FNA; do not “improve”
  formulas during translation.

### Ownership and errors

- CNA GPU/audio resources use explicit RAII and device-loss restoration rules.
- Raw asset cache keys include canonical path plus decode/load options.
- Racing sidecar references fail with source path, material, parameter, mesh, and
  part context; no silent null material or skipped technique.
- All platform fallbacks are capability-driven and logged once, not selected by
  catching arbitrary draw exceptions.

## Content strategy

### Canonical contract

Use the modern repository's **GLB + `.material` + raw assets** as the canonical
development contract:

- GLB owns vertices, indices, scene nodes, hierarchy, pivots, mesh names, material
  names, and embedded tangent/binormal attributes;
- `.material` owns effect/technique selection, typed parameter values, texture
  references, and ordered mesh-part bindings;
- raw TGA/PNG/DDS, track/combi/height/font data, WAV/XAP and generated XACT banks
  remain ordinary assets.

An optional small manifest may add schema version, canonical case-sensitive paths,
hashes, source commit, and conversion provenance. It must not duplicate geometry or
invent a package runtime.

### Model semantic proof set

Before bulk use, validate exactly these representatives:

1. one car — named four-wheel hierarchy, glass/car multi-parts, matrices/pivots,
   `_TANGENT`, ordered material mapping and bounds;
2. windmill — named `Windmill_Wings` parent-bone pivot animation;
3. one `Alpha*` model — alpha naming convention, render order/state;
4. sky cube — position-driven cube sampling plus a documented normalization of the
   malformed, shader-unused VEC3 `TEXCOORD_0` accessor; do not weaken CNA's generic
   glTF bounds validation to imitate DigitalRiseModel's out-of-view read.

The CNA model/glTF module should preserve unskinned scene nodes/instances, names,
parents and matrices; group primitives as ordered parts; compute/expose bounds;
expose material name/index; accept/repack `_TANGENT`; and allow the sky cube's VEC3
coordinate. Do not port unused DigitalRise skins/animation/renderer features.

### Material reader

Define and validate a typed Racing-side schema:

- effect and technique are required strings for active materials;
- parameters support scalar, vector, color, and texture reference types;
- every mesh mapping exists and part count matches;
- texture paths are canonicalized and constrained to the asset root;
- missing/extra parameters are reported against the selected Effect schema;
- alpha/render-state intent is made explicit in the game mapping instead of being
  hidden in a filename check;
- the modern loader's ignored numeric `lightDir` values are interpreted deliberately,
  not copied as a bug.

### Raw content

Use CNA's future raw-content stream/provider/cache interface for TGA/PNG/DDS and
ordinary bytes/text. Keep game-specific track/combi/font parsing in this repository.
The supplied uncompressed 24-bit RGB normalization cube needs a CNA loose-DDS cube
test/fix or a documented runtime-neutral equivalent after provenance review.

Do not create mobile/Web optimized derivatives during the reference milestone.
Those belong to later platform qualification and must be reproducibly derived from
the canonical assets.

## Effect and rendering strategy

### Existing CNA compiled-Effect runtime

Use the exact ten FNA `.efb` files as the canonical Effect assets. They are XNA/FNA
Direct3D 9 Effect Framework binaries produced by the retained `fxc /T:fx_2_0`
scripts, which live CNA supports through `Effect(GraphicsDevice&, byte[])` when
`CNA_EASYGL_COMPILED_EFFECTS=ON`.

The Milestone 0 oracle created all ten files on OPENGL33 and applied every reflected
technique/pass. It also found and fixed `FX-128`: automatic profile selection chose
MojoShader's experimental `glspirv` adapter on modern desktop GL and crashed on a
valid pixel-only pass. CNA now selects `glsl120`/`glsles`/`glsles3` from the actual
EasyGL profile; the focused regression and all 330 renderer tests pass on a real
OpenGL 4.5 core llvmpipe context.

Racing code keeps the original named technique/parameter/pass flow and loads the
bytes through CNA's public Effect path. It must not hand-translate shader behavior,
invent a second Effect graph, or parse renderer internals. Milestone 3 is therefore
an integration/fidelity gate for the existing runtime, not a new Effect architecture.

MonoGame `MGFX` remains out of scope. Later GLES/Web milestones qualify the same
authored semantics through their supported CNA route rather than changing Linux
reference assets pre-emptively.

### Rendering order to preserve

Freeze FNA captures and then reproduce, in stages:

1. shadow map generation;
2. sky/landscape/track and static world;
3. car, wheels, glass, objects and batched meshes;
4. brake tracks, lines and particles/lens flare;
5. glow/menu/blur/render-to-texture composition;
6. UI.

At each stage assert viewport, render-target dimensions/format, clear values, depth,
blend, cull, sampler and texture bindings. `Rgba64` capability reporting must be
truthful; mobile/Web may select a tested `Color` fallback and reduced target sizes.

## Audio strategy

Use the checked-in banks directly as local reference inputs:

```text
RacingGame/Assets/Audio/RacingGameManager.xgs
RacingGame/Assets/Audio/Sound Bank.xsb
RacingGame/Assets/Audio/Wave Bank.xwb
```

The first gate is non-destructive: load version-46 files, enumerate expected banks,
categories, variables and cues, decode representative waves, and compare errors with
FNA. Then test music, engine/gear variables, brake/skid variants, collision and UI
cues, pause/resume/category volume, fire-and-forget lifetime, and repeated race
transitions.

Do not regenerate banks unless a proven incompatibility requires a controlled
experiment. Keep full banks as opt-in/local fixtures until redistribution is clear.
Android and Web need separate real-device/browser XACT playback gates; CNA's desktop
parser implementation alone is not proof of platform audio behavior.

## Logical input and Android controls

### Common action model

Introduce a game-side `RacingControls` state (name illustrative, not a mandate):

| Value/action | Shape | Existing sources |
|---|---|---|
| steering | continuous `[-1,+1]` | keyboard, mouse delta, gamepad stick/D-pad, touch pad, optional tilt |
| throttle | continuous `[0,1]` | key/mouse/gamepad trigger/button, touch pedal |
| brake/reverse | continuous `[0,1]` | key/mouse/D-pad, touch pedal |
| handbrake | held boolean/amount | Space/middle mouse/left trigger/B, touch button |
| accept/back/pause | edge-triggered actions | keyboard/gamepad/mouse/touch/system Back |
| camera preset/distance | optional action | existing PageUp/PageDown/X/Y/wheel, mobile toggle |

Physics and screens consume actions; providers own device details. Preserve existing
desktop controls and Bluetooth gamepads on Android/Web.

### Android default layout

Landscape orientation, safe-area aware:

- left-bottom analog steering pad/wheel;
- large right-bottom throttle pedal;
- adjacent brake/reverse pedal;
- smaller handbrake button reachable while throttling;
- pause in a top corner; optional camera-preset button in the other;
- direct touch for visible menu controls;
- system Back maps to game back/pause.

The overlay supports simultaneous fingers, left-handed mirroring, size/opacity and
sensitivity settings. It hides when a gamepad is active if the player chooses.

### Optional tilt mode

Use CNA's Android accelerometer for steering only. Require neutral calibration,
landscape orientation handling, low-pass filter, dead zone, clamp, sensitivity and
invert options. Touch pedals/buttons remain. Fall back to touch when unsupported or
permission/sensor startup fails. Do not require browser accelerometer support for
the Web milestone.

Touch UI/controls are **port-side work**. `TouchPanel` and Android Accelerometer
already exist in CNA; only defects discovered by focused hardware tests belong in
CNA.

## Persistence and lifecycle

- Put settings, highscores and replay data behind CNA's platform storage boundary.
- Desktop uses user storage, never the source/asset tree.
- Android handles pause/background/resume, surface/context recreation, rotation
  policy, and process death; lock the game to landscape for the first release.
- Web persists through an explicitly synchronized browser-backed store; a transient
  in-memory virtual filesystem is not sufficient.
- Screenshots are optional per platform and must not block game completion.

## Staged implementation milestones

Every milestone is gated; do not continue after an unexplained render/audio/data
failure.

### Milestone 0 — Freeze the post-modularization baseline

**Status: complete (2026-09-02).** See [`racing_baseline.md`](racing_baseline.md).

- Pin CNA/dependency/source SHAs.
- Record FNA build/run recipe and capture known screens/race/audio behavior.
- Hash assets and document redistribution questions.
- Write accepted Effect/model module notes.

**Exit:** reproducible evidence bundle; no gameplay C++ yet.

### Milestone 1 — Minimal `OPENGL33` harness

**Status: current.**

- Empty Game lifecycle, deterministic clear/present/capture.
- Resize/fullscreen/input and path diagnostics.
- Sanitizer/debug diagnostics and exact backend/profile logging.
- Focused render-target/readback/custom-layout/cube probes.

**Exit:** pinned harness is stable before content or gameplay translation.

### Milestone 2 — GLB/material/raw proof

- Implement only the bounded CNA model/raw prerequisites.
- Load car, windmill, alpha model and sky cube.
- Validate every sidecar structurally without drawing all models.
- Compare node matrices, names, part order, tangents and bounds with DigitalRise/FNA.

**Exit:** four proof assets render with correct transforms/material assignments.

### Milestone 3 — Compiled Effect integration proof

- Load one representative normal/specular `.efb` and one multi-pass post `.efb`.
- Prove parameters, textures, clone/instance behavior, pass order, state and pixels.
- Compare the result with the frozen FNA captures and runtime state.

**Exit:** no Racing-only effect dispatcher is required to draw the proof scene.

### Milestone 4 — Static track scene

- Translate raw track/combi/landscape readers and geometry generation.
- Draw sky, terrain, road, guard rails and representative objects.
- Establish FNA capture points and CPU/GPU diagnostics.

**Exit:** first representative rendered scene, visually and structurally compared.

### Milestone 5 — Car, hierarchy, camera and desktop controls

- Translate car/player/chase-camera essentials.
- Animate wheel nodes using preserved transforms.
- Introduce logical input snapshot and map existing keyboard/mouse/gamepad controls.

**Exit:** first drivable car with correct wheel/camera behavior.

### Milestone 6 — Gameplay correctness

- Collision, off-road/air state, checkpoints, laps, race timing, AI/ghost/replay.
- Deterministic simulation tests and saved traces compared with FNA.
- Explicitly preserve or document old quirks instead of silently fixing them.

**Exit:** complete race possible without final rendering/audio polish.

### Milestone 7 — Complete world and rendering

- All materials/models, batching, glass/alpha/reflections.
- All ten effect families and intended techniques/passes.
- Shadows, lens flare, glow, blur/menu composition, brake tracks and UI.
- Quality switches and truthful capability fallbacks.

**Exit:** all visible game states render on `OPENGL33`.

### Milestone 8 — XACT, screens and persistence

- Validate/play supplied banks and authored behaviors.
- Complete splash/menu/options/selection/help/highscore/race transitions.
- Settings, highscores, replay and restart/exit lifecycle.

**Exit:** first complete race loop from launch back to menu with audio and saves.

### Milestone 9 — Feature-complete Linux `OPENGL33`

- Fidelity/performance/leak/device-loss tests.
- Repeatable FNA vs CNA capture/audio/control comparison.
- Release-gate asset/license report.

**Exit:** feature-complete reference target; realistic cumulative estimate 690–880 h.

### Milestone 10 — Windows qualification

- Reuse `OPENGL33`; test real Intel/AMD/NVIDIA drivers where available.
- Package assets/runtime, validate paths, XACT, storage, keyboard/mouse/gamepad.
- Consider D3D11 only as a separate later backend project.

**Exit:** supported Windows `OPENGL33` build, +50–90 realistic hours.

### Milestone 11 — Android qualification and controls

- Android Gradle/SDL app shell with explicit `OPENGLES`, landscape lifecycle and
  packaged/cached assets.
- Touch overlay and optional calibrated tilt provider.
- Real-device GPU, memory, thermal, suspend/resume, context-loss, audio/XACT,
  storage and gamepad matrix.
- Mobile quality preset and measured asset residency.

**Exit:** complete race on representative physical devices with touch-only control;
this qualification adds 180–300 realistic hours.

### Milestone 12 — Web qualification

- Emscripten `WEBGL2` app and real-browser test harness.
- Progressive/cacheable content delivery, load progress/failure handling, memory
  budget and context-loss restore.
- Browser audio unlock plus XACT proof, persistent storage synchronization,
  keyboard/mouse/touch/gamepad tests.
- Desktop and mobile browser matrix; touch scheme reused without mandatory tilt.

**Exit:** complete race after a production-reasonable load on supported WebGL2
browsers; this qualification adds 250–420 realistic hours.

## Validation evidence

For each milestone retain:

- source/CNA/dependency SHAs and build flags;
- asset hashes and load/schema report;
- deterministic simulation trace or named visual capture points;
- backbuffer/render-target images with dimensions/formats;
- draw/pass/state/resource diagnostics when relevant;
- audio cue/category/variable logs and listening checklist;
- performance, peak resident memory, GPU texture estimate and load times;
- platform/device/browser identity for non-Linux gates;
- known differences classified as bug, intentional deviation, or unknown.

## Test ownership

### Tests in this repository

- Racing material-schema and complete-asset-reference validation;
- GLB hierarchy/part expectations for known models;
- track/combi/font/replay/settings parsing;
- car physics, checkpoints/laps, replay and logical input mapping;
- screen/race/render-pass integration and FNA capture comparisons;
- touch layout/hit/multi-touch/tilt-filter tests;
- platform packaging and end-to-end race smokes.

### Tests in CNA

- compiled Effect collections/selection/parameter/pass/state behavior;
- backend shader-module compilation/binding;
- generic glTF nodes/instances/parts/material hooks/tangents/bounds;
- raw path/stream/cache/lifetime and loose RGB DDS cube;
- truthful `Rgba64` render-target query/create behavior;
- XACT parser/playback semantics using appropriately licensed local fixtures;
- TouchPanel/Accelerometer defects only if focused tests expose them;
- `OPENGL33`/`OPENGLES`/`WEBGL2` platform resource/context behavior.

Do not move game sidecar rules, control layout, race behavior, or mobile quality
policy into CNA tests.

## Effort summary

For Linux `OPENGL33`:

| Scenario | Racing port/tooling | CNA framework | Baseline/reference | Total |
|---|---:|---:|---:|---:|
| Optimistic | 320–370 | 110–130 | 15–20 | 450–550 |
| Realistic | **480–600** | **190–240** | **20–30** | **690–880** |
| Pessimistic | 760–850 | 400–470 | 35–45 | 1,050–1,350 |

Incremental realistic target qualification: Windows +50–90, Android +180–300,
Web +250–420 hours. With shared GL/input/platform work de-duplicated, supporting all
four targets adds about 430–700 hours and gives an all-platform realistic program
of approximately **1,120–1,580 hours**. D3D11 and release/store operations are not
included.

## Risk register

| Risk | Probability/impact | Mitigation / gate |
|---|---|---|
| Effect runtime or ten-effect fidelity expands | High/high | Milestone 3 first; freeze object model and two representative effects before bulk translation |
| GLB node/part semantics differ | Medium/high | Four-model proof with matrix/name/part/bounds assertions |
| XACT parses but authored behavior differs | Medium/high | Supplied-bank desktop gate, then device/browser playback gates |
| Asset rights incomplete | Medium/high for release | Per-asset manifest before redistribution; keep fixtures local where needed |
| CNA changes during/after modularization | High/high | Exact pinned SHA; update only through explicit rebaseline |
| Android memory/thermal/render formats | High/high | Mobile quality tier, residency measurements, real devices, format fallbacks |
| Web payload/memory/audio/storage | High/high | Progressive content groups, browser harness, audio unlock, persistent sync |
| Touch or tilt is not usable | Medium/high | Default touch steering+pedals, optional calibrated tilt, accessibility settings and gamepad |
| Platform branches corrupt gameplay parity | Medium/medium | One logical-input/simulation core; platform providers and shared traces |

## Definition of done

Linux `OPENGL33` is feature-complete only when a player can launch, navigate all
screens, select car/track, finish races, hear authored audio, save/load settings and
replay/highscore state, and return to menu with no unexplained FNA differences,
resource leaks, sanitizer findings, or unsupported silent fallbacks.

Windows, Android, and Web are separate done states. Android additionally requires a
complete touch-only race and suspend/resume on physical hardware; Web additionally
requires a production-reasonable content load and complete race in real supported
browsers. A platform is not “supported” merely because the library compiles.

## Recommended next action

Implement only the minimal pinned `OPENGL33` harness in Milestone 1. Then take the
four bounded content proofs in Milestone 2, repairing the general CNA layer for the
two measured asset gaps. Do not translate gameplay, rewrite shaders, or generate
platform derivatives before those gates pass.
