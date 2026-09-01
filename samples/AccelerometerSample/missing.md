# SAMPLE-084 — AccelerometerSample_4_0 audit

**Status: complete — no known behavior or content differences from the XNA 4.0 original.**

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
