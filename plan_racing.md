# Future implementation plan: Racing Game on CNA

> **OWNER OVERRIDE (2026-09-02): read [`RACING-CONTENT-SOURCE-POLICY.md`](RACING-CONTENT-SOURCE-POLICY.md)
> before doing any content/model work below.** This plan's "canonical content strategy" (GLB from
> the modern repository) is withdrawn. The canonical source is the original XNA 4.0 `.X` models
> through the real content pipeline. No glTF/GLB for Racing.

## Status and governing rule

This is now the **active final sample plan**. Milestones 0 through 9 completed on
2026-09-02/03; their evidence is frozen in [`racing_baseline.md`](racing_baseline.md)
and the corresponding milestone reports. Milestone 10 Windows qualification is
ready to resume after its host-device gate was cleared; Milestone 11 Android and
Milestone 12 Web qualification are current.

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
Continue with Windows qualification in Milestone 10. The Linux reference result is
frozen in [`racing_milestone9.md`](racing_milestone9.md).

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

**Status: complete (2026-09-03).** See
[`racing_milestone5.md`](racing_milestone5.md).

- Translate car/player/chase-camera essentials.
- Animate wheel nodes using preserved transforms.
- Introduce logical input snapshot and map existing keyboard/mouse/gamepad controls.

The unchanged original C# physics/player oracle and C++ port agree across 739
records, including 600 driving frames, 60 narrow-road collision frames and 72
free-camera frames. Non-camera state is bit-exact; repeated camera interpolation is
bounded to the measured 16-ULP native/JIT difference. The Track oracle is 70/70
exact after adding the original checkpoint segment lists for all three tracks.

`RacingGame_cna_samples` is now a real product executable. Its concrete environment
connects `Player` directly to generated `Track`, original smoothed desktop
keyboard/mouse/gamepad input, owned checkpoint/replay/highscore state and the game
view. `CarRenderer` loads only authentic XNA products and submits the solid pass
before reflection/glass. The 420-frame real-GL scene probe produces bit-identical
Debug and ASan captures, moves 14.150156 metres after the original countdown and
submits all 12 car parts. The then-cumulative harness was 113/113 in both builds.

**Exit: satisfied.** The car is drivable with correct wheel/camera behavior.

### Milestone 6 — Gameplay correctness

**Status: complete (2026-09-03).** See
[`racing_milestone6.md`](racing_milestone6.md).

- Collision, off-road/air state, checkpoints, laps, race timing, AI/ghost/replay.
- Deterministic simulation tests and saved traces compared with FNA.
- Explicitly preserve or document old quirks instead of silently fixing them.

The replay data model is translated and connected to the product environment. The
Track oracle compiles unchanged original `Replay.cs` and agrees exactly with CNA on
default generation for all three tracks (385/760/1135 matrices), checkpoint
schedules, playback interpolation, empty recording, append, deep clone and the
144-byte two-matrix binary wire fixture. Its expanded gate is 79/79 exact records.

The physics oracle compiles unchanged original `BasePlayer.cs`, `CarPhysics.cs`,
`ChaseCamera.cs` and `Player.cs`. Its 753-record trace includes a complete
three-lap checkpoint sequence, faster/slower checkpoint feedback, replay sampling,
two best-replay replacements, lap timing, victory transition and result text. All
non-camera records are bit-exact; the existing camera boundary remains 16 ULP. A
missed `CheckpointBetter`/`CheckpointWorse` event in the C++ translation was fixed
at the common Racing environment boundary. The product scene separately proves
the 385-matrix beginner ghost, active recording at 0.2-second cadence and live
ghost interpolation.

**Exit: satisfied.** A complete race is possible without final rendering/audio
polish. Ghost rendering remains in Milestone 7; screen, XACT and persistent
lifecycle integration remain in Milestone 8.

### Milestone 7 — Complete world and rendering

**Status: complete (2026-09-03).** See
[`racing_milestone7.md`](racing_milestone7.md).

- All materials/models, batching, glass/alpha/reflections.
- All ten effect families and intended techniques/passes.
- Shadows, lens flare, glow, blur/menu composition, brake tracks and UI.
- Quality switches and truthful capability fallbacks.

**Completion evidence (2026-09-03):** the generated best replay is visible through the
original `ShadowCar` technique from the authentic XNA 4
`Shaders/LightingShader.xnb`. The product preserves the original replay
pre-orientation, uses the player's current wheel angle exactly as the source does,
and submits all 12 parts of the authentic `Models/Car.xnb`.

