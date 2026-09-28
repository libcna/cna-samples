# SAMPLE-098 — MicrophoneEchoSample_4_0 audit

**Current status: `🔎`, reopened 2026-09-28.** The owner reports weak/insufficient OPENGLES3 echo
after separating native and web playback. Controlled transient tests now establish the original
150 ms spacing and 0.5 attenuation in XNA, both native products and WEBGL2. The reported difference
with the physical microphone remains unresolved; an owner comparison of the current native
product alone with Wine is pending. No audio algorithm or gain was changed. Earlier qualification
remains evidence. See [`diff.md`](diff.md) for project/C++ mechanics and the diagnosis below.

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

## Historical original XNA reference

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

## Historical CNA qualification

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

## Status re-audit 2026-09-09

The `✅` holds. Re-verified rather than re-asserted:

| check | result |
|---|---|
| `Content/MyFont.xnb` against XNA Game Studio Windows/HiDef output | byte-identical, `195d450bc86e…` |
| method-for-method correspondence with the 487-line original | complete, including the `MicrophoneExtensions.IsConnected` helper as a static class of the same name, the endian branches in `ReadSample`/`WriteSample`, and the two exception types in the same order |
| focused CNA audio tests | **98/98** today (the row recorded 95; three have been added since and all pass) |
| real-Chrome WEBGL2 gate | re-read: WebGL2, cross-origin isolated, permission `prompt` → `granted`, 600 further frames, `stopped` and `stoppedAgain` share one hash, `started` differs, waveform vertical span 1 → 17 → 1, zero runtime exceptions |
| the one 404 the browser console recorded | `/favicon.ico`, correctly excluded from `httpErrors` — checked, not assumed |

Two things worth writing down.

### The native captures differ in one string, and the cause is in CNA, not in the port

The original and the port render the same frame except for the first line of the HUD, which is
`Microphone.Name + " is " + state`:

| | |
|---|---|
| `evidence/original-stopped.png` (unchanged XNA executable) | `PulseAudio Input is Stopped` |
| `evidence/cna-debug-stopped.png` (this port) | `Default Device is Stopped` |

That is not a porting defect. CNA's SDL3 provider prepends a synthetic device named
`"Default Device"` to the recording-device list and makes `Microphone::Default` that entry —
reproducing `FNA/src/FNAPlatform/SDL3_FNAPlatform.cs:1699,1707` down to the literal string. XNA's
`Microphone.Default` is a real enumerated device and reports the driver's name for it.

The port is faithful to CNA's API; CNA diverged from XNA. It was the fourth XNA-versus-FNA
divergence of that kind and the first backed by a side-by-side capture rather than by reading IL.

**Fixed in CNA the same day** (`cnanext` `30bd6cf60`), on the owner's ruling that CNA follows XNA
faithfully and FNA only after it. The synthetic entry is gone, `Microphone::All` is the machine's
device list and `Microphone::Default` is a real device. Rebuilt against that change and captured
again, this port draws:

```
Ryzen HD Audio Controller Stereo Microphone is Stopped
```

— a real device name from the driver, the shape XNA produces
(`evidence/cna-release-stopped-after-xna-default-fix.png`, taken by
`scripts/capture-cna-native.sh`). The older `cna-debug-stopped.png` and `cna-release-stopped.png`
are kept as the record of the divergence rather than replaced.

**That first fix broke native capture, and the owner caught it.** Removing FNA's synthetic entry
was right; leaving the list sorted by SDL id alone was not. `Microphone::Default` is `All[0]`, so
ordering decides which device a game records from, and the lowest-id device here is the machine's
second, unconnected microphone rather than the host default. Capture started and returned silence:
the echo stopped working on native while the unchanged XNA original and the WEBGL2 build both kept
working, which is exactly the shape of the report. `pactl` confirmed the app's recording stream
attached to PipeWire source 66 (Mic2) while the system default is source 67 (Mic1).

Fixed in `cnanext` `5f491663a`: the provider identifies the host's default among the real devices
through `SDL_GetAudioDeviceName(SDL_AUDIO_DEVICE_DEFAULT_RECORDING)`, marks it, and puts it first,
with the rest in id order. Still XNA's shape — real devices, real names, no invented entry — and
XNA's enumeration puts the system default first too.

Proved end to end rather than by inspection. `scripts/verify-native-echo.sh` feeds a 440 Hz tone
into the app's own recording stream and records the app's own playback stream, both redirected
per-stream so no system default is touched:

| | |
|---|---|
| app's output, mean volume | −21.1 dB |
| dominant frequency of that output | ~440 Hz |

