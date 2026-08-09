# Racing Game API compatibility matrix

## Method and baseline

This matrix records API behavior actually used by the XNA 4 Racing Game Kit. Assembly references that have no corresponding source use are excluded. The runtime source root is abbreviated as `RG/`:

`/rv/tmp/XNAGameStudio/Samples/XNA-4-Racing-Game-Kit-master/RacingGameWindows1/RacingGame/RacingGame/`

The sibling active content root is `RacingGameWindows1/RacingGame/RacingGameContent/`, and the custom processor is under `RacingGameWindows1/RacingGameContentPipelineExtension/`.

The CNA paths refer to the actively integrated implementation at `../cnaintegration` commit `4ac696c748fb` on 2026-08-09. The older requested checkout `../cna` was at `ac3aaaeb2a5b`; it is not used as proof that an integrated feature is absent. The integration branch was still changing, so the future port must revalidate the pinned commit.

Statuses:

- **READY** — source and tests provide strong evidence for this use.
- **LIKELY READY** — implemented, but needs a Racing-shaped live validation.
- **PARTIAL** — only part of the required behavior is present.
- **BACKEND-SPECIFIC** — sufficient only on selected backends.
- **CNA GAP** — framework work is probably required for the stated path.
- **PORT-SIDE WORK** — adapt in the game/tooling; not a CNA defect.
- **CONTENT-PIPELINE GAP** — original pipeline/output cannot be used directly.
- **UNKNOWN** — a focused experiment is needed before classification.
- **BLOCKER** — prevents the specified route, although a different route may remain feasible.

## Core, lifecycle, and math

| Racing Game usage | XNA type/member | Source locations | CNA equivalent/evidence | Status | Backend notes | Port action | Framework action | Risk |
|---|---|---|---|---|---|---|---|---|
| Game construction, initialization, update, draw, exit | `Game`, `Run`, `Initialize`, `LoadContent`, `Update`, `Draw`, `Exit` | `RG/Graphics/BaseGame.cs`, `RG/RacingGameManager.cs`, `RG/Program.cs` | `CNA/Game.*`, existing CNA samples/tests | READY | Common | Preserve lifecycle; avoid copying global ownership | None | Low |
| Graphics setup and resize/fullscreen | `GraphicsDeviceManager`, preferred size, fullscreen, synchronize-with-vblank, apply changes | `BaseGame.cs`, `GameSettings.cs`, `OptionsScreen.cs` | `CNA/Graphics/GraphicsDeviceManager.*` | LIKELY READY | Window/backend interaction differs | Test 640x480 through desktop resolutions and fullscreen toggles | Add regression only for demonstrated failure | Medium |
| Variable time step and elapsed/total time | `GameTime`; `IsFixedTimeStep=false`; `TargetElapsedTime` | `BaseGame.cs`, physics/screens | `CNA/GameTime.*`, `Game.*` | READY | Timing quality platform-dependent | Preserve runtime behavior initially; add deterministic test-time delta | None | Medium |
| Game component used for screenshots | `GameComponent`, component collection | `ScreenshotCapturer.cs` | CNA component APIs | LIKELY READY | Common | Port only after core rendering | Test lifecycle if failure appears | Low |
| Vectors, matrices, quaternions, planes, rays, rectangles, colors, bounds | `Vector2/3/4`, `Matrix`, `Quaternion`, `Plane`, `Ray`, `Rectangle`, `Color`, `BoundingBox/Sphere` and operators/interpolation/transforms | All gameplay/rendering; especially `CarPhysics.cs`, `Track.cs`, `Landscape.cs`, `ChaseCamera.cs` | CNA math types and unit tests | LIKELY READY | Common | Differential telemetry tests for transforms, normalization, interpolation and ray/plane math | Add minimized parity tests for any mismatch | Medium |
| Framework math helpers | `MathHelper.Clamp/Lerp/ToRadians/WrapAngle`, constants | Physics, camera, UI | CNA `MathHelper` | READY | Common | Direct mapping | None | Low |
| Service lookup and device reset/lifecycle events | `Game.Services`, device events/viewport refresh behavior | `BaseGame.cs`, resource wrappers | CNA game/service/device plumbing | LIKELY READY | Backend/context-reset differences | Minimize reliance; explicitly recreate size-dependent targets | Backend regression if context loss fails | Medium |

