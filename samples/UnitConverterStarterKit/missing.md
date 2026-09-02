# SAMPLE-150 — `UnitConverterStarterKit` audit and owner decision

## Status

Fresh audit complete enough to require an owner product decision under `SAMPLES-DEC-005`. The old
"empty/incomplete" classification was false: this is a complete, documented and localized Windows
Phone 7 Silverlight application. It is not an XNA game. No invented `Game`, calculator-only command
line program, reduced conversion widget or generic web form was added.

Artifact root: `/rv/tmp/samples/SAMPLE-150-UnitConverterStarterKit/`.

## Complete delivered product

The licensed delivery contains **60 files / 609,506 bytes**. Its application consists of:

- 28 C# units / 6,581 lines, including model, view model, page code, a reusable touch context menu,
  Phone tilt effect, localization helpers and XML/isolated-storage helpers;
- seven XAML documents / 937 lines, including the two pages, keypad templates, pivot UI, application
  resources, context-menu templates and the two-list-box user control;
- six localized RESX resources / 1,926 lines for US and British English, French, Italian, German and
  Spanish, plus matching Phone MUI title resources;
- `SupportedUnits.xml`, with eight categories and 44 units;
- a complete 2010 starter-kit HTML manual, project, solution, Phone manifests, icon/tile images and
  license.

The readme and manual explicitly call this a complete Windows Phone 7 application and require the
Windows Phone Developer Tools. There is no `Microsoft.Xna` reference, XNA project type, `Game`,
GraphicsDevice, content project or XNB anywhere in the 60-file delivery.

## Defining application behavior

The main portrait page is a purpose-built touch calculator rather than a generic text form. It has
Phone-themed custom keypad templates, culture-specific decimal input, sign/backspace/clear controls,
source and target values, a swap action, a localized summary, and toolbar actions for conversions and
favorites. Input is limited and reformatted to eleven visible digits; long results progressively use
scientific notation and fewer decimals.

The conversion page dynamically creates a Phone `PivotItem` for every category and puts two coordinated
unit lists into each one. A separate favorites pivot supports press-and-hold deletion through the
sample's 1,202-line touch context-menu control. Done/cancel and the hardware Back key have distinct
state transitions. The exact conversion path preserves each unit's multiplier, offset and
`FormulaInvert` behavior, including temperature conversion.

The delivered conversion catalogue contains length, temperature, speed, time, volume, angle, weight
and area. Names, button labels, number parsing/formatting, category lookup and saved-favorite display
are localized. The application selects theme-specific swap imagery and applies the Phone tilt effect
to interactive controls.

Startup is deliberately deferred until after the first page layout. A background worker then loads
the conversion XML and favorites, after which the two toolbar actions become enabled. Main-page and
category-page view state travel through `PhoneApplicationService.Current.State` across deactivation
and activation. Favorites independently persist as `WmUnitConverterFavorites.xml` in isolated
storage and are relocalized after deserialization.

Two XML serializer routes are therefore reachable product behavior:

- `CategoryInformation[]` deserializes the packaged `SupportedUnits.xml` catalogue;
- `FavoriteCollection` serializes and deserializes the user's durable favorites.

The owner reserved `SAMPLES-DEC-008` and the separate Sharp Runtime XML work for an explicit later
assessment. This audit did not modify or independently approve that work. Even a fully qualified XML
serializer would not supply this application's missing Phone/Silverlight UI and lifecycle product.

## Authentic Win7 build evidence

The owner-provided Win7 SP1 VM was booted headless with every VirtualBox network adapter set to
`none`. The exact immutable solution was shared into the guest and built with Windows .NET 4 MSBuild
as `Release|Any CPU`. MSBuild selected the correct application project and stopped at the first
missing platform dependency:

```text
UnitConverter.csproj(176,3): error MSB4019:
The imported project
"C:\Program Files\MSBuild\Microsoft\Silverlight for Phone\v4.0\
Microsoft.Silverlight.WindowsPhone.Overrides.targets" was not found.
```

The diagnostic log records zero warnings, one error, the correct configuration and a 0.15-second
build. This is the same independently measured environment boundary as SAMPLE-149: the VM has
VS2010 and XNA but not the separate Silverlight-for-Phone v4 targets named by the upstream readme.
It is not evidence that the delivered source is incomplete. The VM was cleanly shut down and is
`poweroff`; all eight NICs remain `none`.

## Live CNA / Sharp Runtime boundary

At CNA `9ca0d4188` and Sharp Runtime `9cc96cd5`, the available reusable foundations tested by this
audit are healthy:

- **63/63** isolated-storage tests pass;
- **706/706** globalization tests pass;
- **527/527** threading tests pass.

The live frameworks still expose no `PhoneApplicationService`, `PhoneApplicationPage`,
`PhoneApplicationFrame`, `Microsoft.Phone.Controls` pivot, `System.Windows` UI/XAML stack, Phone
navigation, application bar, touch manipulation/tap-and-hold route, dependency-property/template
engine, layout events, hardware Back-page semantics, Dispatcher-bound UI or Phone tilt transform.

CNA's `Game` lifecycle is not a substitute: wrapping the application in a game would discard the
MVVM pages, Phone control tree, navigation, application lifecycle, accessibility/input behavior and
visual contract. A native/browser version would be a substantial product modernization requiring an
explicit control/layout, navigation, persistence, localization, lifecycle, touch and visual-fidelity
scope. No bounded CNA or Sharp Runtime defect was identified, so neither repository was changed.

## Evidence and reproducibility

- `xna-original/` — complete byte-identical upstream snapshot;
- `evidence/source-inventory.tsv`, `original.sha256`, `snapshot-diff.txt` — all-file inventory and
  integrity;
- `scripts/build-win7-release.cmd`, `win7-build/release-msbuild.log` — exact offline Win7 build;
- `evidence/win7-offline-state.txt` — final VM power, NIC and shared-folder state;
- `scripts/qualify.sh`, `evidence/qualification.log` — snapshot, product-shape, build-boundary,
  live-capability and 1,296 focused Sharp Runtime tests.

All four delivered PNGs were visually inspected: matching application/tile arrows and dark/light
swap-control variants. No original/native/browser runtime comparison is claimed because the authentic
Phone SDK/emulator is absent and no replacement product has been authorized.

## Owner decision required

Choose one:

1. accept an evidence-backed historical non-port boundary for this complete retired
   Phone/Silverlight starter kit;
2. authorize a faithful Windows Phone 7 Silverlight compatibility product, including its XAML,
   controls/templates, pivot/navigation, touch/context-menu, lifecycle/tombstoning, localization and
   storage stack plus an authentic Phone reference gate; or
3. explicitly define a complete native and WEBGL2 modernization, including the two-page UI and
   navigation, custom keypad, touch interaction, all eight categories/44 units, favorites,
   localization, persistence/lifecycle mapping and visual/accessibility gates.

Any implementation choice must also consume the eventual owner-approved `SAMPLES-DEC-008` XML
contract rather than adding a sample-specific parser. Until those rulings, a calculator core, `Game`
wrapper or generic HTML form would be a source-fidelity workaround rather than a port of SAMPLE-150.
