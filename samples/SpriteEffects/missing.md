# Missing / Differences from XNA 4.0 original

No known behavioral or visual differences remain after the `SAMPLE-006` audit.

## Reference and source audit

- Authoritative source: `SpriteEffectsSample_4_0/SpriteEffects/SpriteEffects.cs` from the local
  XNA Game Studio sample archive, retained in
  `/rv/tmp/samples/SAMPLE-006-SpriteEffectsSample_4_0/xna4-original`.
- The C++ game was reviewed line by line against that source. It keeps the same five effect modes,
  ordering, animation formulae, parameter assignments, secondary texture slots, source/destination
  rectangles, Space/A input transition and Escape/Back exit behavior.
- Removed port-only `ShaderEffect`, hand-translated GLSL/JSON shaders, F1 help overlay and direct
  image substitutes. The port now loads the original compiled XNA assets through `Content.Load`.

## Exact XNA content

The official XNA 4.0 Content Pipeline was run under the isolated Wine prefix. This includes the
sample's own `TexturePlusAlphaProcessor` and `NormalMapProcessor`, so `cat.xnb` contains the original
alpha-combined cat and `cat_normalmap.xnb` is the original signed `NormalizedByte4` normal map.
The four effects are the compiled outputs of the original `.fx` files. All eight XNBs checked into
this sample are byte-identical to that build:

| Asset | SHA-256 |
|---|---|
| `cat.xnb` | `30b52c1062a6a8e6ab07a21f602605bf38ea7a0acc747ca94cfdacade488a78d` |
| `cat_normalmap.xnb` | `6118e4fb01f3478b18d3137fa66cbc3e03f9e828c5c80aad904739fc72e184c3` |
| `desaturate.xnb` | `9e36e284d4fcf6a17a50a529bcec890b31d8ea7ac9344bc59924a12ce61b3345` |
| `disappear.xnb` | `a84ab8bb2a779a3a2fb35bec4a765aacaf7b19566719ea1e3e7047af2fc0dced` |
| `glacier.xnb` | `bebd5698742e168247431feec602cf50ad7b8f4324ed005953a1d2159682be33` |
| `normalmap.xnb` | `7ef231fa2be875bcb1b36b8d74ce3c34f257d7c776c54da4760a1073928bd302` |
| `refraction.xnb` | `70be16823c88842f53a0c3aaef51068a242f509065a625b7fc40110238e70c21` |
| `waterfall.xnb` | `a7767220e03ab1e36fb538a394bbf05aeaef51941f4b8905d6dcfc61ce173f87` |

## CNA defects fixed by this audit

- `Texture2DReader` now preserves `SurfaceFormat.NormalizedByte4` packed signed texels. EasyGL maps
  that format to `GL_RGBA8_SNORM` on the OPENGLES3 and WEBGL2 reference profiles instead of treating
  the normal map as ordinary RGBA color data.
- EasyGL SpriteBatch now follows FNA's compiled-effect order: it first applies the stock XNA
  `SpriteEffect`, allowing the original pixel-only custom effects to inherit its vertex shader and
  projection matrix, then applies the custom pass.
- Unassigned custom-effect samplers are resolved from the owning `GraphicsDevice.Textures` slots;
  SpriteBatch still overwrites slot 0 with the sprite texture. This restores the original overlay,
  displacement and normal-map textures in slot 1 without sample-side binding code.

These are reusable framework fixes. No sample workaround was retained.

## Verification evidence

All generated files, build trees, logs and captures are under
`/rv/tmp/samples/SAMPLE-006-SpriteEffectsSample_4_0`:

- `xna4-build`: official pipeline output and original Windows XNA executable.
- `evidence/xna-original`: five live Wine/WineD3D captures, one for each effect mode, plus build and
  runtime logs.
- `cna-native-opengles3` and `evidence/cna-native-opengles3`: native reference build, clean run,
  all five captures and successful Space/Escape input gate.
- `cna-web-webgl2` and `evidence/cna-web-webgl2`: Emscripten build and all five live system-Chrome
  captures. Chrome reports a WebGL 2.0 / OpenGL ES 3.0 context, the correct format capabilities and
  no application, wasm or WebGL exception.

The effects are time-dependent, so captures from different runs need not be pixel-identical at the
same wall-clock instant. Static modes and common scene geometry were visually checked against the
live original; every animated branch and its input transition was exercised.
