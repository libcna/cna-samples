# SAMPLE-060 — SoundAndMusic_4_0 fidelity audit

**Status: COMPLETE (2026-09-05).** The complete Windows Phone/Reach source, project, content
declarations, documentation and port were reviewed. The original `Content.Load<Song>` contract is
restored with authentic Microsoft XNA 4.0 SongProcessor output, the old loose-content substitutes
and invented controls are gone, and the original/native/browser gates are complete. The owner later
approved one opt-in CNA mouse-to-touch extension for desktop and browser operation; it is isolated
and documented in `diff.md`.

Upstream directory: `/rv/tmp/XNAGameStudio/Samples/SoundAndMusic_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-060-SoundAndMusic_4_0/`.

## Original product and behavior

Upstream ships one Windows-Phone-only game. Its five source/assembly units request a 480x800
full-screen portrait backbuffer and 30 Hz timing, read exactly one `TouchPanel` contact, and build
eleven `DrawableGameComponent` controls. The sample demonstrates:

- fire-and-forget `SoundEffect.Play()`;
- play, pause, resume and stop on a looped `SoundEffectInstance`;
- pan, pitch and volume changes on that instance;
- play, pause, resume, stop, repeat and volume on a `Song` through `MediaPlayer`.

The Song-volume handler retains the original physical-device/emulator split. A device assigns the
requested value directly; only the Windows Phone emulator clamps zero to `0.000001f` to avoid the
documented emulator bug. The upstream sample has no desktop keyboard or mouse behavior. The port's
single owner-approved pointer accessibility difference is documented separately in `diff.md`.

The exact upstream directory is retained under `xna4-original/`, including the phone solution and
project, manifests, HTML documentation, Microsoft sample licence and reference screenshot.

## Source audit and removed workarounds

`Button.cs`, `UIHelper.cs`, `SoundAndMusicSampleGame.cs`, `Program.cs`, `AssemblyInfo.cs`, the phone
project and the content project were compared with their ported representation. The port now
preserves the original namespace/class/file decomposition, component registration and ordering,
touch rectangle logic, drag restrictions, event order, UI coordinates, media state checks,
fullscreen dimensions and update interval.

The previous port's deviations are removed:

- no direct `Mouse` structures, parallel mouse controls or Escape path remain; the only pointer
  support is the owner-approved CNAEXT mouse-to-touch opt-in documented in `diff.md`;
- `Button` is again a real component with its own `SpriteBatch` and content lifecycle;
- all original sound and Song state transitions are present;
- `Microsoft.Devices.Environment.DeviceType` selects the original volume branch;
- the old loose PNG/WAV/font inputs and F1 overlay are absent;
- historical `help.png` remains only at the sample root and is not runtime content.

The original's redundant `DoOnNotTouching` sequence is intentionally preserved rather than
silently repaired. C# `string` now uses `SharpRuntime::String`; reference ownership, nullable touch
state, out parameters, property syntax and event callbacks use the established lossless C++
mappings. Required `CNAEXT GetTypeName()` overrides supply CLR names but add no game behavior.
The owner-approved behavior change is limited to
`TouchPanel::setMouseTouchEmulationEnabledEXT(true)`. It keeps the original `TouchPanel` path as the
only input path, is off by default framework-wide and is documented in `diff.md`.

## Exact content and Song provenance

The checked-in Phone/Reach content contains ten authentic XNA pipeline products. `cmp` verifies all
nine ordinary XNBs against the retained official build, while the Song XNB/WMA pair is byte-for-byte
identical to the offline Win7 export:

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
| `Sounds/Music.xnb` | `3554610e02ee60aaff062817a659cae26d5671f6d789caaf5133449fbb280b2f` |
| `Sounds/Music.wma` | `5b6ecb15235f85e89a5cbf101854a6a846b83ffdb293f43b104e7fe3dcca628a` |

The owner-provided offline 32-bit Win7 SP1 VM ran the unchanged content project through XNA Game
Studio 4.0 `Mp3Importer`/`SongProcessor` with `/m:1`. The XNB is the genuine 123-byte `XNBm`
product, names `SongReader`/`Int32Reader` and `Music.wma`, and records 32,054 ms. Its matching
801,351-byte stream is stereo 44.1 kHz WMA v2 of the same duration. All VM adapters were `none`,
the files left only through the `cna_song_export` shared folder, no credential is recorded, and the
VM shut down normally. The build log and hashes remain under
`/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/export/`.

SDL3_mixer does not decode WMA on the Linux/browser targets. The authentic XNB and WMA remain
checked in unchanged, and `Music.oga` is a deterministic lossless deployment companion selected
by CNA/FNA's normal sibling-media probing. It was generated from that exact WMA with:

```text
ffmpeg -nostdin -hide_banner -loglevel error -y -i Music.wma -map_metadata -1 \
  -af aformat=sample_fmts=s16:sample_rates=44100:channel_layouts=stereo \
  -fflags +bitexact -flags:a +bitexact -c:a flac -sample_fmt s16 \
  -serial_offset 0 Music.oga
```

Repeated conversion gives SHA-256
`c3d16b64b3ecfe72e0bcfbb40dc2001d505ca563439d40d15a7f2c3e9b7fb26b`.
Decoding the authentic WMA and Ogg-FLAC companion to stereo 44.1 kHz signed 16-bit PCM produces
byte-identical 5,652,480-byte streams with MD5 `8c0a3b34c5b4b1977b773689e4f01ecb`.
This changes neither asset identity nor audio samples and is the documented lossless conversion
allowed by the content policy, not a loose MP3 or handwritten-XNB substitute.

## Original XNA qualification

