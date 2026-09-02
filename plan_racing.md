# Future implementation plan: Racing Game on CNA

> **OWNER OVERRIDE (2026-09-02): read [`RACING-CONTENT-SOURCE-POLICY.md`](RACING-CONTENT-SOURCE-POLICY.md)
> before doing any content/model work below.** This plan's "canonical content strategy" (GLB from
> the modern repository) is withdrawn. The canonical source is the original XNA 4.0 `.X` models
> through the real content pipeline. No glTF/GLB for Racing.

## Status and governing rule

This is now the **active final sample plan**. Milestones 0 through 4 were completed on
2026-09-02/03; their evidence is frozen in [`racing_baseline.md`](racing_baseline.md)
and the corresponding milestone reports. Milestone 5 is current; track coordinates,
BasePlayer, logical control mapping, CarPhysics and ChaseCamera are now translated
and qualified.

> **NO RACING IMPLEMENTATION BEFORE CNA MODULARIZATION AND STABILIZATION.**

That gate is satisfied. The frozen Milestone 0/1 implementation baseline is CNA
`51d61ef42d1105d97387feeba11eae91a2f3e2e9`, including its `FX-128` correction.
Milestone 2 was replanned against CNA `1caa45c84`; the authentic Win7 build, four
bounded public XNB load proofs, unchanged-XNA structural comparison and meaningful
four-model draw are now qualified at CNA `756096626`. Milestone 3 then proved the
authentic normal/specular and two-pass blur Effect XNBs against unchanged XNA 4 and
FNA/OpenGL pixel oracles, with CNA/FNA bit-exact output. The bounded Effect gate is
closed. Milestone 4 then established the first static track scene from original raw
geometry and authentic XNA XNB content, with a bounded FNA/OpenGL image comparison.
Continue with Player, wheel hierarchy and the first drivable scene in Milestone 5.

## Source hierarchy

Use the original sample as the only implementation and content authority:

1. `/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/xna4-original/`
   `RacingGameWindows1/RacingGame/` — **canonical XNA 4.0 source and content**;
2. `/rv/data/library/github.com/FNA-XNA/FNA` — XNA API/runtime behavior not
   established by the original sample;
3. `/rv/tmp/RacingGame` at
   `d8092633e4e43e014ff168d8e913a9373538b851` — runnable behavior oracle only;
4. original XNA-era notices/docs — behavioral and licensing provenance.

The modern repository must never supply code or assets to the port. Its GLB,
`.material`, `.efb`, generated banks and raw files are not fallback inputs.

Do not update these inputs silently during implementation. Record source and CNA
SHAs in every reference capture and milestone report.

## Goals

- Preserve the complete race/gameplay/screen experience on one CNA reference
  backend before enabling more platforms.
- Keep simulation and authored semantics independent of graphics backend and input
  device.
- Consume the original `RacingGameContent.contentproj` outputs through XNA-shaped
  `ContentManager::Load<T>` calls and preserve the original content identifiers.
- Add only CNA capabilities that have a reusable framework boundary.
- Produce deterministic tests and FNA comparison evidence at every visible
  milestone.
- Qualify Windows, Android, and Web only after Linux `OPENGL33` is complete.

## Non-goals for the reference implementation

- Parsing `.X` files at runtime or implementing a second content pipeline in CNA.
- Substituting GLB, `.material`, FNA `.efb`, or modern-repository banks for
  authentic XNA 4.0 pipeline outputs.
- Repacking all assets into a custom `RacingPackage`.
- Loading MonoGame `MGFX` or compiling HLSL/`.fx` source at runtime.
- Porting XNAssets or DigitalRiseModel wholesale.
- Supporting more than `OPENGL33` during the first feature-complete gate.
- Redesigning CNA beyond module boundaries directly evidenced by Racing.

## Withdrawn interim content decisions

