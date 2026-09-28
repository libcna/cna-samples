# SAMPLE-102 — Orientation_4_0 audit

## Owner closure and artifact prune — 2026-09-28

The owner explicitly requested pruning and committing/pushing all affected repositories.
The single-root helper removed **43 intermediate paths**, including the CNA/unit-test build
trees, CMake/object output and original pipeline runner. Native products were stripped;
identical retained files were consolidated within this root. The artifact root is approximately
**74 MiB allocated**, down from approximately **1.8 GiB** before the prune. Closure receipts
and the new smoke capture are included in the final figure.

All original files, original executables/content, scenario #4 references, build/run helpers,
historical/current sample products and cited evidence remain. Immediately after pruning,
**374 retained files** were byte-identical; only the three expected native ELF files and generated
manifest changed. The complete qualified sensor diagnostic was subsequently relocated to
`evidence/closure-20260928/sensor-diagnostic/`, preserving its bytes. The current four-file web
bundle remains byte-identical to the gallery. A repeat dry run removes zero paths. The stripped
current native product starts on a private display, has the exact qualified 800×480 pixels and
exits **0** through WM_DELETE_WINDOW.

`evidence/closure-20260928/` contains the before inventory, actual prune log, removed-file list,
retained hashes, previous manifest, repeat dry run and native smoke results. The current
`MANIFEST.md` gives reproduction commands; closing heads and publication verification are in
`final-heads.json`. This closure changes neither game logic nor the previously qualified input
behavior. Later unrelated CNA service commits do not requalify the retained 102 binary.

## Current completion — owner-requested orientation emulation, 2026-09-28

**Status: `✅`.** The owner requested two independent CNA emulations and orientation input in
102. CNA now supplies the off-by-default Accelerometer keyboard mode and per-window keyboard
orientation mode (framework task INPUT-EMU-001, local commit `31a560af9`). 102 enables only the
latter with one CNAEXT constructor call. It selects the **existing original scenario #4** so
rotation and the original tap-to-lock branch have a visible effect. This owner-requested default
configuration difference and the shared mouse opt-in are explicitly recorded in `diff.md`.

### Controls and fidelity

- Up: Portrait; Left: LandscapeLeft; Right: LandscapeRight.
- Tap/left-click: the original lock/unlock action. A blocked orientation request is applied after unlock.
- Gamepad Back: original exit action. Native window close/browser tab close remain host actions.

The port contains no keyboard handler or orientation-state substitute. CNA honors
SupportedOrientations, updates presentation parameters and the real logical backbuffer, and raises
OrientationChanged after application. Fullscreen, 30 Hz, source scenario explanations, content
identifiers, update/draw order and the original lock algorithm remain. Both landscape positions
have the same image because the original Draw code does not vary its artwork between them.
The separate accelerometer emulation is **not enabled** by 102.

All 18 original files / 167,138 bytes freshly match upstream. Font.xnb and directions.xnb again
verify byte-identical to retained fresh official Phone/Reach pipeline output; web data is the same
252,265-byte two-XNB bundle. Metadata/license and the earlier unchanged-source diagnostic remain.
The task's new XNA references compile separate copies with the tutorial's documented scenario #4
block uncommented. They never alter `xna4-original/` or the retained scenario #1 executable.

### Qualification

Fresh Release OPENGLES3 and nonthreaded Release WEBGL2 use canonical CNA `next 31a560af9` and the
owner's unchanged Sharp Runtime branch `feature/gamer-services-collections 6c4a857d`, all cores,
the shared ccache and CCACHE_BASEDIR=/rv. Current native product:
`/rv/tmp/samples/SAMPLE-102-Orientation_4_0/cna-native-opengles3/samples/Orientation/Orientation_cna_samples`.

- Native real keys/mouse: 800×480 → 480×800 portrait, lock, rejected Right while locked, pending
  Right applied after unlock, Left, focus-loss rejection and normal WM close **exit 0**. The
  locked/blocked frames are identical; both returned landscape frames equal the initial frame.
- Original scenario #4 Windows diagnostic, native, current WEBGL2 and the actual gallery screenshot
  have **AE=0 at 800×480**, including both original unlocked-state text lines.
- Plain-HTTP private system Chrome: **719 current-product / 717 exact-gallery game draws**,
  actual WebGL 2, crossOriginIsolated=false, three orientation shapes, real mouse lock/unlock,
  pending rotation, no runtime/required-resource errors. Standard browser Gamepad Back input
  fixture releases GL contexts **1→0** and subsequent draws stop. This fixture is outside the product.
