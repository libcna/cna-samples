# SAMPLE-007 audit — SpriteSheetSample_4_0

No known behavioral or visual difference remains after the fresh 2026-09-13 sequential re-audit.

## Reference and source audit

- The authoritative `/rv/tmp/XNAGameStudio/Samples/SpriteSheetSample_4_0` tree and the retained
  `/rv/tmp/samples/SAMPLE-007-SpriteSheetSample_4_0/xna4-original` snapshot each contain 35 files;
  `diff -qr` reports no difference.
- `Program.cs`, `SpriteSheetGame.cs`, `SpriteSheetRuntime/SpriteSheet.cs`, both original project
  branches, the content project and all four custom-pipeline sources were checked against the C++
  port and runtime package.
- The game retains the 853x480 backbuffer, two labels, rotating cat, seven glow frames at 20 fps,
  checkerboard behind the complete 198x264 atlas, draw order and CornflowerBlue clear color.
- The only input branches in the original are keyboard Escape and player-one GamePad Back. There
  is no Space-key behavior; the older handoff instruction that mentioned Space was incorrect.

The runtime package contains exactly `Checker.xnb`, `SpriteSheet.xnb` and `hudFont.xnb`. The loose
source sprites are absent, no texture is read back or repacked at runtime, and there is no
renderer-specific helper. Historical `help.png` remains beside the sample project, outside
`Content`, and is never loaded.

The sample-local `ContentReaders.hpp` is the required C++ AOT counterpart to XNA's generic
`ReflectiveReader<SpriteSheetRuntime.SpriteSheet>`. It reads the unchanged original object graph
and registers its exact serialized runtime names; it does not translate or replace the asset.

## Exact XNA content

The unchanged XNA 4.0 runtime, custom processor assembly and game were rebuilt with the local XNA
assemblies. The sample-owned `SpriteSheetProcessor` freshly packed the original cat plus seven glow
images into a 198x264 sheet at 78% efficiency. The new `Checker.xnb` and `SpriteSheet.xnb` are
byte-identical to the checked-in files.

The current Wine/fontconfig environment exposes XNA Game Studio's official Segoe UI Mono file but
selects a host fallback inside `FontDescriptionProcessor`. That newly generated, noncanonical font
is preserved as `evidence/xna-original/hudFont-host-fallback.xnb` with its provenance and parser
report; it was not put into a runtime product. The reference executable instead uses the already
established official XNA output. Its font atlas and metrics were independently parsed again, and
the resulting `hudFont.xnb` is byte-identical across the repository, XNA reference product and
native product. This is an audit-host font-selection limitation, not a CNA or sample behavior gap.

| Asset | SHA-256 | Repository = XNA product = native product |
|---|---|---|
| `Checker.xnb` | `a01446d7bc4142ff2c64641e22fe8cfd7ac132fd1415db96a16149a56b136e61` | yes |
| `SpriteSheet.xnb` | `ad590419cda80d37b53404ff8b540d96b51a8987854e2b100598feadbc206a78` | yes |
| `hudFont.xnb` | `533f35303a0e62daa693033b6a9413897012567134d2f4125a22a89d52020d66` | yes |

The independent XNB parser accepts the stock texture and sprite-font payloads. As expected, its
generic decoder reports that it has no decoder for the sample-specific reflective root type in
`SpriteSheet.xnb`; the original XNA runtime and CNA's explicit AOT reader both load that same file
successfully. `evidence/content-identity.tsv`, `content-conformance.json` and
`custom-content-conformance.json` retain these results.

## Fresh runtime gates

- Original XNA 4.0: the HiDef-profile Windows executable built and ran under isolated Wine/Xvfb.
  Two frames changed as the cat/glow animation advanced, the client area was 853x480, and a real
  keyboard Escape event ended the process with exit code 0.
- Native CNA: a clean, sample-only Release `OPENGLES3` build initialized EasyGL as OpenGL ES 3.2,
  rendered changing frames at 853x480, and ended with code 0 after keyboard Escape.
- Browser CNA: a clean, sample-only, non-threaded Release `WEBGL2` bundle ran from ordinary local
  HTTP in system Google Chrome with `crossOriginIsolated = false`. The HTML, JavaScript, WebAssembly
  and data files all returned HTTP 200; WebGL2 was active, `gl.getError()` remained zero, and no
  relevant page, console, network, wasm or WebGL error was recorded. Independent runs observed
  animation before both keyboard Escape and standard-gamepad Back stopped the loop.
- The Release wasm contains zero `debug_info` markers and the JavaScript contains zero pthread or
  shared-memory markers.

The time-dependent cat and glow cannot be compared at a shared instantaneous animation phase.
Instead, the static left-label crop and the complete right label/atlas crop were compared directly:
both native and browser renders have zero differing pixels from the XNA original in both regions.
The exact geometries and metrics are in `evidence/visual-parity.tsv`.

## Dependency and implementation result

No CNA, sharp-runtime, meta-gl or easy-gl change was needed in this pass, and no stub or new missing
dependency was introduced. The general string-reference XNB collection handling and nullable
`SpriteBatch.Begin` behavior repaired by the earlier SAMPLE-007 audit remain covered in their owning
repositories; the current port requires no sample-side workaround.

The only intentional deviations are normal C++ syntax/ownership adaptations and explicit AOT
reader registration in place of C# reflection. There is no unresolved CNA or sharp-runtime gap for
this sample.

## Artifact package

The pruned 17 MB audit root is
`/rv/tmp/samples/SAMPLE-007-SpriteSheetSample_4_0`. It contains one canonical original product, one
native product, one browser product, the unchanged source snapshot, fresh evidence, reproducible
scripts, `README.md`, `MANIFEST.md` and `SHA256SUMS`; disposable build trees and superseded captures
were removed only after product promotion and verification.

The byte-identical WEBGL2 product was published to `samples.libcna.com` with a new detail page,
gallery card, screenshot and previous/next navigation. The published copy itself then passed the
full Chrome keyboard/gamepad gate before site commit `e807c3b` was created locally.
