# CNA samples active handoff

Updated: 2026-10-03 after the owner completed the SAMPLE-001–153 review.

This is the current handoff required by `AGENTS.md`. `rules.md` remains binding and `plan.md` is
the status source of truth. The obsolete root `handoff.md`, which stopped at SAMPLE-130, was removed
by owner instruction. Its last revision remains available with `git show 4280df0:handoff.md`; the
long historical version of this file remains available with `git show 3d76f01:NEXT.md`.

## Active multi-renderer qualification override

The owner subsequently authorized the existing-sample multi-renderer qualification campaign. That
instruction supersedes only the older EasyGL-only renderer boundary below and in `rules.md`; all
source-fidelity, anti-workaround, visual-comparison, evidence and no-push rules remain binding.

The Linux native corpus now uses one executable per sample with CNA's runtime renderer selector.
The Windows preparation build cross-compiles `DIRECTX9`, `DIRECTX11`, `DIRECTX12`, `VULKAN`,
`WEBGPU`, `SDL_GPU` and `FNA3D` into the same executables. Cross-build success is not native Windows
GPU qualification. Current measured results and exact follow-up commands belong in `renderers.md`.

## Active handoff

There is no next unreviewed sample. Every physical upstream entry from SAMPLE-001 through
SAMPLE-153 has an owner-reviewed disposition. Do not start a port, renewed audit, build or cleanup
without a new owner instruction.

The synchronized implementation baseline is:

| Repository | Branch | Baseline HEAD |
|---|---|---|
| `cna-samples` | `develop` | `3d76f01509e86ce49612683fb8d32f70cbe26c90` |
| `../cna` | `next` | `db68149e31bcd80d3c54ca6517a578715563dc64` |
| `../sharp-runtime` | `next` | `db86514c5bb86a5886d8015b8e2916d49be04ae8` |

The samples baseline is pushed to `origin/develop`. This compact handoff is documentation after
that baseline; use `git rev-parse HEAD` for the current documentation commit.

### OPENGLES3 requalification against current CNA (2026-11-09)

Owner-ordered requalification of the whole directory against CNA `next` `679ab9d64` (C ABI
0.60.0) and SharpRuntime `next` `0abe8a91` (easy-gl `b582bd85f`, meta-gl `795bcdcfe`), on
cna-samples `develop` `662c33a`. One Release OPENGLES3 tree, `build/`, with CNA built once inside
it (`CNA_BUILD`); each sample's target was then built alone, run through the matrix runner, and
its outputs deleted once it passed. The captures and two contact sheets
(`build/captures-contact-sheet-{1,2}.png`) stay in `build/` for review.

```bash
export CCACHE_DIR=/rv/cnaccache CCACHE_BASEDIR=/rv
taskset -c 0-5 cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCNA_GRAPHICS_RENDERER=OPENGLES3 -DCNA_MAX_VENDORED_BUILD_JOBS=6 \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
taskset -c 0-5 cmake --build build --target cna_shared --parallel 6
taskset -c 0-5 cmake --build build --target <target> --parallel 6
env -u TMPDIR -u DISPLAY -u WAYLAND_DISPLAY -u SDL_VIDEODRIVER SDL_AUDIODRIVER=dummy SDL_ASSERT=abort \
  python3 tools/run_renderer_matrix.py --build-dir build --renderers OPENGLES3 --categories all \
  --samples <target> --observe-seconds 5 --capture-after-seconds 3 --output-dir build/requal/<target>
```

Result: all 107 targets of the 105 configured directories build. All 97 runnable targets (gallery,
native-only, partial and experimental) reach `AUTOMATED_PASS` with the expected first frame
(LensFlare with its owner-accepted occlusion-query limit); `CollisionSampleUnitTests` passes; the
nine support/cancelled targets are build-only. ShipGame and Spacewar first failed in `LoadContent`:
the ports had never declared their projects' HiDef profile, so they ran as Reach, and CNA's TQ-024
Reach validation of `RenderTarget2D` now refuses their mipmapped non-power-of-two targets, as XNA
does. Each now declares HiDef in `src/Properties/AssemblyInfo.cpp` (see their `diff.md`) and
passes. No CNA or SharpRuntime defect was found. Other ports also declare no profile and run as
Reach although an original project is HiDef: every project of AvatarShadows, InputReporter,
InverseKinematics, NetRumble and RolePlayingGame is HiDef, and about fifteen more pair a HiDef
Windows/Xbox project with a Reach phone project. They pass, so they were not changed; declaring
HiDef there needs the selected original project confirmed per sample and is an owner decision.