## Content and asset loading

| Racing Game usage | XNA type/member | Source locations | CNA equivalent/evidence | Status | Backend notes | Port action | Framework action | Risk |
|---|---|---|---|---|---|---|---|---|
| Typed content load/unload | `ContentManager.Load<T>`, `Unload`, `RootDirectory` | `BaseGame.cs`, model/texture/shader/sound wrappers | `CNA/Content/ContentManager.*`; XNB-first plus loose-file branches | PARTIAL | Common loader, backend resource creation varies | Define which types use CNA loading versus Racing package loader | Fix only demonstrated loader defects | High |
| Register built-in XNB readers | implicit XNA reader registration | All processed content | `CNA::Internal::Xnb::RegisterAllBuiltInXnbReaders()` required explicitly | PORT-SIDE WORK | Common | Call once in app bootstrap if any XNB is retained | Consider safer automatic registration separately, not required here | Low |
| Primitive/math XNB objects and shared resources | XNB reader graph | Models/effects/content dependencies | XNB reader infrastructure supports shared resources and common types | READY | Common | Use only for selected XNB path | None | Low |
| LZX-compressed and uncompressed XNB | content output | Any retained processed content | Implemented; LZ4 is not, but XNA 4 output does not require it | READY | Common | Capture exact toolchain output version | None | Low |
| Texture2D processed content | `Content.Load<Texture2D>` | UI, materials, landscape, effects | XNB Texture2D reader supports Color and DXT1/3/5; loose image loader recognizes PNG/TGA/etc. | LIKELY READY | Upload/mipmap/premultiply details vary | Golden-image representative textures, including four non-premultiplied items | Add regression for proven mismatch | Medium |
| Loose `.tga` textures | content pipeline originally imports 129 TGA files | `RacingGameContent.contentproj` | Loose Texture2D extension path includes `.tga` | LIKELY READY | Decoder/upload backend-sensitive | Validate origin, alpha, premultiplication, mipmaps, sRGB assumptions | Fix generalized decoder issue only if found | Medium |
| Sky cube | `Content.Load<TextureCube>` from DXT1 cube DDS/XNB | `BaseGame.cs`, `PreScreenSkyCubeMapping.cs` | DXT1 DDS cube path exists | LIKELY READY | Cube sampling must be tested per backend | Verify face order/seams on OPENGL33 | Add fixture if mismatch | Medium |
| Normalization cube | uncompressed RGB888 cube DDS through pipeline | normal-mapping shaders/content | Loose cube loader accepts DXT1/3/5, not supplied RGB888 | CONTENT-PIPELINE GAP | All current loose DDS cube path | Offline lossless conversion or supported XNB experiment | Optional uncompressed cube support | Medium |
| Binary XNB model graph | `Content.Load<Model>`, bones, mesh parentage, bounds, declarations/buffers/effects | `Model.cs`, `CarModel.cs`, `Landscape.cs` | Real `ModelReader` in `Internal/Xnb/ModelContentTypeReaders.cpp`; multi-bone fixture tests | PARTIAL | GPU buffers/backend-specific | Useful as oracle or for stock Cube; custom models still hit custom-effect gap | None for the model graph itself | High |
| General compiled custom Effect in XNB | `EffectReader`; effect bytecode constructor | Every custom-processed `.x` model and ten `.fx` files | Known-unsupported reader and throwing `Effect` bytecode constructor | BLOCKER | DX9 bytecode is not portable | Do not choose original custom-effect XNB as primary runtime path | Major framework design if direct XNB compatibility is required | Critical for direct-XNB route |
| Stock-effect model content | standard `ModelProcessor` on `Cube.x` | sky model | Model reader plus stock effect readers | LIKELY READY | Verify OPENGL33 draw | Use as early XNB proof | Add fixture only if it fails | Low |
| Legacy `.x` model source | `XImporter`, `ModelProcessor` | 57 ASCII `.x` files; content project | No direct CNA `.x` runtime/content processor was found; glTF facilities are not drop-in | CONTENT-PIPELINE GAP | Offline work independent of backend | Build Racing-specific deterministic converter/package | Generic importer enhancement optional | Critical |
| Model processor tangent generation | `MeshHelper.CalculateTangentFrames`, `GenerateTangentFrames=true` | `RacingGameModelProcessor.cs`; 56 item parameters | CNA glTF tangent generation exists but is not proven bit/semantic equivalent | PORT-SIDE WORK | Tangent basis affects shader output | Generate position/UV/normal/tangent; compare handedness and normal-map output | Add generic support only if broadly useful | High |
| Processor mesh-name technique suffix | append each material `OpaqueData["technique"]` integer to mesh name | `RacingGameModelProcessor.cs`, runtime `Model.cs` | No generic CNA equivalent | PORT-SIDE WORK | Common | Preserve as explicit per-part manifest field, not encoded name if avoidable | None | High |
| Processor effect material values | custom `EffectInstance` values in `.x` | all custom models; `Model.cs` reads parameters/technique | CNA glTF maps to stock material model and omits these Racing values | CONTENT-PIPELINE GAP | Common | Preserve colors, shininess, alpha, textures, cube bindings, technique ID | Material extension optional | High |
| Node hierarchy, pivots, and instances in converted models | parent bones, wheel/windmill transforms | `CarModel.cs`, `Model.cs`, landscape objects | Current glTF mesh collection simplifies unskinned nodes/identity bones | PARTIAL | Common | Sidecar/package must preserve full hierarchy and stable names | Improve generic glTF scene preservation only if selected | Critical |
| Raw track and object metadata | `TitleContainer.OpenStream`; `XmlSerializer` for `.Track`/`.CombiModel` | `Track.cs`, `Landscape.cs`, `CombiModel.cs` | `TitleContainer` exists and normalizes separators; Sharp Runtime XML serializer not equivalent | PORT-SIDE WORK | Filesystem case differs | Implement explicit versioned XML readers and path normalization | None | Medium |
| Raw height data | `TitleContainer.OpenStream`, `BinaryReader.ReadByte` | `Landscape.cs`; `LandscapeHeights.data` is 66,049 bytes | TitleContainer/stream primitives exist | READY | Common | Validate 257x257 size and checksum | None | Low |