The unchanged content project built successfully in Win7 for both WindowsPhone/Reach and the
separate Windows/Reach audit target. The latter produced an authentic `XNBw`/WMA pair; its WMA
decodes to exactly the same PCM as the Phone stream. A labelled audit host compiles all unchanged
game sources with `WINDOWS_PHONE`; it supplies only the entry point normally generated by the
phone target and the unavailable host's `Microsoft.Devices` environment query.

Under the established Wine/WineD3D prefix, that executable loads the authentic Windows Song pair,
creates the original UI and runs without content, platform or runtime exception. Its fullscreen
desktop diagnostic uses the 960x900 Xvfb display while retaining the original phone-space UI
coordinates; the left 480x800 reference region has NCC 0.9601 against CNA. Wine's synthetic mouse
delivery into XNA `TouchPanel` is not deterministic enough to claim every timed control transition,
so the audit does not infer that claim from the desktop harness. The unchanged source,
documentation and real XNA load/render path establish the reference, while both CNA gates below
exercise every control with real touch events.

Evidence: `xna4-build/`, `scripts/{build-original,capture-original}.sh`, and
`evidence/xna-original-windows-song-full/`.

## CNA runtime repairs

Two general CNA changes support the faithful port:

- `cnanext a66fc61b5` added and tested the XNA-shaped `Microsoft.Devices.DeviceType` and
  `Environment.getDeviceTypeProperty()` API;
- `cnanext 0a6158e4f` normalizes XNA backslashes before Song/Video readers resolve an external
  sibling beside a nested XNB. Before it, `Content.Load<Song>("Sounds\\Music")` incorrectly sought
  `Content/Music.wma` on POSIX. Focused real-fixture coverage now exercises Song direct-reader and
  `ContentManager` nested-backslash loads plus Video sibling resolution; all 13 focused tests pass.

The second fix is shared content behavior, not a SAMPLE-060 special case. No sharp-runtime,
EasyGL or MetaGL repair was needed.

## Native OPENGLES3 qualification

The Release `SoundAndMusic_cna_samples` target builds with the active cnanext/sharp-runtimenext
chain and `--parallel 8`. Its clean product contains the ten XNBs plus authentic WMA and lossless
OGA, with no source PNG/WAV/MP3/font sidecar.

The isolated Xvfb/PulseAudio gate uses an evidence-only SDL input shim to turn synthetic mouse
events into the real SDL finger events that a digitizer would send; it is neither linked nor shipped
with the sample. The 480x800 OPENGLES3 run exercised every button and all four drags. The final
43.84-second stereo 44.1 kHz capture records:

- Laser at 10.70 s and the looped instance from 12.30 s;
- stored-instance pause from 19.97–21.57 s, resume, then stop at 24.18 s;
- Song play from 26.25–35.22 s, pause for 2.944 s, resume from 38.16–43.15 s, then terminal silence.

The screenshots prove pan, pitch and both volume handles moved. The log selects OPENGLES3 and has
no load, fatal or runtime error. Because the original phone game exits only through GamePad Back,
the harness terminates it after the completed scenario rather than inventing an Escape mapping.

Evidence: `cna-native-opengles3-song-qualified/`, `scripts/capture-cna-native.sh`, and
`evidence/cna-native-opengles3-song-qualified-qualified/`.

The 2026-09-05 requalification uses a clean Release
`cna-native-opengles3-mouse-touch/` build and removes the evidence-only SDL event shim entirely.
Ordinary left-button input now enters the unchanged sample through CNA's explicit mouse-to-touch
opt-in. The 480x800 run exercised all eleven controls and four drags, produced three distinct UI
captures, and recorded 45.696871 seconds of PCM 16-bit stereo at 44.1 kHz (mean -22.7 dB, peak
-2.4 dB). Its result records `all_controls_exercised=true`; the OPENGLES3 log has no fatal,
exception or runtime error.

Fresh evidence: `cna-native-opengles3-mouse-touch/` and
`evidence/cna-native-opengles3-mouse-touch-qualified/`.

## Real-browser WEBGL2 qualification

The complete pthread/Asyncify game bundle builds with `CNA_GRAPHICS_RENDERER=WEBGL2` and contains
the `.html`, `.js`, `.wasm` and preloaded `.data` product. The system Google Chrome run obtains
`WebGL 2.0 (OpenGL ES 3.0 Chromium)`, `crossOriginIsolated=true`, a 480x800 canvas and the normal
WebAudio mixer. CDP injects browser touch events, not a sample API shortcut.

The gate exercised the same eleven buttons/four drags, recorded real stereo 44.1 kHz audio and
completed 600 additional `requestAnimationFrame` callbacks. Song plays 24.33–32.93 s, pauses for
2.603 s, resumes 35.53–40.18 s, and remains silent for 10.35 s after Stop. There are no runtime
exceptions, unhandled promise rejections, relevant HTTP failures or fatal console messages.
Native and browser screenshots are pixel-identical for the ready, sound-control and Song-control
states; the stopped frame is also unchanged across the additional 600 callbacks.

Evidence: `cna-web-webgl2-song-qualified/`, `scripts/capture-cna-web.sh`, and
`evidence/cna-web-webgl2-song-qualified/`.

A clean `cna-web-webgl2-mouse-touch/` pthread/Asyncify bundle was built on 2026-09-05. The owner
served it with the required COOP/COEP headers and manually confirmed that pointer clicks activate
the controls and that both sound and music are audible. An initially silent run was traced to the
desktop selecting a different speaker, not to CNA or the bundle.

There is no remaining SAMPLE-060 blocker or sample-side workaround. The only active difference is
the owner-approved mouse-to-touch opt-in recorded in `diff.md`.
