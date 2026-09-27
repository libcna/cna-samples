# SAMPLE-092 audit — ContentManifestExtensions_4_0

No known behavioral, visual, content or platform difference remains in the runnable sample game.
The sample-owned design-time pipeline assembly was audited and reproduced, but is intentionally not
claimed as a separate CNA authoring-tool port under the owner-approved `SAMPLES-DEC-002` boundary.

## Source and processor audit

The complete upstream directory was retained unchanged under
`/rv/tmp/samples/SAMPLE-092-ContentManifestExtensions_4_0/xna4-original/`. Every solution,
project, source, content declaration and documentation file was reviewed. There are two parts:

- `ContentManifestExtensions` is a pipeline assembly. `ManifestImporter` passes the manifest path
  through. `ManifestProcessor` locates the sole `.contentproj`, adds it as a dependency, records
  compiled logical names without extensions, records every non-`None` deployment copy with its
  content-root prefix, rewrites the source manifest for diagnostics, and returns `List<string>` for
  XNA's automatic writer.
- `SampleGame` is the runnable Windows/Phone game. The port retains `SampleGame.Game1`, its
  `Game1`/`Program` decomposition, 333333-tick target time, inactive 480x800 fullscreen phone
  branch, exact `Font` and `manifest` content identifiers, `Path.HasExtension` partition,
  `StringBuilder` output, CornflowerBlue draw, Escape/Back exit and lifecycle order.

The unchanged pipeline assembly and game compile against Microsoft XNA Game Studio 4.0. The
official pipeline was run for Windows/HiDef and Windows Phone/Reach. Both builds contain the same
14 manifest entries in the same order; the platform reader identities differ only in the expected
`mscorlib` profile. The unchanged Windows game runs under the campaign Wine prefix with WineD3D,
draws the generated list and exits cleanly when Escape is held across several 30 Hz update cycles.

## Exact content and DEC-002 boundary

The checked-in `Content/` is the exact Windows/HiDef deployment directory produced by the original
pipeline: ten XNB files, the copied `Characters/Duck.png`, and four copied text files. Keeping all
deployment files is important even though the game loads only `Font.xnb` and `manifest.xnb`: the
manifest's purpose is to describe which compiled and copied files are available to the title.

`manifest.xnb` is the 454-byte official output with SHA-256
`84bc94f58c304101061c37a5c9b235c47761b27966442fb14511c62308a6949e`. Its root reader is
the `ListReader` for `System.String`, followed by `StringReader`, and
its payload is exactly:

```text
Characters\Bear
Characters\Cardinal
Characters\Dog
Characters\Duck
clock
flashlight
heart
heart_grey
Font
Content\Characters\Duck.png
Content\CopiedFile1.txt
Content\CopiedFile2.txt
Content\CopiedFile3.txt
Content\CopiedFile4.txt
```

All asset and deployment hashes are retained in `evidence/xnb-sha256.txt`; the reader table and
independent payload dump are in `evidence/xna-original/manifest-xnb-dump.txt`. Exact pregenerated
XNA output is the owner-approved faithful runtime boundary from `SAMPLES-DEC-002`, as already used
by SAMPLE-012. It does not falsely claim that CNA implements XNA's design-time importer/processor
host.

## CNA defect fixed

CNA already implemented generic `ListReader<T>` and the XNA string reference-element rules, but
its built-in reader registry omitted the standard `ListReader<string>` combination. Before the
fix, a fresh `ContentManager` rejected the official fixture as an unregistered type reader. CNA
commit `e5ae0820e` registers the standard framework reader pair generally; there is no sample-side
registration or binary parsing.

The CNA regression fixture is the same official 454-byte file and verifies a fresh content manager
returns all 14 strings exactly. Debug and Release focused qualification each pass 3/3: string
reference-index consumption, primitive registration, and real-fixture loading. The Release
`CnaContentTests` target builds completely. A broader Debug invocation reached an unrelated,
pre-existing `GltfConformanceLadder.EveryGltfSuiteBelongsToExactlyOneRung` failure; the full log is
retained rather than misreported as a SAMPLE-092 regression.

## Native and browser fidelity

The Release OPENGLES3 target runs against the exact deployment directory, initializes Mesa OpenGL
ES 3.2, loads the official font and manifest, draws every item, and exits cleanly through the
original Escape branch. The complete Release Emscripten bundle runs in system Google Chrome on an
actual WebGL 2 context. Because the campaign's shared Web build uses pthreads, the standard audit
server supplies the required COOP/COEP headers; the page is cross-origin isolated. The browser
gate completes 600 animation frames with the original title and 800x480 canvas, no runtime
exception, unhandled rejection, relevant HTTP failure or fatal console message.

