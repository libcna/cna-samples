# Racing Game API and dependency compatibility matrix

## Method and baselines

This delta matrix uses the modern primary source
`/rv/tmp/RacingGame@d8092633e4e43e014ff168d8e913a9373538b851`,
the older XNA 4 conversion for comparison, `../cna@ac3aaaeb2a5ba27d…`,
and newer integration evidence `../cnaintegration@4ac696c748fb18e…`.

Status vocabulary:

- **CNA ALREADY HAS EQUIVALENT** — usable API/behavior exists, subject to game
  integration tests;
- **SMALL CNA GAP** — bounded reusable correction or format/layout extension;
- **PORT-SIDE WRAPPER** — belongs in Racing, not CNA;
- **NEW GENERAL CNA FEATURE WOULD BE USEFUL** — reusable boundary justified by
  Racing and other consumers;
- **THIRD-PARTY DEPENDENCY STILL REQUIRED** — only when replacement is not
  established;
- **UNKNOWN** — requires a focused runtime/platform/provenance experiment;
- **SUPERSEDED** — old primary-route requirement removed by modern evidence.

## Core, lifecycle, math, and gameplay

| Modern requirement | Source usage | Classification | Future action |
|---|---|---|---|
| `Game`, `GameTime`, `GameComponent`, window/device lifecycle | `BaseGame`, manager, screens | CNA ALREADY HAS EQUIVALENT | Minimal pinned `OPENGL33` lifecycle/resize harness first |
| vectors, matrices, quaternions, planes, rays, bounds | physics, track, camera, render code | CNA ALREADY HAS EQUIVALENT | Differential tests for conventions and floating-point drift |
| C# collections/events/delegates/properties | throughout | PORT-SIDE WRAPPER | Idiomatic C++/sharp-runtime usage; preserve observable order/lifetime |
| `System.Text.Json` | `.material` adapter | PORT-SIDE WRAPPER | Typed Racing JSON schema with CNA/project JSON dependency; no API emulation |
| reflection/serialization | settings XML only | PORT-SIDE WRAPPER | Explicit settings/replay serializers |
| Windows Forms/GamerServices | removed from modern route | SUPERSEDED | Do not restore |

## Content and raw asset loading

| Requirement/dependency | Actual modern use | Classification | Exact delta/action |
|---|---|---|---|
| XNA `ContentManager` + `.x`/`XImporter` | no longer used for models | SUPERSEDED | No `.x` loader or Racing model processor |
| custom `RacingPackage` | old proposed design only | SUPERSEDED | Use GLB + `.material` + raw assets |
| AssetManagementBase path context | nested relative material/effect/texture references | PORT-SIDE WRAPPER | Canonical, case-sensitive-safe paths constrained to asset root |
| AssetManagementBase cache/lifetime | shared managed loaded objects | NEW GENERAL CNA FEATURE WOULD BE USEFUL | Raw stream/provider + typed cache key + explicit RAII after modularization; do not copy managed `Unload()` semantics |
| `ReadAsString` / raw byte reads | JSON, track/combi/font/height data | CNA ALREADY HAS EQUIVALENT | Keep game format parsing port-side |
| XNAssets `LoadTexture2D` | TGA/PNG and premultiply options | CNA ALREADY HAS EQUIVALENT | Focused TGA/PNG/premultiply/color-key tests |
| XNAssets `LoadTextureCube` / `LoadTexture` | DXT1 sky and RGB24 normalization DDS cubes | SMALL CNA GAP | Add/test RGB24 loose DDS cube; DXT1 cube already fits current path |
| XNAssets SpriteFont | not used | SUPERSEDED / NOT REQUIRED | Game uses its bitmap `TextureFont` |
| XNAssets SoundEffect | not used | SUPERSEDED / NOT REQUIRED | Game uses XACT directly |
| XNAssets `LoadEffect` | reads backend-specific `.efb` and constructs `Effect` | THIRD-PARTY FORMAT STILL UNSUITABLE | Replace with CNA-owned portable Effect assets; do not port loader |
| XNAssets caching/path library as a whole | narrow subset above | NOT REQUIRED | Do not port XNAssets wholesale |
| 57 GLB files | all former `.x` models | SMALL/MEDIUM CNA GAP | Preserve scene nodes, transforms, names, instances, ordered parts, material hooks and bounds |
| `_TANGENT`/`_BINORMAL` VEC3 attributes | all 106 GLB primitives | SMALL CNA GAP | Accept/repack `_TANGENT`; binormal may be ignored or derived per vertex contract |
| 32-bit GLB indices | all primitives | CNA ALREADY HAS EQUIVALENT | Verify actual backend/index-buffer path |
| sky-cube VEC3 `TEXCOORD_0` | one special model | SMALL CNA GAP / PORT-SIDE ADAPTER | Do not force generic VEC2 unpack; preserve needed coordinate |
| JSON `.material` sidecars | effect/technique/parameters/textures/ordered part mapping | PORT-SIDE WRAPPER | Validate typed schema and complete references; no package reconstruction |
| GLB material names/indices | sidecar binding key | SMALL CNA GAP | Expose importer binding hook |
| bounds | DigitalRise recomputes part boxes | SMALL CNA GAP | Compute/expose boxes/spheres and compare Racing radius behavior |
| DigitalRiseModel renderer | none; Racing draws parts itself | NOT REQUIRED | Do not port/reimplement library |
| DigitalRiseModel model representation | `DrModel/Bone/Mesh/Part/Material` subset | NEW GENERAL CNA FEATURE WOULD BE USEFUL | Extend CNA Model/glTF model, then thin Racing adapter |

