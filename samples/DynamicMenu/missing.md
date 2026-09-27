# SAMPLE-077 — DynamicMenu_4_0 audit

## Owner-requested mouse input correction — 2026-09-27

**Status: `✅` with the owner-approved input addition recorded in `diff.md`.**
The first current-head requalification removed `TouchPanel::setMouseTouchEmulationEnabledEXT(true)`
because it lacked a recorded SAMPLE-077 owner request. That made mouse clicks inert in the
OPENGLES3 and WEBGL2 desktop ports. The owner then explicitly requested mouse-as-touch input.
The current game constructor opts into CNA's existing general mouse-to-touch bridge, which is
off by default in the framework. No CNA or Sharp Runtime code was changed, and the original
Tap-driven menu logic is unchanged. This is an intentional input addition, not a workaround for
missing XML/content or rendering support.

The rebuilt native OPENGLES3 product passed both mouse navigation and externally injected
genuine SDL finger events through Pages 1–3, four progress advances and the index action,
then exited 0 in each run. Its five captured states are pixel-identical across the current
input modes and to the earlier finger qualification. The rebuilt
nonthreaded WEBGL2 product passed the same path in real Chrome both with mouse events and with
genuine touch events, plus 600 additional frames per run; its five state captures match exactly
between input modes, with no runtime exceptions, rejected promises or relevant HTTP errors.
The exact gallery copy passed the mouse route and 600-frame Chrome gate again. Evidence is under
`/rv/tmp/samples/SAMPLE-077-DynamicMenu_4_0/evidence/owner-mouse-20260927/`.

The unchanged XNA Windows/HiDef diagnostic still builds, but Wine fails before drawing in
`WindowsGameWindow.ScreenFromDeviceName` with `System.ArgumentException: The device name is not
valid`. The earlier successful unchanged-source XNA captures remain the visual reference.
The prior claim that `/dev/vboxdrv` was absent came from checking the sandbox's restricted
`/dev`, not the host. On the host, `/dev/vboxdrv` and `/dev/kvm` exist, both VirtualBox and KVM
modules are loaded, and no Android emulator or VM process was found. A real host attempt to
start the registered Win7 VM returned `VERR_SVM_IN_USE`: AMD-V is held by KVM. After confirming
no other KVM user needs it, root can run `modprobe -r kvm_amd kvm`; the Windows VM has not yet
been run in this correction. This host issue does not change the port's status.

## Earlier current-head requalification — 2026-09-27

**Status: `✅` against CNA `next 7301f386a` and Sharp Runtime
`next d86adb65`.** The 44 physical original files still match the retained
snapshot exactly. The unchanged 15-source Phone/Reach project and its official
content pipeline rebuilt, as did the Windows/HiDef diagnostic product. All 11
fresh Phone/Reach XNBs match the port and current native content byte for
byte; each occurs exactly once in the new web `.data` file. Both XML-authored
menu graphs still load through ordinary `Content.Load` with the original
polymorphic structure and closed AOT reader registration.

The 2026-09-08 desktop mouse-as-touch opt-in had no recorded SAMPLE-077 owner
request, so it was removed. The sample again reads only the original
`TouchPanel` Tap gestures and GamePad Back. An external SDL3 finger-event
test harness drove the native game without linking into it or CNA. A mouse
click on Page 2 left Page 1 **byte-identical**; eight actual finger taps then
opened Pages 2 and 3, advanced the progress bar four times, returned to Page
1 and incremented its index. Current Release OPENGLES3 reported a real Mesa
ES 3.2 context and exited 0 on window close. Against the retained successful
XNA diagnostic captures, all four matched states have normalized RGB RMSE
`0.0000993915`; at most one channel level out of 255 differs, on 0.099% of
pixels. This is the same fixed rasterization difference measured historically.

The new nonthreaded Release WEBGL2 WASM is **8,131,855 bytes**, has no DWARF
sections or pthread code and ran in real system Chrome over ordinary HTTP with
`crossOriginIsolated=false`. Real touch events exercised the same pages and
progress/index actions; WebGL 2 rendered all states, 600 further animation
frames completed, and there were zero runtime exceptions, rejected promises,
relevant HTTP errors or fatal console messages. Browser RMSE against the four
retained XNA states is `0.00034113`, `0.00049645`, `0.00038442` and
`0.00038442`. The exact four-file bundle and an actual Page 3/40% gameplay
image are in the local gallery commit `2be5663`. Its copied bundle passed an
independent Chrome touch/600-frame gate, and the card, detail and neighbouring
navigation were rendered and checked. Gallery has 72 cards in six full pages.

