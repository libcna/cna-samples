# CNA Samples Master Port and Re-audit Plan

## Authority and scope

This is the only authoritative plan for the sample collection. It merges the former
`cna-samples/PLAN.md` migration list and `cnanext/plans/plan_samples.md` CNA-gap list. Sample
implementation, framework/runtime gaps, original-reference evidence and web delivery are tracked
in one place so that a sample cannot be called complete while a workaround or a hidden CNA bug
remains elsewhere.

The local upstream snapshot is `/rv/tmp/XNAGameStudio/Samples`. A direct inventory on 2026-08-22
found **153 directories after excluding `.idea`**. All 153 have an individual row below. Historical
labels such as `Done`, `Placeholder`, `Ignored`, `permanently out of scope` and earlier owner
declines are evidence to re-check, not current conclusions. Every directory, including all 67
previously ignored entries, receives a fresh source audit.

The large Racing Game remains governed exclusively by [`plan_racing.md`](plan_racing.md). Its
source directory is present in the inventory as `SAMPLE-152`, but this plan must not duplicate,
renumber or modify Racing tasks, `plan_racing.md`, `racing_api_matrix.md` or
`racing_feasibility.md`.

## Goal

Port the maximum possible number of original XNA samples to C++ with behavior and structure as
close as practical to the original source, using CNA and sharp-runtime rather than sample-local
substitutes. Every port must work natively and must also produce a tested browser build suitable
for later publication.

The active development dependency chain is temporarily:

| Repository | Required checkout | Current branch at plan creation |
|---|---|---|
| `cna-samples` | this repository | `develop` |
| CNA | `../cnanext` | `next` |
| sharp-runtime | `../sharp-runtimenext` | `next` |

`cna-samples` must set `CNA_SHARP_RUNTIME_ROOT` to `../sharp-runtimenext` before adding
`../cnanext` as a subdirectory. It must no longer build against `../cna` or indirectly select
`../sharp-runtime` during this development campaign.

## Sources of truth

Use these in descending order for each audit:

1. The exact original sample under `/rv/tmp/XNAGameStudio/Samples` for source structure, control
   flow, content declarations, assets, controls and visible behavior.
2. A running original XNA build for observable behavior. First rediscover and document the
   previously used direct Linux-side route. If that is unavailable or unreliable, use the local
   Windows 7 VirtualBox VM with Visual Studio 2010 and XNA Game Studio 4.0.
3. `/rv/data/library/github.com/FNA-XNA/FNA` for XNA API and behavioral details not established by
   the sample itself.
4. Local XNA documentation/specification mirrors where runtime observation is impractical.
5. MonoGame ports only as supplemental evidence, never as authority over the original sample or
   FNA.

The machine currently has 268 `.sln` and 359 `.csproj` files under the source snapshot. The VM
definition exists at `/home/robertvokac/VirtualBox VMs/win7/win7.vbox`, but on 2026-08-22
`VBoxManage` reported that `/dev/vboxdrv` was unavailable. This is an environment prerequisite to
resolve, not permission to skip original-reference validation.

## No-workaround and fidelity policy

The following rules apply to existing ports as strongly as to new ports:

- Compare every original C# source file and relevant content declaration line by line with the C++
  port. Preserve class/file decomposition, names, lifecycle, algorithms, constants, default values,
  input mapping and draw/update order wherever C++ permits.
- Use the public XNA-shaped CNA API. Do not call backend helpers, construct replacement runtime
  objects to bypass `Content.Load<T>()`, use raw-mesh loaders because `Model` is broken, add sidecar
  data because buffer readback is missing, or replace a missing property/event/network behavior in
  sample code.
- Do not omit or simplify scenes, screens, gameplay, effects, audio, networking, content processing
  or controls. Do not invent keyboard controls, substitute bodies/services, fake data or a reduced
  demonstration and then call the sample ported.
- Remove legacy sample-side workarounds as their rows are audited. A workaround previously approved
  for expediency is not grandfathered into the new completion definition.
- The existing mandatory F1 help overlay is not part of the original XNA samples. Remove it from the
  porting requirements and from each audited sample unless the owner later requests a separate,
  non-invasive launcher-level help feature.
- Prefer `System::*` types and primitive aliases from `sharp-runtimenext` when the C# source uses a
  .NET concept. Do not replace missing runtime functionality with an unrelated STL implementation
  merely to make a sample compile.
- Unavoidable C#-to-C++ mechanics such as ownership/RAII and a lossless offline asset-container
  conversion are allowed. Asset conversion is not a license to change behavior: the port must keep
  the same content identifiers and use the corresponding public CNA load call. Converted output
  must be reproducible from the original source asset.
- `missing.md` is an audit record, not a waiver. A sample with an active behavior difference,
  missing feature or workaround cannot be `✅`.

