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

The official assets exposed three framework problems; all were fixed in `cnanext`, not hidden in
the sample:

1. CNA had concrete texture readers but had not registered FNA/XNA's inert base `TextureReader`.
   `sky.xnb` declares that reader for its texture member.
2. `ModelReader` rejected every non-null custom Tag. It now deserializes and owns model, mesh and
   mesh-part Tags; `dude.xnb` carries `SkinnedModel.SkinningData` in `Model.Tag`.
3. EasyGL assigned stock-shader inputs by declaration-list position. Official model XNBs order
   TextureCoordinate before Normal, so stock shaders now validate and bind by XNA semantic and
   usage index. Custom-effect declaration-order behavior remains unchanged.

Focused regression result in `cnanext`: 49/49 tests pass (25 vertex-declaration tests and 24 XNB
model/texture/registration tests). No sharp-runtimenext change was required.

## Native OPENGLES3 result

The final native build is configured only with `CNA_GRAPHICS_RENDERER=OPENGLES3`. It loads all 22
official XNBs, runs for 20 seconds without an application exception, and all seven screens were
captured with their real content. In particular, SkinnedDemo renders the animated dude and skydome;
DualDemo uses the original dual-UV model; EnvmapDemo uses the original SpriteBatch background,
model and TextureCube.

`CNA_PLATFORM_RATCHET=OFF` was needed only because the shared build-time audit budget was stale.
This option does not change sample/runtime behavior or select another renderer.

## WEBGL2 result — browser run still required

The final `CNA_GRAPHICS_RENDERER=WEBGL2` build succeeds and produces:

```text
ReachGraphicsDemo_cna_samples.html
ReachGraphicsDemo_cna_samples.js
ReachGraphicsDemo_cna_samples.wasm
ReachGraphicsDemo_cna_samples.data
```

The current terminal Codex session exposes no connected controllable browser, so a real browser
load, scene/input smoke test and capture have not yet been claimed. This is the sole remaining
SAMPLE-005 gate. Do not mark the row complete until that generated page has rendered in a real
browser; a successful Emscripten link alone is not sufficient.

## Remaining differences

- Normal C++ syntax, ownership and property-call adaptations.
- Explicit registration of sample-specific AOT readers instead of C# reflection.
- Liberation Sans in the build-only reference copy because Arial is absent on this host.
- Pending real-browser verification described above.

There is no known sample-side workaround and no unresolved CNA/sharp-runtime implementation gap.