## Graphics resources, states, and drawing

| Requirement | Modern usage | Classification | Action |
|---|---|---|---|
| vertex/index buffers and custom declarations | track lines, tangent models, screen/sky geometry | CNA ALREADY HAS EQUIVALENT | Live stride/semantic/index tests on representative models |
| `DrawIndexedPrimitives`, user primitives | all 3D rendering | CNA ALREADY HAS EQUIVALENT on GL family | Verify base vertex, winding and part offsets |
| blend/depth/rasterizer/sampler states | opaque, glass/alpha, shadow, UI, post | CNA ALREADY HAS EQUIVALENT with validation | Capture state per pass; no implicit leakage |
| `Texture2D`, `TextureCube`, mipmaps | materials/sky/reflection/normalization | CNA ALREADY HAS EQUIVALENT plus RGB cube gap | Format/cube/mip/context-loss tests |
| multiple `RenderTarget2D` chains | shadows, glow, menu, blur | CNA ALREADY HAS EQUIVALENT but VALIDATION GAP | Integrated bind/clear/sample/readback/resize proof |
| `Rgba64` target | unchanged Racing render path | SMALL CNA GAP / CONFIRMED CONTRADICTION | Query and construction must agree; capability-gated `Color` fallback for mobile/Web |
| `SpriteBatch` UI | every screen/HUD | CNA ALREADY HAS EQUIVALENT | Preserve 1024×640 logical layout and safe-area mobile overlay |
| screenshot/readback | FNA/reference/testing | CNA ALREADY HAS EQUIVALENT with platform caveats | Keep diagnostics; not a gameplay requirement on all targets |

## Effects, shaders, and model semantics

| Requirement | Modern evidence | Classification | Action |
|---|---|---|---|
| ten named custom Effects | all retained and runtime-loaded | NEW GENERAL CNA FEATURE WOULD BE USEFUL | CNA-owned portable Effect description/runtime after modularization |
| `EffectParameter` and collection | named scalar/vector/matrix/texture updates | PARTIAL CNA EQUIVALENT | General custom-effect population, validation and clone behavior |
| `EffectTechnique`/collection + `CurrentTechnique` | named selection per material/pass | PARTIAL CNA EQUIVALENT | General arbitrary technique graph |
| `EffectPass`/collection + `Apply()` | all draw wrappers; multi-pass post effects | PARTIAL CNA EQUIVALENT | Apply backend program/state/resources in order |
| FNA `.efb` | committed DX9 FX binaries | THIRD-PARTY FORMAT STILL UNSUITABLE | Oracle/toolchain evidence only; no CNA runtime dependency |
| MonoGameDX `.efb` / `MGFX` | committed DX11 artifacts | THIRD-PARTY FORMAT STILL UNSUITABLE | Oracle/toolchain evidence only |
| MonoGameOGL output | scripts only, no committed binaries | UNKNOWN / NOT PRIMARY | No reason to reconstruct MGFX OpenGL artifacts for CNA |
| direct legacy `.fx` loading | source exists, build via `fxc`/`mgfxc` | NOT RECOMMENDED | Do not build a full FX compiler for Racing |
| explicit backend shader modules | required by portable CNA Effect | NEW GENERAL CNA FEATURE WOULD BE USEFUL | Common GLSL ES 300/GLSL 330 body where possible |
| named model bones/nodes | wheels and windmill actively accessed | SMALL CNA GAP | Preserve real graph; synthetic one-bone-per-primitive model is insufficient |
| ordered mesh parts/materials | car and sidecar assumptions | SMALL CNA GAP | Keep mesh grouping and part order |

## Input and controls