When the faithful C++ translation exposes a missing or incorrect API, fix the owning repository in
the **same sample session**:

| Gap belongs to | Required action |
|---|---|
| XNA/FNA behavior or API | Implement/fix it in `../cnanext`, add the required CNA tests and follow `../cnanext/AGENTS.md` plus `CHECKLIST.md`. |
| .NET `System.*` behavior or primitive/runtime type | Implement/fix it in `../sharp-runtimenext`, add its required tests and follow that repository's `AGENTS.md`. |
| Sample translation/content | Fix it in `cna-samples`; do not hide a framework/runtime gap here. |

One session may therefore change all three repositories. Use the same sample task ID in their
commit messages and record the resulting commit hashes in the sample row or its audit report.

## Definition of done for one sample

A row may become `✅` only after all applicable gates pass:

1. **Classification:** inspect the physical upstream directory and record whether it is a runnable
   XNA game, tool, library, training sequence, platform variant, asset pack or documentation. Never
   accept the old classification without reopening the files.
2. **Original evidence:** select the correct original project/configuration, build and run it when
   runnable, record exact commands/environment, controls, screenshots or frame captures, audio and
   important state transitions. If it truly cannot execute, record file-level evidence and the
   exact external dependency.
3. **Complete translation:** audit every C# source file and relevant content item. Port the complete
   selected original, including all overload use, screens and optional branches exercised by that
   project.
4. **Zero workarounds:** run a targeted review for bypasses, substitutions, invented behavior and
   stale `missing.md` deviations. Fix CNA or sharp-runtime gaps in their own repositories.
5. **Native verification:** configure/build the sample against `../cnanext` +
   `../sharp-runtimenext`, run it, and compare it with the original reference. Build and run relevant
   CNA/sharp-runtime tests for every dependency change.
6. **Web verification:** produce `.html`, `.js`, `.wasm` and any `.data`/content artifacts with
   Emscripten; serve them over HTTP; run in a real browser; verify rendering, input, audio/content
   loading and a representative interaction path. A successful link or Node-only run is not enough.
7. **Documentation and status:** update the sample's `missing.md` with either `No known differences`
   plus evidence or the still-open differences; update this row and any historical gap documents;
   commit all touched repositories by explicit file list.

Every ported sample must keep its web bundle reproducible. The eventual publication location is not
part of this task, but the generated bundle and instructions must be suitable for copying to the
owner's website without source edits.

## Status legend

| Symbol | Meaning |
|---|---|
| ⬜ | Fresh audit not started; historical status is untrusted. |
| 🔎 | Source/reference audit in progress. |
| 🛠 | Port or cross-repository fix in progress. |
| 🛑 | Audited far enough to require an owner decision before a large new subsystem or scope expansion. |
| ✅ | All native, fidelity, no-workaround and real-browser gates passed. |
| ↗ | Managed by a separate owner-approved plan; do not modify it here. |

## Baseline artifact inventory

This describes only what exists before the fresh audit; it is not a completion count.

| Existing artifact state | Count |
|---|---:|
| Has sample source and `CMakeLists.txt` | 63 |
| Placeholder directory without source/build target | 23 |
| No `samples/` directory | 67 |
| **Total upstream directories** | **153** |

## Foundation tasks

| Task | Status | Work |
|---|---|---|
| SAMPLES-INFRA-001 | ✅ | Merge the two former plans into this lowercase `plan.md`, move all 153 upstream directories into individual rows and remove the duplicate plan from `cnanext`. |
| SAMPLES-INFRA-002 | ⬜ | Change the development build to `add_subdirectory(../cnanext CNA_BUILD)` and explicitly set `CNA_SHARP_RUNTIME_ROOT=../sharp-runtimenext`; remove stale direct assumptions about the monolithic `SHARP_RUNTIME` target and prove one native sample configure/build. |
| SAMPLES-INFRA-003 | ⬜ | Reconcile `README.md`, `CLAUDE.md`, `NEXT.md`, `DEFERRED.md`, `ignored.md`, root `missing.md` and per-sample guidance with this plan: new dependency paths, no permanent-ignore authority, no F1 requirement, no workaround acceptance and lowercase plan links. Preserve useful historical evidence. |
| SAMPLES-INFRA-004 | ⬜ | Add an inventory validator that compares the 153 physical upstream directories with exactly 153 unique `SAMPLE-nnn` rows and reports added, removed, renamed or duplicate sources. |
| SAMPLES-INFRA-005 | ⬜ | Rediscover and document the prior direct Linux-side original-XNA build/run route. Also document the Win7/VS2010/XNA VM fallback, repair its host prerequisite when authorized, and create a repeatable capture checklist. |
| SAMPLES-INFRA-006 | ⬜ | Qualify the existing Emscripten `EASYGL`/WebGL2 path with one small sample in a real browser; fix content preloading, target naming and local HTTP smoke automation, then turn it into the required per-sample web gate. Do not claim browser WebGPU support. |
| SAMPLES-INFRA-007 | ⬜ | Add a per-sample audit template and mechanical scans for known bypass patterns (`RawMesh`/`RawModel`, direct `SetData` content substitutes, NOXNA graphics helpers, sidecars, invented input, omitted/simplified branches). Scans support, but do not replace, line-by-line review. |