Before the ordering fix the same harness had nothing to record. The lesson is recorded here because
the first fix passed 705 unit tests, a full audio suite and a screenshot, and still broke the
product: none of them could see *which* device had been selected, only that a device had been.

The sample itself is unchanged: not a line of the port moved, because the defect was never in it.
Inventing a device name here to match the original would have been exactly the workaround the
campaign forbids.

### Two campaign-wide items this sample is part of

`samples/MicrophoneEcho/help.png` is one of the 51 unreferenced `help.png` files already logged for
end-of-campaign cleanup — no `CMakeLists.txt` in the corpus references one. And
`src/MicrophoneEchoSampleGame.hpp` carries zero `@brief` comments across roughly 28 public members;
200 of the corpus's 338 sample headers have at least one, and the undocumented set is already an
open end-of-campaign decision. Neither is a defect in this port; both are named here so the row's
green status is not read as covering them.

Implementing the game class entirely in the `.hpp` is the corpus convention, not an anomaly — the
same shape appears in more than twenty other ported samples — so it is not a finding either.

## Current-head re-analysis — 2026-09-28

Starting heads: cna-samples `develop 42c52c1`, CNA `next 1ca684199`, Sharp Runtime `next fc033a0e`.
This turn analyzed the existing port; no game/framework source or new build was added.

### Source, content and controls

The physical package is **18 files / 198,464 bytes**, with one shared game and three target
projects: Windows/x86 HiDef, Xbox360 HiDef and Windows Phone Reach. All use the same game,
entry-point and assembly-metadata source files and one content project. The complete retained
snapshot matches upstream byte for byte, and both its source and original-output hash manifests
pass. The checked-in `MyFont.xnb` still matches official Windows/HiDef output exactly.

The 487-line original game and the complete C++ game, entry point, metadata and content declaration
were reviewed. The default-first microphone selection, disconnect handling, BufferReady events,
100 ms capture, 150 ms circular delay, 0.5 feedback, endian branches, input ordering, waveform and
inactive Phone branch remain present. The targeted bypass scan found only required type identity
and assembly-title metadata; no backend call, invented help overlay or replacement asset path was
found. Event-token removal and negative-index checks remain C++ ownership/bounds mechanics.

This game is **not touch-only**: A starts, B stops, and Escape/Back exits in the original desktop
branch. Tap/DoubleTap are also preserved. The owner's shared mouse-to-touch requirement for
touch-only games does not require inventing an input path here. The original contains no
application use of `System.Threading`.

### Fresh runs of the retained original and native products

The unchanged retained Windows/x86 Debug HiDef XNA executable was rerun with Wine prefix
`/home/robertvokac/.wine-cna-xna40`, `WINEDLLOVERRIDES=d3d9=b`, and an isolated 1280×1024 Xvfb.
The retained Release OPENGLES3 executable was run separately. Both passed
`Stopped → Started → Stopped` with A/B and returned exit code 0 after Escape.

An initial short-XTest-input attempt missed state transitions; its evidence was preserved.
Repeating with 600 ms key holds passed on the first A and B attempt in both products without
changing either game. The stopped 800×480 frames are pixel-identical outside rows 45–84 containing
the driver's microphone name. XNA reports `PulseAudio Input`, while native reports the actual
SDL driver name. This is the documented provider-name difference, not a substituted sample string.

A per-stream native audio probe redirected only the recording and playback streams owned by the
game's PID. A 440 Hz tone passed through the real microphone/echo/output route: **−21.09 dBFS mean,
440.04 Hz dominant frequency**, measured in the 48 kHz stereo host-output recording. The game
keeps its original 44.1 kHz mono processing. No system default recording or playback device was
changed. This verifies the retained native binary, not a fresh build against today's framework.

### Current focused tests and their limit

The existing current aggregate `CnaTests` ran **104 tests: 103 passed, one failed**, using its
configured HEADLESS renderer. All **101 microphone, capture, dynamic-sound and SDL recording-device
tests passed**; two SpriteFont fixture tests passed as well. The additional
`ContentManagerSpriteFontXnbTest.ReachLoadsAnAuthoredNpotDxtSpriteFontAtlas` test failed because
HEADLESS throws `std::runtime_error` instead of `System::NotSupportedException` when constructing
a Dxt3 texture. That fixture is not this sample's font. The failure is retained and is not reported
as a successful GLES regression gate; no HEADLESS renderer change was made during this sample
analysis. Current SDL provider source still contains the real-device/default-first fix.

### Old browser bundle fails the ordinary static-hosting route

The retained JavaScript creates shared WebAssembly memory and pthread workers. A fresh system
Google Chrome probe over ordinary `http.server` without COOP/COEP reproduced:

