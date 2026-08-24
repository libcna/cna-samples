# SAMPLE-011 audit — SafeAreaSample_4_0

No known behavioral or visual differences remain after the `SAMPLE-011` audit. There is no
unresolved CNA or sharp-runtime implementation gap for this sample.

## Reference and source audit

- The unchanged XNA 4.0 source snapshot is retained under
  `/rv/tmp/samples/SAMPLE-011-SafeAreaSample_4_0/xna4-original/SafeArea`.
- `AlignedSpriteBatch.cs`, `SafeAreaGame.cs`, `SafeAreaOverlay.cs`, the Content project and the
  Windows/Xbox projects were reviewed line by line. The Windows project is the runnable reference.
- The original executable and all three assets were built with the real XNA 4.0 assemblies and
  official Content Pipeline in the isolated Wine prefix. The original `Font.spritefont` uses
  Segoe UI Mono Regular; the XNA-distributed face was installed into the prefix before running the
  pipeline.

The C++ translation preserves the original 1280x720 presentation, tiled background, cat movement,
velocity/friction constants, camera clamp, title-safe corner labels, aligned text calculations,
keyboard/gamepad input and Escape/Back exit behavior.

The old port incorrectly created `SafeAreaOverlay` on every platform and displayed its toggle
prompt. The original creates that component only under `#if XBOX && DEBUG`; the Windows reference
leaves it null, draws no red overlay or prompt, and pressing A has no effect. The port now preserves
that Windows behavior while retaining the faithfully translated component class. Both FNA and CNA
return the complete viewport rectangle from `Viewport.TitleSafeArea` on this desktop path, so no
framework change was required.

Normal C++ ownership, optional value storage, explicit vector expressions, property calls and
`CNAEXT GetTypeName()` are the only representation-level adaptations.

## Exact XNA content

The former loose PNG substitutes and JSON/PNG font sidecar were removed. Every asset now loads
through the original `Content.Load` identifier, and all three checked-in XNBs are byte-identical
copies of the official pipeline output:

| Asset | SHA-256 |
|---|---|
| `Background.xnb` | `7cc1205a5e45b6c2be62a656357d3ffd7e37ef1b11616ccb77d5fab6013ed55b` |
| `Cat.xnb` | `86d239312cb9ff5751c12be424b74b83dae2cc9e1c7eacc30f11e9df72a15fe3` |
| `Font.xnb` | `e467c1d5a5c6e158f70cf9137853c4317b3f835397690b68dacf07d1490a9db3` |

The old port-only F1 branch and help overlay are removed. Historical `help.png` is retained beside
the sample's `CMakeLists.txt`, outside `Content`, and is not loaded, copied or preloaded.

## No-workaround and framework review

The audited sample contains no loose content substitute, generated font sidecar, raw mesh/model
helper, backend call, handwritten shader, invented input, omitted branch or help overlay. It uses
only the XNA-facing CNA API. `SafeAreaOverlay::LoadContent` does create a one-pixel texture and call
`SetData(Color::White)`; this is the exact logic of `SafeAreaOverlay.cs`, not a substitute for
pipeline content, and the Windows build does not instantiate the component.

Current CNA already supplies the required XNB texture/font loading, SpriteBatch behavior,
`Viewport.TitleSafeArea`, drawable components and keyboard/gamepad state. Sharp-runtime supplies
the required .NET-compatible primitives. No sample-specific or framework workaround was added.

## Verification evidence

All source snapshots, generated files, builds, scripts, logs and captures are under
`/rv/tmp/samples/SAMPLE-011-SafeAreaSample_4_0`:

- `xna4-build/bin/SafeArea.exe` is the real XNA 4.0 Windows reference. It runs through the
  dedicated Wine/WineD3D prefix at 1280x720. The baseline and post-A captures are byte-identical,
  holding Right moves the cat and scrolls the camera, and Escape exits.
- `cna-native-opengles3/samples/SafeArea/SafeArea_cna_samples` reports EasyGL OpenGL ES 3.2 and
  `OPENGLES3`. It renders at 1280x720, reproduces the title-safe layout and movement, leaves all
  pixels unchanged after A, and exits on Escape. The static CNA/XNA baselines differ in only 39 of
  921,600 pixels and are visually indistinguishable.
- `cna-web-webgl2/samples/SafeArea/SafeArea_cna_samples.{html,data,js,wasm}` is the complete browser
  bundle. System Google Chrome fetched all four files with HTTP 200, reported WebGL 2.0 and
  `CNA: graphics renderer: WEBGL2`, reproduced movement and produced no application, wasm or WebGL
  runtime error. The baseline 1280x720 canvas is pixel-identical to XNA. After keyboard focus,
  Chrome decorates the outer two-pixel canvas perimeter; the remaining 1276x716 rendered game area
  is unchanged after A.

Reproduction scripts are in `scripts/`, and the corresponding captures and logs are in
`evidence/xna-original`, `evidence/cna-native-opengles3` and `evidence/cna-web-webgl2`.
