# SAMPLE-135 — `NonLinear-WP-SLApp-Navigation-Service` audit and owner decision

## Status

Fresh audit complete enough to require an owner product/platform decision under
`SAMPLES-DEC-005`. This delivery is a documented Windows Phone 7 Silverlight navigation recipe,
not an XNA game or content sample. It contains two independently runnable phone applications and
one reusable library. No fake `Game`, generic router, reduced back-stack algorithm, desktop widget
substitution or HTML-only rewrite was added.

A faithful port requires either a complete Phone/Silverlight compatibility product or an explicit
native/browser modernization contract. Only the owner may instead accept the exact retained
delivery as an evidence-backed historical non-port.

## Complete inventory and product topology

The exact upstream directory contains 40 files / 162,487 bytes. Its 12 C# units contain 1,263
lines, eight XAML units contain 357 lines, three projects contain 310 lines and three solutions
contain 70 lines. Every file and SHA-256 digest matches the retained snapshot.

The package has three related build surfaces:

- `NavigationExample.sln` is a standalone two-page baseline application. It intentionally shows
  ordinary Phone navigation creating repeated page instances and a growing back stack.
- `NonLinearNavigationService.sln` builds the 362-line reusable
  `WindowsPhoneRecipes.NonLinearNavigationService` library by itself.
- `WindowsPhoneNonLinearNavigationServiceSample.sln` builds that library and a separate four-page
  application which demonstrates the corrected non-linear flow.

Both applications are AnyCPU Silverlight libraries/XAPs targeting `Silverlight` /
`WindowsPhone` v4.0 and Phone OS 7.0. They produce `NavigationExample.xap` and
`WindowsPhoneRecipes.xap`; their manifest entry pages are respectively `Page1.xaml` and
`MainPage.xaml`. The service is a non-application Phone Silverlight library. All three projects
import both `Microsoft.Silverlight.WindowsPhone.Overrides.targets` and
`Microsoft.Silverlight.CSharp.targets`, and reference `Microsoft.Phone` plus `System.Windows`.

There is no `Microsoft.Xna.*` reference, XNA project property, content project or content-pipeline
item anywhere in the 40-file delivery. If porting is authorized, the two applications remain
separate products under this same SAMPLE row; merging the baseline into the service demonstration
would erase the problem/solution comparison that the recipe teaches.

## Baseline application behavior

`NavigationExample` has `Page1` and `Page2`, a visible application bar/log view and these routes:

- “Go To Main Page (again)” requests the identical `/Page1.xaml` URI. The documentation records
  this as unsupported fragmented navigation and an exception path on the original Phone runtime.
- “Go To Main Page (new)” adds a `PageCount` query parameter, creating another distinct Page1
  instance and displaying the count obtained from `NavigationContext.QueryString`.
- Page1 can navigate to Page2, and Page2 can navigate to Page1. Repeating this loop grows the real
  Phone page back stack; the hardware Back button then visits every retained instance in reverse.

This diagnostic is not interchangeable with the four-page application. It is the observable
before-state against which the service is explained.

## Reusable service contract

`NonLinearNavigationService` is a singleton initialized with the application
`PhoneApplicationFrame`. It waits for the first frame navigation and then subscribes to all five
Phone navigation events: `Navigated`, `Navigating`, `FragmentNavigation`, `NavigationFailed` and
`NavigationStopped`.

Its `_History` is a `List<Uri>` mirroring the page back stack and is retained in
`PhoneApplicationService.Current.State` across Phone tombstoning. For a `NavigationMode.New`
request to an already visited URI other than the current page, it:

1. records that URI as the loop start;
2. sets the public `IsRecursiveBackNavigation` flag;
3. saves the root-frame opacity, sets the frame to transparent and cancels the new navigation;
4. starts `GoBack()` after the resulting stopped-navigation event;
5. removes each departed URI and keeps going back after every successful navigation;
6. stops at the recorded URI, clears the flag/target and restores the exact previous opacity.

The service excludes `app://external/` from its history. `_LastCanceleddUri` distinguishes the
service's initial cancellation from a page/user cancellation during the recursive sequence, so a
second cancellation ends rather than loops forever.

The supplied document also states three intentional limitations: the service cannot permit an
intentional navigation loop; each page must hide its application bar manually to prevent flicker;
and a Phone Back-button/cancel-navigation bug requires every participating page to cancel the
hardware event and call `NavigationService.GoBack()` itself.

## Four-page demonstration

The sample application initializes the service only after constructing its `RootFrame`. Its
navigation graph is complete rather than a single algorithm probe:

- Main Menu → Categories;
- Categories → one of three `CategoryPage.xaml?cat=…` routes or back to Main;
- Category → either `ItemPage.xaml?cat=…&item=…`, Categories or Main;
- Item → the other item, its Category, Categories or Main.

All four pages implement the hardware Back workaround. Categories, Category and Item hide their
application bars when leaving; on recursive return they inspect `IsRecursiveBackNavigation` and
skip simulated expensive page work before making the bar visible again. Category and Item also
preserve query-bound data context. Replacing these pages with a headless URI list would omit the
page lifecycle, back-key, application-bar, opacity, query binding and tombstoning behavior that the
recipe exists to demonstrate.

