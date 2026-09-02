# Racing Game API and dependency compatibility matrix

## Method and baselines

This matrix uses the original XNA 4.0 source/content tree under
`/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/xna4-original/` as the
only implementation and asset authority, FNA for runtime behavior, and live CNA
`../cnanext@1caa45c84`. `/rv/tmp/RacingGame` is a runnable behavior oracle only;
its code and assets are prohibited port inputs. Earlier GLB/material/FNA-asset rows
were withdrawn by [`RACING-CONTENT-SOURCE-POLICY.md`](RACING-CONTENT-SOURCE-POLICY.md).
Milestone 1 qualified lifecycle, resources and input; authentic content remains a
forward-looking gate.

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
| `Game`, `GameTime`, `GameComponent`, window/device lifecycle | `BaseGame`, manager, screens | CNA ALREADY HAS EQUIVALENT | Milestone 1 proved initialize/load/update/draw/present/unload/dispose, resize/fullscreen and paired device events, 31/31 in Debug and ASan/UBSan |
| vectors, matrices, quaternions, planes, rays, bounds | physics, track, camera, render code | CNA ALREADY HAS EQUIVALENT | Differential tests for conventions and floating-point drift |
| C# collections/events/delegates/properties | throughout | PORT-SIDE WRAPPER | Idiomatic C++/sharp-runtime usage; preserve observable order/lifetime |
| reflection/serialization | settings XML only | PORT-SIDE WRAPPER | Explicit settings/replay serializers |
| Windows Forms/GamerServices | original build/runtime support code | UNKNOWN | Preserve only branches actually present in the selected XNA 4 project; do not import modern decisions |

## Content and raw asset loading

| Requirement/dependency | Original use | Classification | Exact delta/action |
|---|---|---|---|
| XNA 4 Content Pipeline | design-time build of every model/texture/effect/audio item | EXTERNAL AUTHENTIC BUILD STEP | Build `RacingGameContent.contentproj` in offline Win7; CNA does not parse `.X` at runtime |
| `ContentManager::Load<T>` | unchanged runtime content IDs | CNA ALREADY HAS EQUIVALENT | Use authentic XNBs only; no loose or converted substitute |
| 56 `RacingGameModelProcessor` XNBs | tangents, parent-derived mesh names, technique suffixes | CNA HAS READERS; AUTHENTIC VALIDATION GAP | Prove `Car`, `Windmill`, `AlphaDeadTree` and then all models through the public XNB route |
| stock `ModelProcessor` `Cube` XNB | sky cube geometry | CNA HAS READERS; AUTHENTIC VALIDATION GAP | Prove graph, vertex data, bounds and draw without asset repair |
| Model XNB graph/material references | bones, meshes, parts, effects, textures | CNA HAS READERS; AUTHENTIC VALIDATION GAP | Verify identity, order, ownership and device behavior against XNA |
| copied track/combi/height files | game-specific raw formats | PORT-SIDE WRAPPER | Preserve bytes and port original readers without framework bypasses |
| 142 pipeline textures | model/UI/cube inputs | CNA HAS READERS; AUTHENTIC VALIDATION GAP | Verify formats, dimensions, mips, alpha/premultiply and cube faces from XNB |
| 28 pipeline `SoundEffect` items | original WAV content | CNA ALREADY HAS EQUIVALENT | Verify authentic SoundEffect XNBs as used |
| custom `RacingPackage`, JSON sidecars, XNAssets, DigitalRiseModel | absent from original | NOT REQUIRED / FORBIDDEN SUBSTITUTE | Do not port or emulate modern content architecture |

## Graphics resources, states, and drawing

