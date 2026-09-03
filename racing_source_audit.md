# Racing Game Kit source-to-source audit

This audit covers the unchanged XNA 4 source snapshot retained at:

```text
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/xna4-original/
```

Its 325-file SHA-256 manifest is
`evidence/xna4-original.sha256`. The selected game project contains 58 C# files.
The canonical content project contains 57 `.X`, 129 `.TGA`, 11 `.PNG`, two
`.DDS`, ten `.fx`, 28 `.wav`, one `.xap`, three `.Track`, ten `.CombiModel` and
one `.data` source. The runtime input is the unchanged XNA Game Studio 4 output,
not the modern repository: 339 XNBs plus the XGS/XSB/XWB and 14 raw data files.

## File mapping

`Direct` means the logical class and algorithms remain in the same named C++
owner. `Factored` means a process-static C# owner was split into explicit
game-owned C++ dependencies without a second behavior path. `Integrated` means
the source behavior is retained by the named rendering/platform owner rather
than by an otherwise empty wrapper.

| Original XNA 4 file | CNA Samples implementation | Classification |
|---|---|---|
| `GameLogic/BasePlayer.cs` | `GameLogic/BasePlayer.*` | Direct |
| `GameLogic/CarPhysics.cs` | `GameLogic/CarPhysics.*`, `CarControlState.*` | Factored; deterministic environment replaces process statics |
| `GameLogic/ChaseCamera.cs` | `GameLogic/ChaseCamera.*` | Direct |
| `GameLogic/Input.cs` | `GameLogic/Input.*` | Direct; real keyboard/mouse/gamepad provider plus injectable test source |
| `GameLogic/Physics/SpringPhysicsObject.cs` | `GameLogic/Physics/SpringPhysicsObject.*` | Direct |
| `GameLogic/Player.cs` | `GameLogic/Player.*` | Direct with explicit environment |
| `GameLogic/Replay.cs` | `GameLogic/Replay.*` | Direct, including the original `replayFileFound` quirk and binary layout |
| `GameLogic/ScreenshotCapturer.cs` | `GameLogic/ScreenshotCapturer.*` | Direct XNA component and PrintScreen/JPEG path |
| `GameScreens/CarSelection.cs` | `GameScreens/CarSelection.*` | Direct |
| `GameScreens/GameScreen.cs` | `GameScreens/GameScreen.*` | Direct |
| `GameScreens/Help.cs` | `GameScreens/Help.*` | Direct |
| `GameScreens/Highscores.cs` | `GameScreens/Highscores.*` | Direct |
| `GameScreens/IGameScreen.cs` | `GameScreens/IGameScreen.hpp` | Direct interface |
| `GameScreens/LoadingScreen.cs` | `GameScreens/LoadingScreen.*` | Direct; stages run cooperatively on the graphics owner thread |
| `GameScreens/MainMenu.cs` | `GameScreens/MainMenu.*` | Direct |
| `GameScreens/Options.cs` | `GameScreens/Options.*` | Direct |
| `GameScreens/SplashScreen.cs` | `GameScreens/SplashScreen.*` | Direct |
| `GameScreens/TrackSelection.cs` | `GameScreens/TrackSelection.*` | Direct |
| `GameSettings.cs` | `Properties/GameSettings.*`, `Serialization/XmlSerializationAdapters.hpp` | Direct wire format; compile-time XML metadata replaces reflection |
| `Graphics/BaseGame.cs` | `RacingGameManager.*`, `Graphics/ResolutionMapper.*`, game-owned renderers | Factored process-static owner |
| `Graphics/LensFlare.cs` | `Graphics/LensFlare.*` | Direct |
| `Graphics/LineManager2D.cs` | `Graphics/LineManager2D.*`, `UIRenderer.*` | Direct |
| `Graphics/LineManager3D.cs` | `Graphics/LineManager3D.*` | Direct dormant branch; authentic effect is GPU-tested |
| `Graphics/Material.cs` | `Graphics/Material.*` | Direct |
| `Graphics/MeshRenderManager.cs` | `Graphics/MeshRenderManager.*` | Direct grouping/order logic |
| `Graphics/Model.cs` | `Graphics/Model.*`, `Rendering/CarRenderer.*`, `LandscapeObjectRenderer.*` | Factored model policy and concrete draw owners |
| `Graphics/PlaneRenderer.cs` | `Rendering/StaticTrackScene.*` city-ground mesh | Integrated exact two-triangle plane |
| `Graphics/TangentVertex.cs` | `Graphics/TangentVertex.*` | Direct 44-byte runtime layout |
| `Graphics/Texture.cs` | `Graphics/Texture.*` | Direct XNB texture wrapper |
| `Graphics/TextureFont.cs` | `Graphics/TextureFont.*` | Direct |
| `Graphics/TextureFontBigNumbers.cs` | `Graphics/TextureFontBigNumbers.*` | Direct |
| `Graphics/UIRenderer.cs` | `Graphics/UIRenderer.*` | Direct atlases, text, trophies and debug FPS branch |
| `Helpers/ColorHelper.cs` | `Helpers/ColorHelper.*` | Direct |
| `Helpers/Directories.cs` | `RacingRunConfiguration`, `ContentManager::RootDirectory`, CNA storage | Integrated platform paths |
| `Helpers/FileHelper.cs` | `Replay.*`, `GameSettings.*`, `ScreenshotCapturer.*`, CNA `TitleContainer`/storage | Integrated stream/storage operations |
| `Helpers/Log.cs` | `Helpers/Log.*` | Direct isolated-storage log and 2 MiB rotation |
| `Helpers/RandomHelper.cs` | `Helpers/RandomHelper.*` | Direct |
| `Helpers/Vector3Helper.cs` | `Helpers/Vector3Helper.*` | Direct |
| `Landscapes/Landscape.cs` | `Landscapes/Landscape.*`, `Rendering/StaticTrackScene.*` | Factored CPU generation and GPU owner |
| `Program.cs` | `Program.cpp` | Direct entry point |
| `Properties/AssemblyInfo.cs` | `CMakeLists.txt`, program/window metadata | Integrated build metadata |
| `RacingGameManager.cs` | `RacingGameManager.*` | Direct game/screen/race owner |
| `Shaders/PostScreenGlow.cs` | `Shaders/PostScreenGlow.*` | Direct authentic five-pass effect |
| `Shaders/PostScreenMenu.cs` | `Shaders/PostScreenMenu.*` | Direct authentic four-pass effect |
| `Shaders/PreScreenSkyCubeMapping.cs` | `Rendering/StaticTrackScene.*` sky draw | Integrated authentic sky effect/cube/model |
| `Shaders/RenderToTexture.cs` | `Shaders/RenderToTexture.*` | Direct |
| `Shaders/ShaderEffect.cs` | `Graphics/Model.*` and concrete renderers' shared XNA `Effect` binding | Integrated common parameter/technique behavior |
| `Shaders/ShadowMapBlur.cs` | `Rendering/ShadowMapRenderer.*` | Integrated authentic blur passes |
| `Shaders/ShadowMapShader.cs` | `Rendering/ShadowMapRenderer.*` | Integrated caster/receiver passes |
| `Shaders/VBScreenHelper.cs` | `Shaders/VBScreenHelper.*` | Direct |
| `Sounds/Sound.cs` | `Sounds/Sound.*` | Direct XACT banks, cues, categories and five-gear behavior |
| `Tracks/GuardRail.cs` | `Tracks/GuardRailGeometry.*`, `StaticTrackScene.*` | Factored CPU geometry and GPU owner |
| `Tracks/Track.cs` | `Tracks/Track.*`, `TrackGeometry.*` | Factored CPU geometry and runtime owner |
| `Tracks/TrackColumns.cs` | `Tracks/TrackColumnsGeometry.*`, `StaticTrackScene.*` | Factored CPU geometry and GPU owner |
| `Tracks/TrackCombiModels.cs` | `Tracks/TrackCombiModels.*` | Direct XML data model |
| `Tracks/TrackData.cs` | `Tracks/TrackData.*` | Direct XML data model |
| `Tracks/TrackLine.cs` | `Tracks/TrackLine.*` | Direct spline/helper generation |
| `Tracks/TrackVertex.cs` | `Tracks/TrackVertex.*` | Direct data model |

