# SAMPLE-011 audit — SafeAreaSample_4_0

The fresh 2026-09-13 audit found no remaining behavioral or visual difference and no unresolved CNA
or sharp-runtime dependency. No sample, CNA or sharp-runtime source change was required.

## Reference and source audit

- The artifact's `xna4-original/` is now an exact copy of all 16 files in the complete upstream
  `SafeAreaSample_4_0` directory. The old retained snapshot contained the `SafeArea/` subtree but
  omitted both solutions, the HTML documentation and the root Microsoft Permissive License; those
  four files were restored before the fresh gate.
- `AlignedSpriteBatch.cs`, `SafeAreaGame.cs`, `SafeAreaOverlay.cs`, the content project and both
  Windows/Xbox project variants were reviewed line by line. An automated source gate separately
  checks the full inventory, identifiers, constants, flags, branches and runtime type names.
- The C++ translation preserves the original 1280x720 presentation, tiled background, cat movement,
  velocity/friction constants, 70% camera clamp, title-safe corner labels, all 15 alignment values,
  keyboard/gamepad input and Escape/Back exit behavior.
- The original creates `SafeAreaOverlay` only under `#if XBOX && DEBUG`. The equivalent C++ branch is
  `XBOX && !NDEBUG`; Windows, Linux and browser products leave it null, show no red overlay or prompt,
  and treat A as inert. A separate Debug OPENGLES3 compile with `XBOX` defined proves the translated
  component construction, `Game.Components.Add`, one-pixel texture and toggle path still compile.

Normal C++ ownership, optional value storage, explicit vector expressions, property calls and the
required `CNAEXT GetTypeName()` overrides are the only representation-level adaptations.

## Exact XNA content

The sample consumes only the three retained XNB files through the original `Content.Load` names.
There are no loose runtime texture or font substitutes:

| Asset | Retained SHA-256 | Fresh XNA pipeline result |
|---|---|---|
| `Background.xnb` | `7cc1205a5e45b6c2be62a656357d3ffd7e37ef1b11616ccb77d5fab6013ed55b` | byte-identical |
| `Cat.xnb` | `86d239312cb9ff5751c12be424b74b83dae2cc9e1c7eacc30f11e9df72a15fe3` | byte-identical |
| `Font.xnb` | `e467c1d5a5c6e158f70cf9137853c4317b3f835397690b68dacf07d1490a9db3` | host-sensitive; fresh hash `62e04ac27f0a10f46424bdae3c53d9371e164e20480aa77f0ee3e88796ac2d59` |

The official XNA 4.0 `BuildContent` task successfully rebuilt all three assets after installing the
sample's XNA-distributed Segoe UI Mono face in the isolated Wine prefix. The textures are stable;
the font atlas rasterized differently on the current host. The newly generated file and hashes are
retained as evidence, while both reference and CNA runtime products use the previously verified
official XNB so the comparison is against one content set. This is host-dependent font rasterization,
not a sample-side fallback.

Historical `help.png` remains beside `CMakeLists.txt`, outside `Content`; it is not referenced,
copied, packaged or loaded.

## No-workaround and dependency review

The audited sample has no raw mesh/model helper, backend call, handwritten shader, `NOXNA` path,
invented input, loose content sidecar, omitted branch or help overlay. Its sole `SetData` call is the
original `SafeAreaOverlay.cs` logic that creates a 1x1 white diagnostic texture, and current reference
targets do not instantiate that Xbox Debug-only component.

CNA already provides the required XNB texture/font loading, SpriteBatch behavior,
`Viewport.TitleSafeArea`, drawable components and input state. Sharp-runtime already provides the
required .NET-compatible surface. No workaround, stub, API extension or intentional behavioral
deviation was added.

## Fresh verification evidence

All new work trees, scripts, logs, hashes and captures are under
`/rv/tmp/samples/SAMPLE-011-SafeAreaSample_4_0`:

- The unchanged XNA sources compile in Debug and Release. The Debug reference runs through the
  isolated Wine/WineD3D environment at 1280x720; baseline and post-A frames are pixel-identical,
  Right scrolls the camera, and Escape exits with code 0.
- A fresh native Release build targets only `SafeArea_cna_samples` with OPENGLES3 and at most four
  compile jobs. It reports OpenGL ES 3.2 and `OPENGLES3`, runs at 1280x720, keeps A inert, scrolls on
  Right and exits with code 0. Its baseline is pixel-identical to the XNA reference (AE = 0).
- The fresh WEBGL2 build is Release and non-threaded. Its 7,534,704-byte WASM has no `debug_info`,
  and its JavaScript has no pthread/shared-memory runtime marker. System Chrome fetched all four
  bundle files with HTTP 200, created a 1280x720 WebGL 2 canvas, returned `glError = 0`, reproduced
  camera movement and reported no application, wasm or WebGL error. The baseline is pixel-identical
  to both desktop products (AE = 0).
- Keyboard focus decorates only Chrome's outer two-pixel canvas perimeter. The inner 1276x716 game
  image remains pixel-identical after A; the gate records both full-image and inner-region hashes.
- The four canonical WEBGL2 files were copied byte-for-byte to `samples.libcna.com`, and that exact
  gallery copy passed the same independent Chrome gate.

Canonical runnable products remain in `xna4-build/bin`, `cna-native-opengles3/samples/SafeArea` and
`cna-web-webgl2/samples/SafeArea`. `evidence/fresh-*` and `scripts/fresh-*` reproduce the audit.