```text
DataCloneError: Failed to execute 'postMessage' on 'Worker':
SharedArrayBuffer transfer requires self.crossOriginIsolated.
```

The page stays at a 300×150 canvas, reports an exception, and never logs the WEBGL2 renderer or
starts the game. The earlier isolated-server microphone/600-frame qualification remains valid
historical evidence for that old bundle; it does not prove compatibility with ordinary static
hosting. The game itself does not need this shared-memory ABI.

The retained build script and manifest also reference nonexistent `openeggbert` source/runtime
paths. The native binary has an obsolete RUNPATH, although today's system SDL libraries let it
run. Refresh the reproduction scripts to the active `libcna` checkouts, shared ccache settings and
unrestricted `--parallel` build convention. Rebuild Release OPENGLES3 and WEBGL2 with
`CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF`; then verify permission, real capture/echo, A/B/Escape
and further frames in real Chrome over plain static HTTP before updating the gallery and status.
This needs renewed build/qualification work, not a sample workaround or a newly identified large
framework subsystem. No new bundle or gallery entry was produced by this analysis.

Current evidence:
`/rv/tmp/samples/SAMPLE-098-MicrophoneEchoSample_4_0/evidence/current-head-analysis-20260928/`
(`inventory.json`, `focused-tests.log`, `retained-games-result.json`, `held-input/result.json`,
`native-echo-current-analysis.wav`, `frame-comparison.json`, `static-web-result.json`, captures and
logs). The `probe-static-web.mjs` reproduces the Chrome probe against a plain local server.

## Current-head completion — 2026-09-28

The owner requested implementation/requalification of SAMPLE-098. Build heads were cna-samples
`develop 780cae6` plus the sample changes recorded here, CNA `next b2fd47a45` and Sharp Runtime
`next fc033a0e`. CNA's intervening MorphTarget commit belongs to another session; this task changed
neither framework repository. The preceding analysis's `1ca684199` head and old products remain
historical evidence.

### Faithful source and project settings

The exact **18 files / 198,464 bytes** upstream snapshot was rechecked. The 487-line original's
microphone selection, events, input, disconnect handling, capture buffers, circular delay,
feedback, endian conversion and waveform remain represented in the complete port.

The current `EffectPassCollection` returns a pointer, so its `Apply()` invocation was mechanically
corrected from `.` to `->`. Final project review also found an omitted setting: the Windows/Xbox
projects declare HiDef and the Phone project Reach. `AssemblyInfo.cpp` now carries that setting
through CNA's existing `ProjectGraphicsProfileEXT`, including the inactive Phone branch. The exact
upstream `Game.ico` and `GameThumbnail.png` are restored at sample root. These are project/language
mechanics documented in [`diff.md`](diff.md); the audio algorithm and original controls are intact.
No sample workaround, renderer-specific branch or replacement behavior was added.

### Fresh original and exact content

`scripts/build-original.sh` rebuilt the unchanged Windows/x86 Debug HiDef game and both official
XNA content targets. The compiler embeds `Windows.v4.0.HiDef` in the original runtime-profile
resource. Fresh executable SHA-256:

```text
9a8c45f96d6ee40baf97644882eb06262c60b5852772506e750baee4191a80b4
```

Windows/HiDef `MyFont.xnb` remains byte-identical across the checked-in file, official pipeline
output, original deployment and native deployment (`195d450bc86e…`). The freshly rebuilt
Phone/Reach font also retains its recorded `4b01b7c7c08c…` hash. The reproduction script now
handles content hard-linked by an earlier prune without failing on a same-file copy.

The fresh original ran with `/home/robertvokac/.wine-cna-xna40`, `WINEDLLOVERRIDES=d3d9=b` and a
private Xvfb display. It passed A/B's `Stopped → Started → Stopped`, drew the waveform and exited
with code 0 on Escape. An initial 16-second window wait was insufficient after rebuilding;
that failed harness attempt is preserved. The runner now waits up to 80 seconds and records the
window tree on timeout; the repeated original run passed without changing the game.

### Final Release OPENGLES3

`scripts/build-cna-native.sh` uses the active libcna CNA/Sharp Runtime roots, static CNA,
`CNA_SAMPLES_ONLY=MicrophoneEcho`, the shared ccache with `CCACHE_BASEDIR=/rv` and all available
CPU cores. The final product is:

```text
cna-native-opengles3/samples/MicrophoneEcho/MicrophoneEcho_cna_samples
```

