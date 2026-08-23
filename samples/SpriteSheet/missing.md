# SAMPLE-007 audit — SpriteSheetSample_4_0

No known behavioral or visual differences remain after the `SAMPLE-007` audit.

## Reference and source audit

- Authoritative source: the unchanged local XNA 4.0 `SpriteSheetSample_4_0` snapshot retained in
  `/rv/tmp/samples/SAMPLE-007-SpriteSheetSample_4_0/xna4-original`.
- `Program.cs`, `SpriteSheetGame.cs`, the complete `SpriteSheetRuntime/SpriteSheet.cs` runtime
  type, and the custom `SpriteSheetPipeline` importer/processor path were reviewed against the C++
  port.
- The game keeps the original 853x480 Windows configuration, two text labels, rotating cat,
  seven-frame glow animation, checkerboard sampler, full-atlas display, draw order and
  Escape/Back input.
- Removed the port-only runtime atlas builder, loose-image `GetData`/`SetData` path, omitted-font
  behavior and F1 help overlay. The historical `help.png` remains beside `CMakeLists.txt`, outside
  `Content`, and the game does not load it.

The sample-local `ContentReaders.hpp` is the C++ AOT equivalent of XNA's generic
`ReflectiveReader<SpriteSheetRuntime.SpriteSheet>`. It reads the unchanged official object graph;
it does not translate, repack or replace the asset at runtime.

## Exact XNA content

The official XNA 4.0 Content Pipeline was run under the isolated Wine prefix, including the
original `SpriteSheetProcessor` and `SpritePacker`. It packs `cat.tga` and `glow1-7.png` into the
198x264 atlas stored inside `SpriteSheet.xnb`. All three XNBs checked into the sample are
byte-identical to that build:

| Asset | SHA-256 |
|---|---|
| `Checker.xnb` | `a01446d7bc4142ff2c64641e22fe8cfd7ac132fd1415db96a16149a56b136e61` |
| `SpriteSheet.xnb` | `ad590419cda80d37b53404ff8b540d96b51a8987854e2b100598feadbc206a78` |
| `hudFont.xnb` | `533f35303a0e62daa693033b6a9413897012567134d2f4125a22a89d52020d66` |

The unchanged original runtime, pipeline and game projects also compile through the local XNA 4.0
assemblies. The audit compiler embeds the same `Microsoft.Xna.Framework.RuntimeProfile` HiDef
resource that the XNA MSBuild targets normally add, so the resulting executable accepts the HiDef
content rather than silently changing the sample to Reach.

## CNA defects fixed by this audit

XNB collection readers previously classified serialized reference types solely from C++ pointer
shape. XNA serializes `System.String` as a reference type, however CNA maps it to value-shaped
`std::string`. Consequently the generic dictionary reader did not consume each string key's
1-based type-reader index and failed while reading the official sprite-name dictionary.

`cnanext` now treats `System.String` as an XNB reference type in array, list and dictionary readers,
matching FNA's per-element `ReadObject` dispatch. Focused regression tests cover all three
collection shapes. This is a reusable framework fix; no manual byte parsing or sample-side
workaround remains.

The original checkerboard batch passes `null` for `BlendState`, which XNA resolves to
`BlendState.AlphaBlend`. CNA's stateful `SpriteBatch.Begin` overloads previously accepted blend
state only by value, forcing ports to spell out the default. CNA now also exposes nullable overloads
for the five-, six- and seven-parameter XNA forms and applies the original null default. The sample
therefore retains the original `nullptr` call, and focused tests cover every added overload.

## Verification evidence

All generated sources, builds, scripts, logs and captures are under
`/rv/tmp/samples/SAMPLE-007-SpriteSheetSample_4_0`:

- `xna4-original` and `xna4-build`: unchanged source snapshot, official pipeline output and the
  original Windows XNA executable.
- `evidence/xna-original/spritesheet-xna-original.png`: live original run through the dedicated
  Wine/WineD3D reference prefix. Wine's keyboard state did not observe `xdotool`'s synthetic
  Escape, so this gate is visual; the original input branch was source-audited and Escape was
  exercised in both CNA targets.
- `cna-native-opengles3` and
  `evidence/cna-native-opengles3/spritesheet-native.png`: native OPENGLES3 reference build,
  rendered capture and successful Escape exit.
- `cna-web-webgl2` and `evidence/cna-web-webgl2/spritesheet-webgl2.png`: Emscripten WEBGL2 build
  and live system-Google-Chrome capture. HTML, JavaScript, WebAssembly and preload data each loaded
  with HTTP 200; the log contains no application, wasm or WebGL runtime error.

The atlas content and static layout were visually compared across the original, native and browser
captures. Cat rotation and glow selection are time-dependent, so their instantaneous animation
frames are not expected to be pixel-identical across separately started processes.

## Remaining differences

- Normal C++ syntax, ownership and property-call adaptations.
- Explicit registration of the sample-specific AOT reader instead of C# reflection.

There is no unresolved CNA/sharp-runtime implementation gap for this sample.
