# SAMPLE-003 audit — `TexturesAndColorsSample_4_0`

Status: complete. No known sample-side workaround or fidelity omission remains.

## Authoritative reference

- Original XNA 4.0 source: `/rv/tmp/XNAGameStudio/Samples/TexturesAndColorsSample_4_0`.
- Retained audit snapshot: `/rv/tmp/samples/SAMPLE-003-TexturesAndColorsSample_4_0/xna4-original`.
- The original Windows project compiles unchanged against XNA 4.0. Its executable requires the
  prepared 32-bit prefix `WINEPREFIX=/home/robertvokac/.wine-cna-xna40` on this host.
- The original content project requests `GenerateMipmaps=True` for `Clouds.png`. Re-running the
  Microsoft XNA 4.0 pipeline with that processor parameter produced a 1,398,323-byte `XNBw`
  texture containing ten mip levels. That exact file is now the committed `Content/Clouds.xnb`
  (`SHA-256 0edf37d8751b6599d28dada54703048f4022232d79fb1306fa82ff2a0bd354ef`).
- The preceding 1,048,763-byte, one-level file came from the audit runner omitting the processor
  parameter; it was not faithful output of the unchanged content project.

## Fidelity result

The C++ port follows `TexturesAndColors.cs`, `SampleCamera.cs` and `SampleGrid.cs` line by line:

- it loads the original Cube, high/low sphere, Cylinder and Cone models in the original order;
- it loads the Clouds texture, DebugText font and original compiled `TexturesAndColors.fx` effect;
- it exposes all 13 original effect techniques and their exact names;
- it binds the original world/view/projection, light, ambient and texture parameters;
- it draws the original lime-green 32-unit reference grid and safe-area technique label;
- it preserves the original camera, world rotation, Tab/Space, gamepad X/Y and Escape/Back controls;
- it uses the original ModelMesh/ModelMeshPart draw path rather than generated primitives; and
- `SampleGrid` now implements `System::IDisposable`, exposes the original public idempotent
  `Dispose()`, and retains the original `Dispose(bool)`/destructor lifecycle split.

Removed substitutions remain absent: the local `GeometricPrimitive` hierarchy, `BasicEffect` in
place of the sample effect, inert technique counter and invented F1 overlay. `help.png` remains at
the sample root as historical documentation only. The runtime `Content/` directory now contains
exactly eight pregenerated XNB files produced by Microsoft XNA 4.0. The loose `Clouds.png` sidecar
was removed and is not copied or preloaded. The original authoring sources remain in the retained
upstream snapshot, not in the runtime sample.

## Content provenance

SAMPLE-003 intentionally does not invoke CNA's new XNA Content Pipeline. Its eight checked-in
runtime files are retained Microsoft XNA 4.0 pipeline products: seven are byte-identical to the
original executable's content directory, and `Clouds.xnb` is byte-identical to the corrected
official XNA rebuild with the project's `GenerateMipmaps=True` parameter. The compiled Effect XNB
therefore remains the standard input without requiring Microsoft `fxc` on native, browser or
ordinary sample builds.

CNA's source Content Pipeline is a separate framework capability that may be relevant to later
authoring/tool samples. It is not needed to run or reproduce this sample's shipped content.

## CNA defects fixed during the original audit

1. `Game` did not register built-in XNB readers before `LoadContent`; construction now performs
   the one-time registration and has a regression test.
2. CNA's public Emscripten link contract used the 64 KiB default Wasm stack; it now propagates a
   1 MiB stack, enough for the original compiled effect parser.

No new CNA or sharp-runtime change was required for this correction.

## Final verification — 2026-09-12

- Active repositories: `cna-samples/develop`, `cna/next`, `sharp-runtime/next`.
- Native Release OPENGLES3: builds and runs; automated X11 capture proves 13 distinct techniques,
  five distinct models and clean Escape shutdown.
- Web Release WEBGL2: builds with Emscripten threads disabled. The publishable bundle is
  19,624-byte HTML, 253,738-byte JS, 7,591,557-byte WASM and 1,521,917-byte data.
- Real system Chrome/SwiftShader: WebGL2 context present, CNA reports WEBGL2, all 13 techniques and
  five models are visually distinct, with zero runtime exceptions, relevant HTTP errors or fatal
  console messages.
- The Emscripten preload manifest names only the eight XNB files and contains no `Clouds.png`.

Evidence and reproduction scripts are under
`/rv/tmp/samples/SAMPLE-003-TexturesAndColorsSample_4_0`. No other renderer is claimed.
