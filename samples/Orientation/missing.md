# SAMPLE-102 — Orientation_4_0 audit

## Current analysis — 2026-09-28

**Current status: `🔎`, analysis complete; current-chain qualification and gallery delivery next.**
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
