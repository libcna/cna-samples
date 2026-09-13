# SAMPLE-008 audit — ShapeRenderingSample_4_0

The sequential `SAMPLE-008` re-audit completed on 2026-09-13. No known behavioral or visual
difference, sample workaround, CNA defect or sharp-runtime gap remains.

## Reference and source audit

- The authoritative source is `/rv/tmp/XNAGameStudio/Samples/ShapeRenderingSample_4_0`; its exact
  16-file snapshot is retained in
  `/rv/tmp/samples/SAMPLE-008-ShapeRenderingSample_4_0/xna4-original`. `diff -qr` is clean.
- `Program.cs`, `ShapeRenderingSampleGame.cs`, the complete `DebugShapeRenderer.cs`, and the
  Windows, Xbox and Phone project/configuration branches were reviewed line by line against the
  C++ port. Windows HiDef Debug is the visible reference configuration.
- Both unchanged Windows configurations were compiled against the installed Microsoft XNA 4.0
  assemblies. IL inspection finds all seven renderer call sites in Debug and zero in Release,
  exactly as required by `[Conditional("DEBUG")]`.
- The unchanged Debug executable ran under the isolated Wine/WineD3D Xvfb route at 800x480,
  changed across timed frames and exited with status 0 after Escape. The Release executable builds
  and continues running, but this Wine route creates only its 1x1 GDI/IME helpers and never exposes
  the game window. That host limitation is recorded rather than represented as a source failure;
  actual Release behavior is independently exercised by CNA native and Chrome below.
- This sample has no Content project or gameplay asset. `Background.png`, the icon and thumbnail
  are project metadata. The historical port `help.png` remains at the sample root and is not loaded,
  copied, preloaded or displayed.

The port preserves the original namespace and `ShapeRenderingSampleGame`/`DebugShapeRenderer`
split, unused `SpriteBatch`, CornflowerBlue clear, rotating camera, projection values, draw order,
player-one Escape and GamePad Back checks. The only source omission found was the inactive Phone
constructor branch; it now faithfully sets 333333 ticks and fullscreen and was compile-verified
with `WINDOWS_PHONE` defined.

## DebugShapeRenderer fidelity

The translation preserves every renderer overload and its body, including:

- no-lifetime and explicit-lifetime forms for a line, triangle, box, sphere and frustum;
- the one-time initialization guard and exact `InvalidOperationException` message;
- the initial 64-vertex batch, shared eight-corner array and 30-step unit sphere;
- cache selection, active/cached transitions, lifetime expiration and ascending cache sort;
- exact vertex order and the Reach limit of 65,535 line primitives per draw;
- `BasicEffect` vertex-color, texture, diffuse-color, world/view/projection and pass state.

The XNA project defines `DEBUG` only in Debug and marks every public renderer method with
`[Conditional("DEBUG")]`. C++ has no equivalent call-site-eliding attribute, so the collision-free
`SHAPE_RENDERING_SAMPLE_DEBUG` macro guards the original seven call sites. Normal CNA Debug renders
the shapes and normal CNA Release omits them. The build/profile detail for the static gallery is
recorded in [`diff.md`](diff.md).

## No-workaround and dependency result

The fresh scan and manual review found no backend/renderer helper, `RawMesh`, `RawModel`, substitute
`SetData`, loose content sidecar, handwritten shader, special state initialization, omitted branch,
invented input or F1 overlay. The sample uses the public XNA-shaped `BasicEffect`, bounding-volume,
`DrawUserPrimitives`, viewport and input APIs. No CNA, sharp-runtime, meta-gl or EasyGL change, test,
stub or missing dependency was required.

## Fresh build and runtime evidence

All source snapshots, scripts, products, logs and captures live under
`/rv/tmp/samples/SAMPLE-008-ShapeRenderingSample_4_0`. At the owner's request, every CMake build in
this audit used no more than four parallel jobs; both shared ccache variables remained enabled.

- Native OPENGLES3 Debug, Release and a compile-only Phone Debug variant all build. Debug reports
  EasyGL OpenGL ES 3.2/`OPENGLES3`, animates six exact colors and exits 0 through Escape. Release
  remains a pixel-stable CornflowerBlue frame and exits 0. The retained canonical native binary is
  the stripped visible Debug reference.
- Non-threaded WEBGL2 Debug and ordinary Release both build. System Google Chrome loads each
  `.html`, `.js` and `.wasm` with HTTP 200 on an 800x480 WebGL2 canvas, reports GL error 0 and no
  runtime problem. Debug animates; Release is the exact one-color conditional-compilation result.
  Escape and injected standard GamePad Back both stop the loop in each configuration.
- The canonical site bundle is a true CMake Release build with the selected original XNA Debug
  semantic symbol. It is optimized, has zero `debug_info` and pthread/shared-memory markers, has no
  `.data` because there is no content, and passes the same Chrome gate through both exit paths.
- `evidence/visual-parity.tsv` proves that original XNA Debug, native Debug, web Debug and the final
  site Release product are all 800x480 and use exactly
  `#008000 #6495ED #800080 #A52A2A #FF0000 #FFFF00`. Timed camera positions naturally differ.
  Native and web ordinary Release frames are pixel-identical CornflowerBlue.

Canonical products are under `xna4-build/bin/`,
`cna-native-opengles3/samples/ShapeRendering/` and
`cna-web-webgl2/samples/ShapeRendering/`. The exact canonical web hashes were copied to
`samples.libcna.com`, browser-gated again after copying, and committed there as `0a068c8`.
Reproducible `fresh-*` scripts are authoritative; work trees remain available until the owner
separately authorizes pruning.
