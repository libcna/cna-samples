# SAMPLE-098 — MicrophoneEchoSample_4_0 audit

**Status: complete.** The C++ sample now follows the original XNA 4.0 source and
content without a sample-side framework workaround.

## Source and behavior

The authoritative package contains one shared game source plus Windows, Xbox 360 and
Windows Phone projects. The port retains the original
`MicrophoneEchoSample::MicrophoneEchoSampleGame` surface and its complete behavior:

- 800x480 landscape presentation, visible mouse and `Content` root;
- the original inactive `WINDOWS_PHONE` fullscreen, 30 Hz and instruction branch;
- Tap/DoubleTap and edge-triggered keyboard/gamepad A/B input;
- default-first microphone selection and reconnect status handling;
- 100 ms capture buffers and 44.1 kHz mono dynamic playback;
- the original 150 ms circular delay, 0.5 feedback mix and endian-safe 16-bit sample
  conversion;
- the original `BasicEffect` line-strip waveform;
- Back/Escape exit and the original update/draw order.

The stale renamed namespace/class, F1 overlay, early return after `Exit()`, loose
font atlas and runtime help texture were removed. `ReadSample` and `WriteSample`
again enforce the original alignment and range contracts. Their explicit negative
index guard is the C++ equivalent of the managed array access that would otherwise
throw. The stored `EventHandler` token is removed during C++ destruction and failed
initialization so CNA's cached microphone cannot retain a dangling `this` callback;
it does not change sample behavior.

The Windows Phone branch is source-complete, but this audit does not claim an actual
Windows Phone CNA host or physical mid-capture device-disconnect test.

## Authentic content and package files

The loose `font.font.json`, `font.png` and invented `Content/help.png` files are
gone. `Content/MyFont.xnb` is unchanged Windows/HiDef output from the original
`MyFont.spritefont` processed by XNA Game Studio 4.0:

```text
195d450bc86e37ee2b57e7fded7ca5b141ab377d5f798e5afe06235ef105b3b0  Content/MyFont.xnb
```

The same official content project also completed for Windows Phone/Reach; its
separate reference XNB has SHA-256
`4b01b7c7c08ccfb71a29a234e65be3887a3c821dd6ea1c99e40ba41b7beb5444`.
The upstream license and documentation image are retained at sample root and are not
loaded by the game.

## Original XNA reference

The unchanged Windows project and content project compiled with XNA Game Studio 4.0.
The resulting executable has SHA-256
`46ade66ac2ac34e6d7e907cb29ce800b6c658cdb69a17748db0acd4c9ba1854e`.
An isolated XNA run enumerated `PulseAudio Input`, showed
`Stopped -> Started -> Stopped` after A/B, drew the captured waveform and exited
cleanly with Escape.

Reference source, projects, build output, logs and captures are preserved under:

```text
/rv/tmp/samples/SAMPLE-098-MicrophoneEchoSample_4_0/
```

## CNA qualification

- Debug OPENGLES3 build: passed.
- Clean Release OPENGLES3 build: passed.
- Native A/B/Escape lifecycle: passed in isolated X11/PulseAudio runs.
- Deterministic native capture: a routed 440 Hz PCM source produced
  `Default Device is Started`, a clear waveform with approximately the expected
  66 periods across the 150 ms echo buffer, and clean B/Escape teardown. The captured
  source monitor measured -21.3 dB mean and -18.1 dB peak.
- SpriteFont XNB regression: 2/2
  `ContentManagerSpriteFontXnbTest` tests passed.
- Focused CNA audio regression: 95/95 `MicrophoneTest`,
  `MicrophoneCaptureTest` and `DynamicSoundEffectInstanceTest` tests passed.
- WebGL2 Release build: passed with the renderer's effective
  `MIN_WEBGL_VERSION=2` and `MAX_WEBGL_VERSION=2`.
- Real Chrome: a fresh profile began with microphone permission `prompt`, granted
  it through the browser permission route, and used a trusted A key event to start
  the actual fake-device 440 Hz capture stream. The waveform changed from a 1-pixel
  flat line to a 17-pixel span; B restored the exact original frame hash. The game
  completed a further 600 browser frames on a real WebGL 2 context with no runtime
  exception, unhandled rejection or relevant HTTP error. Captured output was
  non-silent PCM at 44.1 kHz stereo (-42.9 dB mean, -22.5 dB peak).

The browser runner waits for the asynchronous capture buffer to contain a meaningful
waveform before taking the Started capture; it does not inject audio into CNA memory
or repair product state.

No CNA, Sharp Runtime, EasyGL or MetaGL source change was required by this sample.
All browser and native evidence, including reusable qualification scripts, remains
in the artifact directory above.