## Owner decision queue — ask when the owner is at the computer

Auditing and small fixes continue without asking. When one of these is confirmed by a concrete
sample, stop before implementing the large subsystem, mark the affected row `🛑`, present measured
scope/options and ask the owner. Do not silently reintroduce a sample workaround.

| Decision | Current evidence to verify | Owner choice needed |
|---|---|---|
| SAMPLES-DEC-001 — Native/custom `.fx` path | CNA has custom-effect support and several translated shader implementations, but direct XNA `.fx`/compiled-effect content loading may still be absent. Many shader samples are placeholders. | Choose a reusable faithful effect ingestion/translation strategy versus the exact bounded alternative supported by evidence. Do not hand-wire effect behavior inside each sample. |
| SAMPLES-DEC-002 — XNA content-pipeline extensibility | `CustomModelEffect`, `ModelImporterSample`, `ContentManifestExtensions`, `StockEffects` and other tool/pipeline projects may require importer/processor/writer/MSBuild behavior, not just runtime asset loading. | Decide whether to implement the measured pipeline subset, port a tool as a tool, or explicitly end that audited row after accepting a documented scope boundary. |
| SAMPLES-DEC-003 — Skeletal/animation completeness | Four placeholders and the omitted `ReachGraphicsDemo::SkinnedDemo` historically cite partial skeletal animation support. | After retesting current `cnanext`, approve any remaining large model/content/animation work if it is more than a bounded bug fix. |
| SAMPLES-DEC-004 — Retired platform services and hardware | Avatar/Xbox LIVE, Bing Maps, invites/presence, GPS, push notifications and WP7 media APIs were previously skipped or replaced. Faithful ports cannot use substitutes under this plan. | Decide per capability cluster whether to build a real/emulated platform contract, retain a target-specific original project, or accept an evidence-backed non-port conclusion. |
| SAMPLES-DEC-005 — Tools, WinForms, Silverlight, older XNA and duplicate variants | Forty-two directories were previously grouped away, and several numbered entries are tools/libraries rather than games. They now receive individual audits. | Decide after audit whether the project expands beyond XNA 4.0 games to faithful tool/library/older-version ports; never reject them merely because the old plan did. |
| SAMPLES-DEC-006 — Web feature gaps | Every successful port now requires a real browser build. XACT, microphone/network services, large content, persistent storage or a WebGL2 limitation may expose substantial work. | Choose the browser support boundary or authorize the measured CNA/sharp-runtime work; native-only completion is not allowed by default. |
| SAMPLES-DEC-007 — Original-reference environment | The local source has hundreds of projects and prior real-XNA evidence, but the earlier Linux route is not documented and the Win7 VM driver is currently unavailable. | Confirm the preferred reference route and any host/VM changes once the discovery task reports concrete options. |

## Per-directory audit queue

`Existing` means only the artifact state observed on 2026-08-22: `port` has a sample
`CMakeLists.txt`, `placeholder` has a directory but no implementation, and `absent` has no sample
directory. `Focus` preserves useful findings from the old plans without accepting their verdicts.

### SAMPLE-001–030 — foundation, 2D and gameplay