## Graphics resources, states, and drawing

| Racing Game usage | XNA type/member | Source locations | CNA equivalent/evidence | Status | Backend notes | Port action | Framework action | Risk |
|---|---|---|---|---|---|---|---|---|
| Device, viewport, clear, presentation parameters | `GraphicsDevice`, `Viewport`, `Clear`, `PresentationParameters` | `BaseGame.cs`, all render passes | CNA graphics device API | LIKELY READY | Strongest evidence on OPENGL33 family | Create focused state/resize smoke test | Fix minimized failures | Medium |
| Capability negotiation | `GraphicsAdapter.DefaultAdapter.QueryRenderTargetFormat` | `RenderToTexture.cs` | API exists, but `Rgba64` answer cannot be constructed | CNA GAP | Shared contradiction; D3D9 selection also reaches validation | Use `Color` until fixed, then compare fidelity | Make query result constructible and test end to end | High |
| Full/half/quarter scene targets | `RenderTarget2D` constructors, `SetRenderTarget`, read resolved target | post-screen classes | Implemented for supported formats | LIKELY READY | OPENGL33 recommended; exact rebind chain untested | Build render-target probe before porting effects | Regression if resolve/rebind fails | High |
| Shadow targets | 1024/2048 color RT plus depth buffer | `ShadowMapShader.cs`, `RenderToTexture.cs` | Ordinary color/depth render target path | LIKELY READY | No depth texture sampling needed | Test PCF color-depth encoding | None unless failure | Medium |
| Vertex buffers and declarations | `VertexBuffer`, `SetData`, `SetVertexBuffer`, `VertexDeclaration`, `VertexElement` | `Landscape.cs`, screen grid, line shader | Generic declarations implemented; OPENGL test covers a 48-byte custom layout | BACKEND-SPECIFIC | 44-byte Racing layout still needs proof | Exact tangent-vertex smoke test | Backend test for exact layout if it fails | Medium |
| 32-bit indexed landscape | `IndexBuffer(IndexElementSize.ThirtyTwoBits)`, `DrawIndexedPrimitives` | `Landscape.cs` | 32-bit indices implemented on programmable 3D backends | LIKELY READY | Not meaningful on 2D backends | Render full 393,216-index terrain | Add backend regression on failure | Medium |
| 16-bit indexed screen grid | dynamic/fullscreen grid buffers | `PostScreenGlow.cs` | Implemented | LIKELY READY | Common 3D backends | Validate 10x10 radial grid | None | Low |
| Immediate user primitives | `DrawUserPrimitives<VertexPositionColor>` | `LineRendering.cs`, track/debug helpers | Implemented on suitable backends | LIKELY READY | Layout support backend-dependent | Use for early track-line proof | None unless failure | Low |
| Blend states | `Opaque`, `AlphaBlend`, `Additive`, custom color/alpha factors/functions | shaders, UI, lens flare, shadows | CNA state classes/backends exist | LIKELY READY | Exact separate-alpha behavior needs capture | Recreate explicit complete states; do not copy under-specified source states blindly | Add conformance test for demonstrated mismatch | High |
| Depth/stencil states | `Default`, `None`; depth read/write switching | all 3D/post/UI | Implemented | LIKELY READY | Backend state leakage risk | State-transition capture tests | Regression if leak/mismatch | Medium |
| Rasterizer states | `CullNone`, `CullCounterClockwise` | sky, alpha, landscape, models | Implemented | LIKELY READY | Front-face convention differs across APIs | Golden images of winding/culling | Fix backend convention if needed | Medium |
| Sampler states | `LinearWrap`, `LinearClamp`, `AnisotropicWrap`; per-slot assignment | material and post shaders | Implemented; GL-family has recent fixes | LIKELY READY | Anisotropy/cube behavior needs runtime proof | Explicitly bind every used slot/pass | Backend regression if wrong | Medium |
| Texture binding | 2D and cube textures, multiple sampler slots | normal mapping, landscape, glass, sky, post | `ShaderEffect` named 2D/cube/3D bindings on capable backends | BACKEND-SPECIFIC | OPENGL33/D3D paths strongest; Vulkan/Bgfx limited | Typed shader binding layer | Backend-specific work later | High |
| Backbuffer readback and JPEG | `GraphicsDevice.GetBackBufferData`, `Texture2D.SaveAsJpeg` | `ScreenshotCapturer.cs` | APIs exist | UNKNOWN | Origin/pitch/color conversion backend-specific | Late focused screenshot test and platform-safe path | Fix only if generalized failure | Medium |
| Sprite rendering | `SpriteBatch.Begin/Draw/DrawString/End`, transforms, source/destination rectangles, effects, blend/sampler/depth/rasterizer | BaseGame/UI/screens/lens flare | Substantial CNA implementation/tests | LIKELY READY | OPENGL33 recommended | Reproduce two-batch layering only after 3D proof | Regression for game-shaped sequence | Medium |
| SpriteFont | processed font loading and `DrawString` | UI renderer and screens | XNB SpriteFont reader/renderer exists | LIKELY READY | Glyph rendering/backend differences | Verify original font XNB availability/license; otherwise package equivalent | Fix only proven issue | Medium |
| SpriteBatch before/around 3D | two SpriteBatches begin before screen renders 3D | `BaseGame.cs` | Current integration includes regression example marking historical ordering issue fixed | LIKELY READY | Re-test selected backend/commit | Prefer clearer pass boundaries in new code while preserving layering | Do not reopen stale bug without reproduction | Medium |