The application's Phone 7 manifest requests nine capabilities even though the sample logic does
not directly consume all of them. They are retained as part of the exact delivery rather than
silently normalized.

## Documentation verification

The authoritative 42,251-byte `NonLinear WP SL App Navigation Service.docx` was rendered from the
exact snapshot to an 11-page Letter PDF and 11 PNGs at 1,224×1,584. Every page was inspected at
original detail. All text, code, diagrams and back-stack tables are readable with no clipping or
overlap; the large white area after the short page-11 summary is intentional.

The packaged document-rendering helper could not start because this environment lacks its optional
`pdf2image` Python module. The documented isolated LibreOffice headless fallback plus `pdftoppm`
was used instead; no network package install and no document edit occurred.

## Authentic build and missing delivery boundary

The unchanged snapshot was submitted to .NET 4 MSBuild in the local offline XNA 4.0 Wine prefix,
using `Rebuild`, `Debug|Any CPU`, `/m:1` and diagnostic file logs.

- `NavigationExample.sln` selects its correct project, then stops at its first Phone import with
  0 warnings / 1 MSB4019 error.
- `WindowsPhoneNonLinearNavigationServiceSample.sln` selects both correct projects; each stops at
  the same import, for 0 warnings / 2 MSB4019 errors.

The missing prerequisite is exactly:

```text
C:\Program Files\MSBuild\Microsoft\Silverlight for Phone\v4.0\
Microsoft.Silverlight.WindowsPhone.Overrides.targets
```

This environment therefore cannot produce an unchanged XAP or Phone emulator capture. Separately,
both application projects reference `../../3rd Party/Logger/Debug/WindowsPhoneLogger.dll`, but
neither that DLL nor logger source exists anywhere in `/rv/tmp/XNAGameStudio`. No substitute logger,
project edit or removed reference was used to manufacture a successful build.

## Live CNA / Sharp Runtime boundary

A source scan at live CNA HEAD `5347b52eae13` and Sharp Runtime HEAD `9cc96cd57cde` finds no
`System::Windows`, `PhoneApplicationFrame`, `PhoneApplicationPage`, `PhoneApplicationService`,
`NavigationService`, fragment-navigation event, application-bar, `RootVisual` or equivalent Phone
page-stack/XAP surface. This is not a missing XNA 4.0 method that can be added narrowly to CNA.

Implementing the product faithfully means defining page/frame ownership, URI/query navigation,
back-stack identity, cancellation and stopped/failed/fragment events, hardware Back dispatch,
application/page lifecycle and tombstoning state, opacity/composition, application bars, XAML/UI
layout and browser/native equivalents. Implementing only `List<Uri>` recursion would not be a port.
No CNA or Sharp Runtime source was changed for this decision-sized subsystem.

## Owner choice required

Choose one product boundary:

1. Accept an evidence-backed historical Windows Phone Silverlight non-port. Retain both applications,
   the library, documentation and exact build limitations as support/reference data.
2. Authorize a faithful retired-platform compatibility product. Supply/install the offline Phone 7
   Developer Tools, missing logger dependency and an executable Phone/emulator reference route,
   then scope the complete Silverlight/Phone UI, navigation and lifecycle stack. Preserve both
   standalone applications and the reusable library.
3. Authorize an explicit modern native/WEBGL2 product. Select the UI, browser-history versus private
   page-stack, Back control, lifecycle/persistence, application-bar and composition contracts.
   Preserve the baseline growing-stack diagnostic, separate corrected application, complete route
   graph, recursive event order, cancellation behavior, opacity restoration and tombstoning intent.

Do not call a generic browser router, command-line URI stack, one-screen XNA `Game` or service-only
unit test the original sample. Those may test an algorithm, but they omit the delivered products.

## Reproduction and evidence

Artifact root:

`/rv/tmp/samples/SAMPLE-135-NonLinear-WP-SLApp-Navigation-Service/`

Important retained material:

- `xna4-original/` — exact 40-file snapshot;
- `evidence/original-sha256.txt`, `file-inventory.tsv` and empty `snapshot-diff.txt` — complete
  identity evidence;
- `evidence/navigation-example-msbuild.log` and `nonlinear-sample-msbuild.log` — exact unchanged
  MSBuild selections and missing-target failures;
- `evidence/reference-boundary.txt` — toolchain and missing-logger claim boundary;
- `evidence/docx-render/`, `docx-text.txt` and `docx-visual-review.txt` — complete document evidence;
- `evidence/live-runtime-surface-search.txt` — empty result for the audited live surface names;
- `scripts/audit.py` — source/project/behavior/document/build/runtime assertions;
- `scripts/qualify.sh` — repeatable offline qualification.

Re-run with:

```bash
/rv/tmp/samples/SAMPLE-135-NonLinear-WP-SLApp-Navigation-Service/scripts/qualify.sh
```

The qualification performs no compilation. Both original build probes used one worker, within the
session-wide eight-core ceiling. Native OPENGLES3 and WEBGL2 gates do not apply until the owner
authorizes an actual product rather than a sample-local substitute.
