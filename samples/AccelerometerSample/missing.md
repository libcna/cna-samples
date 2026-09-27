# SAMPLE-084 — AccelerometerSample_4_0 audit

## Current-head analysis before requalification — 2026-09-27

**Status: current-head requalification pending.** The physical 23-file upstream directory is
byte-identical to `xna4-original/`, including its Phone project, documentation, package artwork,
manifests, licence and IDE files. This is one Windows Phone/Reach game library, not a desktop XNA
executable. Its four C# units are represented by the existing C++ accelerometer, game, entry-point
and assembly-metadata units. The original Phone host does not compile the dormant desktop
`Program.Main` branch (which refers to a template `Game1`); the C++ executable host is documented
in `diff.md`. The original content project has only `space` and `asteroid`, both processed as
textures. The two checked-in Windows/Reach XNBs are still byte-identical to the retained official
pipeline outputs and each occurs exactly once in the old web `.data` package. The Phone-specific
XNB pair remains in the artifact root. The current inventory and hashes are in
`/rv/tmp/samples/SAMPLE-084-AccelerometerSample_4_0/evidence/current-head-analysis-20260927/inventory.json`.

The focused original/C++ review found the same sensor event subscription, `Start()` and caught
`AccelerometerFailedException` on a device; the emulator's normalized Left/Right/Up/Down vector;
30 Hz/fullscreen 480×800 setup; original content loads; viewport-based movement/clamping;
immediate alpha-blended drawing; and GamePad Back exit. There is no sample-side mouse, Escape,
F1, loose-asset, renderer or forced-sensor path. The only source marker is CNA's required logical
type name; the portable executable host and event-lock representation are recorded in `diff.md`.
No touch-only mouse opt-in applies because this sample does not use touch input.

One historical runtime statement is now stale. Active CNA `next 629554a95` uses
`Environment::getDeviceTypeProperty()` from CNA `cb2c90208`: **desktop and web return
`DeviceType::Emulator`; Android and iOS return `DeviceType::Device`.** That compile-time mobile
mapping does not distinguish a mobile simulator from a physical handset. The historical native
qualification below describes Linux desktop entering `Device` and remaining still without sensor
hardware. That describes its old `openeggbert/cnanext` executable, not a product built from the
current source. A fresh native build must verify that desktop Right/Up now move the asteroid
through the original emulator path. The real-sensor branch remains in source, but neither the old
Wine diagnostic's synthetic event nor a desktop emulator run proves a physical mobile sensor.
No physical handset or mobile simulator was tested here. Sharp Runtime was `next 9e58c955` at
analysis. No dependency source was changed here.

The retained Release native executable dates from 2026-09-09 and its RUNPATH points to retired
`openeggbert/cnanext`. The retained WEBGL2 wasm dates from 2026-09-01; its JS contains 35
`PThread` markers. Old build scripts target `openeggbert/cna-samples`, omit the active
`CNA_SHARP_RUNTIME_ROOT` and shared `CCACHE_BASEDIR=/rv`, cap `--parallel` at eight and use a
separate Emscripten cache. The browser capture helper uses a threaded-server route. Historical
Chrome evidence shows WebGL 2, Right/Up movement, 600 further frames and no errors; it does not
qualify the active heads or an ordinary-HTTP nonthreaded bundle. There is no `AccelerometerSample`
gallery entry or bundle.

Six original package files are absent beside the port: `Microsoft Permissive License.rtf`,
`Background.png` (the Phone tile), `Game.ico`, `GameThumbnail.png`, `AppManifest.xml` and
`WMAppManifest.xml`. They should be restored verbatim outside runtime `Content`. Completion then
requires repairing the retained helpers, rebuilding the unchanged Phone-source diagnostic and
both CNA targets against active checkouts, comparing emulator movement with the original,
rechecking the current desktop and browser branches, and testing a nonthreaded WEBGL2 bundle in
system Chrome over ordinary HTTP. Publish and independently test the exact bundle with an actual
game screenshot after those gates pass. The Phone sensor path needs an honest compile/API and
failure-path check; no physical phone test is claimed without hardware.

