# SAMPLE-053 — CustomModelEffect fidelity report

**Status: complete on the current libcna chain (2026-09-25).**

## Current-head requalification — 2026-09-25

The complete 20-file physical upstream directory under
`/rv/tmp/XNAGameStudio/Samples/CustomModelEffectSample_4_0` remains byte-identical to the
retained `xna4-original/`. The Windows and Xbox projects both select Reach and share the
same game. The Windows project builds the sample-owned processor assembly: `saucer.fbx`
passes through `EnvironmentMappedModelProcessor`, which invokes
`EnvironmentMappedMaterialProcessor`; it compiles the original `EnvironmentMap.fx`, preserves
the diffuse texture and sends `seattle.bmp` to `CubemapProcessor` to make the six-face DXT1
cubemap. The original `.fx`, C# game, all three processors, content and project declarations
were audited with the C++ port. The port still loads the official `Model` and follows the
original matrix, parameter, mesh-draw, rotation and Escape/Back paths. No sample workaround,
replacement shader, generated model or loose-content substitute was found.

The missing `Microsoft Permissive License.rtf`, `Game.ico` and
`CustomModelEffectSample.png` were restored byte-identical to the physical original. Its
`CustomModelEffect.htm` was already identical. The original has no `help.png`.

### Builds and content provenance

`scripts/build-original.sh` rebuilt the unchanged XNA game using the established
`/home/robertvokac/.wine-cna-xna40` prefix; its official content pipeline succeeded for
Windows/Reach, Windows/HiDef and Xbox/Reach. The four checked-in Windows/Reach XNBs match
both the fresh `xna4-build/bin/Content/` and native Content byte for byte; exact SHA-256 values
are in the historical table below. `scripts/build-current.sh native|web` configured fresh
Release OPENGLES3 and nonthreaded WEBGL2 trees from this repository with
`CNA_SAMPLES_CNA_ROOT=/rv/data/development/github.com/libcna/cna` and
`CNA_SHARP_RUNTIME_ROOT=/rv/data/development/github.com/libcna/sharp-runtime`. Their heads are
CNA `5229c992e` and sharp-runtime `41b918c9`. The new native program has a current libcna
RUNPATH, not the historical `openeggbert` one.

### Live and frozen native behavior

The unchanged XNA executable ran on a private Xvfb display under WineD3D with
`WINEDLLOVERRIDES=d3d9=b`; the Release OPENGLES3 program ran on another private display.
Both drew the textured reflective saucer, their two ordinary screenshots had different
hashes as it rotated, and Escape closed each window. The native run log had no fatal error.
Evidence: `evidence/requal-20260925/{xna-original,cna-native-opengles3}/`.

`scripts/build-diag.sh` built the frozen CNA source in `diag-source/` and relinked the
original from `xna4-diag/`; it did not replace checked-in source. The only diagnostic
change in each game is the rotation-time source. `scripts/compare-frozen.sh` captured two
identical frames per engine at each pinned time:

| Time | RGB pixels within 8 | within 16 | within 32 | Mean absolute difference / 255 | Blur within 8 |
|---:|---:|---:|---:|---:|---:|
| 1 s | 99.944% | 99.974% | 99.992% | 0.034 | 100.000% |
| 3 s | 99.966% | 99.996% | 99.999% | 0.018 | 100.000% |
| 7 s | 99.944% | 99.986% | 99.997% | 0.030 | 100.000% |

The model coverage differs by at most 0.002 percentage points; centroids agree within
0.02 pixels. Per-time results and PNGs are under `evidence/requal-20260925/frozen/`.

### Browser and gallery