| Withdrawn task/assumption | Current disposition |
|---|---|
| Load 57 modern GLBs and `.material` sidecars | **Forbidden for Racing.** Build the original 57 `.X` assets with the XNA 4 pipeline and load their XNBs. |
| Repair modern `Cube.glb` or extend glTF specifically for the proof | **Remove.** It is not an original input and cannot gate Racing. |
| Retain modern FNA `.efb` files as canonical effects | **Remove.** Build the original ten `.fx` items to XNB and load them through `ContentManager`. |
| Reuse modern generated XACT banks | **Remove as an asset source.** Build the original `RacingGame.xap` and use only its pipeline products. |
| Parse JSON material sidecars | **Remove.** The original processor records technique metadata in mesh names and XNB owns the model/effect references. |
| Reconstruct a custom `RacingPackage` | **Remove.** Preserve the original XNB graph and public XNA load route. |
| Use the modern implementation/content as primary | **Remove.** It is a runnable behavior oracle only. |

## Milestone 0 baseline gates

These were the implementation preconditions. Their measured closure is in
[`racing_baseline.md`](racing_baseline.md):

1. exact stable CNA SHA, sharp-runtime SHA, graphics backend, compiler, SDL and
   relevant dependency versions;
2. clean focused tests for `OPENGL33` startup/clear/present/readback, resize/input,
   custom vertex buffers, indexed draws, texture/cube loading, render targets, and
   context/resource restoration;
3. a decision and small design note for the existing CNA compiled-Effect route;
4. a decision for CNA's XNB `ModelReader` and original processor output route;
5. a repeatable FNA Linux build/run recipe with dependency SHAs, launch directory,
   settings, and representative captures;
6. an asset/license inventory sufficient for development, with redistribution
   questions explicitly separated as a release gate;
7. checksum lock for the original `.X`, texture, shader, audio and auxiliary
   content inputs.

All seven were sufficient for starting Milestone 1. The GLB/material/FNA-asset
content conclusions in that frozen evidence were later withdrawn by the owner and
must not be used by Milestone 2. `NormalizeCubeMap.dds` is processed by the original
XNA pipeline; CNA's generic loose RGB24 DDS cube support was nevertheless repaired
and remains useful framework coverage. Asset-level redistribution provenance also
remains a release gate.

## Architecture decisions

### Module boundaries influenced by Racing