## Historical completed-port audit

**Historical status: complete at the then-active heads, with no known behavior or content
differences from the XNA 4.0 original.** The current-head gates above supersede that status.

Artifact root: `/rv/tmp/samples/SAMPLE-084-AccelerometerSample_4_0/`

## Original surface audited

The complete upstream directory is retained byte-for-byte under `xna4-original/`; its manifest and
SHA-256 inventory are at the artifact root. The shipping product is a Windows Phone/Reach game with
three runtime sources: `Accelerometer.cs`, `Game.cs` and phone-hosted startup. The port preserves
the original logical `AccelerometerSample::AccelerometerState`, static `Accelerometer` and
`AccelerometerSample::Game` decomposition rather than the old renamed/merged game class.

Both original accelerometer branches are present:

- on `DeviceType::Device`, `Initialize()` subscribes to the real sensor's `ReadingChanged` event,
  calls `Start()`, catches only `AccelerometerFailedException`, and protects the most recent X/Y/Z
  value across the callback and game threads;
- on the phone emulator, the original arrow-key simulation sets Z to -1, applies Left/Right to X
  and Up/Down to Y, and normalizes the vector.

`Game` retains the 480×800 preferred back buffer, fullscreen request, 30 Hz target, exact content
identifiers, live viewport centering/clamping, velocity integration, white clear, immediate
alpha-blended draw order and GamePad Back exit. The old port's always-emulator sensor path,
windowed mode, Escape/F1 controls, forced acceleration, fixed viewport constants and runtime help
overlay are gone.

## General CNA correction

The faithful source exposed a platform-classification bug. Live CNA previously reported
`Microsoft::Devices::Environment::DeviceType::Device` in every build. That is correct for native
desktop/mobile targets, where CNA has a real SDL-backed accelerometer and a missing sensor must
follow the original caught-Start-failure path. It is wrong for CNA's browser target, which has no
physical sensor backend and is the direct analogue of the sample's keyboard-capable phone
emulator. The old sample hid this by deleting the device branch and always using the keyboard.

CNA commit `35268971c` fixes the owning layer: browser builds now report `Emulator`; native
desktop, Android and iOS retain `Device`. Native and C-ABI environment tests cover the platform
contract, and the actual generated WebAssembly C module returns `DeviceType::Emulator`. The sample
therefore executes its unchanged runtime branch selection without an environment variable,
sample-name check or sensor substitute.

On this Linux machine the native product honestly enters `Device`, attempts the real sensor and
continues inactive when hardware is absent. In Chrome it enters `Emulator`, so the original arrow
keys provide deterministic acceleration. No sample workaround remains.

## Authentic content

`scripts/build-original.sh` runs the two unchanged texture declarations through XNA Game Studio
4.0's official `TextureImporter`/`TextureProcessor` pipeline for both Windows Phone/Reach and
Windows/Reach. The selected native/browser CNA product checks in the exact Windows outputs used by
the runnable unchanged-source diagnostic:

| File | SHA-256 |
|---|---|
| `asteroid.xnb` | `532af2b9c8d9732c364016413ec8f3c31c1b5d636587aaeb9ebfc738a2c0736d` |
| `space.xnb` | `2e68156a90e13e8bc45cb04fa3e66b4bd1e60e2171216f86d8c1e1ce12c73d30` |

The platform-specific Phone pair and all four hashes remain under `xna4-build/` and
`evidence/xna-content-sha256.txt`. The old loose PNG runtime substitutes are removed; both textures
load only through the original `Content.Load<Texture2D>()` identifiers. `Accelerometer.htm` is
byte-identical to upstream. Repository-policy `help.png` is retained beside `CMakeLists.txt`, is
not packaged, loaded or displayed, and does not alter the game.

## Original XNA qualification

