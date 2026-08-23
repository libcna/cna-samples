# SAMPLE-003 audit — `TexturesAndColorsSample_4_0`

Status: complete. No known sample-side workaround or fidelity omission remains.

## Authoritative reference

- Unmodified XNA 4.0 source: `/rv/tmp/samples/SAMPLE-003-TexturesAndColorsSample_4_0/xna4-original/TexturesAndColors`.
- The original Windows project compiles unchanged to
  `bin/x86/Release/TexturesAndColors-audit.exe` against the installed XNA 4.0 assemblies.
- The official XNA `BuildContent` task, `FbxImporter`, `TextureImporter`, stock processors and
  `EffectImporter` compiled all eight original assets successfully. The committed XNB files are
  byte-for-byte identical to the outputs in `xna4-original/Content-xna-pipeline-all`.
- The original package's `TexturesAndColors.png` supplies a published visual reference. The
  isolated Wine/DXVK runtime loaded the original project and content without a content exception,
  but did not expose a capturable X window within the audit timeout; that host capture limitation
  is retained in `evidence/textures-and-colors-xna-original.log` and is not hidden as a visual run.

## Fidelity result

The C++ port now follows `TexturesAndColors.cs`, `SampleCamera.cs` and `SampleGrid.cs` line by line:

- loads the original Cube, high/low sphere, Cylinder and Cone models in the original order;
- loads the Clouds texture, DebugText font and original compiled `TexturesAndColors.fx` effect;
- exposes all 13 original effect techniques and their exact names;
- binds the original world/view/projection, light, ambient and texture parameters;
- draws the original lime-green 32-unit reference grid and safe-area technique label;
- preserves the original camera, world rotation, Tab/Space, gamepad X/Y and Escape/Back controls;
- uses the original ModelMesh/ModelMeshPart draw path rather than generated primitives.

Removed substitutions were the local `GeometricPrimitive` hierarchy (including Torus in place of
Cone), `BasicEffect`, the inert technique counter and the invented F1/help overlay. No sidecar
geometry, hand-written shader replacement, raw mesh path or other sample workaround remains.

## CNA defects found and fixed

1. `Game` did not register the built-in XNB readers before `LoadContent`. Game construction now
   performs the one-time registration; a regression test clears the registry, constructs a game
   and verifies Model, Effect and SpriteFont readers are present.
2. CNA's public Emscripten link contract used the 64 KiB default Wasm stack. Parsing the official
   compiled effect measured a stack need above 340 KiB and aborted in Chrome. CNA now propagates a
   1 MiB stack to consumers. This is a framework fix, not a sample linker workaround.

No `sharp-runtimenext` change was required.

## Verification evidence

All generated files live below
`/rv/tmp/samples/SAMPLE-003-TexturesAndColorsSample_4_0`:

- `xna4-original/Content-xna-pipeline-all`: successful official XNA content-pipeline outputs;
- `cna-native-opengles3/build`: Release native build, configured only for OPENGLES3;
- `evidence/native-opengles3`: clean renderer log, 13 distinct technique captures, five correct
  model captures and successful Escape shutdown;
- `cna-web-webgl2/build`: Release Emscripten build, configured only for WEBGL2;
- `evidence/web-webgl2`: real Chrome/SwiftShader WebGL 2 run, 13 distinct technique captures, five
  correct model captures, HTTP 200, no application page errors or failed application responses,
  and successful Escape input. Chrome's automatic `/favicon.ico` request is the sole retained 404.

No other native or web renderer was tested or claimed.