The original Microsoft Permissive License and the Phone package assets
`Background.png`, `Game.ico`, `GameThumbnail.png` and `SplashScreenImage.jpg`
were restored byte for byte outside runtime `Content/`. The native and web
build/capture scripts now use the active repositories and shared ccache;
`MANIFEST.md` lists the products and restoration commands. No CNA or Sharp
Runtime source change was needed. No new artifact prune was applied.

**Original reference limitation.** The unchanged original and official
content build completed again. A fresh Wine/Xvfb execution of its Windows
diagnostic executable fails before drawing with
`System.ArgumentException: The device name is not valid` in XNA's
`ScreenFromDeviceName`; a virtual-desktop attempt also failed, and the
Windows 7 VirtualBox fallback was initially judged unavailable from the restricted sandbox's
`/dev`; that diagnosis was corrected in the owner-requested mouse input correction above.
The successful earlier run of the unchanged
original source remains the four-frame XNA reference. The fresh CNA native
and Chrome captures reproduce those retained reference images to the pixel
differences above. This host failure is not an observed port difference or
a reason to change the sample's fullscreen request. Exact commands, results
and checksums are under
`/rv/tmp/samples/SAMPLE-077-DynamicMenu_4_0/evidence/requal-20260927/qualification.json`;
the root's `MANIFEST.md` gives the active reproduction commands.

## Earlier current-head analysis — 2026-09-27

**Status: `🔎`; the historical completion below is not current-head qualification.**
The physical 44-file Windows Phone sample is byte-identical to the retained
`xna4-original/` snapshot. Its 15 C# units comprise the game and separate
controls/transitions library. All 11 checked-in Phone/Reach XNBs still match
the retained official pipeline output and the retained native content byte
for byte, including the two XML-authored menu graphs. The port still calls
ordinary `Content.Load` with closed AOT reader registration. The historical
unchanged-original diagnostic, OPENGLES3 and real-Chrome WEBGL2 runs cover
Pages 1–3, progress and index; their captures and pixel comparisons remain
valid historical evidence, but no product was rebuilt or run in this pass.

The port currently opts into CNA's mouse-to-touch extension at
`src/DynamicMenuSample.cpp:55`. The original game accepts `TouchPanel` Tap
gestures and GamePad Back; it has no mouse input. The extension feeds the
existing gesture path and is off by default in CNA, but this is an additional
desktop input behavior, not one of the two lossless C++ mechanics in
[`diff.md`](diff.md). That file records the mechanism but no explicit owner
request for SAMPLE-077. The older statement below that there is no
mouse-to-tap bridge predates its addition on 2026-09-08. Under `rules.md`, do
not treat the addition as approved merely because SAMPLE-071 used the same
mechanism. A faithful zero-addition route is to remove this opt-in and qualify
native menu interaction by injecting actual touch events from a test harness;
alternatively the owner can explicitly request desktop mouse control and
that decision must be recorded in `diff.md` and `plan.md`.

The retained Release native binary has a `RUNPATH` into the retired
`openeggbert/cnanext` checkout. The retained WEBGL2 product is a 109,276,797
byte Debug WASM with DWARF sections and pthread machinery; its historical
Chrome gate used `crossOriginIsolated=true`. Neither the original nor port
uses application threads. The retained build scripts and `MANIFEST.md` point
to the retired `openeggbert/cna-samples` source checkout and old cache. There
is no Dynamic Menu gallery entry. The port also omits the original Microsoft
Permissive License and the Phone project's `Background.png`, `Game.ico`,
`GameThumbnail.png` and `SplashScreenImage.jpg` package assets; the originals
remain in the exact snapshot.

