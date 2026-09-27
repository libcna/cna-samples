# SAMPLE-084 — AccelerometerSample_4_0 audit

## Current-head requalification — 2026-09-27

**Status: complete at CNA `next 629554a95`, Sharp Runtime `next 9e58c955` and this
samples checkout.** Artifact root:
`/rv/tmp/samples/SAMPLE-084-AccelerometerSample_4_0/`. The physical 23-file upstream
Windows Phone/Reach project is byte-identical to `xna4-original/`. All four C# units, including
the dormant desktop `Program.cs`, have their faithful C++ representation; the normal Phone host
and the diagnostic executable-host seam are explained in `diff.md`. The sample's original
`DeviceType::Emulator` arrow-key branch is kept for both desktop OPENGLES3 and WEBGL2 at the
owner's explicit request. Current CNA `Environment::getDeviceTypeProperty()` returns `Emulator`
on desktop and web and `Device` on Android/iOS. The old 2026-09-09 native executable was linked
to retired `openeggbert/cnanext` and entered the inactive no-sensor `Device` branch; its failure
to react to arrows does not describe the rebuilt product. No sample-side input substitute, forced
sensor, renderer condition, loose-asset path, Escape/F1 help or mouse path was added. Physical
mobile accelerometer input remains source/API reviewed, not hardware tested.

The six missing upstream package files were restored **byte-for-byte** outside runtime `Content`:
`Microsoft Permissive License.rtf`, `Background.png`, `Game.ico`, `GameThumbnail.png`,
`Properties/AppManifest.xml` and `Properties/WMAppManifest.xml`. The original `help.png` stays at
the sample root and is not loaded. The two checked-in Windows/Reach XNBs are still byte-identical
to the rebuilt official XNA Content Pipeline outputs (`asteroid.xnb`
`532af2b9c8d9732c364016413ec8f3c31c1b5d636587aaeb9ebfc738a2c0736d`; `space.xnb`
`2e68156a90e13e8bc45cb04fa3e66b4bd1e60e2171216f86d8c1e1ce12c73d30`). The
Phone-specific XNB pair is retained in `xna4-build/`. The package/source inventory and hashes
are in `evidence/current-head-analysis-20260927/inventory.json` and
`evidence/xna-content-sha256.txt`.

`scripts/build-original.sh` rebuilt the unchanged C# sources under their real `WINDOWS_PHONE`
definition with the documented diagnostic host and Phone shim, plus both official platform content
sets. The rebuilt executable is `xna4-build/bin/AccelerometerDiagnostic.exe`. Xvfb currently
makes XNA show a WineD3D **No suitable graphics card found** dialog, so the fresh original run
used the machine's working X11 display (`DISPLAY=:0`,
`WINEPREFIX=/home/robertvokac/.wine-cna-xna40`, `WINEDLLOVERRIDES=d3d9=b`).
`CNA_CAPTURE_DISPLAY=:0 CNA_CAPTURE_USE_EXISTING_DISPLAY=1
CNA_ORIGINAL_EVIDENCE_ROOT=.../evidence/current-head-analysis-20260927
scripts/capture-original.sh` captured both branches. The emulator's Right-key transition changes
17,028 pixels in the 2048×1152 host capture; the diagnostic Device mode's synthetic sensor event
changes 17,124 pixels. These captures verify the original branch behavior, not a physical sensor.
Historical isolated 480×800 original captures remain under `evidence/xna-original-{emulator,device}`.

`scripts/build-cna-native.sh` rebuilt Release OPENGLES3 against the active libcna repositories,
using the shared ccache and no job cap. `scripts/capture-cna-native.sh` first confirmed the neutral
480×800 frame is pixel-identical to the original isolated XNA emulator reference. Its Xvfb/GNOME
session did **not** deliver synthetic arrow keys to SDL, although the render kept running. A
separate normal-desktop run isolated that capture-environment limitation: instrumented polling
reported `DeviceType::Emulator` and Right becoming pressed, and the final uninstrumented product's
Right and Up captures changed 32,714 and 32,723 pixels respectively at the 2048×1152 host
resolution. The asteroid visibly moves right and then up; screenshots and renderer logs are in
`evidence/current-head-analysis-20260927/native-final/`. The renderer is Mesa OpenGL ES 3.2,
with no fatal application error. The exact game window closed normally with `wmctrl -ic` and
process exit code 0 (`native-wmctrl-status.txt`). The temporary input probes were removed before
the final rebuild. The final product is
`cna-native-opengles3-release/samples/AccelerometerSample/AccelerometerSample_cna_samples`.

`scripts/build-cna-web.sh` rebuilt the **nonthreaded** WEBGL2 product; its JS has zero
`PThread`/`pthread`/`SharedArrayBuffer` markers. `scripts/capture-cna-web.sh` ran it in the
system Chrome over ordinary HTTP. It obtains real WebGL 2 at 480×800, renders the authentic
asteroid/starfield, changes 15,352 pixels after held Right and 15,489 after held Up, completes
600 further animation frames, and reports no runtime exception, unhandled rejection, fatal
console message or relevant HTTP error. The neutral WEBGL2 frame is pixel-identical to the
original XNA emulator reference. The exact four-file `.html/.js/.wasm/.data` bundle was copied
to `samples.libcna.com/AccelerometerSample/`; its SHA-256 hashes match the tested build. A second
Chrome run from that copied bundle passed the same gates. The 79th gallery card, detail page and
actual game screenshot were added. Evidence is under `evidence/cna-web-webgl2-qualified/` and
`evidence/gallery-webgl2/`; the web product is under
`cna-web-webgl2/samples/AccelerometerSample/`.

No CNA or Sharp Runtime source change was needed for this requalification. The old historical
sections below describe their then-current heads and must not be read as the current desktop
`DeviceType` contract. The only remaining hardware limit is that no physical phone sensor was
available to run the preserved `Device` branch.

## Owner-authorized artifact prune — 2026-09-27

The owner requested pruning after commits `d70a09c` and `630f326` reached `origin/develop`
and gallery commit `54c1e9f` reached `origin/main`.
`tools/prune-completed-sample.sh --apply SAMPLE-084-AccelerometerSample_4_0` reduced the
artifact root from 255.3 MB to 34.3 MB; `MANIFEST.md` records the retained products and
rebuild commands. The retained Release OPENGLES3 executable still rendered and moved under
Right/Up (32,714/32,693 changed host-display pixels), then exited through its exact window
with code 0. Its console, captures and status are in `evidence/post-prune-20260927/`.
The retained web `.html`, `.js`, `.wasm` and `.data` files remain byte-identical to the
published gallery copy. The original diagnostic, XNBs, source snapshot, scripts and earlier
evidence remain in the pruned root.

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