The isolated original XNA, native OPENGLES3 and WEBGL2 captures are byte-for-byte pixel identical:

| comparison | exact pixels | RMSE |
|---|---:|---:|
| XNA vs OPENGLES3 | 100.0000% | 0 |
| XNA vs WEBGL2 | 100.0000% | 0 |
| OPENGLES3 vs WEBGL2 | 100.0000% | 0 |

## Intentional C++ mappings

- C# reference fields map to `std::optional` while retaining construction and load order.
- CNA's XNB representation of `List<string>` is `std::vector<std::string>`; the two LINQ filters
  map to one stable-order loop using the same `System::IO::Path::HasExtension` predicate.
- The original `System.Text.StringBuilder` remains `System::Text::StringBuilder`; it is not
  replaced by a sample-local formatter.
- `using (Game1 ...)` maps to automatic storage, C# properties use CNA property accessors, and
  `Main` maps to `int main()`.
- `GetTypeName()` is CNA's required `CNAEXT` runtime identity and returns the original logical name
  `SampleGame.Game1`.

These are lossless language/runtime mappings. There is no owner-approved behavioral addition and
no `diff.md` is needed.

## Reproduction artifacts

Everything needed to reproduce the audit remains under
`/rv/tmp/samples/SAMPLE-092-ContentManifestExtensions_4_0/`:

- `xna4-original/`: complete untouched upstream snapshot;
- `xna4-build/`: unchanged pipeline assemblies, Windows/Phone products and runnable Windows game;
- `cna-native-opengles3/` and `cna-web-webgl2/`: reusable Release build trees and products;
- `scripts/`: original pipeline/build/capture, isolated native capture and real-Chrome harness;
- `evidence/xna-original/`: pipeline log, reader/payload dump and original capture;
- `evidence/cna-native-opengles3/` and `evidence/cna-web-webgl2/`: captures, runtime logs,
  browser result and HTTP evidence;
- `evidence/cna-list-string-{before-fix,after-fix,release}.log`: concrete framework regression
  evidence;
- `evidence/{xnb-sha256,pixel-comparison}.txt`: exact content and image measurements.

There is no remaining SAMPLE-092 blocker, substitute or sample-side workaround.

---

## Re-audited 2026-09-09: every claim verified, nothing to correct

**The pixel-identity claim is exact, and it is the strongest form of it.** All three frames compare
at RMSE `0 (0)`:

| pair | result |
| --- | --- |
| XNA original vs native OPENGLES3 | **`0 (0)`** |
| XNA original vs browser WEBGL2 | **`0 (0)`** |
| native vs browser | **`0 (0)`** |

Byte-identical output from real XNA, from a Mesa GLES 3.2 build and from a WebGL 2 bundle. Only
SAMPLE-079 and SAMPLE-084 have also matched real XNA exactly in the browser.

**The content is complete and authentic.** All **15** files under `Content/` — the ten official XNBs
and the five copied files — are byte-identical to the pipeline output.

**The manifest is what it is claimed to be.** `manifest.xnb` is **454 bytes**, header `XNBw`, and
carries exactly **14 entries**: nine asset names (`Characters\Bear`, `Characters\Cardinal`,
`Characters\Dog`, `Characters\Duck`, `clock`, `flashlight`, `heart`, `heart_grey`, `Font`) and five
raw paths (`Content\Characters\Duck.png`, `Content\CopiedFile1.txt` … `CopiedFile4.txt`). The
distinction between the two kinds is the sample's whole lesson, and it survives the port intact.

**The framework fix is in place.** the `ListReader` for `System.String`
is registered in `modules/content/src/Xnb/PrimitiveContentTypeReaders.cpp:47`, which is what lets the
manifest load at all.

**`ManifestPipeline` has no counterpart, correctly.** It is the design-time importer/processor
assembly, and this document is right not to relabel it as a CNA authoring-tool port — the same
position SAMPLE-074 takes with `TerrainProcessor` and SAMPLE-078 with its font processor.

The headers carry 8 `@brief` at 37 % comment density, the highest of any sample measured in this
campaign.

---

## Current-head re-analysis — 2026-09-27

The owner requested analysis of SAMPLE-092 after SAMPLE-091 was pruned and pushed. The current
heads were cna-samples `0f2b18c`, CNA `5cc244f23`, and Sharp Runtime `9e58c955`. All 32 files in
the retained upstream snapshot still match the original source. The port still follows the
owner-approved `SAMPLES-DEC-002` boundary: `ManifestPipeline` is an audited XNA design-time
assembly; the runnable `SampleGame` is ported, with pregenerated official content. No sample-local
workaround or behavior difference was found.

