# SAMPLE-124 — `CustomIndeterminateProgressBarSample` audit and owner decision

## Status

**🛑 Brief non-XNA classification renewed 2026-10-03; independent owner scope pending.**
Owner: **"123 ponech cancelled analyzuj 124"** cancels SAMPLE-123 and requests this analysis.
Project/version/language were checked and reported first: **C# + XAML, Windows Phone 7 /
Silverlight 4; no XNA version or XNA references.** `TargetFrameworkVersion=v4.0` means
Silverlight, not XNA 4.0. A product-boundary decision remains required under
`SAMPLES-DEC-005`. This is a complete Windows Phone 7 Silverlight control demonstration, not an
XNA `Game`. No fake game, reduced animation or HTML/CSS rewrite was added without an explicit
modernization decision.

## Current brief classification — 2026-10-03

All **15 files / 242,276 bytes** match the complete retained `xna-original` snapshot.
The actual project declares Silverlight/WindowsPhone and references Microsoft.Phone,
Microsoft.Phone.Interop and System.Windows. The readme requires Windows Phone Developer Tools.
The page is a progress-bar demo: its button toggles `IsIndeterminate` and visible/collapsed;
the template animates five cached rectangles on a repeating **4.4-second** storyboard.

Current CNA/Sharp source searches find no Silverlight page/control/storyboard/visual-state UI
types named by this product. CNA now has general Phone shell/notification services, so the old
claim must not be broadened to all Phone support. Those services do not provide this UI stack.
The old Win7 missing-Silverlight-targets build and PushRecipe reuse comparison below remain
**historical**; no VM, SDK availability recheck, new build/runtime gate or extended source audit
was performed for this initial non-XNA scope classification. No sample/dependency code or tests
were changed, no replacement UI was created, and no cleanup was requested.

Recommend owner cancellation of the standalone non-XNA product while retaining all data.
Alternatives are historical support data or an explicitly complete native/browser UI modernization,
with a reference route and visual/compositor-performance contract. This is a separate UI product,
not a routine XNA4 port. SAMPLE-124 stays independently 🛑 until that choice.

Current evidence: `/rv/tmp/samples/SAMPLE-124-CustomIndeterminateProgressBarSample/evidence/current-head-classification-20261003/`
contains the full hash inventory, project classification, readme, exact current UI search command/
result and bounded review. Reproduce with `scripts/current-head-20261003/classify.py`.
Earlier source/scripts/VM logs and detailed audit evidence remain intact. Two initial helper
assertions (HTML whitespace and counting static rectangles with animated ones) were corrected
without source edits; their notes are retained. The sections below preserve the earlier audit.

## Complete product inventory

The upstream delivery contains **15 files / 242,276 bytes**:

- one Visual Studio solution and one Windows Phone Silverlight v4 application project;
- four C# units / 681 lines, including the 455-line `RelativeAnimatingContentControl`;
- two XAML units / 199 lines defining the phone page and full control template;
- Windows Phone deployment/application manifests;
- application icon, tile background and splash image;
- a 2010 usage page and license.

The project is an AnyCPU Silverlight library/XAP application with
`TargetFrameworkProfile=WindowsPhone`, `SilverlightApplication=true` and a `MainPage.xaml`
default task. It references Microsoft.Phone, Microsoft.Phone.Interop and the Silverlight
`System.Windows`/Core/Net/XML assemblies. There are no XNA framework references or XNA content
items.

## Audited behavior

The page is fixed to portrait phone layout. It displays the application/page titles, one custom
indeterminate progress bar and a `Toggle ProgressBar` button. Each click flips both
`IsIndeterminate` and `Visibility`, so the bar alternates between running/visible and
determinate/collapsed states.

The dedicated control works around Silverlight's inability to encode relative translation values
in visual-state storyboards:

- `SizeChanged` records the current width/height, clips to that rectangle and discovers animation
  values in template-root visual-state groups;
- decimal suffix `.1` identifies width percentages and `.2` height percentages, with epsilon
  `0.000009`; adapters strip the marker and recompute `To`, `From` or key-frame values on resize;
- the template has determinate and indeterminate visual states, twelve animation timelines and
  32 key frames;