The current four-file WEBGL2 product ran in the system Google Chrome. Its 800x480 canvas
showed the reflective saucer, frame hashes changed with rotation, the title was
`Custom Model Effect`, and Escape stopped the game. The browser reported WEBGL2, zero
unhandled rejections, runtime exceptions, required-asset HTTP errors or fatal messages.
The incidental missing browser favicon is excluded from the required-asset gate.
The copied gallery bundle has the same four SHA-256 hashes and passed the same Chrome gate.
The Release WASM contains no `debug_info`, and the JavaScript has no pthread/shared-memory
marker. The gallery now has its 52nd card on page 5, a detail page, screenshot, thumbnail and
reciprocal navigation with the preceding sample. All eight gallery routes tested over local
HTTP returned 200 with nonempty bodies. Gallery local commit: `d9d90d8` (not pushed).
Browser evidence: `evidence/requal-20260925/{cna-web-webgl2,gallery-webgl2}/` and
`gallery-routes.json`.

### Result and reproduction

CNA's general XNB-35A external-reference/cubemap fix remains an ancestor of the current
head, with its focused content test. No CNA or sharp-runtime source change was needed for
this requalification. There is no known sample behavior gap or intentional deviation.
The artifact root is `/rv/tmp/samples/SAMPLE-053-CustomModelEffectSample_4_0/`; its
`MANIFEST.md` and `scripts/build-current.sh`, `build-diag.sh`, `compare-frozen.sh`,
`capture-original.sh`, `capture-cna-native.sh` and `capture-web.sh` document the exact
rebuild and replay commands. The current build trees remain reusable; no current-head prune
was authorized or performed.

## Historical completion report

The old placeholder described the sample as blocked by a custom shader and
by its three-stage content-processor chain. Both claims have now been tested against the real XNA
4.0 toolchain rather than worked around. The unchanged upstream processors build successfully,
their official Reach `.xnb` outputs are shipped by the C++ port, and CNA loads and renders the
resulting `Model` with its compiled effect and generated cubemap.

The authoritative snapshot and all reproducible evidence are retained under:

```text
/rv/tmp/samples/SAMPLE-053-CustomModelEffectSample_4_0
```

## Ported behavior

The C++ game is a direct port of `CustomModelEffect.cs`:

- `Content.Load<Model>("saucer")` loads the processor-produced model graph;
- the model rotates from `TotalGameTime` around X and Y with the original factors;
- the camera, field of view and near/far planes are unchanged;
- every mesh effect receives `World`, `View` and `Projection`, then `ModelMesh::Draw()` renders it;
- Escape and gamepad Back exit; and
- the backbuffer is cleared to `Color::CornflowerBlue` and the window title is
  `Custom Model Effect`.

No sample-side shader translation, custom renderer path, loose-model replacement, cubemap
reconstruction, material reassignment or generated C++ geometry exists.

## Original content pipeline

`scripts/build-original.sh` compiles the unchanged upstream
`CustomModelEffectPipeline` assembly and passes it to XNA's `BuildContent`. All three original
processors therefore execute:

1. `EnvironmentMappedModelProcessor` sends the saucer's materials through the custom material
   processor.
2. `EnvironmentMappedMaterialProcessor` assigns the compiled `EnvironmentMap.fx`, preserves the
   diffuse texture and routes the environment-map input to the cubemap processor.
3. `CubemapProcessor` mirrors and folds `seattle.bmp` into six faces, blurs the seams, generates
   mipmaps and compresses the result as DXT1.

The Windows project declares Reach, and its four exact outputs are checked into this sample:

| Asset | Bytes | SHA-256 |
|---|---:|---|
| `EnvironmentMap_0.xnb` | 6,076 | `25827011cb0b13ab50a5520ead8d51c04ff90b6ed1a289d5055cdc77cc0d531a` |
| `saucer.xnb` | 128,056 | `3d4654e351d437257ba9e606bb10477d7874ca58b09722ad500c11f7b24c58ad` |
| `saucer_texture_0.xnb` | 87,627 | `d610632e8c8d94a228e145ea585d7246a26f4db13a1003443bf66e9bd078eac5` |
| `seattle_0.xnb` | 262,621 | `7f4106f124b4f60a1277a780c237d16ad554a0f9c2c9ecfac8f7770753528e35` |