| Sample directory | Target | Category | Build | Run | Capture verdict | Failure cause / note |
|---|---|---|---|---|---|---|
| AccelerometerSample | `AccelerometerSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| AchievementsLeaderboards | `AchievementsLeaderboards_cna_samples` | experimental-project | pass | pass | expected content | sign-in hint screen (expected startup) |
| AimingSample | `AimingSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Audio3D | `Audio3D_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| AvatarAnimationBlending | `AvatarAnimationBlending_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| AvatarMultipleAnimations | `AvatarMultipleAnimations_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| AvatarShadows | `AvatarShadows_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| BillboardSample | `BillboardSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| BloomSample | `BloomSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Bounce | `Bounce_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| CameraShake | `CameraShake_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| CardsStarterKit | `CardsStarterKit_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| CatapultWars | `CatapultWars_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| CatapultWarsTrainingAllFeatures | `CatapultWarsTrainingAllFeatures_cna_samples` | cancelled (SAMPLE-068) | pass | build only | n/a | owner-cancelled; build only |
| CatapultWarsTrainingHealthBar | `CatapultWarsTrainingHealthBar_cna_samples` | cancelled (SAMPLE-068) | pass | build only | n/a | owner-cancelled; build only |
| CatapultWarsTrainingScrollingScreen | `CatapultWarsTrainingScrollingScreen_cna_samples` | cancelled (SAMPLE-068) | pass | build only | n/a | owner-cancelled; build only |
| CatapultWarsTrainingSecondHuman | `CatapultWarsTrainingSecondHuman_cna_samples` | cancelled (SAMPLE-068) | pass | build only | n/a | owner-cancelled; build only |
| CatapultWarsTrainingShotAngle | `CatapultWarsTrainingShotAngle_cna_samples` | cancelled (SAMPLE-068) | pass | build only | n/a | owner-cancelled; build only |
| CatapultWarsTrainingShotGuide | `CatapultWarsTrainingShotGuide_cna_samples` | cancelled (SAMPLE-068) | pass | build only | n/a | owner-cancelled; build only |
| CatapultWarsTrainingSupplyCrate | `CatapultWarsTrainingSupplyCrate_cna_samples` | cancelled (SAMPLE-068) | pass | build only | n/a | owner-cancelled; build only |
| ChaseAndEvade | `ChaseAndEvade_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| ChaseCamera | `ChaseCamera_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| ClientServerSample | `ClientServerSample_cna_samples` | native-runnable | pass | pass | expected content | CNA Guide sign-in prompt (expected startup) |
| CollisionSample | `CollisionSampleUnitTests_cna_samples` | test-project | pass | pass | n/a (console) | `Passed: 420000 Failed: 0` |
| CollisionSample | `CollisionSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| ColorReplacement | `ColorReplacement_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| ContentManifestExtensions | `ContentManifestExtensions_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| CPUSkinning | `CPUSkinning_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| CustomAvatarAnimation | `CustomAvatarAnimation_cna_samples` | gallery-runnable | pass | pass | expected content | ground only: documented empty-profile startup |
| CustomModelAnimation | `CustomModelAnimation_cna_samples` | gallery-runnable | pass | pass | expected content | idle HUD: documented startup (models play on A/B) |
| CustomModelClass | `CustomModelClass_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| CustomModelEffect | `CustomModelEffect_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| DistortionSample | `DistortionSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| DynamicMenu | `DynamicMenu_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| FlockingSample | `FlockingSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| FuzzyLogic | `FuzzyLogic_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| GameStateManagement | `GameStateManagement_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| GeneratedGeometry | `GeneratedGeometry_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| GesturesSample | `GesturesSample_cna_samples` | gallery-runnable | pass | pass | expected content | instructions only; sprites need touch input |
| Graphics3D | `Graphics3D_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| HeightmapCollision | `HeightmapCollision_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| HoneycombRush | `HoneycombRush_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| InputReporter | `InputReporter_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| InputSequence | `InputSequence_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| InstancedModel | `InstancedModel_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| InverseKinematics | `InverseKinematics_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Invites | `Invites_cna_samples` | experimental-project | pass | pass | expected content | CNA Guide sign-in prompt (expected startup) |
| LensFlare | `LensFlare_cna_samples` | gallery-runnable | pass | pass | known limitation | terrain only: owner-accepted OPENGLES3 boolean occlusion-query limit (`SAMPLES-DEC-010`) |
| LocalizationSample | `LocalizationSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| MarbleMaze | `MarbleMaze_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| MicrophoneEcho | `MicrophoneEcho_cna_samples` | gallery-runnable | pass | pass | expected content | idle recorder UI (dummy audio) |
| ModelImporterSample | `ModelImporterSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| NetRumble | `NetRumble_cna_samples` | native-runnable | pass | pass | expected content | title menu (Sign In / Exit) |
| NetworkPrediction | `NetworkPrediction_cna_samples` | experimental-project | pass | pass | expected content | CNA Guide sign-in prompt (expected startup) |
| NetworkStateManagement | `NetworkStateManagement_cna_samples` | experimental-project | pass | pass | expected content |  |
| NinjAcademy | `NinjAcademy_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| NonPhotoRealistic | `NonPhotoRealistic_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| NormalMappingEffect | `NormalMappingEffect_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| ObjectPlacementOnAvatar | `ObjectPlacementOnAvatar_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Orientation | `Orientation_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Particles2DPipeline | `Particles2DPipeline_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Particles3D | `Particles3D_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| ParticleSample | `ParticleSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| PathDrawing | `PathDrawing_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Pathfinding | `Pathfinding_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| PeerToPeer | `PeerToPeer_cna_samples` | experimental-project | pass | pass | expected content | CNA Guide sign-in prompt (expected startup) |
| PerformanceMeasuring | `PerformanceMeasuring_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| PerformanceUtility | `PerformanceUtility_cna_samples` | partial-runnable | pass | pass | expected content |  |
| PerPixelCollision | `PerPixelCollision_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| PerPixelLighting | `PerPixelLighting_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| PickingSample | `PickingSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Platformer | `Platformer_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Primitives3D | `Primitives3D_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| PrimitivesSample | `PrimitivesSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| ReachGraphicsDemo | `ReachGraphicsDemo_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| RectangleCollision | `RectangleCollision_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| RimLighting | `RimLighting_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| RolePlayingGame | `RolePlayingGame_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| SafeArea | `SafeArea_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| ShadowMapping | `ShadowMapping_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| ShapeRendering | `ShapeRendering_cna_samples` | gallery-runnable | pass | pass | expected content | CornflowerBlue only: faithful Release build, `[Conditional("DEBUG")]` draws removed |
| ShatterEffect | `ShatterEffect_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| ShipGame | `ShipGame_cna_samples` | gallery-runnable | pass | pass (after fix) | expected content | first run: Reach `RenderTarget2D` NPOT-mipmap `NotSupportedException` (exit -6); HiDef project profile now declared ([diff.md](samples/ShipGame/diff.md)); rerun passes, title screen |
| SimpleAnimation | `SimpleAnimation_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| SkinnedModelExtensions | `SkinnedModelExtensions_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| SkinningSample | `SkinningSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| SnowShovel | `SnowShovel_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| SoccerPitch | `SoccerPitch_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| SoundAndMusic | `SoundAndMusic_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Spacewar | `Spacewar_cna_samples` | gallery-runnable | pass | pass (after fix) | expected content | first run: Reach `RenderTarget2D` NPOT-mipmap `NotSupportedException` (exit -6); HiDef project profile now declared ([diff.md](samples/Spacewar/diff.md)); rerun passes, title screen |
| SplitScreen | `SplitScreen_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| SpriteEffects | `SpriteEffects_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| SpriteSheet | `SpriteSheet_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| StockEffects | `CompileEffect_cna_samples` | supporting-project | pass | build only | n/a | support tool; prints its usage |
| TankOnHeightmap | `TankOnHeightmap_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| TexturesAndColors | `TexturesAndColors_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| TiltPerspective | `TiltPerspective_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| TouchThumbsticks | `TouchThumbsticks_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| TransformedCollision | `TransformedCollision_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| TransformedCollisionTest | `TransformedCollisionTest_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| TrianglePicking | `TrianglePicking_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| UISample | `UISample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| VertexLighting | `VertexLighting_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| WaypointSample | `WaypointSample_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| XmlParticles | `XmlParticles_cna_samples` | gallery-runnable | pass | pass | expected content |  |
| Yacht | `YachtServer_cna_samples` | supporting-project | pass | build only | n/a | server companion; build only |
| Yacht | `Yacht_cna_samples` | gallery-runnable | pass | pass | expected content | title menu; the 2026-10 gamer-services transport failure no longer occurs |
| AvatarAnimPackBIN | — | not built | — | — | — | SAMPLE-112 deferred; licensed assets only, no target |
| AvatarAnimPackFBX | — | not built | — | — | — | SAMPLE-113 deferred; licensed assets only, no target |
| MemoryMadnessLab | — | not built | — | — | — | SAMPLE-097 deferred; no port |
| NormalMapping | — | not built | — | — | — | stale placeholder; SAMPLE-034 is the `NormalMappingEffect` port |
| RacingGame | — | not built | — | — | — | SAMPLE-152; governed by `plan_racing.md`, not in the root project |
| RacingGameHarness | — | not built | — | — | — | Racing diagnostic harness; governed by `plan_racing.md` |
| RolePlayingGamePhone | — | not built | — | — | — | SAMPLE-143 deferred; no port |
| SavingEmbeddedImages | — | not built | — | — | — | SAMPLE-106 deferred; no port |
| XnaXnbFormat | — | not built | — | — | — | SAMPLE-153 cancelled; reference package, no target |

### Final sample inventory

| Status | Count | Current scope |
|---|---:|---|
| ✅ complete | 91 | Qualified under the exact owner-approved scopes recorded in `plan.md` and each `missing.md`. |
| 🗑 deleted | 49 | Permanently removed from all three sample locations; names remain in `plan.md`. |
| ⏸ deferred | 8 | SAMPLE-096, 097, 100, 103, 106, 112, 113 and 143. |
| ⛔ cancelled | 3 | SAMPLE-068, 075 and 153 remain physically retained. |
| 🟡 partial | 1 | SAMPLE-104 PerformanceUtility; local Web product works, browser networking does not. |
| ↗ separate plan | 1 | SAMPLE-152 Racing Game; governed only by `plan_racing.md`. |
| **Total** | **153** | No row is unreviewed, pending or actively being ported. |

`plan.md` must be recounted from its SAMPLE rows after every future status change. Do not infer
completion from an old narrative paragraph.

## Work that remains available only after an owner decision

### SAMPLE-152 Racing Game

Racing is the only separately active product and must be handled through `plan_racing.md` and
`samples/RacingGame/missing.md`.

- It is the canonical C# XNA 4.0 Racing Game Kit, translated as a complete C++ CNA game.
- Linux OPENGL33 is the feature-complete reference. Task `RACING-MULTI-001` additionally qualifies
  one native executable on OPENGLES3, OPENGL33, Vulkan, WebGPU, SDL_GPU and FNA3D through captured
  drive/menu/device-reset probes and bounded product startup. It remains directly runnable and uses
  authentic XNA4 content; audible XACT is not claimed by the dummy-audio test environment.
- Current Chrome WEBGL2 startup/menu/race smoke passes. Full-race persistence, progressive cache,
  context loss, resize/fullscreen and touch evidence also exists.
- Windows runtime qualification remains open.
- Physical Android device qualification remains open: touch-only race, ergonomics, performance,
  thermal/memory behavior, lifecycle/context loss, audible XACT, persistence and gamepad coexistence.
- Web hosted-network/residency, audible XACT and browser/device-matrix gates remain open.
- Playable publication is blocked by the missing canonical redistribution licence. The public
  gallery intentionally contains only information, an in-game screenshot, the `cna-samples`
  source link and the YouTube video `https://www.youtube.com/watch?v=bTThYkZJzYw`.