The complete original 53-model landscape catalog is now loaded from the authentic
XNA 4 model XNB graph and batched by its processor-selected technique and material.
All authored neutral objects and ten original `CombiModel` files are resolved, and
the original `GenerateObjectsForTrack` population is translated in source order:
palms/lamps, start gantry/light, checkpoint banners, curve/warning signs and
high-detail random surroundings. The port also preserves the one global
`RandomHelper` sequence used by world generation and camera shake. Seed 152 freezes
the Beginner scene at 1,252 post-collision objects; its 420-frame real OPENGL33
Debug and ASan/UBSan runs both submit 295 visible model parts.

The original three-stage shadow path is now active: authentic `ShadowMap.xnb`
generates the 2048x2048 caster map and receiver map, and authentic
`PostScreenShadowBlur.xnb` performs the authored horizontal/vertical passes before
the original multiplicative overlay. Track road/tunnels/rails, nearby landscape
objects and the car participate with the source culling/alpha rules. The product
probe reports 22 caster and 22 receiver submissions, meaningful Rgba64 contents in
both maps, and bit-identical Debug/ASan captures with SHA-256
`a00eb9c2c9dbaa79a494aa71bcea9093f508a195c93aa2b330514f7f850466dd`.
CNA and meta-gl now provide real desktop RGBA16 UNORM render-target storage rather
than substituting `Color`; the focused EasyGL test is 18/18 and meta-gl is 7/7.
The then-cumulative OPENGL33 harness was 113/113. The city-ground overlay is also
restored from the first source-ordered hotel/building anchor, the original two
triangles and tiling, and authentic `CityGround.xnb`/`CityGroundNormal.xnb`.
It submits exactly once and produces bit-identical Debug/ASan captures with
SHA-256 `07411bf5df7c4ea55dd17f71c9e8ea49c2f72d49dd717ddb47c900cdd01bb917`.
Major braking now feeds the original overlap/rate/cap filters and exact six-vertex
tire-mark geometry into the landscape renderer. The probe retains 18 vertices,
submits six triangles through authentic `Textures/track.xnb` and
`LightingShader.xnb` `Diffuse20`, and produces bit-identical Debug/ASan captures
with SHA-256 `4cd857708ea238880a84aea1fe6129b0f080352a31b14cf312fce571915e4615`.
The source `LensFlare.cs` composition is now complete: all seven authentic XNA 4
texture XNBs, 17 authored flare records, camera projection/border fade and smoothed
sun intensity render through the original additive SpriteBatch contract. The
ten-frame track-segment tunnel gate is connected, and flare sprites flush after
the world/shadow passes as in the original deferred batch. A dedicated real-GL
camera proves all 17 submissions and non-black backbuffer output; tunnel suppression,
rotation/origin and idempotent disposal are also covered in both Debug and ASan/UBSan.
The 420-frame product runs remain clean and bit-identical at
`936dcb0a510eeff0053264c10dbaccf962a39960b07bf31e1e43a049531a3d5c` (the
Beginner start camera correctly has the directional sun outside its last frame).

The original render-to-texture composition is now active as well. `RenderToTexture`
preserves the full/quarter sizing policies, adapter-selected `Rgba64` format,
backbuffer depth, zero desktop MSAA, discard usage, resolve validation and reset
recreation. `PostScreenGlow.xnb` executes its authored radial blur, downsample,
two blur and final-composition passes around the complete game scene; the original
speed-dependent radial scale, alpha-write blend and `ScreenBorderFadeout.xnb` are
unchanged. `PostScreenMenu.xnb` and `Noise128x128.xnb` also pass the corresponding
four-pass real-GL integration gate for the upcoming screens. The focused pre-HUD harness was
133/133 in Debug and ASan/UBSan and checked actual high-precision targets, disabled
and idempotent start behavior, pass counts, meaningful full-screen pixels and state
restoration. The 420-frame product probe is sanitizer-clean and bit-identical across
both configurations at
`7c725e0859e94dd906d84eb3b34948042890534fb7834aaf4b3b515c59445c89`.
The cumulative OPENGL33 qualification also passes both Debug and ASan/UBSan CPU
oracles, the bounded FNA static-geometry comparison (`RMSE 0.002230`, 99.05% of
channels within two), and the classified external Mesa LeakSanitizer result.

The prior sharp FNA screenshots were captured with the persisted setting
`PostScreenEffects=false`. A separate full FNA screen-stack oracle retained under
`evidence/fna-postprocess-oracle` uses the same settings with that single value set
to `true`; its race frames confirm the intended bright bloom/radial-blur behavior.