- five cached 4×4 accent rectangles begin 0.2 seconds apart, travel through 0%, 33%, 66% and 100%
  of the control width with exponential easing, disappear after 2.5 seconds and repeat on a
  4.4-second compositor-thread cycle.

The upstream readme identifies compositor-thread performance as the point of the sample and
explicitly requires Windows Phone Developer Tools.

## Historical authentic reference boundary — 2026-09-01

The owner-supplied Win7 VM was booted headless with all eight network adapters set to `none` and
the unchanged snapshot exposed through one narrowly scoped shared folder. Its .NET 4 MSBuild
accepted the solution and selected the authentic `Debug|Any CPU` project configuration, but
stopped before compilation because this installation lacks:

```text
C:\Program Files\MSBuild\Microsoft\Silverlight for Phone\v4.0\
Microsoft.Silverlight.WindowsPhone.Overrides.targets
```

This is the precise Windows Phone Developer Tools prerequisite named by the readme. The source
contains no XAP/binary, the local XNA documentation mirror contains no replacement runtime
capture, and no Windows Phone emulator is available. Therefore this audit does **not** claim an
original build or animation capture. The VM was shut down normally and verified `poweroff`, with
all NICs still `none`.

## Historical shared-control consumer

SAMPLE-139 `PushRecipe_WP7_SL` embeds this control in its login progress UI. After removing
comments and normalizing only the namespace plus the two renamed helper methods, all 212 logical
C# lines are sequence-identical. Its `PerformanceProgressBar` template is also semantically
identical to this sample's `CustomIndeterminateProgressBar` style after normalizing the style key
and XML namespace.

That reuse is valuable support evidence, but it does not make the two products duplicates:
SAMPLE-124 is the isolated interactive control demonstration; PushRecipe is a separate push
client/server product that consumes the control and remains scheduled for its own audit.

## Historical CNA and browser boundary

At live CNA HEAD `7712534d3d22` and Sharp Runtime HEAD `9cc96cd57cde`, a source scan finds no
`System.Windows`, PhoneApplicationPage, ContentControl, VisualStateManager, Storyboard,
DependencyProperty or double-animation/key-frame implementation. These are Silverlight/WP UI
framework concepts, not missing XNA 4 APIs that belong in CNA.

A truthful native/browser product would require an explicit UI modernization contract covering
the page, button, visual states, resize-relative timeline, easing, 4.4-second cadence and the
defining compositor/render-thread performance behavior. Porting only the percentage arithmetic or
drawing five dots in an invented XNA `Game` would not be the delivered product.

No CNA or Sharp Runtime source change was made.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-124-CustomIndeterminateProgressBarSample/`.

- `xna-original/CustomIndeterminateProgressBarSample/` is the complete byte-for-byte snapshot;
- `evidence/file-inventory.txt`, `sha256sum.txt`, `image-metadata.tsv`, `readme-text.txt` and the
  empty `snapshot-diff.txt` cover every input;
- `win7-export/msbuild-debug.log` retains the authentic solution selection and exact missing-SDK
  failure; `evidence/win7-reference-boundary.txt` records the offline/headless VM boundary;
- `evidence/consumer-reference-scan.txt` and `pushrecipe-reuse-comparison.txt` retain the measured
  SAMPLE-139 relationship;
- `evidence/live-runtime-surface-search.txt` is empty, recording the searched CNA/Sharp surface;
- `scripts/compare_pushrecipe.py` and `audit.sh` reproduce the offline evidence.

No CNA build, native runtime or WEBGL2 gate applies to an absent port. Creating one before the
product ruling would test newly authored behavior.

## Owner decision required

Choose one:

1. accept an evidence-backed Windows Phone Silverlight/non-XNA non-port boundary;
2. authorize a complete modern native/browser UI control demonstration, explicitly defining the
   acceptable replacement UI/runtime and compositor-performance fidelity contract; or
3. retain the source as shared historical support data for SAMPLE-139 without a standalone CNA
   product.

If an authentic original reference is required before deciding, provide/install the offline
Windows Phone Developer Tools/SDK and a usable emulator path. Do not treat the missing SDK as a
CNA defect or silently reduce the sample to animation math.