## Audited C++ mechanics

- The original content identifiers and `ContentManager::Load<T>` routes remain.
  There is no GLB, glTF, `.material` or loose-image runtime path.
- XNB model vertices retain their processor-authored 56-byte tangent/binormal
  declaration. Runtime-generated `TangentVertex` remains a separate 44-byte
  layout; neither is coerced into the other.
- The original worker loading order is visible unchanged, but GPU resources are
  created cooperatively on CNA's graphics owner thread. This is the required
  thread-affinity translation, not a content bypass.
- Process-static C# state is held by one `RacingGameManager`; deterministic
  qualification dependencies are injected only at construction. The product
  executable always creates the real CNA input provider.
- Screenshot enumeration and output use the same CNA storage-container root so
  the original relative path remains confined on every supported platform.
- The Release-only render exception budget, debug-only FPS toggle, adaptive
  view distance, isolated `Log.txt`, screenshot component and device-reset
  resource recreation are retained.
- `LineManager3D` has no callers outside original `BaseGame` in the selected
  source, and no product class submits a world-space line. Its retained branch
  nevertheless loads `LineRendering.xnb`, submits both overloads and resets its
  frame collection in the GPU harness.

## Observable parity evidence

- Control/simulation/audio-event trace: all 753 FNA/CNA records agree, with a
  measured maximum camera difference of 16 ULP. The trace includes better/worse
  checkpoint cues, victory/loss selection and gear stop.
- Track/replay: 79/79 cumulative FNA/CNA records agree.
- Static render: normalized RMSE `0.002230`, with `99.0503%` of channels within
  two values between the FNA and CNA reference captures.
- Authentic XACT: all 27 source cue names load from the unchanged XGS/XSB/XWB;
  screen flow starts menu/selection/race cues and `Gear1`, while the natural
  race outcome emits its cue and stops the looping gear sound.

The release/license inventory is separate in
[`racing_release_gate.md`](racing_release_gate.md). Runtime qualification does
not imply permission to redistribute the retained Microsoft assets.