Next, resolve the mouse input addition according to the zero-workaround rule,
restore the original license and package assets, refresh the retained build
scripts, then rebuild and compare the unchanged original and current Release
OPENGLES3 product. Qualify every menu page and state through touch input.
Build a lean nonthreaded Release WEBGL2 bundle and test it in real Chrome on
ordinary HTTP with touch input and 600 further frames. Publish the exact
tested bundle and a genuine gameplay capture to the gallery. Current heads:
samples `develop 647c931`, CNA `next 7301f386a`, Sharp Runtime
`next d86adb65`, gallery `main 28f11f7`. Static evidence:
`/rv/tmp/samples/SAMPLE-077-DynamicMenu_4_0/evidence/current-head-analysis-20260927/inventory.json`.
No source, CNA, Sharp Runtime, gallery, build, run or prune change was made
for SAMPLE-077 in this analysis.

## Historical result — 2026-09-08

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

---

## Re-audited 2026-09-08

The port and this document both hold up. Verified again: all 15 original C# units have
counterparts, no original method is without one, and all 11 checked-in XNBs are byte-identical to
the official pipeline output. The browser RMSE figures above were re-measured and match to the
digit -- `0.000341` / `0.000496` / `0.000384` / `0.000384` -- and this document is right to
attribute them to the browser captures specifically.

Two things did need correcting, neither of them in the port.

**`plan.md` credited the native builds with the browser's page coverage.** Its row said "Debug/
Release OPENGLES3 and real-Chrome WEBGL2 cover all three pages"; only WEBGL2 does. The native
section above never claimed otherwise -- it claims a build, a real GLES 3.2 context, the right
window and a clean exit, and points at `-qualified/` directories that hold `01-page1.png` and
nothing else. The row is corrected.

**`evidence/cna-native-opengles3/` holds three misleading files.** `02-page2.png`,
`03-page3.png` and `04-page3-progress40.png` there are **byte-identical to `01-page1.png`** --
RMSE `0` against it and against each other. They are page 1 under three other names, they are not
cited by this document, and compared against the original's real pages they score 0.26–0.34, which
reads as a rendering defect and is nothing of the kind. The directory is not one of the two this
document cites as evidence.

**Why a desktop run cannot leave Page 1.** The sample is touch-only: it reads `GestureSample`s from
`TouchPanel` and has no keyboard or mouse route, exactly as the original does. The port does not
enable `TouchPanel::setMouseTouchEmulationEnabledEXT`, and `scripts/capture-cna-native.sh` sends no
input at all -- it screenshots once after `sleep 6` and exits. So the native evidence is Page 1 by
construction, and Page 1 matches XNA at RMSE `9.9e-05`, tighter than the browser's.

That seam is available and is already used elsewhere in this campaign: SAMPLE-071 enables it in
`YachtGame::Initialize` and records it in its `diff.md` as a platform seam, on the grounds that the
game is all touch and a desktop has no touch screen. Enabling it here plus driving the same taps
would let the native builds cover the pages the browser already covers. Not done in this pass,
because it changes the sample and the campaign's rule is that a change to a sample is a deliberate
act with its own evidence, not a side effect of an audit.

### Resolved 2026-09-08: the native builds now cover every page

`TouchPanel::setMouseTouchEmulationEnabledEXT(true)` was added to `DynamicMenuSample.cpp` (recorded
in `diff.md`) and `scripts/capture-cna-native.sh` now walks the same route `chrome-smoke.mjs` does,
at the same window coordinates: Page 2, Page 3, four progress advances, back to Page 1 and the index
action. The three misleading page-1 duplicates in `evidence/cna-native-opengles3/` were deleted.

Against the original's diagnostic captures, per frame:

| frame | native | browser |
| --- | --- | --- |
| `01-page1` | **9.9e-05** | 3.4e-04 |
| `02-page2` | **9.9e-05** | 5.0e-04 |
| `03-page3` | **9.9e-05** | 3.8e-04 |
| `04-page3-progress40` | **9.9e-05** | 3.8e-04 |

The native figure is identical across all four frames because the difference is: **at most one
level out of 255, on 0.098 % of pixels, in the same places every frame** — 376 pixels of 384,000,
mean difference 0.0006/255. It is least-significant-bit rounding on a fixed part of the frame, not
content. The pages are genuinely different from each other (0.26, 0.32, 0.048 between them), so the
walk really happened.

Native is now tighter against XNA than the browser is, on every page.