- Chrome's fullscreen transition can resize its physical canvas independently of the logical
  backbuffer. The final gallery test checks actual GL viewport shape, retains screenshot/focus/
  fullscreen metadata and compares equivalent landscape presentations.
- Separate public-API browser sensor client: support is true after opt-in for a preconstructed
  instance; normal sensor events deliver neutral/right/normalized diagonal/released values and
  Back cleanup succeeds. This diagnostic is retained in scripts/build artifacts, not deployed.
- CNA private GPU runner: runtime **43/43**, sensors **91 passed + 4 physical-hardware skips** out
  of 95, input **83/83**. The six new tests exercise real API/lifetime/input paths; existing
  accelerometer/gyroscope and mouse-to-touch regressions pass. No Sharp Runtime fix or stub.
- Gallery desktop/mobile controls, actual 800×480 game image/560×336 thumbnail, adjacent navigation
  and all **83 unique playable entries** pass. The exact four-file bundle is deployed locally.

The private native display is tall enough for portrait; bare Xvfb's existing fullscreen handshake
fallback requires waiting for transitions and explicit test focus. It is not changed in the sample.
The desktop XNA diagnostic stays landscape even in the attempted combined portrait-preference
configuration; it does **not** qualify physical Phone rotation/hardware scaling. Native physical
Gamepad Back was not exercised. These boundaries are retained rather than represented as measured.

### Evidence and reproducibility

All task evidence: `/rv/tmp/samples/SAMPLE-102-Orientation_4_0/evidence/keyboard-emulation-20260928/`.
It includes `before/`, initial/final heads, build/test logs, desktop results, current/exact-gallery
browser results, sensor browser results, gallery UI results and `integrity.json` with source,
content and product hashes. The earlier qualification below remains historical evidence.

Task helpers in the same root's `scripts/`:
`build-scenario4-original.sh`, `build-cna-native.sh`, `build-cna-web.sh`,
`build-keyboard-sensor-probe-web.sh`, `capture-keyboard-desktop.py`, `capture-keyboard-web.py` and
three task-specific Chrome drivers. The manifest records arguments and the exact diagnostic role.
At that implementation checkpoint only a prune dry run was authorized; the later owner closure
above authorizes and records the actual prune and publication.

## Historical scenario #1 completion — 2026-09-28

**Status: `✅`, shipping scenario #1 qualified on the current chain.** The runtime C++ remains
unchanged and faithful to the original; no new sample workaround was added. The only intentional
input addition remains the owner's shared mouse-to-touch opt-in in `diff.md`. Default #1 is static:
a real click changes no pixels. The full inactive lock branch and commented source configurations
#2–#4 remain. No runtime scenario picker, keyboard rotation or Escape action was invented.

All **18 upstream files / 167,138 bytes** again match the retained snapshot. The original
Game.ico, GameThumbnail.png and Background.png are restored byte-identically at the port root,
with the original Microsoft license. They are package metadata, not runtime content substitutes.
The fresh official WindowsPhone/Reach pipeline reproduces both checked-in XNBs byte for byte;
Windows/Reach output is retained separately for the original desktop diagnostic. The 252,265-byte
web data file is exactly the two Phone XNBs, with no help overlay or loose asset sidecars.

### Builds and comparison

Fresh unchanged-source XNA Windows/x86 Debug Reach, Release OPENGLES3 and nonthreaded Release
WEBGL2 all pass. The XNA executable now embeds the original icon. This is the selected Phone
sample's unchanged code under its existing `WINDOWS` entry-point guard, **not a Phone-host run**.
The current native product is
`/rv/tmp/samples/SAMPLE-102-Orientation_4_0/cna-native-opengles3/samples/Orientation/Orientation_cna_samples`;
the older debug/release trees remain historical. Current CNA is clean `next 8d56fa2fa`; Sharp
Runtime is the owner's independently active clean `feature/gamer-services-collections 6c4a857d`,
preserved without edits or switching. Canonical sibling paths, all cores, the shared ccache and
`CCACHE_BASEDIR=/rv` are recorded in refreshed build helpers/caches.

All decoded 800×480 XNA, current native, current web and exact gallery-copy frames have **AE=0**.
Real desktop and browser clicks preserve the default frame. Both desktop products exit **0** through
a normal WM_DELETE_WINDOW request. Native physical gamepad Back was not exercised. Plain-HTTP
system Chrome renders **619 draws / 1,241 RAF callbacks**; the exact gallery copy renders **624
draws**. Both use actual WebGL 2 with `crossOriginIsolated=false` and have no page error, rejection,
runtime exception, fatal console message or required-resource error. The harness supplies a
standard `navigator.getGamepads()` input fixture: button 8 traverses the real original Back path,
releases GL contexts **1 → 0** and stops subsequent draws. This is test input outside the unchanged
product, not physical-gamepad qualification or a deployed synthetic input path.