## Effects, models, and rendering semantics

| Racing Game usage | XNA type/member | Source locations | CNA equivalent/evidence | Status | Backend notes | Port action | Framework action | Risk |
|---|---|---|---|---|---|---|---|---|
| Load ten custom DX9 `.fx` effects | `Content.Load<Effect>` | eight shader wrapper classes; `RacingGameContent/Shaders/*.fx` | General custom compiled Effect load unsupported; source-pair `ShaderEffect` is NOXNA alternative | CONTENT-PIPELINE GAP | GLSL for OPENGL33; separate variants later | Manually port required semantics, not syntax | General FX runtime optional/large | Critical |
| Parameter lookup and setting | `Effect.Parameters[name].SetValue` for matrices, vectors, scalars, arrays, textures/cubes | all shader wrappers and model materials | `ShaderEffect` named setters exist on capable backends; no XNA reflection collection | BACKEND-SPECIFIC | Vulkan binding is insufficient; Bgfx source compile absent | Typed interfaces; fail fast on missing bindings | Improve backend binding later | High |
| Technique selection by name/index | `Effect.Techniques[...]`, `CurrentTechnique`; per-part technique integer | shader wrappers, `Model.cs` | No multi-technique `ShaderEffect` abstraction | PORT-SIDE WORK | Common | Map technique enum to explicit shader program | General multi-technique abstraction optional | High |
| Pass iteration and pass-state application | `technique.Passes`, `Apply`; post effects rely on 2/4/5 ordered passes | all effects, especially postprocessing | One program per `ShaderEffect`; no FX pass state blocks | PORT-SIDE WORK | Common | Explicit render graph/pass objects and states | General FX support optional | Critical |
| DX9 shader profiles/semantics | `vs_1_1`, `vs_2_0`, `ps_2_0`, SAS-like semantics | ten `.fx` files | No automatic HLSL-to-portable-source translation | PORT-SIDE WORK | Primary target GLSL 3.3 | Rewrite semantically; image compare | None required for first port | Critical |
| Model bone transforms | `CopyAbsoluteBoneTransformsTo`, `ModelMesh.ParentBone.Index`, bone transforms | `Model.cs`, `CarModel.cs` | Binary XNB model reader supports multi-bone graph; CNA Model API exists | LIKELY READY | Content route is the main uncertainty | Preserve stable hierarchy in package; differential transforms | Add API test only if mismatch | High |
| Mesh/part traversal and effects | `Model.Meshes`, `ModelMesh.MeshParts`, effect replacement and dictionaries | `Model.cs`, `MeshRenderManager.cs` | CNA Model/Mesh/Part collections and effect objects exist | LIKELY READY | Ownership/lifetime differs in C++ | Use stable asset IDs/handles; explicit material table | None | Medium |
| Bounding sphere scale/culling | `ModelMesh.BoundingSphere.Radius`, frustum/distance checks | `Model.cs`, landscape/track | Model bounds and math types implemented | LIKELY READY | Converter must preserve processor output semantics | Store/validate bounds in package | None | Medium |
| Transparent/alpha-tested parts | authored Alpha models, alpha shader techniques, `CullNone`; old alpha-test calls commented out | model/landscape rendering | Programmable shader discard possible | PORT-SIDE WORK | Source conversion itself has fidelity ambiguity | Establish reference captures and explicit alpha policy | None unless backend discard bug | High |

