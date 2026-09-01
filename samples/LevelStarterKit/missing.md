# SAMPLE-128 — `LevelStarterKit` audit and owner decision

## Status

The complete source, XAML, resource, binary, installed-toolchain and live-dependency audit is
finished. This directory is a complete Windows Phone 7 **Silverlight application**, not an XNA
game, content pack or missing variant of another CNA sample. Its project has zero XNA references.
No fake `Game`, reduced bubble simulation, desktop-control substitution or HTML/CSS rewrite was
added. A faithful port requires an explicit product/UI/platform modernization decision under
`SAMPLES-DEC-005`, or an evidence-backed historical non-port decision.

## Complete inventory and project contract

The retained directory has exactly 35 files / 393,444 bytes. Its ten C# files contain 2,195 lines,
its two XAML files contain 233 lines and its six `.resx` files contain 750 lines. `Level.csproj`
names every one of those source, application/page and embedded-resource files with no physical
source omitted and no nonexistent item substituted.

The project is a `SilverlightApplication` library/XAP targeting `Silverlight` / `WindowsPhone`
v4.0. It references `Microsoft.Devices.Sensors`, `Microsoft.Phone`, `System.Windows` and ordinary
.NET assemblies, but not `Microsoft.Xna.*`. It imports both Phone Silverlight v4 target files,
declares six cultures (`en-US`, `en-GB`, `fr-FR`, `it-IT`, `de-DE`, `es-ES`) and produces
`Level.xap`. The Phone 7.0 manifest has runtime type `Silverlight`, one and only one capability
(`ID_CAP_SENSORS`), a `MainPage.xaml` default task and resource-DLL shell title/tile tokens. The
original readme explicitly calls this a complete C# Level application and requires Windows Phone
Developer Tools.

## Complete application behavior

The delivery is substantially more than a sensor reading or moving sprite. `AccelerometerHelper`
is a 560-line singleton sensor service with:

- raw, first-order low-pass, threshold-triggered low-amplitude and rolling-average outputs;
- a 25-sample / nominal 500 ms average and stability window;
- low-pass coefficient `0.1`, noise threshold `0.05g`, maximum calibration tilt 20 degrees and
  stability delta 0.5 degrees;
- independent X/Y zero calibration stored through `IsolatedStorageSettings.ApplicationSettings`;
- start, stop, dispose, failed-sensor and reading-event lifecycle handling.

This is not the 152-line `TiltPerspective_4_0` helper: their complete sources and hashes differ.
Borrowing that existing port would discard calibration, filtering, stability and persistence.

`DeviceOrientationHelper` maps seven states (unknown, two flat, two portrait and two landscape)
to angle, polarity and normal-gravity vectors. It applies a `0.8g` threshold/hysteresis and raises
a separate orientation event.

`MainPage` supplies two complete visual/physical modes:

- a 358-pixel circular surface level with two projected angles, moving/deforming bubble, target
  box and highlight;
- a rotating 800-pixel tube level with one angle, clipped glass, bubble translation, stretch and
  skew;
- a no-accelerometer state and a dynamically enabled calibration application-bar button;
- theme resources and localized UI strings rather than baked colours/text;
- UI-thread dispatch, fade and orientation storyboards;
- adaptive numeric refresh at 10 Hz, 2 Hz or after 3 seconds using 0.6/0.3-degree thresholds;
- 50 Hz nominal integration with buoyancy `15`, viscosity `10`, edge-loss `0.8`, collision,
  stretch and skew behavior rather than directly mapping sensor coordinates to pixels.

`App` connects launch, activate, deactivate, close, obscured and unobscured Phone lifecycle events
to sensor ownership. A port cannot silently replace this with an always-running desktop loop.

## Localization and vendor resources

The root `AppResLib.dll` and six locale `.mui` files are 2,048-byte PE32 resource-only shell-name
deliveries. Their UTF-16 resources preserve these app/tile names:

| Locale | Name |
|---|---|
| `0407` / de-DE | Wasserwaage |
| `0409` / en-US | Level |
| `040c` / fr-FR | Niveau |
| `0410` / it-IT | Livella |
| `0809` / en-GB | Level |
| `0c0a` / es-ES | Nivel |