The bare private Xvfb has no EWMH window manager, so SDL reports its fullscreen handshake timeout;
the 800×480 native frame still matches XNA exactly and the original fullscreen request is retained.
Physical Phone rotation/hardware scaling and source-edited alternatives were not claimed measured.
There is no audio or networking in this sample.

### Shared build dependency repair

Initial WEBGL2 configuration failed because the samples root unconditionally enabled unused
Gamer Services, whose current native account transport requires libcurl. Shared configuration now
reads the selected samples' existing `NET`/`GAMER_SERVICES` declarations before adding CNA.
Unrelated samples omit that optional subsystem; samples declaring it and the unrestricted corpus
retain it. Six focused selection checks pass (single, mixed and unrestricted selections), followed
by successful actual native/web configuration and builds. No account transport was substituted,
no game behavior was bypassed and no CNA/Sharp Runtime source changed. Historical framework test
counts below remain historical; they were not needed or rerun for this build-only repair.

### Gallery and reproduction

Local gallery commit **80e1834** adds the **83rd unique entry**, truthful static-default
controls, a real running-game screenshot, thumbnail and the exact four-file bundle. Real Chrome
passes desktop/mobile layout, both adjacent detail links, all gallery counts and zero runtime or
resource errors. SAMPLE-100 remains absent (its page and images still return 404). This request
created local commits only; **no push or SAMPLE-102 prune was requested or performed**.

Artifact root: `/rv/tmp/samples/SAMPLE-102-Orientation_4_0/`.
Current commands are in `MANIFEST.md` and `scripts/build-{original,cna-native,cna-web}.sh`;
qualification uses `scripts/capture-current-desktop.py`, `capture-current-web.py`,
`chrome-current-orientation.mjs` and `chrome-gallery-orientation.mjs`. Owned private displays,
processes and temporary Chrome profiles are cleaned up. Historical scripts/products were
preserved before renewal under `evidence/requal-20260928/before-requalification/`.

Current evidence: `evidence/requal-20260928/` contains `inventory.json`, `build-heads.json`,
`module-selection.log`, original/native/web build logs, `desktop/result.json`,
`web/browser-result.json`, `gallery-game/browser-result.json`, `gallery-ui/result.json`, exact
bundle-copy hashes and closing `final-heads.json`. Product hashes and file sizes are in the
inventory; the current WASM is 7,850,492 bytes with no DWARF/shared-memory/pthread dependency.

## Historical analysis before renewed qualification — 2026-09-28

**Historical status: `🔎`; the work listed below is now resolved by the qualification above.**
The existing translation is faithful in the shipped scenario #1. No unapproved sample workaround
was found. The one owner-approved mouse-to-touch opt-in remains intact and documented in `diff.md`;
it is inactive as a game action while `enableOrientationLocking` is false.

All **18 upstream files / 167,138 bytes** match `xna4-original/` exactly. The selected Phone/Reach
project compiles only OrientationSample, Program and AssemblyInfo; LayoutSample is excluded and
is not a second product. The documentation requires editing the source to switch among four
scenarios. Preserve scenario #1 and all commented scenario edits plus the complete inactive #4
lock/unlock branch. Do not introduce a runtime scenario picker or keyboard rotation controls.
The shipped game uses 30 Hz, fullscreen, a centered 240×240 directions texture, Tap registration
and gamepad Back exit. It has no audio, network session, GamerServicesComponent, custom processor,
reader, serializer, application thread or direct accelerometer dependency.

Both checked-in Phone/version-5 XNBs and both native deployments retain their original official
pipeline hashes below. Their stock reader tables were decoded again: Texture2D, SpriteFont and
standard list/value readers only. The 252,265-byte web data file is exactly Font.xnb followed by
directions.xnb. The port documentation matches upstream byte for byte, and help.png is not deployed.

### Fresh runs of retained products

The retained unchanged-source Windows/Reach **diagnostic** runs under the established Wine prefix
and WineD3D on an owned 800×480 Xvfb display. It is not execution of the original Phone host.
The retained Release OPENGLES3 product runs on its own display and still loads the exact content.
Their decoded frames are pixel-identical, **AE=0**, and a real mouse click changes neither frame,
as expected for scenario #1. Both processes remain alive through capture and are then terminated
by the helper; the original GamePad Back exit was not exercised in this analysis.