| Task | Upstream directory | Existing | Fresh audit focus | Status |
|---|---|---|---|---|
| SAMPLE-001 | `PrimitivesSample_4_0` | port | Establish the full native/original/web audit pattern. | ⬜ |
| SAMPLE-002 | `Primitives3DSample_4_0` | port | Remove the dummy-UV/`VertexPositionNormalTexture` workaround; fix CNA if the original vertex type/path is missing. | ⬜ |
| SAMPLE-003 | `TexturesAndColorsSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-004 | `StockEffectsSample_4_0` | absent | Reinspect the effect compiler/tool project; route a large pipeline decision to DEC-001/002. | ⬜ |
| SAMPLE-005 | `ReachGraphicsDemo_4_0` | port | Restore `SkinnedDemo`; remove model/cubemap/background/dual-texture bypasses; inspect mesh winding. | ⬜ |
| SAMPLE-006 | `SpriteEffectsSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-007 | `SpriteSheetSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-008 | `ShapeRenderingSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-009 | `InputReporter_4_0` | port | Verify every input state/control and browser mapping. | ⬜ |
| SAMPLE-010 | `InputSequenceSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-011 | `SafeAreaSample_4_0` | port | Verify viewport/safe-area behavior natively and in a browser canvas. | ⬜ |
| SAMPLE-012 | `GeneratedGeometrySample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-013 | `Platformer_4_0` | port | Full game parity; MonoGame is supplemental only. | ⬜ |
| SAMPLE-014 | `Spacewar_4_0` | placeholder | Re-evaluate models, custom effects, render targets and XACT; route large gaps to DEC-001/006. | ⬜ |
| SAMPLE-015 | `TicTacToe_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-016 | `BounceSample_4_0` | port | Full original/native/web parity; verify recorded differences are original behavior, not simplifications. | ⬜ |
| SAMPLE-017 | `CollisionSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-018 | `PerPixelCollisionSample_4_0` | port | Verify pixel data/readback fidelity and browser behavior. | ⬜ |
| SAMPLE-019 | `RectangleCollisionSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-020 | `TransformedCollisionSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-021 | `PathDrawing_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-022 | `Pathfinding_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-023 | `WaypointSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-024 | `FlockingSample_4_0` | port | Reconcile any deliberately unreproduced original behavior with the zero-deviation gate. | ⬜ |
| SAMPLE-025 | `ChaseAndEvadeSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-026 | `AimingSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-027 | `FuzzyLogicSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-028 | `ColorReplacementSample_4_0` | placeholder | Recheck the `ReplaceColor.fx` path against current CNA; route systemic work to DEC-001. | ⬜ |
| SAMPLE-029 | `ParticleSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-030 | `CameraShake_4_0` | port | Compare tank-family mesh winding, bone transforms and depth behavior with real XNA. | ⬜ |

### SAMPLE-031–060 — effects, 3D, models, animation and audio

| Task | Upstream directory | Existing | Fresh audit focus | Status |
|---|---|---|---|---|
| SAMPLE-031 | `BloomSample_4_0` | placeholder | Port the complete effect pipeline through public CNA APIs; prove multi-pass output and web parity. | ⬜ |
| SAMPLE-032 | `DistortionSample_4_0` | placeholder | Reuse already-proven CNA shader capability without sample-local wiring; port all used techniques. | ⬜ |
| SAMPLE-033 | `NonPhotoRealisticSample_4_0` | placeholder | Port every technique and post-process stage; compare exact selectable modes. | ⬜ |
| SAMPLE-034 | `NormalMappingSample_4_0` | placeholder | Port the original tangent-space vertex/content path and effect. | ⬜ |
| SAMPLE-035 | `PerPixelLightingSample_4_0` | placeholder | Port every lighting mode/effect combination and validate selection behavior. | ⬜ |
| SAMPLE-036 | `VertexLightingSample_4_0` | placeholder | Port both original effects/techniques and validate flat/directional lighting. | ⬜ |
| SAMPLE-037 | `RimLighting_4_0` | port | Remove direct `TextureCube`/`Model` construction bypasses; use `Content.Load<T>()`. | ⬜ |
| SAMPLE-038 | `ShadowMappingSample_4_0` | placeholder | Port both shadow-map passes through the normal effect/render-target API. | ⬜ |
| SAMPLE-039 | `BillboardSample_4_0` | placeholder | Port custom vertex layout, processor output, wind and alpha-test behavior. | ⬜ |
| SAMPLE-040 | `InstancedModelSample_4_0` | placeholder | Port actual hardware-instancing behavior and compare counts/motion/performance semantics. | ⬜ |
| SAMPLE-041 | `LensFlareSample_4_0` | port | Verify occlusion/color-write behavior and remove any stale model/tool bypass. | ⬜ |
| SAMPLE-042 | `ShatterEffectSample_4_0` | placeholder | Port processor-generated per-triangle data and full shatter effect. | ⬜ |
| SAMPLE-043 | `Particles3DSample_4_0` | placeholder | Port the complete GPU particle system, including lifecycle and custom vertex data. | ⬜ |
| SAMPLE-044 | `Particles2DPipeline_4_0` | port | Audit content-pipeline semantics and generated particle settings, not just rendering. | ⬜ |
| SAMPLE-045 | `XmlParticles_4_0` | placeholder | Port XML content loading plus the shared 3D particle system. | ⬜ |
| SAMPLE-046 | `Graphics3DSample_4_0` | port | Remove port-only component initialization if absent in C#; fix CNA lifecycle only if behavior differs. | ⬜ |
| SAMPLE-047 | `PickingSample_4_0` | port | Audit component lifecycle, model loading and picking against the original. | ⬜ |
| SAMPLE-048 | `TrianglePickingSample_4_0` | port | Remove the generated picking sidecar; use faithful buffer/content APIs. | ⬜ |
| SAMPLE-049 | `HeightmapCollisionSample_4_0` | port | Verify terrain generation, texture/model path and collision values. | ⬜ |
| SAMPLE-050 | `SimpleAnimation_4_0` | port | Pixel/frame compare with real XNA; recheck tank winding, bones, default depth/blend and asset conversion. | ⬜ |
| SAMPLE-051 | `CustomModelAnimation_4_0` | placeholder | Re-test current skeletal/model support; route a confirmed large gap to DEC-003. | ⬜ |
| SAMPLE-052 | `CustomModelClassSample_4_0` | port | Check tank/model-family winding, near-plane and model-loader bypasses. | ⬜ |
| SAMPLE-053 | `CustomModelEffectSample_4_0` | placeholder | Audit its chained content processors; route systemic work to DEC-002. | ⬜ |
| SAMPLE-054 | `SkinningSample_4_0` | placeholder | Canonical end-to-end skeletal animation reference; route remaining large work to DEC-003. | ⬜ |
| SAMPLE-055 | `SkinnedModelExtensions_4_0` | placeholder | Port complete extension/content/animation behavior after canonical skinning support. | ⬜ |
| SAMPLE-056 | `CPUSkinningSample_4_0` | placeholder | Port the CPU skinning algorithm and exact data pipeline; do not replace with GPU skinning. | ⬜ |
| SAMPLE-057 | `InverseKinematics_4_0` | port | Remove raw model/avatar substitutes and verify the full original IK paths. | ⬜ |
| SAMPLE-058 | `ChaseCamera_4_0` | port | Remove raw model bypasses; verify camera dynamics and original input paths. | ⬜ |
| SAMPLE-059 | `Audio3DSample_4_0` | port | Compare positional audio, looping and listener/emitter behavior; pass browser audio gate. | ⬜ |
| SAMPLE-060 | `SoundAndMusic_4_0` | port | Compare sound/music state transitions and browser user-gesture behavior. | ⬜ |

### SAMPLE-061–083 — full games, UI and advanced samples

| Task | Upstream directory | Existing | Fresh audit focus | Status |
|---|---|---|---|---|
| SAMPLE-061 | `MarbleMaze_4_0` | port | Audit the selected original exercise endpoint, all game states, model conversion and controls. | ⬜ |
| SAMPLE-062 | `NetRumble_4_0` | placeholder | Port full networking plus all four effects; verify multi-peer native behavior and web feasibility. | ⬜ |
| SAMPLE-063 | `HoneycombRush_4_0` | port | Full game/original/native/web parity. | ⬜ |
| SAMPLE-064 | `HoneycombRushTrainingKit_4_0` | absent | Reinspect every exercise/starter/end project; old “redundant” verdict is not accepted. | ⬜ |
| SAMPLE-065 | `NinjAcademy_4_0` | port | Full game/original/native/web parity. | ⬜ |
| SAMPLE-066 | `ShipGame_4_0` | placeholder | Port the whole game and all distinct effects, including point sprites; supplemental MonoGame only. | ⬜ |
| SAMPLE-067 | `CatapultWars_4_0` | port | Full game/original/native/web parity. | ⬜ |
| SAMPLE-068 | `CatapultWarsTrainingKit_4_0` | absent | Reinspect all exercises and advanced project; old “redundant” verdict is not accepted. | ⬜ |
| SAMPLE-069 | `CardsStarterKit_4_0` | port | Audit the complete game/framework, AI, screens and rules; no omitted modes. | ⬜ |
| SAMPLE-070 | `RolePlayingGame_4_0_Win_Xbox` | port | Restore simplified combat/screens and any animated content; compare the complete game. | ⬜ |
| SAMPLE-071 | `Yacht_4_0` | port | Full game/original/native/web parity. | ⬜ |
| SAMPLE-072 | `GSMSample_4_0_WIN_XBOX` | port | Audit the complete Game State Management sample and original project selection. | ⬜ |
| SAMPLE-073 | `SoccerPitchSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-074 | `TankOnAHeightMapSample_4_0` | placeholder | Port multi-part tank bones and terrain; compare winding/depth with `SAMPLE-030/050/076`. | ⬜ |
| SAMPLE-075 | `NGSMSample_4_0` | absent | Reinspect lobby/session code and its documented empty gameplay path; do not dismiss before audit. | ⬜ |
| SAMPLE-076 | `SplitScreenSample_4_0` | placeholder | Port multi-part tank bones and split viewports; remove historical asset-generation gaps. | ⬜ |
| SAMPLE-077 | `DynamicMenu_4_0` | port | Full menu/navigation/original/native/web parity. | ⬜ |
| SAMPLE-078 | `LocalizationSample_4_0` | port | Verify cultures/resources/font/text behavior across native and browser builds. | ⬜ |
| SAMPLE-079 | `GesturesSample_4_0` | port | Verify every gesture and browser pointer/touch mapping. | ⬜ |
| SAMPLE-080 | `TouchThumbsticksSample_4_0` | port | Verify multi-touch controls natively and on real touch-capable browser input. | ⬜ |
| SAMPLE-081 | `PerformanceMeasuringSample_4_0` | port | Preserve timing/query semantics; compare displayed measurements structurally. | ⬜ |
| SAMPLE-082 | `UISample_4_0` | port | Full UI/control/original/native/web parity. | ⬜ |
| SAMPLE-083 | `SnowShovelSample_4_0` | port | Full original/native/web parity. | ⬜ |

