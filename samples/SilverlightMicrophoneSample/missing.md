# SAMPLE-144 — SilverlightMicrophoneSample audit

**Status: owner decision pending.** This is a complete Windows Phone 7.0 Silverlight application
that uses XNA's microphone and raw-audio APIs; it is not an XNA `Game`. No standalone game, reduced
recorder, HTML mock-up or alias of SAMPLE-098 was invented.

## Authoritative product

The complete upstream directory contains 20 files. Its one VS2010 solution selects one
`SilverlightApplication`/WindowsPhone-v4 project and produces a XAP library application. There is
no `Game`, `Program`, content project, XNB, processor, second solution or second runnable endpoint.
The runtime source consists of 369 C# lines, 77 XAML lines, two Phone manifests, nine loose UI/shell
images and the supplied documentation and licence.

The project references `Microsoft.Phone`, `Microsoft.Phone.Interop`, `System.Windows` and
`Microsoft.Xna.Framework`. Its defining product behavior is all present in `App.xaml`,
`MainPage.xaml` and their code-behind:

- `PhoneApplicationService` launch/activate/deactivate/close lifecycle and a
  `PhoneApplicationFrame` root;
- a portrait 480x696 Phone page, visible system tray, title/help text, state illustration and a
  three-button `ApplicationBar` with the exact Record, Play and Stop states;
- `Microphone.Default`, a 500 ms `BufferDuration`, `BufferReady` capture and an exactly sized
  byte buffer written into a reusable `MemoryStream`;
- a 33 ms `DispatcherTimer` that calls `FrameworkDispatcher.Update()` and observes playback state;
- playback on the sample's worker thread by constructing `SoundEffect` directly from the captured
  signed-16-bit mono PCM at the microphone's negotiated sample rate, then using
  `SoundEffectInstance` so the UI can return to its ready state when playback ends;
- the original blank/microphone/speaker images, exact help strings and button transitions.

The manifest requests `ID_CAP_MICROPHONE` (alongside the broad capabilities emitted by this early
Phone template). The readme dated 2010-11-17 explicitly describes a Silverlight for Windows Phone
microphone application and says that Windows Phone Developer Tools are required.

SAMPLE-098 is not this product. `MicrophoneEchoSample_4_0` is an 800x480 XNA `Game` with a live
waveform, circular echo DSP and keyboard/gamepad/touch controls. It proves the shared audio APIs,
but substituting it would discard SAMPLE-144's Phone page, ApplicationBar, lifecycle, images and
record-then-play workflow.

## Original XNA/Phone reference

The complete bytewise snapshot used for this audit is retained at:

```text
/rv/tmp/samples/SAMPLE-144-SilverlightMicrophoneSample/xna4-original/
```

The offline Win7 SP1 VM ran the unchanged Release solution through .NET 4 MSBuild. The exact
solution and project were selected, then the build stopped before compilation at the project's
first Phone import:

```text
SilverlightMicrophoneSample.csproj(110,3): error MSB4019:
  C:\Program Files\MSBuild\Microsoft\Silverlight for Phone\v4.0\
  Microsoft.Silverlight.WindowsPhone.Overrides.targets was not found

0 Warning(s)
1 Error(s)
```

This is the required-platform boundary stated by the readme, not a source diagnostic. The VM has
VS2010 and XNA Game Studio 4.0 but not the retired Windows Phone Silverlight targets, so no original
runtime or microphone-device capture is claimed. All eight VM network adapters remained `none`;
the build used no Internet and the VM shut down normally. The diagnostic log is
`xna4-build/release-msbuild.log`, and `scripts/build-win7-release.cmd` reproduces it from the
credential-free shared snapshot.

## Live CNA and Sharp Runtime audit

At CNA `5347b52eae1311fbea1f89955ae8a48c6843a88a`, the XNA audio portion is not a blocker.
`Microphone` uses the selected SDL3 recording provider for real enumeration, negotiated mono/S16
capture, queued-byte polling and bounded reads; `FrameworkDispatcher.Update`, raw-buffer
`SoundEffect` and `SoundEffectInstance` are implemented. The focused current regression run passed
198/198 tests across `MicrophoneStateTest`, `MicrophoneTest`, `MicrophoneCaptureTest`,
`FrameworkDispatcherTest`, `SoundEffectTest` and `SoundEffectInstanceTest`, including nonzero
capture, `BufferReady`, raw PCM construction and playback lifetime.

The Emscripten SDL3 archive contains its real `navigator.mediaDevices.getUserMedia` recording path.
More strongly, the retained SAMPLE-098 real-Chrome qualification already exercises current CNA's
same public microphone route: permission changed from `prompt` to `granted`, the captured waveform
changed from a one-pixel baseline to a non-flat 17-pixel span, Stop restored the exact baseline
hash, and 600 WebGL2 frames completed without runtime exception or unhandled rejection. The
machine-readable subset is retained as `evidence/cna-web-microphone-precedent.json`; the complete
source evidence remains under `/rv/tmp/samples/SAMPLE-098-MicrophoneEchoSample_4_0/`.

At Sharp Runtime `9cc96cd57cde394940cc24d58743edf9bf63d3fb`, neither `System.Windows` nor the
`Microsoft.Phone` page/frame, XAML, ApplicationBar, BitmapImage, dispatcher, lifecycle and XAP
application stack exists. CNA also has no compatibility host for that Silverlight product. The
missing subsystem is therefore the defining UI/application platform, not XNA microphone fidelity.

## Owner decision — `SAMPLES-DEC-005`

A faithful implementation now needs one explicit product-scope choice:

1. accept the evidence-backed historical Phone/Silverlight non-port;
2. authorize a complete retired-platform compatibility product, including XAML/page layout,
   ApplicationBar, image resources, Phone lifecycle/permissions/dispatcher and XNA audio
   integration; or
3. explicitly scope a complete native/WEBGL2 modernization that preserves the exact single-page
   record/play/stop UI, assets, state machine, microphone permission and lifecycle behavior.

Option 3 is a new product contract, not permission to wrap the audio calls in a generic `Game` or
reuse SAMPLE-098's waveform screen. No choice was made autonomously, so no CMake target and no
native/browser product were added. No CNA, Sharp Runtime, EasyGL or MetaGL source change was needed.

## Reproduction evidence

The stable artifact root is:

```text
/rv/tmp/samples/SAMPLE-144-SilverlightMicrophoneSample/
```

It contains the full original snapshot and SHA-256 manifest, source inventory, Win7 diagnostic and
offline state, the browser-microphone precedent subset, and credential-free build/qualification
scripts. `scripts/qualify.sh` rechecks the snapshot, the 198 focused CNA tests and the compiled
Emscripten recording route; its current result is `evidence/qualification.log`.
