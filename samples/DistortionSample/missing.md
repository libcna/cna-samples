# DistortionSample — SAMPLE-032 audit record

Upstream: `DistortionSample_4_0`, ported against the unchanged XNA 4.0 sources snapshotted at
`/rv/tmp/samples/SAMPLE-032-DistortionSample_4_0/xna4-original`, per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## Current requalification — 2026-09-20

The physical upstream still matches the retained snapshot exactly. I rebuilt its unmodified
Windows/Reach Release game and content twice with the official XNA 4.0 pipeline, including the
sample's own `DistortionPipeline` assembly; Xbox/HiDef content also builds. Seven of the nine
checked-in Windows XNBs match both fresh builds byte-for-byte. The two effect XNBs have tiny
byte differences **between the two fresh builds themselves**: the compiled `Distort.xnb` changes
by five bytes, and `Distorters.xnb` by twelve; the latter's second build matches the checked-in
file exactly. The checked-in effects are unchanged official pipeline outputs, not translated or
hand-authored shaders. The old nine-of-nine byte-match statement in §3 describes the original
audit build, not the repeated 2026-09-20 builds. Logs and first-build effect outputs are under
`evidence/requal-20260920/`.

Fresh Release `OPENGLES3` and non-threaded `WEBGL2` builds use the active `libcna/cna` and
`libcna/sharp-runtime` checkouts, compiled effects enabled and at most four compiler jobs. No
source or framework change was required: the earlier `NormalizedByte2` texture and
`EffectMaterial` lifetime fixes remain present. The only port additions are the two byte-exact
original non-Content PNGs: `Distortion/Distortion.png` (sample icon) and `distortion.png` (the picture
referenced by `Distortion.htm`). No game workaround, stub, shader rewrite or intentional
behavioral deviation was introduced.

The unchanged XNA game and fresh native port were captured through the same eight-state
sequence at 800×480. Whole-frame comparison, with no pixels excluded, is in
`evidence/requal-20260920/native-comparison.txt`:

| State | Pixels within 8/255 | Pixels over 64/255 |
|---|---:|---:|
| Pull-In, blurred | 383966 / 384000 | 0 |
| Pull-In map | 383984 / 384000 | 16 |
| Pull-In, unblurred | 383915 / 384000 | 0 |
| Heat-Haze, animated | 371772 / 384000 | 2 |
| Heat-Haze map | 383998 / 384000 | 2 |
| Displacement-Mapped | 380854 / 384000 | 131 |
| Displacement-Mapped map | 384000 / 384000 | 0 |
| Return to Pull-In | 383966 / 384000 | 0 |

Heat-Haze depends on elapsed time, so the two processes cannot display its identical phase. The
final Pull-In frame is byte-identical to the initial frame in each engine. The stripped retained
native executable reproduces the work-build capture pixel-for-pixel and exits on Escape.

Real Chrome checks of the fresh work build, retained bundle and byte-identical local gallery
copy pass: all three distorters, B/map views, X/blur toggle, full A cycle, 800×480 WebGL2
canvas, expected title/renderer, successful HTML/JS/WASM/data requests, and no runtime,
promise, relevant HTTP, fatal console or WebGL driver errors. The sole 404 is Chrome's optional
`favicon.ico`; driver performance warnings concern screenshot readback. The local gallery now
contains 31 samples on 12/12/7 pages. This requalification was later pushed and deployed:
gallery commit `c8b9dbf`, successful Pages deployment `35505124897`, public files byte-identical
to the Chrome-tested bundle. The owner-authorized prune removed only the two fresh work build
trees and `xna4-build/{obj,pipeline-runner}/`, saving 240.5 MB; all products and evidence remain.

## Original port and audit record

## 1. What was ported

The whole sample, as `.hpp`/`.cpp` pairs mirroring the original's own layout. There was no port
before this one: the directory held a help page and a `missing.md`.

| Original | Port |
|---|---|
| `Distorter.cs` | `src/Distorter.{hpp,cpp}` |
| `DistortionComponent.cs` | `src/DistortionComponent.{hpp,cpp}` |
| `Game.cs` | `src/DistortionSampleGame.{hpp,cpp}` + `src/Program.cpp` |
| `Properties/AssemblyInfo.cs` | `src/Properties/AssemblyInfo.cpp` |

`DistortionPipeline/` is not ported and does not need to be: it is a **build-time** content
pipeline extension. Its output is in the XNBs, which are built by the real thing (§3).

## 2. What the previous record claimed

It named one blocker: the two custom HLSL effects, "with no CNA/GLSL equivalent yet",
"requires hand-translating HLSL to GLSL and authoring a `.shader.json` descriptor per effect".

**Stale.** CNA loads compiled XNA Effect bytecode through MojoShader on EasyGL. `Distort.fx` and
`Distorters.fx` are the sample's own, compiled by the official pipeline; nothing was rewritten,
and both of `Distort.fx`'s techniques and all four of `Distorters.fx`'s are exercised below.

## 3. Content — and two properties of the original worth knowing

This is the first sample in the campaign with its **own content pipeline extension**. Three of
the four models are built by `DistorterModelProcessor`, which lives in `DistortionPipeline.csproj`
rather than in XNA, and which chains through `DistorterMaterialProcessor` (assigns
`Distorters.fx` and carries the displacement map over) and `DisplacementMapProcessor` (converts a
height map to a normal map and then to a 2D displacement map). `scripts/build-original.sh`
compiles that assembly and hands it to `BuildContent` alongside the stock importers.

Two things about the original surfaced while doing it. Neither is about CNA:

- **`Distorters.fx` does not compile in the Debug configuration.** `EffectProcessor`'s `DebugMode`
  defaults to `Auto`, which skips optimization for a Debug build, and the shader then needs 73
  arithmetic instruction slots against the 64 its own `compile ps_2_0` allows:
  *"Compiled shader code uses too many arithmetic instruction slots (73)."* Release compiles it.
  Release is therefore the audited configuration.
- **`DistorterMaterialProcessor` names its effect relatively** —
  `new ExternalReference<EffectContent>("Distorters.fx")` — which the pipeline resolves against the
  current directory, not the content root. MSBuild runs a content project from the project's own
  directory; the runner script does the same, or the build fails with *"Missing asset"* pointing
  at wherever the process happened to start.

In the original audit build, all **9** XNBs were byte-identical to this sample's own official
pipeline output for the Windows (Reach) target (`cmp`), hashes in
`evidence/content-sha256.txt`. `PrivacyGlass_0.xnb` is the
displacement map the extension produces; it is not listed in the content project.

The Xbox 360 (HiDef) target builds here too and every file differs from its Windows counterpart.

## 4. Two framework defects, both found by this sample

### 4.1 `NormalizedByte2` textures could not be loaded at all

`DisplacementMapProcessor` ends with `ConvertBitmapType(PixelBitmapContent<NormalizedByte2>)`: a
2D displacement map has an X and a Y and nothing else. CNA's `Texture2DReader` refused the format
outright and the sample died on its first frame.

`NormalizedByte2` already existed as a packed vector and `Texture2D::SetData` already had its
overloads, so the two ends were what was missing. The reader now accepts it and — the part that is
not a one-line addition — sizes both its decoded-byte bound and its per-level byte check by the
format's own bytes per texel instead of a fixed four. EasyGL uploads it as `RG8_SNORM` through the
branch that already handled `RGBA8_SNORM`. Pinned by two tests, the first verified to fail without
the fix.

### 4.2 A textured `EffectMaterial` pointed at freed memory

With the format loading, the Displacement-Mapped distorter's map view was a **black frame that
never recovered**. It was not a rendering fault: the process had segfaulted, inside
`__dynamic_cast`, reached from `EasyGL::ResolveSamplerTexture` ←
`EffectMaterial::SetParameterTexture` ← `Effect::SyncCompiledParameters` ← `ModelMesh::Draw`.

`EffectParameter` stores a raw `Texture*`, and `EffectMaterialReader` (added for SAMPLE-028)
pointed those parameters straight into the value table it reads them from — a local that dies when
the reader returns. One branch handed over a `shared_ptr`'s raw pointer and let the last reference
go; the other took the address of a `Texture2D` living inside a `std::any`.

**This was never specific to this sample**: any model whose material carries a texture went
through it. DistortionSample is where it became reproducible, because its `Window` model is the
only one of the three with a `DisplacementMap`. The material now owns the textures its parameters
name.

## 5. Verification

Both builds are EasyGL: native `OPENGLES3` and web `WEBGL2` under Emscripten. Both windows are
titled `Distortion Sample`, from the ported `AssemblyInfo.cs`.

Eight frames, whole-window, 384000 px, nothing excluded (`evidence/comparison.txt`) — the key
sequence walks every distorter, both `Distort.fx` techniques and the distortion-map view of each:

| Frame | within 8 levels | >64 |
|---|---|---|
| Pull-In, warping | **99.7 %** | 7 px |
| Pull-In, map shown | **99.8 %** | 861 px |
| Pull-In, blur disabled | **99.4 %** | 108 px |
| Heat-Haze, warping | 90.4 % | 1818 px |
| Heat-Haze, map shown | **99.9 %** | 503 px |
| Displacement-Mapped, warping | 92.9 % | 1970 px |
| Displacement-Mapped, map shown | **99.8 %** | 585 px |
| back to Pull-In | **99.7 %** | 7 px |

The last row is the same frame as the first and matches it to the pixel, which is what says the
cycle leaves no state behind. Heat-Haze animates with `Time` and is the one technique that cannot
be exact between two runs; it is still 90 % within 8 levels.

The browser gate (`scripts/capture-web.sh`, `scripts/chrome-smoke.mjs`) asserts the postprocess
rather than merely that something rendered, with thresholds taken from the XNA measurement:

| Assertion | XNA | CNA on WEBGL2 |
|---|---|---|
| the scene renders, sky above ground | 78.3 | 79.0 |
| Pull-In's map replaces it with something far darker | 11.3 | 11.4 |
| Displacement-Mapped's map is drawn, not empty | 28.9 | 28.1 |
| cycling all the way round returns to the start | — | exact to 0.0 |
| Heat-Haze still renders a scene | — | 79.5 |

together with an 800×480 WebGL2 canvas, `CNA: graphics renderer: WEBGL2`,
`document.title == "Distortion Sample"`, and no rejections, runtime exceptions, HTTP errors or
fatal console messages. Full record in `evidence/cna-web-webgl2/browser-result.json`.

## 6. C++ mapping notes — not deviations

- `Distorter.Model` and `Distorter.Technique` cross-reference `DistortionComponent`, so the enum
  lives in the component's header and `Distorter.hpp` includes it; the component holds a
  `Distorter*` behind a forward declaration.
- `Distorter.Technique.ToString()` selects the `Distorters.fx` technique by name. C# gets that
  from reflection; the port spells the four names out, next to the four friendly ones the overlay
  already needed.
- `Distorter[3]` is a `std::array` member, so `Initialize()` resets only the index — the elements
  already exist rather than being allocated there.
- XNA's `Model` is a reference type whose field starts null; CNA loads a `Model` by value, so the
  field is a `std::optional`.

## 7. Deviations

None.