### SAMPLE-084–111 — phone, service, networking and tool candidates

| Task | Upstream directory | Existing | Fresh audit focus | Status |
|---|---|---|---|---|
| SAMPLE-084 | `AccelerometerSample_4_0` | port | Verify the original emulator keyboard branch and real sensor path; remove only port-invented behavior. | ⬜ |
| SAMPLE-085 | `AvatarAnimationBlendingSample_4_0` | absent | Fresh API/content audit; no substitute avatar body. Route retired-service scope to DEC-004. | ⬜ |
| SAMPLE-086 | `AvatarMultipleAnimationsSample_4_0` | absent | Fresh API/content audit; no substitute avatar body. Route retired-service scope to DEC-004. | ⬜ |
| SAMPLE-087 | `AvatarShadows_4_0` | absent | Fresh API/content audit; no substitute avatar body. Route retired-service scope to DEC-004. | ⬜ |
| SAMPLE-088 | `BingMaps_4_0` | absent | Reinspect API/service dependencies and any offline-testable logic; route scope to DEC-004. | ⬜ |
| SAMPLE-089 | `BingMapsPathFinding_4_0` | absent | Reinspect mapping/path logic and service dependencies; route scope to DEC-004. | ⬜ |
| SAMPLE-090 | `BitmapFontMaker_4_0` | absent | Audit as a WinForms tool rather than a game; route faithful tool scope to DEC-002/005. | ⬜ |
| SAMPLE-091 | `ClientServerSample_4_0` | port | Remove GamerServices/network identity/event workarounds now that CNA claims fixes; test two processes. | ⬜ |
| SAMPLE-092 | `ContentManifestExtensions_4_0` | absent | Audit the actual library/tool outputs and processor contracts; route to DEC-002. | ⬜ |
| SAMPLE-093 | `CurveEditor_4_0` | absent | Audit as a WinForms curve tool and identify reusable XNA `Curve` behavior; route to DEC-005. | ⬜ |
| SAMPLE-094 | `CustomAvatarAnimation_4_0` | absent | Fresh API/content audit; no substitute avatar body. Route retired-service scope to DEC-004. | ⬜ |
| SAMPLE-095 | `GeolocationSample_4_0` | absent | Audit platform abstraction and emulator behavior; no invented GPS data. Route to DEC-004. | ⬜ |
| SAMPLE-096 | `InvitesSample_4_0` | absent | Audit the exact Xbox LIVE account/invite/presence contract; route to DEC-004. | ⬜ |
| SAMPLE-097 | `MemoryMadnessLab_4_0` | absent | Inspect every lab stage/document and identify runnable endpoints before classification. | ⬜ |
| SAMPLE-098 | `MicrophoneEchoSample_4_0` | port | Compare microphone buffering/latency and pass real-browser permission/gesture testing. | ⬜ |
| SAMPLE-099 | `ModelImporterSample_4_0` | absent | Audit importer UI/content behavior and route the measured pipeline/tool scope to DEC-002. | ⬜ |
| SAMPLE-100 | `NetworkPredictionSample_4_0` | port | Remove the explicit options-packet substitute by implementing faithful `SessionProperties` behavior in CNA. | ⬜ |
| SAMPLE-101 | `ObjectPlacementOnAvatarSample_4_0` | absent | Fresh API/content audit; no substitute avatar body. Route retired-service scope to DEC-004. | ⬜ |
| SAMPLE-102 | `Orientation_4_0` | port | Confirm the old phone-hardware classification was wrong; full orientation/native/web parity. | ⬜ |
| SAMPLE-103 | `PeerToPeerSample_4_0` | port | Verify genuine peer topology, events and two/multi-process behavior plus browser feasibility. | ⬜ |
| SAMPLE-104 | `PerformanceUtility_4_0` | absent | Audit as a reusable library; identify dependent samples and faithful testable output. | ⬜ |
| SAMPLE-105 | `PushNotificationsSample_4_0` | absent | Audit exact WP7 service/emulator contracts; route faithful platform scope to DEC-004. | ⬜ |
| SAMPLE-106 | `SavingEmbeddedImages_4_0` | absent | Audit exact media-library behavior and assets; route faithful platform scope to DEC-004. | ⬜ |
| SAMPLE-107 | `TiltPerspective_4_0` | port | Remove the invented keyboard-tilt scheme; preserve original sensor/fallback behavior exactly. | ⬜ |
| SAMPLE-108 | `WinFormsContentSample_4_0` | absent | Audit XNA-in-WinForms hosting/content behavior; route faithful tool/hosting scope to DEC-005. | ⬜ |
| SAMPLE-109 | `WinFormsGraphicsSample_4_0` | absent | Audit XNA-in-WinForms graphics hosting behavior; route faithful tool/hosting scope to DEC-005. | ⬜ |
| SAMPLE-110 | `WP7MusicManagement_4_0` | absent | Audit media-library behavior and emulator branches; route platform scope to DEC-004. | ⬜ |
| SAMPLE-111 | `XnaGraphicsProfileChecker_4_0` | absent | Audit the WinForms diagnostic and exact graphics-profile checks; route tool scope to DEC-005. | ⬜ |