The application also includes the exact shell icons, calibration icon, theme-aware XAML resources
and six localized string tables. Treating the `.dll` files as executable dependencies or omitting
their shell-identity role would be incorrect.

## Live CNA / Sharp Runtime boundary

Live CNA already has the real `Microsoft::Devices::Sensors::Accelerometer` surface and backend.
The focused suite passes 89 executable tests; two supported-real-hardware routes are skipped on
this sensorless host. This proves a useful foundation, not application parity: the sample-owned
filter/calibration/orientation/physics code has not been translated.

Live Sharp Runtime passes 8/8 `ResourceManager` tests and 63/63 isolated-storage confinement tests.
It does not expose the exact Phone `IsolatedStorageSettings` application-settings dictionary.
Neither live CNA nor Sharp Runtime contains the Silverlight/Phone page, application bar,
dispatcher-timer, storyboard/control/layout/theme/navigation and XAP application stack used by
the product. Implementing that complete framework merely for this row would be a large new
subsystem, while replacing it with an unapproved UI changes the product.

## Authentic toolchain boundary

The exact 35-file directory was copied to the owner's authorized
`C:\Users\vboxuser\Desktop\XNAGameStudio\SAMPLE128` workspace in the offline/headless Win7 VM.
The installed .NET 4 MSBuild selected the unchanged `Level.csproj` for `Debug|Any CPU` Rebuild and
stopped at its first Phone import because
`Microsoft.Silverlight.WindowsPhone.Overrides.targets` is absent. This is the prerequisite named
by the original readme, not a C# compile failure. The result is 0 warnings / 1 MSB4019 error.

No source/import was altered, no network was enabled and no different desktop project was used.
Therefore no XAP, emulator/device run, accelerometer interaction or rendered original frame is
claimed. The VM finished `poweroff` with `nic1` through `nic8` all `none`; the owner's real display
was not used.

## Owner choice required

Choose one product boundary:

1. Accept an evidence-backed historical Windows Phone Silverlight non-port. Retain this audit and
   the exact delivery without inventing an XNA game.
2. Authorize a complete modernized native/WEBGL2 level application. Explicitly select the UI,
   theming, localization, browser/native motion-sensor permission, persistence, lifecycle and
   application-bar equivalents. Preserve all filter, calibration, orientation, physics, surface
   and tube behavior; this is a new scoped product, not a mechanical XNA port.
3. Treat the reusable filter/orientation/physics units as support/reference material only. This
   does not mark the complete application ported and must not create a reduced target named as the
   original sample.

Do not alias `TiltPerspective`, wrap the source in a generic `Game`, remove calibration or
localization, substitute direct sensor-to-sprite movement, or claim native/browser completion from
the existing accelerometer tests.

## Reproduction and evidence

Artifact root:

`/rv/tmp/samples/SAMPLE-128-LevelStarterKit/`

Important retained material:

- `xna4-original/LevelStarterKit/` — exact 35-file source/resource/binary snapshot;
- `scripts/win7-build-original.cmd` — unchanged offline Win7 build, capped at `/m:1`;
- `scripts/audit.py` — exact hash, inventory, project, behavior, localization and live-boundary
  assertions;
- `scripts/qualify.sh` — repeatable focused CNA/Sharp plus audit qualification;
- `evidence/original-sha256.txt` — SHA-256 manifest for every retained file;
- `evidence/win7-msbuild.log`, `win7-result.txt` and `win7-reference-boundary.txt` — exact missing
  Phone-toolchain result and claim boundary;
- `evidence/win7-final-state.txt` — final offline/poweroff VM state;
- `evidence/cna-accelerometer-tests.log` — 89 passed / 2 hardware-skipped CNA tests;
- `evidence/sharp-resources-tests.log` and `sharp-isolated-storage-tests.log` — 8/8 and 63/63
  live Sharp Runtime tests.

Re-run the host qualification with:

```bash
/rv/tmp/samples/SAMPLE-128-LevelStarterKit/scripts/qualify.sh
```

This qualification performs no compilation. All prior build/tool invocations in this audit used
at most one worker, within the session-wide eight-core limit.