- No dedicated Racing repository is planned. The retained Web bundle is not published.
- `RACING_GAME_TURBO` is an owner-approved compile-time option, off by default and outside the
  fidelity baseline.

### Other retained non-complete rows

| Samples | Reason retained |
|---|---|
| SAMPLE-096, 100, 103 | Faithful networking ports wait for a settled CNA account/browser multiplayer scope and real peer qualification. |
| SAMPLE-097 | Two distinct Memory Madness Phone products; a future faithful port must retain both. |
| SAMPLE-104 | Owner-approved partial release. Finish only through general CNA browser authentication, discovery/hosting, address handoff and realtime transport. |
| SAMPLE-106 | Deferred general CNA Guide/modal-frame/`MediaLibrary.SavePicture` fixes; detailed 6–10 hour checklist is in its `missing.md`. Porting the sample remains a later decision. |
| SAMPLE-112, 113 | Licensed Xbox Avatar BIN/FBX support assets without a standalone application or authorized decoder/viewer product. |
| SAMPLE-143 | Materially distinct XNA4 Phone generation of the completed Role Playing Game; retained for a possible future variant port. |
| SAMPLE-068, 075, 153 | Owner-cancelled and still physically retained. Deletion or pruning requires a new explicit instruction. |

No work on these rows is implied by their retention.