| Requirement | Current/target use | Classification | Action |
|---|---|---|---|
| keyboard | desktop driving/screens | CNA ALREADY HAS EQUIVALENT | Preserve bindings through logical action provider |
| mouse position/buttons/wheel/delta | menus, steering, throttle/brake, camera | CNA ALREADY HAS EQUIVALENT | Preserve desktop; do not make mobile emulate one mouse for gameplay |
| gamepad sticks/triggers/D-pad/buttons | analog driving/screens | CNA ALREADY HAS EQUIVALENT | Preserve, including Bluetooth pads on Android/Web |
| `TouchPanel` multi-touch | new Android/Web overlay | CNA ALREADY HAS EQUIVALENT | PORT-SIDE overlay/provider; simultaneous steering+pedals |
| Android system Back | menu/pause/back | CNA ALREADY HAS EQUIVALENT | CNA maps `SDLK_AC_BACK` to `Keys::Escape`; verify in app |
| Android Accelerometer | optional steering | CNA ALREADY HAS EQUIVALENT, HARDWARE VALIDATION GAP | Game-side calibration/filter/deadzone; never mandatory |
| Web accelerometer | optional idea only | UNKNOWN / NOT LAUNCH REQUIREMENT | CNA currently excludes it; touch/gamepad is sufficient |
| touch control artwork/layout | not in modern game | PORT-SIDE WRAPPER | Analog left steering, throttle, brake/reverse, handbrake, pause/camera; safe area/accessibility |
| logical input snapshot | needed for parity/platform independence | PORT-SIDE WRAPPER | Feed physics from steering/throttle/brake/handbrake/actions |

## Audio and XACT

| Requirement | Evidence | Classification | Action |
|---|---|---|---|
| `AudioEngine` | exact modern `Sound.cs` construction | CNA ALREADY HAS SUBSTANTIAL EQUIVALENT | Test supplied `RacingGameManager.xgs` |
| `WaveBank` | supplied 18,779,556-byte version-46 XWB | CNA ALREADY HAS SUBSTANTIAL EQUIVALENT | Load/decode/play every representative format/wave |
| `SoundBank`/`Cue` | named cue playback and long-lived motor/gear cues | CNA ALREADY HAS SUBSTANTIAL EQUIVALENT | Test prepare/play/stop/pause/dispose and fire-and-forget lifetime |
| categories/variables/RPC/variation | music/effects volume, motor/gear behavior | PARTIAL / VALIDATION GAP | Compare authored behavior with running FNA |
| acquisition/generation of banks | real banks are checked in and used | SUPERSEDED | Do not regenerate by default |
| XACT on Android | parser is portable-looking; demo excluded and no game proof | UNKNOWN | Real-device playback/latency/lifecycle gate |
| XACT on Web | demo excluded; browser audio needs user gesture | UNKNOWN | Browser unlock, playback, suspend/resume and latency gate; fallback only after evidence |

## Storage, lifecycle, and platform integration

| Requirement | Classification | Action |
|---|---|---|
| settings/highscores/replay files | PORT-SIDE WRAPPER over CNA storage | Versioned explicit serialization and atomic writes |
| desktop user storage | CNA ALREADY HAS EQUIVALENT / validate | Never write into assets/source tree |
| Android app storage/lifecycle | NEW PLATFORM INTEGRATION WORK, not automatically CNA gap | App-private storage, pause/resume/process death/context recreation |
| Web persistent storage | SMALL GENERAL CNA/PLATFORM GAP OR PORT ADAPTER | Explicit browser-backed synchronization; in-memory VFS is insufficient |
| FNA/MonoGame compile branches | PORT-SIDE DECISION | Use FNA behavior primary and old source to resolve divergences |
| Emscripten non-blocking main loop | CNA ALREADY HAS EQUIVALENT | Verify with complete game/load states |
| Android Gradle/SDL app shell | PORT-SIDE PACKAGING | Dedicated app target/shared library and landscape manifest |
| Web content preload/stream/cache | PORT-SIDE PACKAGING; GENERAL RAW PROVIDER HELPFUL | Avoid monolithic 289 MiB production preload |

## Backend/platform recommendation

| Stage | Backend | Status for Racing |
|---|---|---|
| 1. Linux reference | `OPENGL33` | Primary; estimated 690–880 h total implementation |
| 2. Windows | `OPENGL33` | High feasibility; +50–90 h realistic qualification |
| 3. Android | `OPENGLES` / GLES 3 | Feasible with +180–300 h and touch/asset/audio/device work |
| 4. Web | `WEBGL2` | Feasible but highest risk; +250–420 h and content/audio/browser work |
| Later native Windows | D3D11 | Separate shader/backend scope, not included |
| Rejected for Racing 3D | `SDL_RENDERER`, `CANVAS`, `WEBGL1`, ES1/fixed-function | Cannot provide the required custom multi-pass 3D path cleanly |

## Matrix conclusion

The modern repository moves Racing from a speculative content-reconstruction
project to a normal, though large, game/framework port. The genuine CNA work is now
concentrated in a portable custom Effect runtime, semantic GLB/model preservation,
one loose DDS cube format, truthful render-target behavior, authentic-bank XACT
validation, and later platform qualification. JSON/material rules, touch layout,
logical controls, raw game formats, and platform packaging remain port-side work.
