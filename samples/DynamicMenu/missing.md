# SAMPLE-077 — DynamicMenu_4_0 audit

## Result

The sample is completely re-ported and qualified. No known XNA behavior is missing, no old
sample-local workaround remains, and no owner decision is required.

The earlier port was not a faithful endpoint: it merged the library into one header, hand-built
the two XML-authored pages, used loose PNG/font substitutes, added mouse/keyboard behavior, omitted
fullscreen/orientation wiring, changed the random-number lifetime and retained no authentic menu
XNBs. Those substitutions are all removed.

## Source and behavior coverage

The audit used the complete 15-file C# source tree under:

`/rv/tmp/samples/SAMPLE-077-DynamicMenu_4_0/xna4-original/`

The C++ port restores the original two-product structure at source level: the reusable
`DynamicMenu.Controls`/`DynamicMenu.Transitions` library and the `DynamicMenuSample` game. It keeps
the original interfaces, inheritance, namespaces, per-class files, properties, event topology and
the two assembly metadata units. In particular it retains:

- the `IControl` and `ITextControl` interfaces instead of merging them into base classes;
- the complete `Control`, `TextControl`, `Button`, `Container`, `Image`, `Label`,
  `MultilineTextControl`, `ProgressBar`, `PhoneScreen` and `Transition` implementations;
- deferred container add/remove behavior and reference-semantics transition lists;
- Page 1 hue, index, bounce and grow/shrink actions;
- XNB-loaded Page 2 and Page 3 object graphs, including the page-three progress action;
- 30 Hz timing, the 480x800 fullscreen request, all three supported orientations and the
  `OrientationChanged` handler;
- `TouchPanel` Tap gestures as the only menu pointer input, and GamePad Back as the only explicit
  exit input;
- a fresh `System::Random` for every hue tap, matching the original lifecycle.

There is no mouse-to-tap bridge, keyboard orientation toggle, Escape exit, F1 overlay, environment
hook, loose-content route or hand-written replacement menu. The only C++-specific AOT registration
and assembly-topology mechanics are recorded in [diff.md](diff.md).

## Authentic content

The unchanged official XNA Game Studio 4.0 content project was built twice: Windows Phone/Reach for
the checked-in runtime assets and Windows/HiDef for the desktop reference executable. All eleven
checked-in XNBs are byte-identical to the Phone/Reach output:

| Asset | SHA-256 |
|---|---|
| `Fonts/ControlFont.xnb` | `ca1ec7cadf5520672b38950ae8dc390333237f851cbc6720d891288edff1ebe0` |
| `Menus/MenuPage2.xnb` | `f20b48071c0ed052c590d9df395fac1b25102aba65f019e949cc62c28e1ea287` |
| `Menus/MenuPage3.xnb` | `252af368e273f9fab413b9e36f645112f65178e01a90c39e6ce5037243387524` |
| `Textures/UFO.xnb` | `151d18020c925061ff2b098503926fa2ac659bc365579335139d54a9695a4601` |
| `Textures/button.xnb` | `6f42a5f77c4c64ddb79cfb309158201131d1454b4dab046832468a52f19cf934` |
| `Textures/buttondisabled.xnb` | `395bb46c1367a9359a715ee1f34d2b0631aae62112a92351d95af3ca1707af06` |
| `Textures/buttonpressed.xnb` | `e244d8dedbac924f8fff5153ceabc8918757fa626db40a749f2439f445c759a3` |
| `Textures/checkerboard.xnb` | `eb3f4ccf6a88e9608c1f5dcd53080f4ea3c718191ae5c3a4bea2c7923a4a1e63` |
| `Textures/progressleft.xnb` | `081b0f381e5c3f797fdd0d5e959d3100220883c2ef86568baa78c62793941edb` |
| `Textures/progressright.xnb` | `f9fa94ba6e8f09592452f30f7adfe16532f678c2f6fd3c46630c3666b0f82639` |
| `Textures/textbox.xnb` | `9f3bfa0c867df78ebb282bb1e41d158a0c32223354d243e14de9528250757439` |