All 15 checked-in deployment files still match both the original XNA pipeline output and the
retained native deployment byte for byte. The 454-byte manifest still hashes to
`84bc94f58c304101061c37a5c9b235c47761b27966442fb14511c62308a6949e`, and current CNA
still registers the standard `ListReader<string>` pair with a real-manifest regression fixture.

The retained OPENGLES3 executable was rerun in an isolated Xvfb display; it displayed the complete
list and exited cleanly through Escape. Its fresh 800×480 capture is byte-identical to the
retained original XNA capture (SHA-256
`82f3a52a5418210a394a52c07e05c20ec07ff3af94968607946d1d86b884c9e3`, pixel AE 0).
The retained WEBGL2 capture also has pixel AE 0 against XNA. Its earlier real-Chrome gate completed
600 frames without a game runtime exception, rejection or asset HTTP error; its lone 404 was the
browser's `/favicon.ico` request. The native and web products were built in September against older
heads; this analysis did not rebuild either product against the current heads or rerun Chrome.
Those are reproducibility checks for a later build, not evidence of a present behavioral gap.

Machine-readable comparison and head inventory:
`/rv/tmp/samples/SAMPLE-092-ContentManifestExtensions_4_0/evidence/current-head-analysis-20260927/inventory.json`.
The artifact prune dry run proposes zero paths and saves zero bytes. Status remains `✅` on the
approved runtime boundary.

---

## Current-head completion — 2026-09-27

The owner asked to do SAMPLE-092, including WEBGL2. The previous re-analysis did not rebuild the
products; this pass did. Starting heads: cna-samples `d2d9cdd`, CNA `5cc244f23`, Sharp Runtime
`9e58c955`, gallery `54c1e9f`. The 32-file upstream snapshot still matches the physical original.
The unchanged XNA design-time assembly rebuilt both Windows/HiDef and Phone/Reach content and the
Windows game. The Wine run used
`WINEPREFIX=/home/robertvokac/.wine-cna-xna40 WINEDLLOVERRIDES=d3d9=b` inside isolated Xvfb;
Escape exited cleanly. The reproduction script now replaces deduplicated hardlinked copy targets
before the pipeline writes to them, and moves Wine's remembered offscreen window into the Xvfb
viewport before capture. These are artifact-script corrections, not sample source changes.

All 15 deployed Windows files, including ten XNBs, are byte-identical between the fresh Microsoft
pipeline build and the sample. The official `manifest.xnb` remains 454 bytes with SHA-256
`84bc94f58c304101061c37a5c9b235c47761b27966442fb14511c62308a6949e` and 14 entries.
No local reader, parsing bypass, asset substitution or game workaround was introduced.

New reusable `scripts/build-cna-native.sh` and `scripts/build-cna-web.sh` configure the active
checkouts with shared ccache. Release OPENGLES3 statically links CNA and builds and runs. Release
WEBGL2 uses `CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF`, so its four-file bundle is suitable for
ordinary static hosting; the WASM has no `debug_info` and the JS has no pthread marker. Both
render the complete 800×480 frame at pixel AE 0 against the newly captured original. The XNA and
native PNG files have SHA-256
`82f3a52a5418210a394a52c07e05c20ec07ff3af94968607946d1d86b884c9e3`; the browser PNG
has SHA-256 `7b5e6d3631188836276e9b8bce574485eef477183cd95f60d19e1c0f89e5923a` and
identical pixels. Native Escape exits with status 0. Real system Chrome completes 600 WebGL 2
frames, then Escape causes normal game cleanup; there are no runtime exceptions, rejected promises,
asset HTTP errors or fatal console messages.

The local `samples.libcna.com` gallery checkout now contains the byte-identical four-file bundle,
the real game screenshot, a detail page and card 80. The copied bundle passed the same real-Chrome
600-frame and Escape gate, with pixel AE 0 against XNA. Gallery links, eight cards on page 7 and
80 total cards were checked. A separate ordinary static HTTP run, without COOP/COEP headers,
also passes 600 frames, WebGL 2, Escape and pixel AE 0 with `crossOriginIsolated=false`.
The gallery change is local commit `39a2ad3`; publication remains a separate push.

Full commands, paths, product hashes and both browser results are recorded under
`/rv/tmp/samples/SAMPLE-092-ContentManifestExtensions_4_0/evidence/current-build-20260927/`,
especially `inventory.json`. No CNA or Sharp Runtime source changed. The status remains `✅` within
the owner-approved `SAMPLES-DEC-002` runtime boundary; the original design-time assembly is
audited and preserved, not claimed as a separate CNA authoring-tool port.
The artifact prune dry run proposes 27 intermediate paths, saving approximately 197.5 MB;
no deletion was authorized or applied.