### SAMPLE-112–153 — formerly grouped/ignored physical directories

These IDs are newly assigned in bytewise directory-name order. Do not renumber them after
publication; the validator introduced by `SAMPLES-INFRA-004` will pin the mapping.

| Task | Upstream directory | Existing | Historical classification to re-check | Status |
|---|---|---|---|---|
| SAMPLE-112 | `AvatarAnimPack_4_0_BIN` | absent | Avatar animation asset pack (binary). | ⬜ |
| SAMPLE-113 | `AvatarAnimPack_4_0_FBX` | absent | Avatar animation asset pack (FBX). | ⬜ |
| SAMPLE-114 | `AvatarAnimPack_4_0_Maya` | absent | Avatar animation/DCC asset pack (Maya). | ⬜ |
| SAMPLE-115 | `AvatarAnimPack_4_0_Mod_Tool` | absent | Avatar animation/DCC asset pack (Mod Tool). | ⬜ |
| SAMPLE-116 | `AvatarRig_4_0_Max_2010` | absent | Avatar DCC rig. | ⬜ |
| SAMPLE-117 | `AvatarRig_4_0_Maya_2009` | absent | Avatar DCC rig. | ⬜ |
| SAMPLE-118 | `AvatarRig_4_0_SoftImage_Mod_Tool7_5` | absent | Avatar DCC rig. | ⬜ |
| SAMPLE-119 | `BasicEffectShader_ARCHIVE_2_0` | absent | XNA 2.0 shader archive; inspect code/tool relevance. | ⬜ |
| SAMPLE-120 | `ButtonImages` | absent | Shared image resources; identify all consumers and completeness role. | ⬜ |
| SAMPLE-121 | `CardsStarterKit_4_0_VB` | absent | Visual Basic variant; compare for content/behavior absent from the C# port. | ⬜ |
| SAMPLE-122 | `Catapult_ARCHIVE_2_0` | absent | XNA 2.0 sample; compare with, but do not assume duplication of, CatapultWars. | ⬜ |
| SAMPLE-123 | `ControllerImages` | absent | Shared controller resources; identify all consumers and completeness role. | ⬜ |
| SAMPLE-124 | `CustomIndeterminateProgressBarSample` | absent | Silverlight control; inspect runnable/testable scope. | ⬜ |
| SAMPLE-125 | `GSMSample_4_0_Mango` | absent | Mango variant; compare line by line with desktop GSM. | ⬜ |
| SAMPLE-126 | `GSMSample_4_0_Mango_VB` | absent | Mango Visual Basic variant; inspect unique behavior/content. | ⬜ |
| SAMPLE-127 | `GSMSample_4_0_PHONE` | absent | Phone variant; inspect unique behavior/content. | ⬜ |
| SAMPLE-128 | `LevelStarterKit` | absent | Previously considered incomplete/Silverlight; inspect all files and runnable stages. | ⬜ |
| SAMPLE-129 | `LobbyChatImages` | absent | Shared lobby/chat resources; identify consumers and completeness role. | ⬜ |
| SAMPLE-130 | `MaterialsAndLights_ARCHIVE_2_0` | absent | XNA 2.0 graphics sample; inspect API/content value. | ⬜ |
| SAMPLE-131 | `Minjie_ARCHIVE_2_0` | absent | XNA 2.0 sample; inspect complete source and assets. | ⬜ |
| SAMPLE-132 | `ModelViewerDemo_4_0_Mango` | absent | Phone model viewer; inspect unique XNA behavior and controls. | ⬜ |
| SAMPLE-133 | `Movipa` | absent | Third-party video sample; inspect XNA/API/content relevance. | ⬜ |
| SAMPLE-134 | `MultipassLighting_ARCHIVE_2_0` | absent | XNA 2.0 effect sample; inspect reusable effect behavior. | ⬜ |
| SAMPLE-135 | `NonLinear-WP-SLApp-Navigation-Service` | absent | Silverlight navigation sample; inspect runnable/testable scope. | ⬜ |
| SAMPLE-136 | `PaddleBattle_4_0_Mango` | absent | Phone/Mango game; inspect as a distinct port candidate. | ⬜ |
| SAMPLE-137 | `PaddleBattle_4_0_Mango_VB` | absent | Visual Basic phone variant; compare unique content/behavior. | ⬜ |
| SAMPLE-138 | `Pickture_ARCHIVE_2_0` | absent | XNA 2.0 sample; inspect complete source and assets. | ⬜ |
| SAMPLE-139 | `PushRecipe_WP7_SL` | absent | WP7/Silverlight push sample; inspect service and emulator behavior. | ⬜ |
| SAMPLE-140 | `RedistributableTTFs_ARCHIVE_3_1` | absent | Font resource archive; identify sample consumers and licensing/build role. | ⬜ |
| SAMPLE-141 | `Riemers` | absent | Third-party tutorial collection; inventory individual runnable projects before scope decision. | ⬜ |
| SAMPLE-142 | `RobotGame_ARCHIVE_2_0` | absent | XNA 2.0 game sample; inspect complete source and assets. | ⬜ |
| SAMPLE-143 | `RolePlayingGame_4_0_Phone` | absent | Phone variant; compare for unique behavior/content missing from `SAMPLE-070`. | ⬜ |
| SAMPLE-144 | `SilverlightMicrophoneSample` | absent | Silverlight microphone sample; compare with XNA microphone APIs and web needs. | ⬜ |
| SAMPLE-145 | `SoundLab` | absent | Standalone audio authoring tool; inspect formats/workflows used by samples. | ⬜ |
| SAMPLE-146 | `SpaceShooter_ARCHIVE_3_0` | absent | XNA 3.0 game sample; inspect complete source and assets. | ⬜ |
| SAMPLE-147 | `SpriteBatchShader_ARCHIVE_2_0` | absent | XNA 2.0 shader sample; inspect effect/compiler relevance. | ⬜ |
| SAMPLE-148 | `TiledSpritesSample_ARCHIVE_3_1` | absent | XNA 3.1 sample; inspect complete source and assets. | ⬜ |
| SAMPLE-149 | `TombstoningSample` | absent | WP7/Silverlight lifecycle sample; inspect state-persistence behavior. | ⬜ |
| SAMPLE-150 | `UnitConverterStarterKit` | absent | Previously considered empty/incomplete; verify every file before classification. | ⬜ |
| SAMPLE-151 | `VectorRumble_ARCHIVE_2_0` | absent | XNA 2.0 game sample; inspect complete source and assets. | ⬜ |
| SAMPLE-152 | `XNA-4-Racing-Game-Kit-master` | absent | Governed only by `plan_racing.md`; do not change that plan here. | ↗ |
| SAMPLE-153 | `XNA_XNB_Format` | absent | XNB format documentation; assess relevance to faithful content loading/tooling. | ⬜ |