The loose font JSON/atlas and converted PNG runtime substitutes were deleted. The original
documentation image is at the sample root, and `DynamicMenu.htm` is byte-identical to the upstream
`DynamicMenuSample.htm` (`68ef4fce32608d83262b443535a5d8829d930829e49e9ae85f6106b1be23ac3e`).

## CNA content fix

The authentic menu XNB reader table exposed a general CNA limitation. XNA reflective content first
populates serialized base-class members into the derived object, and its reader table may name an
abstract class or interface even though actual object dispatch selects concrete derived types.
CNA's AOT reflective builder could describe neither condition, so a sample-local parser or
hard-coded graph would have been the wrong fix.

`cnanext` commit `96b56b0e4` adds:

- `ReflectiveTypeReaderBuilder::Base(...)`, which composes base members before derived members;
- a resolving-only `AbstractReflectiveTypeReader` and `RegisterAbstract()` route;
- regressions proving inherited fields are read into the same derived object and an abstract table
  entry resolves but still rejects invalid direct dispatch.

The focused qualification runs all ten tests in `ReflectiveTypeReaderTest`,
`ReflectiveInheritanceTest` and `ReflectiveSharedTypeReaderTest`; all ten pass. More importantly,
the standard sample build now loads both authentic polymorphic menu fixtures through ordinary
`Content.Load` behavior and renders them in a real browser.

## Original reference

The official Phone/Reach and Windows/HiDef pipeline builds both completed. The unchanged original
library and game source were compiled into a Windows XNA executable. A separate diagnostic
`GameComponent` was added only to the diagnostic executable; it uses reflection to select the
original private Page 2/Page 3 methods, advances the real Page 3 button four times and exits. The
original sample source itself was not edited.

The reference process exited 0 and captured Page 1, Page 2, Page 3 and Page 3 at 40 percent under:

`/rv/tmp/samples/SAMPLE-077-DynamicMenu_4_0/evidence/original-windows-hidef-diagnostic/`

## CNA qualification

### Native OPENGLES3

Debug and Release targets both build with no sample-specific flags. Both run against a real Mesa
OpenGL ES 3.2 context, report renderer `OPENGLES3`, create the expected `DynamicMenuSample`
480x800 window and exit 0 through the normal window close route. The Xvfb-only fullscreen mode
switch times out and SDL restores the same 480x800 window; this is visible in the logs and does not
change sample behavior.

Evidence:

- `evidence/cna-native-opengles3-qualified/`
- `evidence/cna-native-opengles3-release-qualified/`

### Real Chrome WEBGL2

The complete Debug WEBGL2 bundle runs in the system Chrome with a real WebGL 2 context. CDP sends
actual touch start/end events; it opens Page 2 and Page 3, advances the progress bar four times,
returns to Page 1, increments the index to 2 and then completes 600 additional animation frames.
Both dynamic menu XNBs are present in the content-load log. The final result records:

- WebGL 2 / OpenGL ES 3.0;
- 600 of 600 requested animation frames;
- distinct captures for all three pages and the progress/index states;
- no runtime exception, unhandled rejection, relevant HTTP error or fatal console message.

Evidence: `evidence/cna-web-webgl2-qualified/result.json` and its six captures.

### Visual comparison

The browser captures were compared directly with the matching authentic XNA diagnostic captures.
Normalized RGB RMSE is `0.0003411` for Page 1, `0.0004964` for Page 2 and `0.0003844` for Page 3
and its 40-percent progress state. Only 1.00–2.17 percent of pixels differ at all, from rasterization
rounding around otherwise matching content. Machine-readable results are in
`evidence/visual-comparison.json`.

## Reproduction artifact

The complete retained artifact is:

`/rv/tmp/samples/SAMPLE-077-DynamicMenu_4_0/`

It contains the full original snapshot and manifest, authentic pipeline outputs, original/native/
browser capture scripts, Debug/Release native build trees, the complete WEBGL2 bundle, logs,
screenshots and checksums. No artifact pruning has been authorized.