The runner also builds the original HiDef and Xbox variants for pipeline coverage. Reader-table
inspection proves that the runtime path is the intended one: the model uses `ModelReader`,
`EffectMaterialReader`, `DictionaryReader<String,Object>` and `ExternalReferenceReader`, with
five shared resources; its referenced assets use `EffectReader`, `Texture2DReader` and
`TextureCubeReader`. Thus the shipped files are not hand-authored substitutes for the processor
results.

Retaining externally produced `.xnb` files is the campaign's approved XNA-asset path. CNA still
does not offer a general C# `ContentProcessor<TInput,TOutput>` authoring API, but that tool feature
is not a missing runtime behavior in this sample and no one-off replacement processor was added.
DEFERRED item #18 now records that distinction.

## Framework defect fixed

The first faithful run failed while loading `seattle_0.xnb`. CNA's
`ExternalReferenceReader` hard-coded `ReadExternalReference<Texture2D>()`, even though FNA calls
`ReadExternalReference<object>()`; the real cubemap consequently escaped as
`std::bad_any_cast`.

`cnanext` XNB-35A fixes the framework instead of the sample. The type-erased reference path now
dispatches the referenced XNB through its own root reader and preserves its concrete type.
`EffectMaterialReader` applies and retains `Texture2D`, `Texture3D` and `TextureCube` parameter
values. A focused unit test references the same real 64x64 DXT1 cube XNB twice through a
`Dictionary<String,Object>`, checks both concrete values and checks their shared renderer
resource. `CnaTests` builds successfully and all four
`EffectMaterialContentTypeReaderTest.*` cases pass on OPENGLES3.

Framework commit: `cnanext` `6a85149e2` (`fix(SAMPLE-053): preserve external XNB resource types`).

## Native fidelity evidence

The original XNA executable runs under Wine and the port runs on native OPENGLES3. The frozen
diagnostic changes only the time source, pins both engines at 1, 3 and 7 seconds, and captures two
identical frames per leg. The ordinary unmodified captures still produce different hashes across
time, proving rotation remains live.

| Time | Pixels within 8 | within 16 | within 32 | Mean absolute difference | Blur within 8 |
|---:|---:|---:|---:|---:|---:|
| 1 s | 97.913% | 99.070% | 99.611% | 0.592 / 255 | 99.614% |
| 3 s | 99.247% | 99.668% | 99.881% | 0.224 / 255 | 99.954% |
| 7 s | 98.414% | 99.437% | 99.793% | 0.455 / 255 | 99.713% |

Silhouette coverage agrees within 0.003 percentage points at every instant and centroids stay
within 0.56 pixels. The residue is confined to subpixel/rasterization edges of the moving model;
the reflection colors and environment-map structure agree visually and numerically.

Evidence is in `evidence/{xna-original,cna-native-opengles3,frozen}`. Re-run the fixed comparisons
with `scripts/compare-frozen.sh`.

## Browser evidence

The same source and XNB files build for Emscripten WEBGL2 with compiled-effect support enabled.
`scripts/capture-web.sh` drives the real output in Chromium and records:

- an 800x480 WebGL 2 canvas and `CNA: graphics renderer: WEBGL2`;
- the exact original title;
- two different frame hashes, so the model rotates;
- successful Escape shutdown, with the canvas remaining stopped; and
- no runtime exception, rejected promise, HTTP error or fatal console message.

The browser screenshots show the same blue/yellow reflective saucer. Full machine-readable results
are in `evidence/cna-web-webgl2/browser-result.json`.

## Intentional deviations and remaining gaps

There is no intentional behavior deviation from the XNA sample. Normal C++ mappings are used
(`std::optional<Model>` for the not-yet-loaded reference, pointers yielded by CNA collections and
the established property getter convention). The build-time C# processor API itself remains a
general tooling capability CNA does not implement; the sample uses the exact official outputs of
that processor chain, so no sample behavior is omitted and no workaround remains.