## Owner review rules established by this campaign

- Start every sample with a short language, XNA version, platform, purpose and options
  classification. Report XNA 2.x/3.x and Visual Basic immediately.
- Wait for explicit approval before a detailed audit, build, run, test, migration or implementation.
- “Analyze next” does not authorize changes to the previously analyzed sample.
- A requested deletion means removing the sample from:
  `/rv/tmp/XNAGameStudio/Samples`, this repository's `samples/`, and `/rv/tmp/samples`.
  Preserve its repository name in `plan.md` as
  `| SAMPLE-NNN | — | RepositoryName | deleted | 🗑 |`.
- Never infer cancellation, deletion, pruning or acceptance from an old document. Only the owner
  makes those decisions.
- Preserve unrelated working-tree changes. Commit completed work by explicit file list; push only
  when the owner has authorized it.

## Reusable techniques

### Decode the XNB reader table first

Run `tools/dump-xnb-readers.py <file.xnb>` before implementing a loader or registering types. The
complete reader table must resolve before any object is read, so one missing reader can invalidate
the whole asset. Use the actual table instead of guessing from the root type.

### Build sample-owned content processors faithfully

Compile a sample's processor assembly with the installed XNA toolchain and pass it to
`BuildContent` through `PipelineAssemblies`. If serialized types live in a game/library assembly,
compile and pass that assembly too; the XNB records the reader's assembly-qualified identity.
Use `ProcessorParameters_<Name>` item metadata for per-asset processor settings. The retained
SAMPLE-051 `build-original.sh` and `XnaPipelineRunner.cs` are the most complete precedent.

