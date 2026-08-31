# SAMPLE-060 — SoundAndMusic_4_0 audit record (ready to complete)

Audit date: 2026-08-30. Upstream directory:
`/rv/tmp/XNAGameStudio/Samples/SoundAndMusic_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-060-SoundAndMusic_4_0`.

Status: **🛠 ready for final integration and qualification.** The source translation, nine
ordinary Phone/Reach XNBs and required CNA API are complete. The offline Win7 XNA 4.0 pipeline has
now produced the authentic tenth content item, so no owner/environment blocker remains and no
loose-file or handwritten-XNB substitute is needed.

## What upstream contains

This is a Windows-Phone-only XNA 4.0 Reach game. It requests a 480x800 full-screen portrait
backbuffer and 30 Hz timing, reads one `TouchPanel` contact, and implements its controls as eleven
real `DrawableGameComponent` buttons. The UI demonstrates:

- fire-and-forget `SoundEffect.Play()`;
- play, pause, resume and stop on a looped `SoundEffectInstance`;
- pan, pitch and volume sliders on that instance;
- play, pause, resume, stop, repeat and volume on a `Song` through `MediaPlayer`.

The song-volume handler selects the physical-device branch of
`Microsoft.Devices.Environment.DeviceType`; only the Windows Phone emulator needs the original
`0.000001f` lower clamp that avoids its zero-volume bug. The content project lists six textures,
one Segoe UI 18 pt SpriteFont, two WAV sound effects, and one MP3 processed by
`Mp3Importer`/`SongProcessor`. The exact upstream snapshot, including the phone project,
manifests, documentation screenshot, HTML and Microsoft sample licence, is retained under
`xna4-original/`.

## Source translation completed

The previous port's five recorded deviations have been removed from the source:

- `IsFullScreen`, 480x800 and `TimeSpan::FromTicks(333333)` are restored;
- invented mouse structs and mouse controls are gone; the game reads `TouchPanel` exactly as the
  original does;
- `Button` is again a `DrawableGameComponent`, is registered in `Game.Components`, owns its own
  `SpriteBatch`, and preserves the original touch/drag/event order;
- namespace, class and file decomposition are restored as `SoundAndMusicSample`, `Button`,
  `UIHelper`, and `SoundAndMusicSampleGame`;
- the exact `Microsoft.Devices.Environment` device/emulator branch is restored;
- the F1 overlay and its content wiring are gone; historical `help.png` is at the sample root.

The port deliberately preserves the original's redundant `DoOnNotTouching` sequence (it clears
`dragStarted` and then tests it), rather than silently repairing sample behavior. C++ differences
are mechanical ownership, property-call and event-lambda translations only.

`cnanext` commit `a66fc61b5` adds the general `Microsoft.Devices.DeviceType` enum and static
`Environment.getDeviceTypeProperty()` API in the devices module. CNA hosts report `Device`; two
focused tests pass in `CnaDevicesTests`. The native OPENGLES3 target compiles successfully with
`--parallel 8`.

## Content completed on this host

The official Microsoft XNA 4.0 `BuildContent` task produced these **Windows Phone / Reach** XNBs
directly from the unchanged source files:

| Asset | SHA-256 |
|---|---|
| `Fonts/GameFont.xnb` | `5b011192df99c1652e441067e64dcef4f98b72f21c30b4f7212ceae7d8af6ec6` |
| `Images/bg.xnb` | `9ac711473dae4a8040195c28c6a01ab7138cbb5025171d6ed3b03bc3820831cd` |
| `Images/pauseButton.xnb` | `5beec3ec8213bf05758b0a6922da10587f9ef86059b661e108a1a25186a56af4` |
| `Images/playButton.xnb` | `916031d761b0bd4ead9c64056eade4aa6f3f32222d4fc22bf3210b5f7594de68` |
| `Images/sliderHandle.xnb` | `d79bd2c5ef81eaa6e037ec0de6b2101937ef79860cd196030c3fa11cbf67246b` |
| `Images/sliderStrip.xnb` | `483323e89de037fc3fc964459d4ebf25b8cebc0fdd074442e79bbbb1753a4ba2` |
| `Images/stopButton.xnb` | `6327b3e0c1ae05986ec16c7a7b75b88ab7c3838292fba51461f450bbf7e00b25` |
| `Sounds/EngineLoop.xnb` | `32f567023ef35338139f7aa21b005073b1e4ff413eb022f099c200b1c225ba44` |
| `Sounds/Laser.xnb` | `a786126acd058fb6754b2d3240674bad8b3c55d83d274817acb20830f45098b1` |

These nine replace the old loose PNG/WAV/custom-font substitutes. The old files are retained only
in the artifact's `pre-port-content-backup/`, not in the sample. The documentation HTML was
already byte-identical to upstream; the original licence and screenshot are now retained too.

## Resolved prerequisite: the authentic Song output

The tenth content item is `Sounds/Music.mp3` through `Mp3Importer` then `SongProcessor`.
`SongProcessor` must emit `Music.xnb` plus an external Windows Media stream. On this Linux host:

1. the unchanged MP3 import succeeds, then XNA reports
   `Could not convert audio file Music.mp3 to WindowsMedia format`;
2. a diagnostic, lossless MP3-to-PCM WAV input reaches the same failure, excluding Wine's MP3
   decoder as the cause;
3. forcing the native `wmvcore.dll` already installed in the XNA Wine prefix produces the same
   failure;
4. no authentic `Music.xnb`, built XAP, or matching precompiled sample exists anywhere in the
   local XNA/sample/library trees;
5. after the owner supplied guest access, the offline Win7 SP1 VM rebuilt the unchanged content
   project through official XNA 4.0 `Mp3Importer`/`SongProcessor` for WindowsPhone/Reach. It
   produced `Music.xnb` (123 bytes, SHA-256
   `3554610e02ee60aaff062817a659cae26d5671f6d789caaf5133449fbb280b2f`) and `Music.wma`
   (801,351 bytes, SHA-256
   `5b6ecb15235f85e89a5cbf101854a6a846b83ffdb293f43b104e7fe3dcca628a`). The XNB is `XNBm`,
   names `SongReader`/`Int32Reader` and `Music.wma`, and its 32,054 ms duration exactly matches the
   valid stereo WMA v2 stream. Evidence is retained under
   `/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/export/`.

Logs are `evidence/build-original.log`, `build-original-pcm-pipeline.log`, and
`build-content-native-wmvcore.log`. The unchanged game sources themselves compile successfully
with the generated Windows-Phone entry-point harness and a minimal evidence-only
`Microsoft.Devices` shim (`evidence/build-original-sources.log`); they cannot run through
`LoadContent` without the missing Song artifacts.

The rejected alternatives are intentional: the port does **not** keep the old loose MP3 path,
handwrite a 74-byte `SongReader` XNB, or check in a third-party WMA transcode and call it official.
Any of those would make the sample appear runnable while violating the zero-workaround and exact
content rules. The next step is instead to install the authentic pair at the original
`Sounds/Music` path and complete the native and real-browser touch/audio gates.

### Owner/environment action needed

Build the unchanged content project on the prepared Windows 7 + Visual Studio 2010 + XNA Game
Studio 4.0 VM (or provide that build's Windows Phone `Sounds/Music.xnb` and matching external media
file). Once those two authentic outputs are available, the remaining work is bounded: insert the
files, run the unchanged original, native OPENGLES3 and real-Chrome WEBGL2 interaction/audio gates,
then change this row to `✅`.