## Input

| Racing Game usage | XNA type/member | Source locations | CNA equivalent/evidence | Status | Backend notes | Port action | Framework action | Risk |
|---|---|---|---|---|---|---|---|---|
| Keyboard polling | `Keyboard.GetState`, `KeyboardState.IsKeyDown`, many `Keys` | `Input.cs`, screens/gameplay | Implemented and tested | LIKELY READY | Key mappings/window focus vary | Build action map and manual focus test | Fix only mapping bugs | Low |
| Mouse polling and recenter | `Mouse.GetState`, `Mouse.SetPosition`, buttons/wheel | `Input.cs`, options/UI | Implemented | LIKELY READY | Wayland/window confinement may differ | Prefer relative mode if necessary; test recenter/focus | Backend/window work only if reproducible | Medium |
| Gamepad polling | `GamePad.GetState`, buttons/triggers/thumbsticks, `PlayerIndex.One`, connection state | `Input.cs`, menus/gameplay | Implemented with SDL gamepad support/tests | LIKELY READY | Controller mapping/dead zones | Test one representative controller and disconnect | Add regression for generalized issue | Medium |
| Input edge/repeat logic | previous/current states and timers | `Input.cs` | Game-owned behavior | PORT-SIDE WORK | Common | Port deliberately; correct source's one-sided mouse-delta defect if approved | None | Low |