### Use a frozen diagnostic pair

Keep the production source unchanged and isolate deterministic hooks in paired XNA/CNA diagnostic
copies. Within each leg, repeated frames must be byte-identical; between different pinned legs,
hashes and geometry metrics must change. Restore production source on exit. Compare structured data
before pixels and parse numeric dumps back to `float32` rather than comparing C# and C++ number
spellings.

### Qualify the real products

- Build the unchanged original with its declared profile and real content pipeline when possible.
- Build static Release OPENGLES3 for normal sample qualification unless a documented scope selects
  another renderer.
- Build the complete nonthreaded Release WEBGL2 bundle and run it in system Google Chrome over HTTP.
- Exercise representative input, state transitions, audio and clean exit. A successful link,
  Node-only execution or first frame is insufficient.
- Crop browser chrome/focus borders before pixel comparison and hold synthetic key presses across
  at least one polled frame.
- Calibrate browser gates by deliberately breaking the behavior they claim to detect.

### Run CNA tests without chasing inherited failures

Take a focused baseline at the current CNA head before changing it; old failure counts in historical
handoffs are not current authority. Run graphics tests with Xvfb and the test binary in the same
command so the display survives. Run fixture-loading tests from the CNA repository root. Use suite
filters, rerun failures serially/in isolation, and compare before/after results before attributing a
failure to the new work.

### Put fixes in the owning layer

Framework behavior belongs in `../cna`; .NET behavior belongs in `../sharp-runtime`; sample code
must not emulate missing framework behavior with private transports, fake profiles, replacement
renderers, invented screens or reduced products. Read the dependency repository's `AGENTS.md` and
checklist before changing it.

### Keep artifacts and pruning explicit

Use one stable root: `/rv/tmp/samples/SAMPLE-NNN-<upstream>/`. Keep original snapshots, official
products, scripts, evidence, manifests and final runnable products there. Build intermediates are
reproducible artifacts, but pruning still requires an owner instruction and the repository prune
workflow. Never call cleanup completion or qualification.

## Resume checklist

1. Read `rules.md`, this Active handoff, `plan.md`, and the selected row's `missing.md`/`diff.md`.
2. Confirm the three repository branches and heads; do not overwrite concurrent work.
3. Identify the owner's exact new scope. There is no default next sample.
4. For Racing, read `RACING-CONTENT-SOURCE-POLICY.md` before any content/model work and use only
   the original `.X` → XNA pipeline → XNB route. GLB/glTF is forbidden for Racing content.
5. Record current evidence and update `plan.md` and the affected `missing.md` before committing.

Historical chronology is intentionally omitted here. Git history, per-sample evidence and
`git show 3d76f01:NEXT.md` retain it.
