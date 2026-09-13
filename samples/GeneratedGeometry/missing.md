# SAMPLE-012 audit — GeneratedGeometrySample_4_0

Freshly re-audited on 2026-09-13. No behavioral or visual difference and no unresolved CNA or
sharp-runtime dependency remains.

## Source boundary and behavior

The authoritative upstream directory contains exactly 25 files. The whole directory is retained
unchanged at:

```text
/rv/tmp/samples/SAMPLE-012-GeneratedGeometrySample_4_0/xna4-original
```

`diff -qr` and the retained verification script prove that snapshot identical to
`/rv/tmp/XNAGameStudio/Samples/GeneratedGeometrySample_4_0`. The fresh line-by-line review covered:

- `GeneratedGeometry.cs` and `Sky.cs`;
- all three Windows, Xbox 360 and Windows Phone project files and solutions;
- the Content project and its two processor declarations;
- `SkyContent.cs`, `SkyProcessor.cs`, `TerrainProcessor.cs` and the pipeline project;
- both `AssemblyInfo.cs` files, manifests, documentation and every source image.

The port preserves the `GeneratedGeometry` namespace, 800×480 default presentation, rotating
camera, projection, terrain-first draw order, default lighting, warm specular term, fog, far-plane
skydome projection, `DepthRead`, `Opaque`, `WrapUClampV`, state restoration and Escape/Back exit.
The inactive `WINDOWS_PHONE` constructor branch still selects 333,333 ticks and fullscreen mode;
a dedicated Release compile with `WINDOWS_PHONE` defined passed during this audit.

The original public `Sky.Model` and `Sky.Texture` fields are public with their original names in
the refreshed port. Ordinary C++ ownership, `std::optional`, RAII, property-call syntax and
`std::cos`/`std::sin` are lossless language representations. The one line C++ needs because it has
no assembly reflection is marked `CNAEXT` and documented in [`diff.md`](diff.md).

## Exact original content

`scripts/build-original.sh` freshly compiles the unchanged sample-owned processor assembly, runs
the Microsoft XNA 4.0 Content Pipeline and builds the unchanged Windows Reach game in
`work-xna4-fresh/`. It regenerated these exact outputs:

| Asset | Original pipeline path | SHA-256 |
|---|---|---|
| `terrain.xnb` | `terrain.bmp` → `TerrainProcessor` → `ModelProcessor` | `8b8527739c18fc52b2ef41ce9a22d44f9b8c1bb3da261a26c9410d7f09602d12` |
| `rocks_0.xnb` | terrain external material → `TextureProcessor` | `e25b7875c42b23f9ba10c2cfbc80e11bf70e5bec9af8dfce46caba5d94264c57` |
| `sky.xnb` | `sky.bmp` → `SkyProcessor` → reflective `SkyContent` | `586d055a535d43b3d1f6ce40fd843848bf1fddfb1d04910cabb59042b1aec460` |

Every checked-in XNB is byte-identical to that fresh output. `terrain.xnb` loads through CNA's
stock `ModelReader`, including its external `rocks_0` texture. `sky.xnb` records the original
runtime type `GeneratedGeometry.Sky` and contains the processor-generated `Model` and uncompressed
`Texture2D`. The closed AOT `SkyReader` answers to the recorded reflective-reader identity and reads
the same two fields in the same order. The owner accepted exact pregenerated Microsoft XNB output
as this runtime sample's faithful content boundary under `SAMPLES-DEC-002`; this does not claim a
standalone C++ port of the design-time processor assembly.

The old runtime terrain/skydome generators, loose BMP substitutes, F1 overlay and `CullNone`
overrides remain absent. `Content/` has only the three official XNBs. Historical `help.png` remains
at sample root, is not packaged and is not referenced by runtime code.

## Fresh runtime gates

All builds use the current repositories at
`/rv/data/development/github.com/libcna/{cna-samples,cna,sharp-runtime}`, the shared ccache with
`CCACHE_BASEDIR=/rv`, and no more than four parallel compilation jobs.

- **Original XNA 4.0:** the freshly built x86 Windows Reach executable ran under the dedicated
  XNA Wine prefix with WineD3D on an isolated Xvfb display. It loaded all three official XNBs,
  rendered two different 800×480 frames of the rotating lit/fogged terrain and skydome, and exited
  cleanly when Escape was pressed.
- **Native:** a fresh sample-only Release build configured with
  `CNA_GRAPHICS_RENDERER=OPENGLES3` reported OpenGL ES 3.2 and `OPENGLES3`. It rendered the same
  scene in two changing 800×480 captures and exited cleanly through the original Escape path.
- **Browser:** a fresh sample-only Release build configured with
  `CNA_GRAPHICS_RENDERER=WEBGL2` and `CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF` produced exactly
  `.html`, `.js`, `.wasm` and `.data`. The 7.75 MB WASM has no `debug_info`; the JavaScript has no
  pthread/shared-memory markers. System Google Chrome fetched all four files with HTTP 200,
  reported WebGL 2.0 and `WEBGL2`, rendered changing 800×480 frames, produced no application/wasm/
  WebGL runtime failure, and stopped the game loop after Escape.
- **Published copy:** the four local `samples.libcna.com/GeneratedGeometry/` files are hash-identical
  to the gated artifact and passed the same Chrome/HTTP/animation/Escape gate again from the site
  directory.

Chrome's isolated profile logged only its usual unavailable NSS/GCM service messages and the
informational low-support warning for `WEBGL_polygon_mode`; no sample, WASM or WebGL error occurred.
The missing favicon request is outside the four-file sample product.

## Retained evidence and reproduction

The artifact root is:

```text
/rv/tmp/samples/SAMPLE-012-GeneratedGeometrySample_4_0
```

Its canonical products are:

- `xna4-build/bin/GeneratedGeometry.exe` with framework DLLs and exact content;
- `cna-native-opengles3/samples/GeneratedGeometry/GeneratedGeometry_cna_samples`, stripped;
- `cna-web-webgl2/samples/GeneratedGeometry/GeneratedGeometry_cna_samples.{html,js,wasm,data}`;
- `evidence/{xna-original,cna-native-opengles3-release,cna-web-webgl2-release,cna-web-site-release}`.

`scripts/` contains the original pipeline/game build, native and web rebuilds, isolated runtime
captures, the consolidated audit verifier and atomic product promotion. Both CMake scripts cap
parallelism at four. Reproducible `work-*` trees remain until the owner authorizes pruning.

No CNA or sharp-runtime source change was needed.