The retained nonthreaded WEBGL2 bundle also passes **plain HTTP without COOP/COEP** in real
system Chrome: `crossOriginIsolated=false`, actual WebGL 2, 800×480 canvas/backbuffer, exact
content, trusted mouse click, **622 Clear/DrawElements calls**, 1,245 RAF callbacks and no page
error, rejection, runtime exception, fatal console message or required-resource error. Its decoded
frame is also **AE=0** against XNA. The irrelevant favicon request is excluded from the game
asset gate. Screenshots were visually inspected. This is retained-product verification, not a
fresh current-head build or qualification of physical Phone rotation or alternate source scenarios.

### Remaining work before renewed completion

- Rebuild Release OPENGLES3 and nonthreaded Release WEBGL2 against the selected current committed
  chain. This analysis pins CNA `next 8d56fa2fa` and independently active Sharp Runtime
  `feature/gamer-services-collections 6c4a857d`; preserve that owner branch. The previously
  mouse-enabled products were recorded at CNA fd16e1e52 / Sharp 9e58c955 and must not be
  attributed to today's heads.
- Refresh the old reproduction/manifest commands: they retain retired openeggbert/cnanext roots,
  obsolete job/cache assumptions and fixed/shared-display capture choices. New analysis helpers
  own their displays, processes and evidence; no old evidence was overwritten.
- Restore the upstream Game.ico, GameThumbnail.png and Background.png metadata in the port
  package as appropriate to the native/Phone host, and retain the original title. These files
  are intact in the snapshot but absent from the current port root.
- Add the missing gallery card/detail page, actual running-game screenshot and exact freshly
  qualified web bundle. The gallery currently has **no Orientation entry**. Explain that the
  default scenario is a static orientation guide and alternative scenarios require source edits.

No concrete large CNA subsystem blocker was found for the shipping scenario. Current CNA's
GraphicsDeviceManager follows FNA's desktop dimension handling; physical iOS/Android orientation
and alternate Phone tutorial configurations are not established by the 800×480 desktop/browser
runs. No game, framework, content or retained product was modified/rebuilt, and no prune or push
occurred for this analysis. Earlier framework test counts below remain historical; none was rerun.

Evidence under the artifact root is in `evidence/current-head-analysis-20260928/`:
`inventory.json`, `xnb-readers.json`, `review.json`, `image-comparison.json`, `desktop/result.json`,
`web-plain/browser-result.json` and closing `final-heads.json`. Reproduction helpers are
`scripts/analyze-retained-desktop-20260928.py` and
`scripts/analyze-retained-web-20260928.{py,mjs}`.

## Historical owner-requested desktop mouse input — 2026-09-27

The owner requested CNA's shared mouse-to-touch opt-in for touch-only ports. This source retains
the original Tap-controlled orientation lock in scenario #4, so the constructor now enables the
off-by-default extension with one `CNAEXT` line; see `diff.md`. The shipped scenario #1 still has
no active touch action. Release OPENGLES3 and nonthreaded Release WEBGL2 were rebuilt on CNA
`fd16e1e52` and Sharp Runtime `9e58c955`. The new native default frame and browser frame each
compare at `AE=0` changed pixels against the unchanged XNA reference. The system Chrome run
completed 611 WebGL 2 draws with no runtime/HTTP errors. Evidence is in the existing artifact
root's `evidence/mouse-optin-native/`, `evidence/cna-web-webgl2/` and
`evidence/mouse-optin-rebuild.log`. No alternate tutorial scenario was enabled for this test.

**Historical status: complete at the recorded build revisions, with the owner-approved desktop input addition above.**

Artifact root: `/rv/tmp/samples/SAMPLE-102-Orientation_4_0/`

## Original surface audited

The complete upstream directory is retained byte-for-byte under `xna4-original/`; its manifest and
SHA-256 inventory are at the artifact root. The shipping product is one Windows Phone/Reach game.
Its project compiles `OrientationSample.cs`, `Program.cs` and `Properties/AssemblyInfo.cs`, while the
phone host excludes `Program.Main` through the original preprocessor guard.

`LayoutSample.cs` is physically present beside the game source, but it is not a second shipping
product: the `.csproj` does not compile it and `Program.cs` constructs only
`OrientationSample.OrientationSample`. It was therefore audited as excluded source rather than
invented as another CNA executable.