The desktop in-race HUD is now complete. It loads the authentic 1024x512
`Textures/Ingame.xnb` (`SurfaceFormat.Color`) and `Textures/GameFont.xnb`, preserves
all original atlas source rectangles, 1600x1200/1400x1050 coordinate conversions,
timing/top-five text, tachometer needle, speed, gear, lap and rising checkpoint
overlays. A real OPENGL33 probe proves the source panel texel `(0,0,0,154)` blends
to `(79,79,79,255)` over the controlled background, rather than accepting white
glyphs as sufficient evidence. Debug, ASan/UBSan and classified-LSan HUD captures
are byte-identical at
`c1859ce2fdeb5fc41f637b9b659197be288f563ce09ae96e2ecb8f318ed58364`.
The cumulative harness is 146/146 in both instrumented builds and both 420-frame
product runs pass with 14 atlas sprites and 67 bitmap-font glyphs on their final
frame. Full menu/help/options/highscore UI remains with the screen stack in
Milestone 8, not this in-race rendering gate.

**Exit: satisfied for the complete race scene.** Every visible in-race layer renders
on `OPENGL33`; non-race screen states are explicitly owned by Milestone 8.

### Milestone 8 — XACT, screens and persistence

**Status: complete (2026-09-03).**

- Validate/play supplied banks and authored behaviors.
- Complete splash/menu/options/selection/help/highscore/race transitions.
- Settings, highscores, replay and restart/exit lifecycle.

**Exit:** first complete race loop from launch back to menu with audio and saves.

The original LIFO screen stack, owner-thread staged loading, desktop input,
menu/help/options/highscore/car/track/race UI, supplied XGS/XWB/XSB banks, gear and
music behavior, settings/highscores/replay persistence and race-result return path
are connected. Debug and ASan/UBSan probes traverse every screen, load and play the
authored banks, persist the original XML shape, complete a real Advanced race
session and return to the main menu. The focused GPU harness is 148/148 and proves
all three authentic rank trophies. See
[`racing_milestone8.md`](racing_milestone8.md).

### Milestone 9 — Feature-complete Linux `OPENGL33`

**Status: complete (2026-09-03).**

- Fidelity/performance/leak/device-loss tests.
- Repeatable FNA vs CNA capture/audio/control comparison.
- Release-gate asset/license report.

**Exit: satisfied.** Debug and ASan/UBSan pass the cumulative 151/151 real-GL
harness; Release passes 144/144; all three complete the natural 7,200-frame race
return. The authentic physics and Track/Replay oracles remain 753 and 79/79,
device reset recreates every game-owned render target, and the source/asset audits
cover all 58 C# files, 325 canonical files and 358 authentic build products. The
only Release blocker is the missing canonical redistribution license, explicitly
separated in [`racing_release_gate.md`](racing_release_gate.md). See
[`racing_milestone9.md`](racing_milestone9.md).

### Milestone 10 — Windows qualification

**Status: blocked on host intervention (2026-09-03); qualification is not complete.**

- Reuse `OPENGL33`; test real Intel/AMD/NVIDIA drivers where available.
- Package assets/runtime, validate paths, XACT, storage, keyboard/mouse/gamepad.
- Consider D3D11 only as a separate later backend project.

**Exit:** supported Windows `OPENGL33` build. With the local offline Win7 VM and
the Linux behavior baseline already available, the current bounded estimate is
3–8 active agent hours unless a 32-bit C++23/dependency or VirtualBox OpenGL limit
is demonstrated.

The post-reboot host-device gate is cleared: the installed VirtualBox 7.2.8
modules (`vboxdrv`, `vboxnetflt`, `vboxnetadp`) are loaded for
kernel `6.12.100+deb13-amd64`; the owner restored `/dev/vboxdrv` and
`/dev/vboxdrvu`, and `VBoxManage` can again inspect the saved `win7` machine. Its
configuration remains suitable: Windows 7 SP1, VBoxSVGA with 3D acceleration and
128 MiB VRAM, Guest Additions 7.2.8, no guest network, an existing saved state and
the existing local shared folders. See
[`racing_milestone10.md`](racing_milestone10.md). Milestone 10 is ready to resume
but remains incomplete until its Windows runtime gates pass.

### Milestone 11 — Android qualification and controls

**Status: implementation and emulator qualification in progress (2026-09-03); the
physical-device exit gate is not complete.**

- Android Gradle/SDL app shell with explicit `OPENGLES`, landscape lifecycle and
  packaged/cached assets.
- Touch overlay and optional calibrated tilt provider.
- Real-device GPU, memory, thermal, suspend/resume, context-loss, audio/XACT,
  storage and gamepad matrix.
- Mobile quality preset and measured asset residency.

**Exit:** complete race on representative physical devices with touch-only control;
this qualification currently estimates +15–35 active agent hours after Linux.

The SDL3/Gradle shell, authentic packaged XNA content, `OPENGLES3` build, real CNA
`TouchPanel`/optional `Accelerometer` provider, safe-area multi-touch layout and
game-owned overlay are implemented. Native focused qualification covers the mobile
mapping and a real-renderer overlay, menu/screen flow, complete post-process chain
and a 420-frame drivable scene. Both `x86_64` and `arm64-v8a` debug APKs build
offline. The 40/40 mobile probe also pins the differing-aspect safe-area mapping,
and the API-35 emulator loads the rebuilt application into a live game session.
Headless emulator work is retained as integration evidence only: the host has no
KVM, so it cannot close the physical GPU, touch ergonomics, audio-listening,
memory/thermal or lifecycle gates. See
[`racing_milestone11.md`](racing_milestone11.md).

