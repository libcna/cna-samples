# SAMPLE-149 — `TombstoningSample` audit and owner decision

## Status

Fresh audit complete enough to require an owner product decision under `SAMPLES-DEC-005`. This is
a complete Windows Phone 7.0 Silverlight page application, not an XNA game. No invented `Game`,
reduced state-dictionary console probe, HTML form or sample-specific lifecycle emulation was added.

Artifact root: `/rv/tmp/samples/SAMPLE-149-TombstoningSample/`.

## Complete delivered product

The entire licensed delivery contains **15 files / 236,033 bytes**:

- three C# units / 675 lines: `App`, `MainPage` and the reusable `StateUtils` helper;
- two XAML documents / 85 lines defining the application lifetime service and complete page;
- the Phone/Silverlight project, solution and two deployment manifests;
- 62×62 and 173×173 shell images, a 480×800 splash screen, readme and license.

There is no `Microsoft.Xna` reference, XNA project type, `Game`, GraphicsDevice, content project,
XNB or XNA runtime behavior anywhere in the physical directory. The project targets Silverlight
v4 with the WindowsPhone profile and `AppPlatformVersion="7.0"`; its references are
`Microsoft.Phone`, `Microsoft.Phone.Interop`, `System.Windows`, `System.Net` and ordinary framework
assemblies. The readme explicitly requires Windows Phone Developer Tools.

This is nevertheless a real runnable product, not an empty scaffold. Its portrait page shows the
application/page-state headings, status and retrieved data, plus a scrollable TextBox, CheckBox,
Slider and six RadioButtons in two groups. It owns the normal Phone frame/navigation setup, splash
transition, system tray, primary tile and application icon.

## Defining lifecycle and persistence behavior

The application demonstrates two different persistence tiers and deliberately separates four
Phone lifetime events:

- `Launching` avoids slow storage and has no previous transient state;
- `Deactivated` copies the current data into `PhoneApplicationService.Current.State` for fast
  reactivation and also saves it to `myDataFile.txt` in application isolated storage;
- `Activated` restores the transient state dictionary without rereading durable storage;
- `Closing` knows no tombstone reactivation will occur and saves only durable storage.

Page navigation separately preserves TextBox text/selection, CheckBox state, Slider value, six
radio selections, scroll offsets and focused control. A reconstruction restores those values only
when the page constructor ran again and the `PreservingPageState` marker survived. Deferred scroll
and focus changes are queued onto the first `CompositionTarget.Rendering` event.

Application data has a second observable state machine. A new page starts a background thread. If
`myDataFile.txt` exists and `DataLastSave` is less than 30 seconds old, it reads isolated storage;
otherwise it begins an asynchronous `HttpWebRequest` to the hard-coded Windows Team Blog RSS URL.
Both routes dispatch their result back to the UI thread and display exactly one of
`data retrieved from isolated storage.`, `data retrieved from web.` or the Web failure message.

Three upstream quirks are part of any exact port unless the owner explicitly authorizes repairs:

- the horizontal scroll-state key is populated from `VerticalOffset` rather than
  `HorizontalOffset`;
- the no-focus branch writes `FocusedControlName`, while restoration reads
  `FocusedElementName`;
- `FileMode.OpenOrCreate` rewrites from position zero without explicitly truncating an older,
  longer data file.

A generic key/value round trip would therefore not reproduce the demonstrated page reconstruction,
render scheduling, navigation instance distinction, Phone lifetime ordering or async data route.

## Authentic Win7 build evidence

The owner-provided Win7 SP1 VM was booted headless with all eight VirtualBox network adapters set to
`none`. The exact immutable solution was shared into the guest and built with Windows .NET 4
MSBuild as `Release|Any CPU`. MSBuild selected the correct project and then stopped at its first
missing platform dependency:

```text
TombstoningSample.csproj(92,3): error MSB4019:
The imported project
"C:\Program Files\MSBuild\Microsoft\Silverlight for Phone\v4.0\
Microsoft.Silverlight.WindowsPhone.Overrides.targets" was not found.
```

The diagnostic log records zero warnings, one error and the correct Phone project configuration.
This proves the live VM has VS2010/XNA but not the separate Phone Silverlight v4 targets required by
the upstream readme; it is not a source-code or solution-selection failure. The VM was cleanly shut
down after the test and remains `poweroff` with every NIC still `none`.

## Live CNA / Sharp Runtime boundary

At live CNA `9ca0d4188` and Sharp Runtime `9cc96cd5`, focused available infrastructure is healthy:

- **63/63** `System.IO.IsolatedStorage` tests pass;
- **201/201** `System.Net.Http` tests pass through local loopback servers;
- **527/527** `System.Threading` tests pass.

Those components do not make this product portable by themselves. The live trees expose no
`PhoneApplicationService`, `PhoneApplicationPage`, `PhoneApplicationFrame`, `Microsoft.Phone` or
`System.Windows` UI surface. They also lack this app's TextBox/selection, CheckBox, RadioButton,
Slider, ScrollViewer, focus manager, XAML resource/style/layout system, Phone navigation/page-state
dictionaries, UI Dispatcher/CompositionTarget render scheduling and `IsolatedStorageSettings`.
Sharp Runtime offers the newer `System.Net.Http` family but not the original `HttpWebRequest` /
`BeginGetResponse` contract.

CNA's XNA `Game` correctly has `Activated` and `Deactivated` events, but wrapping this pure
Silverlight application in a game would discard the defining application/page/navigation contract
and invent rendering/input behavior upstream never had. A faithful implementation is a substantial
Phone/Silverlight compatibility layer; a browser or native rewrite is a product modernization that
needs an explicit UI, navigation, persistence, lifecycle, HTTP/security and visual-fidelity scope.
No bounded CNA or Sharp Runtime defect was identified, so neither repository was changed.

## Evidence and reproducibility

- `xna4-original/` — complete byte-identical upstream snapshot;
- `evidence/source-inventory.tsv`, `original.sha256`, `snapshot-diff.txt` — all-file inventory and
  integrity;
- `scripts/build-win7-release.cmd`, `xna4-build/release-msbuild.log` — exact offline Win7 build;
- `evidence/win7-offline-state.txt` — final VM power/NIC/shared-folder state;
- `scripts/qualify.sh`, `evidence/qualification.log` — snapshot, build-boundary, live-capability and
  791 focused Sharp Runtime tests.

The three delivered shell/splash images were visually inspected. No original/native/browser runtime
comparison is claimed because the authentic Phone SDK/emulator is absent and no replacement product
has been authorized.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port boundary for this complete retired Phone/Silverlight sample;
2. authorize a faithful Windows Phone 7.0 Silverlight compatibility product, including the page/UI,
   navigation, lifecycle/tombstoning, XAML, dispatcher and storage/settings stack plus an authentic
   Phone reference gate; or
3. explicitly define a complete native and WEBGL2 modernization, including the control/layout and
   focus/scroll behavior, background/reactivation model, durable/transient state mapping, replacement
   data endpoint and HTTP/CORS/security policy, and which upstream quirks remain observable.

Until that ruling, a `Game` wrapper, local dictionary demonstration or generic web form would be a
source-fidelity workaround rather than a port of SAMPLE-149.
