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
`Microsoft.Xna.Framework.Content.ListReader\`1[[System.String]]`, followed by `StringReader`, and
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