The upstream project is a phone application library rather than a desktop executable.
`scripts/build-original.sh` therefore compiles all unchanged game sources with their real
`WINDOWS_PHONE` definition into a labelled Windows diagnostic host. `PhoneRuntimeShim.cs` supplies
only the unavailable phone `DeviceType`/sensor types; it does not modify `Accelerometer.cs` or
`Game.cs`.

`scripts/capture-original.sh` runs the same binary twice under the established offline .NET 4/XNA
4 Wine environment and WineD3D on isolated 480×800 X displays:

- emulator mode starts with the asteroid centered, then an actual Right key moves it right;
- device mode raises one sensor event from the diagnostic phone shim and the unchanged callback
  moves the asteroid right.

Both executions render the original space/asteroid content at 480×800 and remain stable. These
captures prove the unchanged branch logic; they are not claimed as a real Windows Phone device or
physical-sensor test.

## CNA qualification

- Debug OPENGLES3 builds with at most eight jobs. Under an isolated 480×800 X server plus a real
  window manager, the fullscreen product obtains OpenGL ES 3.2, loads both XNBs, renders the same
  centered asteroid/starfield and remains stable through the no-hardware `Device` path.
- Release OPENGLES3 builds separately and repeats the same real-renderer/content/runtime gate.
- Release WEBGL2 builds a complete self-contained `.html/.js/.wasm/.data` bundle and runs in the
  system Google Chrome. The canvas/backing buffer are 480×800 and the context is genuine WebGL 2.
  A browser user gesture permits the original fullscreen request; the neutral frame contains the
  centered asteroid, held Right moves it to the right, held Up moves it upward, and the product
  then completes 600 additional animation frames.
- The browser probe reports no page exception, unhandled promise rejection, fatal console message
  or relevant HTTP error. Its image gate compares each capture with the exact background and
  proves the asteroid is present, then measures more than 15,000 changed pixels for each requested
  movement.
- The neutral Debug, Release and browser frames each have zero changed pixels against the
  unchanged XNA emulator reference. The Debug/Release PNGs are byte-identical to that reference;
  the browser encodes a different PNG file but decodes to the same 480×800 pixels.
- Focused CNA regression evidence includes the native environment tests, native C API route,
  WebGL2 C++ environment tests and the actual generated C-API Wasm module returning Emulator.

Build, run, browser, image, console and checksum evidence is retained under `evidence/`; all
reproduction helpers are under `scripts/`.

## Known differences

None. The necessary C#-to-C++ ownership, event-locking and executable-host representation is
documented in `diff.md`; it does not change observable behavior.

---

## Re-audited 2026-09-09

Verified independently: all four original C# units have counterparts, both checked-in XNBs are
byte-identical to the official pipeline output, and the source carries no desktop sensor
convenience, no mouse path and no `F1` overlay. The port is 337 lines against the original's 395,
and its headers carry 17 `@brief` at 18 % density, inside the campaign's usual band.

**Capturing the original twice, once per runtime branch, is what makes this sample checkable**, and
the two captures do differ in the way the branch logic predicts. Measuring the asteroid's centroid
in each start frame:

| capture | asteroid centre | note |
| --- | ---: | --- |
| XNA, emulator mode | x = **218** | neutral; no key held yet |
| XNA, device mode | x = **261** | the diagnostic shim's one sensor event has already moved it |
| CNA native | x = **218** | `Device` path with no hardware, so nothing moves it |
| CNA browser | x = **218** | `Emulator` path, no key held yet |

The image centre is x = 240, and the asteroid's pixel count is 4,141 in all three neutral frames
and 4,115 in the displaced one.

**The browser build is bit-exact against the XNA emulator capture on both frames** — RMSE `0 (0)`
for `01-start` and for the moved frame. A WEBGL2 bundle reproducing real XNA byte for byte through
a state transition is the strongest result any sample in this campaign has produced for the browser.

CNA native's start frame is likewise bit-exact against the neutral capture. It differs from the
*device* capture by 0.062, which is exactly the distance between the two XNA captures themselves —
the sensor event the shim raised, which no Linux desktop can raise, and which the sample correctly
does not fake.

Nothing needed correcting.