```text
Racing gameplay/screens
        |
        +-- RacingControls ---- desktop/gamepad/touch/tilt providers
        +-- RacingAssets ------ original content IDs + game asset registry
        +-- RacingRenderer ---- named Effect/model bindings + pass orchestration
        |
        v
CNA core/game + math
        |
        +-- graphics API -------- resources, states, render targets
        +-- effects/shaders ----- compiled Effect graph and named parameters
        +-- model/XNB ----------- bones, transforms, ordered meshes/parts, bounds
        +-- ContentManager/XNB -- primary models, effects, textures and sounds
        +-- raw content --------- original copied track/combi/height data
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
- Content failures retain the original content ID plus reader/type context.
- Model/effect bindings fail with model, mesh, part, technique and parameter
  context; no silent null material or skipped technique.
- All platform fallbacks are capability-driven and logged once, not selected by
  catching arbitrary draw exceptions.

## Content strategy

### Canonical contract

Build the original XNA 4.0
`RacingGameContent/RacingGameContent.contentproj` externally and preserve its
outputs. The similarly named `ContentWindows.contentproj` is a stale XNA 3.0
project with a missing processor project reference and is not the canonical build.

The live XNA 4 project declares 57 `.X` models, 142 textures, ten `.fx` effects,
28 WAV sound effects and one XACT project. Fifty-six models use the original
`RacingGameModelProcessor`; `Cube.X` uses the stock `ModelProcessor`. The custom
processor generates tangents, fills unnamed meshes from their parent bone name and
appends authored technique values to mesh names before delegating to
`ModelProcessor`.

The build products are copied into the sample artifact root, hash-locked, and
loaded by their unchanged identifiers through `ContentManager::Load<T>`. Runtime
`.X` parsing, GLB conversion, JSON sidecars and loose replacements are not allowed.

### Model semantic proof set

Before bulk use, validate authentic XNBs for exactly these representatives:

1. `Car` — named four-wheel hierarchy, glass/car multi-parts, matrices/pivots,
   generated tangent channel, processor-authored technique suffixes, part order and
   bounds;
2. windmill — named `Windmill_Wings` parent-bone pivot animation;
3. one `Alpha*` model — alpha naming convention, render order/state;
4. `Cube` plus the original `SkyCubeMap`/`NormalizeCubeMap` content — stock model
   output, position-driven cube sampling, faces, dimensions, formats and mips.

CNA's existing XNB `ModelReader`, vertex/index readers, stock/effect-material
readers and texture/effect readers must consume the unmodified output. Add only
general reader/runtime fixes demonstrated by those files. The sample must not
decode XNB internals or repair reader output locally.

### Material and effect binding

Translate the original binding logic. Technique selection comes from the mesh-name
metadata emitted by `RacingGameModelProcessor`; model parts and referenced effects
come from the authentic Model XNB graph. Validate names, ordered parts, techniques,
parameters and texture references against the original runtime. Do not recreate the
modern JSON material schema.

### Raw content

Models, TGA/PNG/DDS textures, effects, WAVs and XACT content follow the original
pipeline. The original content project copies track/combi/height data without
processing; keep their game-specific parsing in this repository.

Do not create mobile/Web optimized derivatives during the reference milestone.
Those belong to later platform qualification and must be reproducibly derived from
the canonical assets.

## Effect and rendering strategy

### Existing CNA compiled-Effect runtime

Build the original ten `.fx` items with XNA Game Studio 4.0 and load the resulting
Effect XNBs through `ContentManager::Load<std::shared_ptr<Effect>>`. CNA's
`EffectReader` passes the embedded Direct3D 9 Effect Framework bytecode to the same
compiled-Effect runtime used by direct construction when
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

The previously tested modern FNA `.efb` files remain useful implementation evidence
for CNA's runtime, but are not Racing assets. MonoGame `MGFX` remains out of scope.
Later GLES/Web milestones qualify the same authentic XNBs rather than changing the
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

Use only the outputs generated from the original
`RacingGameContent/Audio/RacingGame.xap` by XNA Game Studio 4.0. Hash and retain the
`.xgs`, `.xsb` and `.xwb` products with the other authenticated build outputs.

The first gate is non-destructive: load those files, enumerate expected banks,
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

**Status: complete (2026-09-02).** See
[`racing_milestone1.md`](racing_milestone1.md).

- Empty Game lifecycle, deterministic clear/present/capture.
- Resize/fullscreen/input and path diagnostics.
- Sanitizer/debug diagnostics and exact backend/profile logging.
- Focused render-target/readback/custom-layout/cube probes.

**Exit:** pinned harness is stable before content or gameplay translation.

### Milestone 2 — Authentic XNA pipeline/XNB proof

**Status: complete (2026-09-02).** See
[`racing_milestone2.md`](racing_milestone2.md).

- **Complete:** build the original XNA 4 content project in the offline Win7 VM and retain exact
  logs, output tree and hashes. Use `RacingGameContent.contentproj`, not the stale
  XNA 3 `ContentWindows.contentproj`.
- **Complete:** load `Car`, `Windmill`, `AlphaDeadTree` and `Cube` plus their referenced
  effect/texture content through CNA `ContentManager`.
- **Complete:** compare bones, matrices, names, part order, generated tangents,
  processor-authored technique suffixes, bounds, effect reflection, texture formats
  and mips with the unchanged XNA run: all 531 semantic records agree.
- **Complete:** draw all 17 parts of the four proof models with their authentic
  transforms, material parameters and processor-selected techniques; every proof
  viewport produces meaningful pixels.

The former Win7 block is resolved. The original VM remains protected by snapshot
`pre-ntfs-repair-2026-09-02`; a temporary linked clone was repaired with `chkdsk` and
the unchanged `RacingGame.csproj` completed an x86 Debug rebuild under XNA Game Studio
4.0 with 3 shader warnings and 0 errors. The shared-folder export contains 358 files
and 347.20 MiB, including 339 XNBs, all 57 model XNBs, all ten compiled effect XNBs
and the original XACT products, with a 358-entry SHA-256 manifest. On CNA `756096626`
the OPENGL33 harness loads all four proof models and their real effects through public
`ContentManager`, matches all 531 records emitted by the unchanged XNA inspector and
reports 69/69 PASS after submitting every part and validating per-model pixels. No
modern-repository asset or GLB participated.

**Exit: satisfied.** Four proof assets render with correct transforms/material assignments.

### Milestone 3 — Compiled Effect integration proof

**Status: complete (2026-09-02).**

- Loaded authentic `Shaders/NormalMapping.xnb` and
  `Shaders/PostScreenShadowBlur.xnb` through public `ContentManager`.
- Proved all authored techniques, parameters/textures, default values, independent
  Effect cloning, original 44-byte runtime tangent layout, pass order, render-target
  use, sampler/state application and GPU-visible pixels.
- The unchanged XNA 4 oracle, FNA/OpenGL oracle and CNA/OpenGL33 harness all pass.
  CNA and FNA are bit-exact for all four normal/clone/blur images; CNA and XNA are
  bit-exact for the normal/clone images. The small D3D9/OpenGL blur sampling
  difference is bounded by normalized RMSE `0.030538`/`0.018422`.
- Full harness qualification is 97/97 in Debug and ASan/UBSan; LSan reports only the
  already-classified external Mesa `libGLX_mesa` allocations.

**Exit: satisfied.** No Racing-only effect dispatcher is required. See
[`racing_milestone3.md`](racing_milestone3.md).

### Milestone 4 — Static track scene

- Translate raw track/combi/landscape readers and geometry generation.
- Draw sky, terrain, road, guard rails and representative objects.
- Establish FNA capture points and CPU/GPU diagnostics.

**Exit: satisfied.** The FNA/CNA CPU oracle agrees exactly on every qualified
landscape, track, road, tunnel, rail, holder and column record. The cumulative
OPENGL33 harness draws the original sky, terrain, road, road back, tunnels, rails,
columns and three authentic representative model XNBs. Its model-free base capture
matches the authoritative FNA/OpenGL render with normalized RMSE `0.002230` and
`99.0503%` of channels within two 8-bit values. Debug and ASan/UBSan are both
102/102 PASS. See [`racing_milestone4.md`](racing_milestone4.md).

### Milestone 5 — Car, hierarchy, camera and desktop controls

- Translate car/player/chase-camera essentials.
- Animate wheel nodes using preserved transforms.
- Introduce logical input snapshot and map existing keyboard/mouse/gamepad controls.

**Progress (2026-09-03):** the original `Vector3Helper` and
`SpringPhysicsObject` formulas are translated. A new FNA oracle compiles the
unchanged original C# files and agrees bit-for-bit with CNA in Debug and
ASan/UBSan for the selected vector operations and complete spring traces. The
oracle runs from the cumulative qualification script. Player hierarchy, wheel
animation and rendered desktop control integration remain in progress. The original
`Track` gameplay-coordinate surface is also translated: start/length properties,
both matrix interpolation routes, car-segment localization and tunnel queries now
match 67/67 exact aggregate FNA records over all three tracks in both Debug and
ASan/UBSan; the cumulative 102/102 and static-scene gates remain green. The periodic
`BaseGame.TotalFrames`-gated lens-flare cache update waits for the frame/render
integration and is not part of this CPU claim. The original `BasePlayer` and
`CarPhysics` calculation surfaces are also translated behind one game-owned logical
input/environment snapshot. An oracle compiling the unchanged original C# agrees
with CNA on 666/666 exact records in Debug and ASan/UBSan: BasePlayer lifecycle,
XNA keyboard/mouse/gamepad mapping, 600 driving frames and 60 narrow-road collision
frames. The unchanged original `ChaseCamera.cs` is now compiled into the same FNA
oracle and compared with the C++ port over 72 free-camera frames and 44 values per
frame (position, rotation/view matrices and all three view axes). The sequence is
bit-exact before zoom and stays within a measured maximum of 16 ULP after repeated
native/JIT float multiply-add rounding; control records remain exact. Debug and
ASan/UBSan pass the resulting 738-record gate. Concrete `Player`, wheel hierarchy
and rendered-scene integration remain open.

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

**Exit:** feature-complete reference target; current evidence-based remaining estimate 60–120 active agent hours.

### Milestone 10 — Windows qualification

- Reuse `OPENGL33`; test real Intel/AMD/NVIDIA drivers where available.
- Package assets/runtime, validate paths, XACT, storage, keyboard/mouse/gamepad.
- Consider D3D11 only as a separate later backend project.

**Exit:** supported Windows `OPENGL33` build, approximately +8–15 active agent hours after Linux.

### Milestone 11 — Android qualification and controls

- Android Gradle/SDL app shell with explicit `OPENGLES`, landscape lifecycle and
  packaged/cached assets.
- Touch overlay and optional calibrated tilt provider.
- Real-device GPU, memory, thermal, suspend/resume, context-loss, audio/XACT,
  storage and gamepad matrix.
- Mobile quality preset and measured asset residency.

**Exit:** complete race on representative physical devices with touch-only control;
this qualification currently estimates +15–35 active agent hours after Linux.

### Milestone 12 — Web qualification

- Emscripten `WEBGL2` app and real-browser test harness.
- Progressive/cacheable content delivery, load progress/failure handling, memory
  budget and context-loss restore.
- Browser audio unlock plus XACT proof, persistent storage synchronization,
  keyboard/mouse/touch/gamepad tests.
- Desktop and mobile browser matrix; touch scheme reused without mandatory tilt.

**Exit:** complete race after a production-reasonable load on supported WebGL2
browsers; this qualification currently estimates +15–35 active agent hours after Linux.

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

- original content-ID/reference validation;
- authentic XNB bone/mesh/part/technique/tangent expectations for known models;
- track/combi/font/replay/settings parsing;
- car physics, checkpoints/laps, replay and logical input mapping;
- screen/race/render-pass integration and FNA capture comparisons;
- touch layout/hit/multi-touch/tilt-filter tests;
- platform packaging and end-to-end race smokes.

### Tests in CNA

- compiled Effect collections/selection/parameter/pass/state behavior;
- backend shader-module compilation/binding;
- XNB model/effect/material/vertex/index/texture reader fidelity;
- raw path/stream/cache/lifetime and loose RGB DDS cube coverage retained as a
  general capability;
- truthful `Rgba64` render-target query/create behavior;
- XACT parser/playback semantics using appropriately licensed local fixtures;
- TouchPanel/Accelerometer defects only if focused tests expose them;
- `OPENGL33`/`OPENGLES`/`WEBGL2` platform resource/context behavior.

Do not move game sidecar rules, control layout, race behavior, or mobile quality
policy into CNA tests.

## Effort summary

The earlier 690–880 hour Linux estimate was a pre-evidence decomposition and is
withdrawn. It did not account for autonomous agent throughput or the already-working
CNA model/effect/XACT/platform surfaces. Current estimates are based on the completed
authentic build and 64/64 public load harness:

| Delivery gate | Remaining active agent time from current Milestone 3 evidence |
|---|---:|
| Close the bounded XNB comparison/draw gate | Complete (2026-09-02) |
| Close the representative compiled Effect integration gate | Complete (2026-09-02) |
| First playable Linux race | 18–40 h cumulative |
| Feature-complete, qualified Linux `OPENGL33` | **55–110 h cumulative** |
| Windows qualification after Linux | +8–15 h |
| Android qualification after Linux | +15–35 h |
| Web qualification after Linux | +15–35 h |
| Feature-complete Linux + Windows + Android + Web | **95–210 h cumulative** |

These are active implementation/qualification hours, not calendar waiting time for
owner input, physical devices or external infrastructure. A newly proven major
subsystem gap can still widen them; the risk register names the concrete candidates.

## Risk register

| Risk | Probability/impact | Mitigation / gate |
|---|---|---|
| Remaining eight Effect families expose new gaps | Medium/high | Representative normal/specular and multi-pass gates are closed; qualify each authentic family before first gameplay use |
| XNA Model XNB reader differs from processor output | Medium/high | Four authentic XNB proofs with matrix/name/part/tangent/technique/bounds assertions |
| Win7 content-build VM filesystem was corrupt | Resolved/low | Original snapshot retained; repaired linked clone produced a hash-locked authentic build |
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

Continue Milestone 5 from the canonical XNA 4 source: translate `Player`, preserve
the authentic car model's wheel hierarchy transforms, and connect the qualified
physics/camera/input path to the first drivable rendered scene. Extend deterministic
hierarchy traces before adding interactive control. Keep content identifiers and authentic XNB model/effect
products unchanged; repair only reusable CNA or sharp-runtime gaps and do not
introduce a Racing-only loader, effect dispatcher, GLB route or renderer-internal
shortcut.