Its RUNPATH points to the active CNA prebuilt SDL, replacing the retired checkout path.
The final HiDef product passed A/B and Escape with exit code 0. Only audio streams owned by the
game PID were routed to private test sinks. An external 440 Hz tone passed through the real
microphone, circular echo and dynamic output APIs:

| Product | Output RMS | Dominant frequency | Recording format |
|---|---:|---:|---|
| Fresh original XNA | −21.115 dBFS | 440.138 Hz | 48 kHz stereo host monitor |
| Final OPENGLES3 | −21.083 dBFS | 440.076 Hz | 48 kHz stereo host monitor |

The game retains its original 44.1 kHz mono processing. No system recording or playback default
was changed. Stopped 800×480 frames match the original pixel for pixel outside rows 45–84, which
contain the provider's actual microphone-name HUD. XNA shows `PulseAudio Input`, SDL shows
`Ryzen HD Audio Controller Digital Microphone`, and the browser reports
`System audio recording device`; the port reads `Microphone.Name` in every case.

### Final nonthreaded Release WEBGL2 and gallery

`scripts/build-cna-web.sh` builds Release with
`CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF`. The original game has no application thread path
to preserve. The final WASM is **7,930,711 bytes**, contains no `debug_info`, and the JavaScript
contains no `PThread`, `shared:true` or `new SharedArrayBuffer` tokens. The Emscripten shell is
unmodified.

Real system Google Chrome, launched from the terminal with a fresh isolated profile, passes over
ordinary static HTTP without COOP/COEP (`crossOriginIsolated=false`):

- real WebGL 2 context and 800×480 canvas;
- microphone permission starts at `prompt` and becomes `granted` through browser permission;
- trusted A input starts actual `getUserMedia` capture from Chrome's external 440 Hz test device;
- the waveform changes from a 1-pixel baseline to a 12-pixel span;
- actual speaker-output recording is non-silent with a dominant 440 Hz tone;
- trusted B input restores the exact original canvas hash;
- 600 further browser frames complete, then Escape logs normal context cleanup;
- zero runtime exceptions, unhandled rejections, relevant HTTP errors or fatal console messages.

The optional `/favicon.ico` request remains excluded from required-asset errors. The browser
runner supplies input and an external audio device; it never injects sample/CNA memory or
replaces product state. Browser stopped pixels also match the fresh XNA frame outside the
microphone-name HUD.

The byte-identical four-file bundle is copied to the local `samples.libcna.com/MicrophoneEcho/`
directory. Its independent plain-HTTP Chrome gate repeats permission, echo, A/B, 600 frames and
Escape. The gallery has its **81st card**, detail page, original source link, adjacent navigation
and a screenshot from the final game's Started state. All 81 cards are unique and the affected
local links resolve. Layout was visually checked in Chrome. Publication awaits an owner-requested
push; no push was made during this task.

Local gallery commit: `25005b6` (`SAMPLE-098: add verified Microphone Echo WebGL2 gallery entry`).
Measured final browser output has a 440.0 Hz dominant tone in a selected one-second active segment:
−31.833 dBFS for the build and −31.988 dBFS for the copied gallery bundle. The full recordings,
selection offsets, hashes and device formats are retained in `inventory.json`.

### Regression and evidence

All **three existing SpriteFont XNB tests pass on current OPENGLES3**, using CNA's required private
Weston/Xwayland runner. This includes the Reach/NPOT fixture that the preceding aggregate
HEADLESS run could not qualify. That older 103/104 result is preserved rather than rewritten;
its 101 passing audio/provider tests remain recorded in the analysis above. No framework source
change was needed. The final targeted scan was reviewed against the original and found only
managed identity, title/profile metadata and original Phone conditionals.

Artifact root:
`/rv/tmp/samples/SAMPLE-098-MicrophoneEchoSample_4_0/`.
Current evidence is under `evidence/requal-20260928/`: `inventory.json`,
`frame-comparison.json`, `original/`, `native/`, `web/`, `gallery-web/`, final build/probe logs,
`font-opengles3-tests.log`, effective build configurations and `no-workaround-scan.txt`.
The `before-profile-metadata/` directory preserves preliminary runs. The old failing threaded
bundle remains under `evidence/current-head-analysis-20260928/legacy-threaded-bundle/`.

Reusable scripts are `build-original.sh`, `build-cna-native.sh`, `build-cna-web.sh`,
`capture-original.sh`, `capture-cna-native.sh`, `capture-cna-web.sh`, `test-font-opengles3.sh`
and `inventory-current.py`; `verify-native-echo.sh` now delegates to the safe per-PID harness.
`MANIFEST.md` records the current products and exact reproduction commands. Build intermediates
are retained pending the owner's prune instruction; no new prune was applied.
The final dry run proposes 28 intermediate paths, approximately **319.2 MB** (419.1 → 100.0 MB,
before stripping/deduplication). Its exact report is `evidence/requal-20260928/prune-dry-run.log`.

