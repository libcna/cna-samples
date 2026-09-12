# SAMPLE-005 audit — ReachGraphicsDemo_4_0

Requalified on 2026-09-12 against the unchanged local XNA 4.0 sample. The port contains the title
screen and all six demos, uses the original pipeline assets, and passes current native and browser
runtime gates without a sample-side workaround.

## Reference and reproducible artifacts

The retained audit root is:

```text
/rv/tmp/samples/SAMPLE-005-ReachGraphicsDemo_4_0/
```

- `xna4-original/` is the unchanged upstream snapshot.
- `scripts/build-original.sh` rebuilds the original executable and all 22 assets with Microsoft's
  XNA 4.0 content pipeline. The top-level `build-original.sh` is a convenience wrapper.
- `xna4-build/Content/` and `xna4-build/bin/` retain the reference XNBs and runnable XNA program.
- `scripts/build-native.sh` produces the OPENGLES3 Release product retained under
  `cna-native-opengles3/samples/ReachGraphicsDemo/`.
- `scripts/build-web.sh` produces the non-threaded WEBGL2 Release bundle retained under
  `cna-web-webgl2/samples/ReachGraphicsDemo/`.
- `scripts/capture-original.sh`, `capture-native.sh` and `browser-gate.py` reproduce the three
  runtime evidence sets under `evidence/`.
- `evidence/SHA256SUMS` records every retained product and content asset.

No `current` alias directories or duplicate native/web products are retained.

## Source fidelity repairs

This pass corrected the remaining port-fidelity defects:

- restored the original logical namespace and runtime type identities, `XnaGraphicsDemo.*`;
- restored the inactive `WINDOWS_PHONE` fullscreen constructor branch and verified it in a
  compile-only Release build;
- marked the C++-only reader registration and all `GetTypeName()` extensions with `CNAEXT`;
- kept `BigFont.xnb` under the content project's official casing. CNA's `ContentManager` resolves
  the original `Content.Load<SpriteFont>("bigfont")` case-insensitively on Linux.

The explicit `ContentReaders::Register()` call is the AOT equivalent of the reflection XNA uses to
find the sample's `SkinnedModel.*` and `GeneratedGeometry.Sky` readers. It deserializes the original
object graph; it does not replace or convert any asset. `diff.md` documents this language adaptation
and the unconditional CMake executable entry point.

The disabled `ResolutionMenu` and the original platform branch remain in the port. All title and
demo behavior is present: BasicEffect, DualTextureEffect, AlphaTestEffect, SkinnedEffect,
EnvironmentMapEffect, SpriteBatch particles and the unattended attract sequence.

## Original XNA pipeline result

The original Windows project and its custom processors build successfully with the local XNA 4.0
toolchain. Twenty of the 22 rebuilt XNB files are byte-identical to the checked-in files. The only
byte differences are `BigFont.xnb` and `font.xnb`: the host lacks Microsoft Arial, so the build-only
input copy substitutes Liberation Sans. Both rebuilt font files have the same sizes as the retained
files (133306 and 70830 bytes), but font rasterization is not byte-stable across those environments.
The checked-in assets were not replaced; both CNA products consume those original XNA-pipeline XNBs.

The original executable was driven through all seven screens under Wine. The screenshots and their
SHA-256 values are in `evidence/xna-original/`; each scene was visually inspected. The empty
`runtime.log` is expected because this XNA executable produced no console diagnostics.

## Native OPENGLES3 result

The current native product is a Release `CNA_GRAPHICS_RENDERER=OPENGLES3` build containing one
executable and the 22 official XNBs. A real X11 input pass opened the title and all six demos, used
each scene's Back action, and exited normally (`exit code=0`). `evidence/cna-native/` contains the
seven visually checked captures, hashes and renderer log. The log shows OPENGLES3 initialization
and clean context release with no application exception or renderer error.

## WEBGL2 result

The current browser product is a non-threaded Release `CNA_GRAPHICS_RENDERER=WEBGL2` bundle:

```text
ReachGraphicsDemo_cna_samples.html   19,624 bytes
ReachGraphicsDemo_cna_samples.js    255,236 bytes
ReachGraphicsDemo_cna_samples.wasm  7,898,453 bytes
ReachGraphicsDemo_cna_samples.data  6,003,677 bytes
```

The WASM contains no `debug_info` marker and the JavaScript contains no pthread/shared-memory
runtime marker. System Google Chrome 152.0.7977.82 loaded the bundle over local HTTP and received
real mouse input for the title, Basic, Dual Texture, Alpha Test, Skinned, Environment Map and
Particles screens. `evidence/cna-web/browser-gate.json` records a live WebGL2 context, the exact
480×800 canvas/backing size, `glError: 0`, zero exceptions and zero application/GL problems.
The seven final captures in the same directory were visually inspected.

The four verified bundle files are also published at
`samples.libcna.com/ReachGraphicsDemo/ReachGraphicsDemo_cna_samples.html`.

## Framework and remaining differences

This requalification required no change in CNA, sharp-runtime, meta-gl or easy-gl. Framework fixes
found by the earlier Sample 5 audit were already present and are exercised by both current products.
There is no renderer substitution, loose-content bypass, generated replacement asset, custom
full-screen quad, placeholder scene or altered sample input.

The remaining differences from C# are ordinary C++ syntax/ownership/property calls, explicit AOT
reader registration, and an unconditional CMake `main`. The Liberation Sans substitution exists
only in the reproducible build workspace. There is no known sample-side workaround or unresolved
runtime/content dependency.