## Audio/XACT

| Racing Game usage | XNA type/member | Source locations | CNA equivalent/evidence | Status | Backend notes | Port action | Framework action | Risk |
|---|---|---|---|---|---|---|---|---|
| Load XACT global settings and banks | `AudioEngine`, `WaveBank`, `SoundBank` from `.xgs/.xwb/.xsb` | `Sounds/Sound.cs`; `Content/Audio/RacingGame.xap` | Real parsers/runtime in CNA | LIKELY READY | Audio backend/decoder; no generated banks supplied | Obtain authentic Windows banks and run a load probe | Fix minimized parser/runtime failure only | High |
| Cue lifecycle | `GetCue`, `Play`, `Pause`, `Resume`, `Stop(Immediate)`, `IsPlaying/Paused/Stopped`, `Dispose` | `Sound.cs` | Implemented with cue tests | LIKELY READY | Common audio | Exercise every cue family | Add failure-specific test | Medium |
| Categories and instance behavior | `GetCategory`, `SetVolume`; Music/Gears max-one replacement/crossfade | `Sound.cs`, XAP categories | Categories, instance limits/behaviors tested | LIKELY READY | Timing/mixing differences | Log cue transitions and compare output | Fix only differential mismatch | Medium |
| Cue variable and RPC pitch | `Cue.SetVariable("Pitch", value)`; authored -12..12 to -1200..1200 cents | engine/brake audio code and XAP | Cue variables/RPC curves tested | LIKELY READY | Resampler quality differs | Test clamping and pitch at telemetry points | Add exact curve test if needed | Medium |
| Random/looping music | two game tracks, one menu track, up to 255 loops, no new variation per loop | XAP | loops/random variations implemented; known reselect limitation is irrelevant to authored setting | LIKELY READY | Timing | Long-run loop test | None unless failure | Low |
| PC MS-ADPCM | authored PC compression for music/motor, block sizes 128/256 | XAP | MS-ADPCM decoder and WaveBank tests pass | LIKELY READY | Decoder/mixer | Decode bank fixture and compare duration/sample statistics | Regression if mismatch | Medium |
| Positional audio | no `Apply3D` use | none | Not required | READY | — | Do not add | None | None |

## Storage, gamer services, and platform integration

