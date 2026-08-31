# SAMPLE-074 — Tank on a Heightmap audit

**Status: complete — no known behavior or content differences from the XNA 4.0 original.**

The historical placeholder diagnosis was stale. Live CNA's authentic XNB `ModelReader` preserves
the complete bone hierarchy, mesh-parent relationships and `Model.Tag`, so the sample no longer
needs a raw-model substitute, sidecar height data or a framework workaround.

## Original surface audited

The runtime translation covers every original unit:

- `TankOnAHeightmap.cs`
- `Tank.cs`
- `HeightMapInfo.cs`, including `HeightMapInfoReader`
- `Properties/AssemblyInfo.cs`

The unchanged original custom pipeline was also audited and used to build the retained content:

- `TerrainProcessor.cs`
- `HeightMapInfoContent.cs`, including `HeightMapInfoWriter`
- pipeline `Properties/AssemblyInfo.cs`
- `TankOnAHeightmapContent.contentproj`

The C++ port preserves the generated terrain model and custom tag payload, bilinear height and
normal sampling, tank surface alignment, four independently animated wheel bones, keyboard and
gamepad controls, chase-camera terrain clamp, default lighting, per-pixel lighting, fog, draw
order and original window title.

## Authentic content

The checked-in files are the exact Windows/HiDef outputs of the unchanged XNA 4.0 content project
and its original custom processor assembly:

| File | Bytes | SHA-256 |
|---|---:|---|
| `engine_diff_tex_0.xnb` | 2,796,447 | `25d4f2184504e2895f43e95e8463108dd84b68ea73c729d6df3f6c3512493380` |
| `rocks_0.xnb` | 174,999 | `ee9125f5e4cd026de9811e67fbb19045fb5825af029a820257c9a4e06debd39b` |
| `tank.xnb` | 840,175 | `c37937290aaec820de8e55c9f531f241a8c08fefde6f7609e6a4ba2220ccec71` |
| `terrain.xnb` | 4,744,426 | `096c3bae00357e27b339f1b5e894a9e409887a12d24ce2044c41602245f5f6da` |
| `turret_alt_diff_tex_0.xnb` | 2,796,447 | `ec73d84bfeaa94fd466b9ab72662b70b943c5593bd96310965b566e46f7b17a0` |

`terrain.xnb` carries the real generated mesh, texture reference and the writer's serialized
terrain scale, dimensions, height grid and normal grid. `tank.xnb` carries the real model bone
hierarchy and mesh-parent assignments. There are no loose source images, raw meshes, JSON
sidecars or runtime-reconstructed substitutes in `Content/`.

## C++ mechanics

CNA deliberately has no assembly reflection. The game therefore registers the faithfully ported
`HeightMapInfoReader` under the exact reader name stored in the authentic XNB. The reader returns
the `shared_ptr<System::Object>` representation required by `Model.Tag`; the game performs the
same checked cast as the original `as HeightMapInfo` path. This is the only intentional language
mechanic and does not alter the payload or behavior. See `diff.md`.

## Qualification

- The unchanged original Windows/HiDef XNA 4.0 game built and ran under Wine/DXVK. It opened the
  800x480 `Heightmap Collision With Normals` window, rendered the textured terrain and tank,
  responded to forward and turn-plus-forward input, and exited with code 0.
- Debug OPENGLES3 built from a clean configuration, loaded all five assets through public
  `Content.Load` paths, exercised the same start/forward/turn scenario on a real Mesa GLES 3.2
  context, and exited with code 0.
- Release OPENGLES3 built from a separate clean Release configuration and passed the same real-GL
  interaction and clean-exit scenario.
- At the deterministic start frame, XNA versus both native builds has normalized ImageMagick RMSE
  `0.00314003`, with only 194 of 384,000 pixels outside 3% tolerance. XNA versus WEBGL2 has RMSE
  `0.00793987`, with 1,454 pixels outside 3% tolerance. Debug and Release native captures are
  byte-for-byte equivalent after decoding.
- WEBGL2 built from a clean Emscripten configuration. Real Chromium obtained WebGL 2, rendered at
  800x480, observed distinct frame hashes after forward and turn input, completed 600 additional
  `requestAnimationFrame` callbacks, and reported no runtime exception, promise rejection, fatal
  console message or relevant HTTP error.
- The runtime and pipeline algorithms were compared line by line against all original C# units.
  No CNA or Sharp Runtime change was required.

## Retained evidence

Artifact root:
`/rv/tmp/samples/SAMPLE-074-TankOnAHeightMapSample_4_0/`

- Exact source and original build: `xna4-original/`, `xna4-build/windows-hidef/`
- Original run: `evidence/xna4-original-windows-hidef/`
- Debug native run: `evidence/cna-native-opengles3-debug-qualified/`
- Release native run: `evidence/cna-native-opengles3-release-qualified/`
- Browser run: `evidence/cna-web-webgl2-qualified/`
- Reproducible build and capture drivers: `scripts/`