## Reopened audio diagnosis — 2026-09-28

The owner ultimately separated simultaneous web/native playback and reaffirmed weak native echo.
Their executable is the retained `cna-native-opengles3-release/samples/MicrophoneEcho/` product.
The current build is `cna-native-opengles3/samples/MicrophoneEcho/`. This distinction does **not**
establish that the old product causes the report: both products were measured separately and both
produce the original echo response. Both initially capture the host's actual Digital Microphone,
source 62, rather than the unconnected second microphone involved in the historical defect.

### Transients instead of a continuous tone

The earlier 440 Hz test establishes capture/playback, but 150 ms is exactly 66 periods of that
tone. Its copies overlap in phase, so it cannot separately establish the delay or attenuation.
`scripts/probe-echo-response.py` now feeds four external 20 ms Hann-window chirps, 900–4500 Hz,
at 1/4/7/10 seconds. Each original/native process runs on its own Xvfb display. Only streams owned
by that process are moved to private input/output sinks; the host defaults remain unchanged.
Separate monitor recordings preserve the actual supplied input and application output.

Matched convolution measures each output copy against the supplied input:

| Product | First-copy gain relative to input | Copy spacing | Second/first-copy gain |
|---|---:|---:|---:|
| Fresh unchanged XNA under Wine | 0.4941–0.4966 | 150 ms, with the gap noted below | 0.49994 |
| Current Release OPENGLES3 | 0.49882 | 150 ms | 0.49993 |
| Retained Release OPENGLES3 used by the owner | 0.50084 | 150 ms | 0.49993 |
| Current WEBGL2, relative to its processed browser input | 0.49444–0.49974 | 150 ms | 0.49993–0.49998 |

All four native bursts have eight measured copies with relative gains approximately
`1, 0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625, 0.0078125`. The retained product has the same
response. One Wine burst contains an additional 32 ms output gap before its fourth copy; the
following copies return to 150 ms spacing. This host-run discontinuity is preserved in the data,
not omitted from the comparison. The original's feedback gain still matches both native products.

Approximate first-response latency in the separate monitor recordings is 285 ms for current
native, 328 ms for retained native and 298–330 ms for Wine. Recorder alignment and host buffering
affect these numbers; copy spacing and gain ratios do not depend on that alignment. The browser
callback method below is different, so its absolute latency is not compared with these values.
All three desktop runs pass A/B and clean Escape exit. No game/framework source changed.

### Browser processing is an observed difference, not a confirmed explanation

An isolated fresh system Chrome uses the exact same external WAV through its fake capture device.
`scripts/chrome-echo-response.mjs` copies the SDL recording/playback callback buffers while calling
the original callbacks unchanged. Analysis uses each actual browser-processed input burst as its
reference, rather than assuming that Chrome supplies the unmodified WAV. Six complete bursts
produce the same 150 ms / 0.5 echo response; the external capture file loops during observation.
A/B restores the exact stopped frame, Escape cleans up normally, and there are no runtime or
required-asset errors.

Actual `MediaStreamTrack.getSettings()` reports `autoGainControl=true`,
`noiseSuppression=true` and `echoCancellation=true`, with 48 kHz input, a 48 kHz AudioContext and
2048-frame recording/playback callbacks. SDL requests `getUserMedia({audio:true, video:false})`
without overriding these settings. The processed first burst peaks at 0.99997 from an external
WAV peak of about 0.4; subsequent bursts peak near 0.376. Thus this browser path demonstrably
processes the input before the sample receives it.

These are measurements of the diagnostic Chrome device, not the owner's browser or physical
microphone. Automatic input processing and host buffering are possible contributors to a different
subjective result; they are **not a confirmed cause** of the reported native weakness. Controlled
tests found no weaker native feedback relative to XNA. Increasing native gain or delay would
change the original sample without correcting a demonstrated defect, so no such change was made.
The physical-microphone report remains open pending the owner's current-native-only comparison.

Evidence is in artifact `evidence/echo-response-20260928/`: `result.json`, original/native/retained
input/output WAV and raw monitor recordings, screenshots/run logs, `system-defaults.json`, and
`web/{result.json,audio-callbacks.json,response-analysis.json,audio-browser.wav}`. Reproduction
uses `probe-echo-response.py`, `capture-web.py --tone-file ... --driver .../chrome-echo-response.mjs`
and `analyze-web-echo-response.py`; the standard 440 Hz qualification remains the runner's default.