| Racing Game usage | XNA type/member | Source locations | CNA equivalent/evidence | Status | Backend notes | Port action | Framework action | Risk |
|---|---|---|---|---|---|---|---|---|
| Storage selector/open container async pattern | `StorageDevice.Begin/EndShowSelector`, `Begin/EndOpenContainer`, `IAsyncResult` wait handle | `GameSettings.cs`, `Highscores.cs`, `Replay.cs`, screenshot code | CNA implements synchronous work behind XNA-shaped async facade; SDL preference path | LIKELY READY | Platform path differs | Create one Racing-shaped round-trip test | Add missing round-trip coverage if generalized | Medium |
| Storage files | `StorageContainer.FileExists/OpenFile/CreateFile`, stream read/write/dispose | settings/highscores/replay/screenshots | Implemented; controlled paths fit API | LIKELY READY | Case/path rules | Explicit filenames, atomic replacement where practical | Containment/disposal hardening separate | Medium |
| XML settings serialization | `[Serializable]`, `XmlSerializer.Serialize/Deserialize` | `GameSettings.cs` | No drop-in behavior should be assumed | PORT-SIDE WORK | Common | Explicit versioned reader/writer | None | Medium |
| Binary replay/high-score data | `BinaryReader/Writer`, arrays/matrices/floats | `Replay.cs`, `Highscores.cs` | Streams and primitives available; format is game-owned | PORT-SIDE WORK | Endianness/versioning | Specify and test format; optionally read legacy file | None | Medium |
| Gamer services component | `GamerServicesComponent`; `Guide.IsVisible` mainly around storage UI | `RacingGameManager.cs`, settings/storage flows | CNA GamerServices surface exists; desktop fake UI is sufficient or removable | PORT-SIDE WORK | Desktop port does not need Xbox overlay | Remove/encapsulate UI dependency; retain storage semantics | None | Low |
| Windows Forms | `Application`, `Form.FromHandle().Hide()`, `MessageBox.Show`; `[STAThread]` | `Program.cs`, `RacingGameManager.cs` | Not an XNA/CNA portability requirement | PORT-SIDE WORK | Windows-only source behavior | SDL/CNA window API and logging/error UI | None | Low |
| Legacy isolated-storage logging | `IsolatedStorageFile.GetUserStoreForDomain`, `StreamWriter` | helper logging code | Sharp Runtime does not provide the exact domain-store overload/serializer ecosystem | PORT-SIDE WORK | Common | Use standard app data/log path | None | Low |
| Threads, events, thread pool | `Thread`, `ThreadPool`, `ManualResetEvent`; background resource load/save | manager/storage classes | C++ standard concurrency/Sharp Runtime pieces exist | PORT-SIDE WORK | Graphics contexts are thread-sensitive | Keep GPU creation on render thread; worker only CPU I/O/decode | Framework work only if API documents otherwise | High |

## Backend recommendation summary

| Tier | Backend(s) | Assessment for this game |
|---|---|---|
| Reference | `OPENGL33` (internally EasyGL) | Best current combination of programmable 3D, GLSL `ShaderEffect`, generic custom layouts, cube textures, render targets, and focused examples/tests. Use on Linux first. |
| Secondary validation | D3D11, D3D12; possibly D3D9 as a Windows/XNA-era visual oracle | Broad programmable pipelines, but require HLSL shader variants and full Racing validation. D3D9 does not avoid the shared `Rgba64` construction problem. |
| Potential after reference completion | `OPENGLES`, OpenGL4, Sokol, Diligent, SDL_GPU, WebGPU, LLGL and other new 3D backends | Evaluate one at a time after a stable render test corpus exists. Maturity and shader-contract differences make predictions provisional. |
| Needs substantial backend work | Vulkan, Bgfx | Current custom-effect binding/compilation paths do not fit the Racing shader set. |
| Unsuitable | SDL_Renderer, Direct2D, Canvas, HTML DOM, Skia, ASCII, GDI and other 2D-only paths; OpenGL ES 1/fixed-function legacy paths | Cannot faithfully provide this programmable multipass 3D pipeline. |

## Matrix conclusion

No current evidence shows that CNA's overall architecture prevents a port. The viable route is narrower than “translate the C# and load the old content”: pin the active integration, target `OPENGL33`, use a deterministic Racing-specific offline model/material package, explicitly port shader semantics to `ShaderEffect`, and validate XACT with genuine banks. Direct reuse of the original custom-effect XNB output is blocked, and the `Rgba64` render-target contract is a concrete CNA defect that should be resolved or explicitly bypassed for the first proof.
