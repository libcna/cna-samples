# SAMPLE-151 — `VectorRumble_ARCHIVE_2_0` audit and owner decision

## Status

Fresh audit complete enough to require an owner product decision under `SAMPLES-DEC-002` and
`SAMPLES-DEC-005`. This is a complete XNA 2.0 Windows/Xbox game, not an incomplete starter or an
effect-only archive. No reduced shooter, NetRumble alias, loose-WAV audio rewrite, fake cue table or
unapproved XNA4 port was added.

Artifact root: `/rv/tmp/samples/SAMPLE-151-VectorRumble/`.

## Complete delivered product

The licensed delivery contains **79 files / 30,483,149 bytes**:

- 46 runtime C# files / 7,676 lines;
- three bloom effects / 109 HLSL lines;
- sixteen PCM16 WAV files, including a 24.7 MB stereo music track;
- two SpriteFonts and two textures;
- Windows and Xbox projects/solutions, documentation, thumbnail and the Ms-PL;
- one XACT2 project whose output contract names Windows/Xbox XGS, XWB and XSB banks.

The game is a complete up-to-four-player 2D arena shooter at 1280×720 on Windows. It draws every
ship, asteroid, projectile, power-up, star and particle as vector `LineList` geometry through a
shared `BasicEffect`, with collision, damage, spawning, multiple weapons, power-ups, vibration and
four independently joining `PlayerIndex` ships. Keyboard input covers player one and the menus;
additional players use gamepads. The complete screen system includes background/main/options menus,
loading, pause, message boxes and game-over flow. A four-pass bloom component performs bright-pass
extraction, horizontal and vertical Gaussian blur, then combines the scene and glow.

Audio is defining behavior rather than incidental packaging. `AudioManager` creates an
`AudioEngine`, `WaveBank` and `SoundBank`; the project defines fourteen named cues for music,
menus, spawning/death, collisions, power-ups, weapons, a looping rocket and several explosion
variants. The XAP is explicitly `XACT2`, version 16/content version 43, August 2007, with sixteen
waves and Global/Default/Music category behavior. Replacing these routes with direct WAV playback
would erase cue variation, categories, music ownership, looping and cue lifecycle semantics.

## Authentic XNA 2 and diagnostic XNA 4 evidence

The owner-provided Win7 SP1 VM was booted headless and kept fully offline: all eight VirtualBox
network adapters remained `none`. An exact `Release|x86` build selected the original Windows
project and stopped only because that VM has XNA Game Studio 4 but not the XNA 2 MSBuild targets:

```text
VectorRumbleWindows.csproj(220,3): error MSB4019:
The imported project
"C:\Program Files\MSBuild\Microsoft\XNA Game Studio\v2.0\
Microsoft.Xna.GameStudio.Common.targets" was not found.
```

This is an environment result, not evidence that the XNA 2 delivery is incomplete.

A separate diagnostic copy, never the immutable source snapshot and never claimed as the sample
port, applies only the measured official XNA2-to-XNA4 migrations:

- v4 project references/imports and removal of the retired minimum-pixel-shader-profile setting;
- `ResolveTexture2D`/`ResolveBackBuffer` bloom replaced by the XNA4 scene-`RenderTarget2D` route;
- current render-target, immutable state, `BasicEffect`, effect-pass and SpriteBatch-effect APIs;
- `TotalRealTime` replaced by the current `TotalGameTime` property;
- the three bloom pixel-shader function names changed from the XNA4-reserved `PixelShader` name.

With only the XACT project temporarily excluded, Microsoft's XNA4 `Release|x86` toolchain compiles
all **7,676 runtime C# lines** into `VectorRumbleWindows.exe` with zero warnings/errors and builds all
seven non-XACT content items: three Effects, two SpriteFonts and two Texture2Ds. Re-enabling the
unchanged audio item produces the precise remaining failure:

```text
The .xap file was created with a version of XACT that is incompatible with the
XNA Framework Content Pipeline version used by this project.
```

No XACT2 authoring/compiler executable is installed in the VM, and the physical delivery contains
the XAP plus source WAVs but not generated XGS/XWB/XSB banks. Therefore neither an authentic XNA2
reference run nor a faithful audio-equipped XNA4 diagnostic run can be fabricated from the present
toolchain. The retained 464-line diagnostic patch records every migration and the successful
runtime/non-XACT build separately from the restored full-build XACT rejection.

## Live CNA boundary

Current CNA converts the two exact XNA4-built SpriteFonts and two Texture2Ds into four native CNBs.
It truthfully refuses the three `EffectReader` roots for CNB transcoding because CNB does not claim
a compiled-effect schema; authentic Effect XNBs are CNA's existing runtime route and were not
silently converted or replaced. Focused live qualification passes:

- **703/703** audio/XACT tests;
- **78/78** Effect, built-in XNB reader, SpriteFont and Texture2D content tests;
- **64/66** relevant real-GLES3 `BasicEffect`, `LineList`, user-primitive, render-target and
  compiled-effect-capability graphics tests, with two renderer-specific cases honestly skipped;
- all four generated CNBs through `cna_tool_cnb_info`.

These results establish healthy reusable framework paths, not a claim that the game has been
ported. A complete C++ product is still a 7,676-line multiplayer game with screen management,
simulation, vector rendering, bloom, XACT cue behavior, gamepad joining and vibration. Implementing
only its line renderer or converting the WAV files directly would be a sample workaround. No
bounded CNA or Sharp Runtime bug was identified, so neither repository was changed.

## Evidence and reproducibility

- `xna2-original/` — complete byte-identical 79-file upstream snapshot;
- `evidence/source-inventory.tsv`, `original.sha256`, `snapshot-diff.txt` — full inventory and
  integrity evidence;
- `scripts/build-win7-xna2-release.cmd`, `win7-build/xna2-release-msbuild.log` — exact XNA2 build
  boundary;
- `xna4-diagnostic/`, `evidence/xna4-diagnostic.patch`,
  `win7-build/xna4-runtime-without-xact-msbuild.log` and
  `win7-build/xna4-xact2-rejection-msbuild.log` — isolated migration, successful complete runtime
  plus seven non-XACT products, and restored XACT rejection;
- `cna-content-qualified/`, `evidence/cna-content.log` and the three focused test logs — exact
  four-success/three-honest-refusal content result and live framework qualification;
- `evidence/win7-offline-state.txt` — clean VM power-off and offline adapter state;
- `scripts/qualify.sh`, `evidence/qualification.log` — repeatable 8-worker-capped integrity,
  content and offscreen regression gates.

No original/native/browser image or interaction comparison is claimed because the authentic XACT2
banks and reference toolchain are absent and no modernization product has been authorized.

## Owner decision required

Choose one:

1. retain the measured source/build/content evidence as a historical XNA2 game non-port;
2. authorize an authentic XNA2 compatibility/faithful-port route, first providing or building the
   exact XACT2 XGS/XWB/XSB outputs and an authentic XNA2 reference gate, then porting the complete
   game without changing cue, renderer, multiplayer or input behavior; or
3. authorize a complete XNA4 modernization, including a deliberate XACT3 project upgrade/rebuild
   from the exact waves and cue graph, then port the entire result to C++ CNA and qualify native and
   WEBGL2 behavior.

Either implementation route must preserve all fourteen cue contracts, four-player joining/control,
gamepad vibration, complete screen flow, vector simulation and four-pass bloom. Browser
qualification also needs real multi-gamepad interaction evidence; keyboard-only substitution for
players two through four would not qualify the product.
