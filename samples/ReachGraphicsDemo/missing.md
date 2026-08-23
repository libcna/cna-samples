# SAMPLE-005 audit — ReachGraphicsDemo_4_0

Audited on 2026-08-23 against the exact local XNA 4.0 C# sample. The previous contents of this
file described a workaround-heavy five-scene port and were obsolete. The current port contains
all six demos plus the title screen and consumes the original XNA content-pipeline output.

## Reference and artifacts

All generated files and evidence are outside the repositories under:

```text
/rv/tmp/samples/SAMPLE-005-ReachGraphicsDemo_4_0/
```

- `xna4-original/` is the unchanged upstream snapshot.
- `build-original.sh` and `xna4-build/pipeline-runner/` reproduce the XNA 4.0 build.
- `xna4-build/Content/` contains the 22 XNBs produced by Microsoft's XNA 4.0 pipeline.
- `cna-native-opengles3/` is the native reference-renderer build.
- `cna-web-webgl2/` contains the web build and its `.html`, `.js`, `.wasm`, and `.data` outputs.
- `evidence/xna-original-title.png` records the original title screen.
- `evidence/cna-native/` contains captures of the title, Basic, Dual, Alpha, Skinned,
  Environment Map and Particles screens.
- `evidence/cna-web/*-final.png` contains real-browser captures of the same title and six demos.

SHA-256 comparison confirms every committed XNB is byte-identical to that pipeline output;
`BigFont.xnb` is named `bigfont.xnb` only to preserve the original logical asset name on a
case-sensitive filesystem.

The Windows XNA executable was compiled with the local XNA 4.0 toolchain and ran stably through
the established Wine prefix `/home/robertvokac/.wine-cna-xna40`. A concurrent process owned that
prefix while the per-screen capture pass was attempted, so only the original title capture is
retained; behavioral comparison therefore also relies on the complete line-by-line source audit.

The host does not provide Microsoft Arial. The exact source snapshot remains untouched, while the
build-only copy substitutes Liberation Sans in the two `.spritefont` files. Both the XNA executable
and CNA consume the same resulting XNBs. This is the only known host-reference asset caveat.

## Fidelity result

The active code paths in `Program`, `DemoGame`, `MenuComponent`, `MenuEntry`, `TitleMenu`,
`BasicDemo`, `AlphaDemo`, `DualDemo`, `EnvmapDemo`, `ParticleDemo`, `Tank`, `Sky`, `SkinnedDemo`,
`Keyframe`, `AnimationClip`, `SkinningData`, and `AnimationPlayer` were compared line by line with
the C# originals. The original but disabled `ResolutionMenu` class is retained as well.

The old port's permanent deviations were removed:

- no `RawMesh`, `RawMeshPosTex`, `TankModel`, `GridModel`, JSON model or loose vertex/index sidecar;
- no generated loose PNG/font replacement and no manually assembled cubemap;
- no direct-`SetData` content substitute for model, texture, cube, sky or animation content;
- no custom full-screen quad replacing the original SpriteBatch background;
- no extra F1/help UI, invented input or SkinnedDemo placeholder;
- no culling or Clear-overload workaround.

`Content.Load<Model>`, `Content.Load<TextureCube>`, `Content.Load<Texture2D>` and the original stock
effects now receive the official XNB data. The sample-local `ContentReaders.hpp` is the C++ AOT
equivalent of XNA's generic `ReflectiveReader` for the sample's own `SkinnedModel.*` and `Sky`
types. It reads the unchanged official object graph and is not a rendering or asset bypass.

## CNA defects fixed by this audit

The official assets and browser gate exposed five framework problems; all were fixed in `cnanext`,
not hidden in the sample:

1. CNA had concrete texture readers but had not registered FNA/XNA's inert base `TextureReader`.
   `sky.xnb` declares that reader for its texture member.
2. `ModelReader` rejected every non-null custom Tag. It now deserializes and owns model, mesh and
   mesh-part Tags; `dude.xnb` carries `SkinnedModel.SkinningData` in `Model.Tag`.
3. EasyGL assigned stock-shader inputs by declaration-list position. Official model XNBs order
   TextureCoordinate before Normal, so stock shaders now validate and bind by XNA semantic and
   usage index. Custom-effect declaration-order behavior remains unchanged.
4. EasyGL's semantic remap configured the model VAO and then unbound it before `glDraw*`, leaving
   WebGL2 to draw against VAO 0 with all vertex attributes disabled. The configured VAO now stays
   bound until the caller restores the declaration and finishes the draw.
5. EasyGL called `glDrawElementsBaseVertex`/its instanced counterpart for WebGL2 and for the
   OPENGLES3 identity, even though WebGL has no such entry point and CNA guarantees only the ES 3.0
   API floor. These profiles now reproduce base-vertex addressing by rebasing each enabled
   per-vertex pointer around the draw, preserve integer attributes such as skinned bone indices,
   and leave nonzero-divisor instance attributes unchanged.

Focused regression result in `cnanext`: 51/51 tests pass on an isolated Xvfb display (25
vertex-declaration tests, 24 XNB model/texture/registration tests and two EasyGL profile tests).
No sharp-runtimenext change was required.

## Native OPENGLES3 result

The final native build is configured only with `CNA_GRAPHICS_RENDERER=OPENGLES3`. It loads all 22
official XNBs, runs for 20 seconds without an application exception, and all seven screens were
captured with their real content. In particular, SkinnedDemo renders the animated dude and skydome;
DualDemo uses the original dual-UV model; EnvmapDemo uses the original SpriteBatch background,
model and TextureCube. After the WebGL base-vertex fix changed the OPENGLES3 path to use the same
guaranteed-ES-3.0 fallback, `evidence/cna-native/basic-after-web-fix.png` reconfirmed the official
model under that final code.

`CNA_PLATFORM_RATCHET=OFF` was needed only because the shared build-time audit budget was stale.
This option does not change sample/runtime behavior or select another renderer.

## WEBGL2 result

The final `CNA_GRAPHICS_RENDERER=WEBGL2` build succeeds and produces:

```text
ReachGraphicsDemo_cna_samples.html
ReachGraphicsDemo_cna_samples.js
ReachGraphicsDemo_cna_samples.wasm
ReachGraphicsDemo_cna_samples.data
```

The generated page was served from local HTTP and tested in system Google Chrome 151.0.7922.71
through its DevTools protocol. The title, Basic, Dual Texture, Alpha Test, Skinned, Environment Map
and Particles screens all rendered their real official content. Menu selection and every demo's
Back action worked. Each gate reported a live WebGL2 context (`WebGL 2.0 (OpenGL ES 3.0 Chromium)`),
`getError() == 0`, no wasm exception and no application/GL validation error. Chrome's unrelated
missing `/favicon.ico` response and readback performance warning are browser-shell diagnostics,
not sample failures.

Final captures are `evidence/cna-web/title-final.png`, `basic-final.png`, `dual-final.png`,
`alpha-final.png`, `skinned-final.png`, `environment-final.png` and `particles-final.png`. The
final `.html`, `.js`, `.wasm` and `.data` files remain together in the web build directory above;
diagnostic assertions and renderer tracing were removed before the final rebuild and capture.

## Remaining differences

- Normal C++ syntax, ownership and property-call adaptations.
- Explicit registration of sample-specific AOT readers instead of C# reflection.
- Liberation Sans in the build-only reference copy because Arial is absent on this host.

There is no known sample-side workaround and no unresolved CNA/sharp-runtime implementation gap.