| Requirement | Modern usage | Classification | Action |
|---|---|---|---|
| vertex/index buffers and custom declarations | track lines, tangent models, screen/sky geometry | CNA ALREADY HAS EQUIVALENT | Milestone 1 proved byte-exact upload/readback and GPU use of Racing's 44-byte Position/UV/Normal/Tangent layout; representative model offsets remain Milestone 2 |
| `DrawIndexedPrimitives`, user primitives | all 3D rendering | CNA ALREADY HAS EQUIVALENT on GL family | Verify base vertex, winding and part offsets |
| blend/depth/rasterizer/sampler states | opaque, glass/alpha, shadow, UI, post | CNA ALREADY HAS EQUIVALENT with validation | Capture state per pass; no implicit leakage |
| `Texture2D`, `TextureCube`, mipmaps | materials/sky/reflection/normalization | CNA ALREADY HAS EQUIVALENT plus RGB cube gap | Milestone 1 proved independent Color upload/readback on all six cube faces; authored formats/mips and RGB24 loose DDS remain Milestone 2 |
| multiple `RenderTarget2D` chains | shadows, glow, menu, blur | CNA ALREADY HAS EQUIVALENT but VALIDATION GAP | Milestone 1 proved one bind/clear/unbind/readback cycle; the complete multi-target chain remains an integration gate |
| `Rgba64` target | unchanged Racing render path | CNA REPORTS HONESTLY UNSUPPORTED ON OPENGL33; PORT-SIDE POLICY | Live query and construction agree; Racing must select a tested `Color` fallback, with separate mobile/Web quality policy |
| `SpriteBatch` UI | every screen/HUD | CNA ALREADY HAS EQUIVALENT | Preserve 1024×640 logical layout and safe-area mobile overlay |
| screenshot/readback | FNA/reference/testing | CNA ALREADY HAS EQUIVALENT with platform caveats | Milestone 1 produced byte-identical 320x180 PPM pixels in Debug, LSan and ASan/UBSan runs; keep it diagnostic rather than a gameplay requirement |

## Effects, shaders, and model semantics

| Requirement | Original evidence | Classification | Action |
|---|---|---|---|
| ten named custom Effects | ten `.fx` items processed by XNA 4 | CNA ALREADY HAS EQUIVALENT ON THE REFERENCE BACKEND; AUTHENTIC VALIDATION GAP | Load authentic Effect XNBs and perform Racing pixel/state integration rather than rewrite shaders |
| `EffectParameter` and collection | named scalar/vector/matrix/texture updates | CNA ALREADY HAS EQUIVALENT | Validate original names/types/defaults against XNA runtime behavior |
| `EffectTechnique`/collection + `CurrentTechnique` | named selection per material/pass | CNA ALREADY HAS EQUIVALENT | All reflected techniques were selected in the Milestone 0 oracle |
| `EffectPass`/collection + `Apply()` | all draw wrappers; multi-pass post effects | CNA ALREADY HAS EQUIVALENT | Every pass of all ten effects applied after `FX-128`; pixel/state fidelity remains Milestone 3 |
| XNA 4 Effect XNB | authentic compiled DX9 FX bytecode in `EffectReader` payload | CNA ALREADY HAS EQUIVALENT; AUTHENTIC VALIDATION GAP | Build externally once, preserve XNB, load through `ContentManager` |
| FNA `.efb`, MonoGame `MGFX` | modern-oracle artifacts only | FORBIDDEN RACING ASSET / THIRD-PARTY EVIDENCE | Never package or load them in the Racing port |
| direct legacy `.fx` runtime compilation | not how the original game loads content | NOT REQUIRED | Do not build a runtime FX compiler; use authentic pipeline output |
| explicit backend shader modules | old portable-Effect proposal | SUPERSEDED FOR OPENGL33 | MojoShader translates the exact authored Effect bytecode; no Racing shader fork |
| named model bones | wheels and windmill actively accessed | CNA HAS XNB MODEL GRAPH; AUTHENTIC VALIDATION GAP | Preserve the processor-produced graph; synthetic replacements are forbidden |
| ordered mesh parts/materials | original Model output and draw logic | CNA HAS XNB READERS; AUTHENTIC VALIDATION GAP | Keep exact mesh grouping, part order and effect references |

## Input and controls

| Requirement | Current/target use | Classification | Action |
|---|---|---|---|
| keyboard | desktop driving/screens | CNA ALREADY HAS EQUIVALENT | Real X11 `R` input reached `Keyboard.GetState` in Milestone 1; preserve bindings through logical action provider |
| mouse position/buttons/wheel/delta | menus, steering, throttle/brake, camera | CNA ALREADY HAS EQUIVALENT | Real X11 motion/button input reached the game-window snapshot in Milestone 1; preserve desktop without making mobile emulate one gameplay mouse |
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
| acquisition/generation of banks | original `RacingGame.xap` is the input | EXTERNAL AUTHENTIC BUILD STEP | Generate with XNA 4, hash-lock outputs, never source banks from the modern repo |
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

Racing is a large but conventional original-source/XNB port. The immediate gate is
restoring the offline Win7 pipeline host and testing its untouched output through
CNA's existing XNB readers. Genuine CNA work is limited to reader/runtime defects
demonstrated by those assets, truthful render-target behavior, authentic XACT
validation and later platform qualification. Touch layout, logical controls, raw
game formats and platform packaging remain port-side work. Modern GLB/material/FNA
assets are oracle evidence only and never an implementation shortcut.
