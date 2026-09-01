# SAMPLE-102 — Orientation_4_0 audit

**Status: complete — no known behavior or content differences from the XNA 4.0 original.**

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

None. C#-to-C++ ownership, type identity and executable-host representation are documented in
`diff.md`; they do not change observable behavior. The three alternate tutorial configurations are
source edits rather than selectable modes in both products and were not misrepresented as runtime
acceptance paths.