The original is a source-editing tutorial with four configurations in one file. The distributed
program uses scenario #1: it leaves `SupportedOrientations` at the landscape default. Scenarios #2
and #3 are two commented preferred-back-buffer edits, and scenario #4 is a commented
all-orientations/lock-enablement block. The complete scenario #4 runtime branch remains compiled but
inactive because `enableOrientationLocking` is false. The C++ source preserves the same arrangement;
it does not silently select another scenario.

The live default retains the original 30 Hz target, fullscreen request, enabled Tap gesture, exact
`directions` and `Font` content identifiers, GamePad Back exit, update/draw order, CornflowerBlue
clear and integer-centered texture. The inactive lock branch also retains the original
`Window.CurrentOrientation` assignment, live viewport-width/height reassertion, all-orientation
unlock and `ApplyChanges()` call. The old port's scenario-#4 default, O-key rotation, mouse-to-Tap
injection, Escape exit, third instruction line, F1 overlay, forced windowed mode and 60 Hz timing are
all removed.

## Authentic content

`scripts/build-original.sh` runs `Font.spritefont` and `directions.png` through XNA Game Studio
4.0's official content pipeline for both WindowsPhone/Reach and Windows/Reach. The checked-in CNA
content is the exact output for the shipping Windows Phone target:

| File | SHA-256 |
|---|---|
| `Font.xnb` | `4b01b7c7c08ccfb71a29a234e65be3887a3c821dd6ea1c99e40ba41b7beb5444` |
| `directions.xnb` | `5a10f24d5de6b3c957db9b7199024ed33703843ce4b17864cfd79b2298294445` |

CNA loads both files directly through the original `Content.Load<T>()` calls. No PNG or generated
bitmap-font runtime sidecar remains. The original Windows outputs and all four hashes are retained
under `xna4-build/` and `evidence/xna-content-sha256.txt` for comparison. `Orientation.htm` is
byte-identical to upstream. Repository-policy `help.png` is retained beside `CMakeLists.txt`, is not
packaged, loaded or displayed, and cannot alter the product.

## Original XNA qualification

The shipping phone project has no desktop entry point. The audit therefore compiles the unchanged
selected game sources with their existing `WINDOWS` guard against official XNA 4 assemblies as a
Windows/Reach diagnostic; no source patch or replacement game logic is used. The executable SHA-256
is `ea8ccf51e4f6e124f8b711485d2bbde31f0378559642150f0e0d99b8278cf925`.

Under the established offline .NET 4/XNA 4 Wine environment, WineD3D and an isolated 800x480 X
display, the program produces the documented shipping frame: CornflowerBlue, a 240x240 white
direction guide centered at `(280,120)`, and no text. The captured PNG SHA-256 is
`1e52814e783724e3795f8f7246ee4d8f3355f60aeca8be1d03b20712c86eab4f`.

## CNA qualification

- Clean Debug and Release OPENGLES3 configurations build `Orientation_cna_samples` with at most
  eight jobs. Both obtain OpenGL ES 3.2, create a borderless 800x480 product window and load the
  exact Phone XNBs. A bare Xvfb has no EWMH window manager, so SDL logs that its fullscreen protocol
  handshake timed out, but the requested window still covers the complete 800x480 display; the
  sample source retains the real `IsFullScreen = true` request.
- Debug and Release captures are byte-identical to the XNA reference PNG. Each decoded comparison
  reports `AE=0` changed pixels.
- A clean Release WEBGL2 build emits the complete `.html/.js/.wasm/.data` bundle. The system Google
  Chrome obtains `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, loads every bundle resource with HTTP 200 and
  runs 609 measured CNA draws (`Clear` plus `DrawElements`) across 1,220 browser RAF callbacks.
- The browser canvas and backing buffer are both 800x480. Its PNG encoding differs from the native
  file, but the decoded image again reports `AE=0` against XNA.
- The browser probe reports no page error, unhandled promise rejection, runtime exception, fatal
  console message, shader failure or relevant HTTP error.
- Focused live-CNA regression suites pass 44/44 runtime/orientation/timing tests, 24/24 authentic
  Texture2D/SpriteFont XNB tests and 77/77 graphics/SpriteBatch/viewport tests. No CNA or Sharp
  Runtime defect was exposed, so neither dependency repository changed.

Build, run, browser, frame-count, image, console and checksum evidence is retained under
`evidence/`; all reproduction helpers are under `scripts/`.

## Known differences

The owner-approved mouse-to-touch opt-in is documented above and in `diff.md`. C#-to-C++
ownership, type identity and executable-host representation do not change observable behavior.
The three alternate tutorial configurations are source edits rather than selectable modes in
both products and were not misrepresented as runtime acceptance paths.