### Milestone 12 — Web qualification

**Status: real-browser progressive baseline in progress (2026-09-04); production delivery and
compatibility exit gates are not complete.**

- Emscripten `WEBGL2` app and real-browser test harness.
- Progressive/cacheable content delivery and context-loss restore are complete;
  hosted load and memory budgets remain.
- Trusted browser audio unlock, an IDBFS-backed settings round trip and
  highscore/replay persistence across a process restart are complete;
  resize/fullscreen/background-resume is qualified in Chrome, while audible XACT
  cues remain.
- Desktop and mobile browser matrix; the shared touch scheme is integrated on Web
  without mandatory tilt and qualified under desktop Chrome touch emulation.

**Exit:** complete race after a production-reasonable load on supported WebGL2
browsers; this qualification currently estimates +15–35 active agent hours after Linux.

The current shared C++ target builds with Emscripten pthreads, Asyncify and
`WEBGL2`. It byte-preserves 326 runtime files across a 35 MiB bootstrap and the
existing Models, Landscape and Textures phases, with stable versioned IndexedDB
cache entries. Clean and cached Chrome profiles both reached and drove the
Advanced race through the scripted real-browser flow with no browser, HTTP or
WebGL errors; the cache state of every package was asserted. A trusted browser
start leaves SDL WebAudio running at 48 kHz, and the original Options XML survives
an IDBFS reload. A production Release run also survived real WebGL context loss
and restoration during progressive loading, in the main menu and during a race,
with valid restored captures and no JavaScript, HTTP or WebGL errors.
The browser race probe now drives all three Advanced laps through unchanged
physics, reaches Game Over, persists the highscore and replay, returns to MainMenu
and reloads both records after terminating and restarting Chrome. Its restarted
800x480 CNA back-buffer capture is visually valid. Hosted-network load and peak
residency, audible XACT cues and the browser/device matrix remain exit gates. A
clean Chrome run has additionally qualified resize, production fullscreen entry/exit and
background freeze/resume; gameplay support remains landscape-only because the
surviving portrait mode is visibly stretched. Desktop Chrome touch emulation has
additionally qualified main-menu selection and simultaneous steering/throttle
through the real shared overlay;
physical mobile-browser ergonomics remain open. See
[`racing_milestone12.md`](racing_milestone12.md).

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

| Delivery gate | Status / remaining active agent time from Milestone 9 |
|---|---:|
| Close the bounded XNB comparison/draw gate | Complete (2026-09-02) |
| Close the representative compiled Effect integration gate | Complete (2026-09-02) |
| First playable Linux race | Complete (2026-09-02) |
| Feature-complete, qualified Linux `OPENGL33` | Complete (2026-09-03) |
| Windows qualification after Linux | **3–8 h** |
| Android qualification after Linux | **8–20 h** |
| Web qualification after Linux | **8–20 h** |
| Windows + Android + Web | **19–48 h** |

These are active implementation/qualification hours, not calendar waiting time for
owner input, physical devices or external infrastructure. A newly proven major
subsystem gap can still widen them; the risk register names the concrete candidates.

## Risk register

| Risk | Probability/impact | Mitigation / gate |
|---|---|---|
| Authentic Effect behavior regresses on a later platform | Low/high | All ten are closed on OPENGL33; rerun the same XNBs and named-pass probes per platform |
| XNA Model XNB behavior regresses on a later platform | Low/high | Keep the four-model and 64/64 authentic XNB gates in every platform build |
| Win7 content-build VM filesystem was corrupt | Resolved/low | Original snapshot retained; repaired linked clone produced a hash-locked authentic build |
| XACT differs on Android/Web audio backends | Medium/high | Desktop supplied-bank behavior is closed; repeat it after device/browser audio unlock |
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

Continue Milestone 12 with hosted-network/residency measurement, audible XACT cue
qualification and expansion of the browser and input matrix. Complete Milestone
11 in parallel on representative physical Android hardware: run a full touch-only race, qualify
GPU/memory/thermal behavior,
suspend/resume and context loss, listen to XACT output, and verify storage plus
gamepad coexistence. Keep the 348 MiB authentic Content set canonical while
measuring package/load/residency; derive a reproducible platform quality tier only
when measurements require it. Resume the independent Milestone 10 Windows gate
from its frozen Linux result now that `/dev/vboxdrv` is restored. Do not modify
canonical content or substitute platform-specific gameplay behavior.