## Execution order

1. Finish `SAMPLES-INFRA-002` through `SAMPLES-INFRA-007` and prove the workflow with
   `SAMPLE-001`.
2. Audit one sample at a time. Prefer small existing ports first to expose systematic workaround
   patterns, then port currently-unblocked placeholders. Do not bulk-mark siblings complete.
3. When a sample exposes a bounded CNA or sharp-runtime defect, fix and test it immediately in the
   same session, then return to the sample and complete all gates.
4. When a genuinely large capability is confirmed, update the sample row with exact evidence,
   mark it `🛑`, add it to the decision queue and ask the owner when available. Continue with other
   independent rows meanwhile.
5. Revisit all historically absent/ignored directories individually. A final non-port conclusion
   requires fresh file-level evidence and, for a scope choice, an explicit owner decision.
6. Keep web verification inside every sample task rather than postponing 153 browser builds to a
   final cleanup phase.

## Session report and commit contract

At the end of each sample session report:

- upstream projects/files and reference runtime used;
- `cna-samples`, `cnanext` and `sharp-runtimenext` files changed;
- workarounds removed and framework/runtime gaps fixed;
- intentional C++/asset-container deviations still present;
- native builds/tests and real-browser result;
- remaining blocker or owner decision, if any;
- commit hash from every touched repository.

Stage only explicit task files. One sample task uses one commit per touched repository, with the
same `SAMPLE-nnn` identifier. Do not bundle unrelated cleanup and do not push unless the applicable
repository instructions and owner authorization require it.
