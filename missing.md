# Genuine CNA gaps relevant to the modern Racing Game route

## Scope and baselines

This file intentionally lists **framework gaps**, not the work of porting a game.
It uses:

- modern primary source `/rv/tmp/RacingGame` at
  `d8092633e4e43e014ff168d8e913a9373538b851`;
- `../cna` at `ac3aaaeb2a5ba27dbd9e22e782c7041e6e40947c`;
- `../cnaintegration` at
  `4ac696c748fb18eef7dd06cca82a0486549bcd5d` only as newer evidence.

Recheck every row against the exact post-modularization CNA commit selected for
implementation. “Validation gap” means not proven; it is not yet a defect.

## Changes after the `rds1983/RacingGame` delta audit

| Previous finding | New status |
|---|---|
| CNA needs a route for 57 legacy `.x` models | **Superseded.** The primary source has 57 checked-in GLBs and no `.x`; direct `.x` support is not a Racing requirement |
| A custom Racing model/material package is needed | **Superseded.** GLB + 57 `.material` sidecars + raw assets are the cleaner contract |
| Hierarchy/tangent/material metadata must be reconstructed | **Substantially reduced.** Data is present; CNA must preserve/expose it |
| Authentic generated XACT banks are missing | **Superseded as acquisition risk.** Real runtime-used version-46 banks are present; CNA validation remains |
| No runnable reference exists | **Superseded.** The user confirmed the modern FNA build runs on Linux |
| `Rgba64` may not matter if rendering changes | **Still relevant.** Modern render code remains and requests it |
| Custom Effect support is important | **More important and better evidenced.** Modern FNA/MG code still relies on the full named Effect/technique/pass/parameter model |
| DDS cube issue | **Still relevant.** The normalization cube is still uncompressed RGB24 |

## Confirmed framework gaps

### 1. No complete portable arbitrary custom-Effect runtime/asset path

CNA has Effect-related public types and stock effects, and it has backend-specific
`ShaderEffect` facilities. It does not yet provide the complete reusable route
Racing needs to populate an arbitrary custom Effect with:

- named parameters and texture values;
- named technique/pass collections;
- `CurrentTechnique`;
- ordered `EffectPass::Apply()` behavior;
- immutable program sharing plus material-instance/clone state;
- pass render-state requirements;
- backend program/shader-module binding.

FNA DX9 `.efb`, MonoGame `MGFX`, and general custom-effect XNB payloads do not
provide a portable CNA solution. The framework gap is a **CNA-owned portable Effect
description/runtime**, not a DX9 FX compiler.

Priority: **highest, after modularization**. Prove it with one normal/specular
technique and one multi-pass post effect before bulk Racing shader work.

### 2. Current glTF/Model import does not preserve Racing's required semantics

CNA can parse GLB, buffers and indices. The inspected integration importer does not
yet preserve the complete unskinned scene-node graph and instances, original node
names/matrices/parents, mesh-to-ordered-parts grouping, material-name/index binding,
or DigitalRise-equivalent bounds. It recognizes standard `TANGENT`, while all Racing
GLBs use `_TANGENT`/`_BINORMAL`; the special sky cube uses VEC3 `TEXCOORD_0` where
the current path assumes VEC2.

This is a **bounded general model-module gap**, not a reason to port DigitalRiseModel
or write a new model converter. Required proof assets: car, windmill, one alpha
model, and sky cube.

### 3. `Rgba64` render-target capability reporting and construction disagree

Modern Racing still requests `Rgba64` in its render-to-texture path. In the audited
CNA baseline, `GraphicsAdapter.QueryRenderTargetFormat` may select it while ordinary
texture/render-target validation rejects it. Capability query and construction must
agree.

The resolution may be either real backend support or an honest fallback/refusal.
Android/Web additionally need a tested `Color` fallback and reduced-quality target
policy at the game level. The contradictory framework answer is the CNA defect.

### 4. Loose DDS cube loading does not cover the supplied RGB24 normalization cube

`SkyCubeMap.DDS` is DXT1 and fits the current route. `NormalizeCubeMap.DDS` is an
uncompressed 24-bit RGB cube and remains outside CNA's known loose-cube format path.
Add a generic RGB24 cube decode/upload test and support, or an explicitly approved
framework-level conversion to RGBA8. This is a small CNA content/texture gap.

## Useful general additions, not proven blockers

### 5. A modular raw-asset provider/cache/lifetime boundary

Racing can implement narrow direct readers today, so this is not strictly missing.
After CNA modularization, a general raw-asset boundary would be useful:

- canonical, case-sensitive-safe asset IDs;
- provider/stream abstraction for desktop files, Android packaged assets, and Web
  preload/cache/stream sources;
- typed cache keys including decode options;
- explicit RAII/unload/device-restoration behavior.

Do not copy AssetManagementBase's managed-object dictionary verbatim and do not
build a parallel XNAssets clone. The Racing `.material` schema and track/combi/font
parsers remain game-side.

### 6. Portable Effect shader-module sharing across the GL family

Newer integration evidence exposes `OPENGL33`, `OPENGLES`, and `WEBGL2` through one
GL-family implementation. Racing would benefit from one semantic Effect graph and
a shared GLSL ES 3.00 body adapted to GLSL 3.30 core. Whether the final module API
needs a new framework feature depends on the post-modularization design; do not
freeze an ABI from the pre-modularized checkout.

## Validation gaps — not yet CNA defects

### 7. Authentic Racing XACT banks have not been run through CNA end to end

The modern repository supplies real version-46 `.xgs`, `.xsb`, and `.xwb` files and
uses them at runtime. CNA must be tested against these exact local inputs for:

- parsing and name/index resolution;
- every wave encoding present;
- cue/category/global and per-cue variables;
- RPC/variation/filter behavior used by motor/gear/brakes;
- fire-and-forget and long-lived cue lifetime;
- pause/resume/volume/device teardown.

Repeat first on Linux desktop, then independently on Android and Web. CNA's XACT
demo is currently excluded on Android/Emscripten, and browser audio requires a user
gesture; neither fact alone proves the library cannot work there. Do not file a
framework defect until a focused supplied-bank test fails.

### 8. Racing render-target chains and custom layouts need an integrated proof

Existing focused CNA tests do not prove the complete shadow → scene → glow/blur/menu
→ UI chain with Racing's tangent layout, state transitions, cube textures, resize,
readback, and resource restoration. Build this incrementally in the pinned harness.

### 9. `OPENGL33` needs non-Linux qualification; `OPENGLES`/`WEBGL2` need target qualification

The newer `OPENGL33` profile has extensive real Linux/Mesa coverage, not Windows
driver coverage. `OPENGLES` is the Android route and `WEBGL2` builds through
Emscripten, but neither has a complete Racing-class target proof. These are platform
acceptance gaps, not automatically framework bugs.

### 10. Android sensor/touch and Web touch need game-shaped hardware tests

CNA already has `TouchPanel`, gamepad support, Android Back mapping, and an Android
Accelerometer implementation with landscape-axis handling. Racing must test
simultaneous steer/pedal touches, safe-area transforms, controller switching, sensor
calibration/orientation, background/resume, and missing-sensor fallback. Any failure
must be isolated before deciding whether it belongs to CNA or the game overlay.

### 11. Web persistent storage and audio-unlock behavior need a complete-game proof

CNA has an Emscripten main loop and browser input path. Racing still needs durable
settings/replay/highscore storage and XACT/audio behavior after a browser user
gesture, tab suspend/resume, and reload. A game-specific synchronization mistake or
packaging choice is not a CNA gap; a missing reusable storage/audio primitive may be.

## Port-side work that is not a CNA gap

- translating roughly 23.6k physical lines of C# to C++;
- typed parsing/validation of Racing `.material` JSON;
- binding Racing effect/technique/parameter names;
- track, combi, height, bitmap-font and replay/settings parsers;
- car/windmill behavior and alpha filename convention;
- desktop logical-control mapping;
- Android touch artwork/layout, left-handed/accessibility options and tilt filter;
- Android Gradle/APK packaging and landscape app policy;
- Web content grouping/download/progress/cache UX;
- platform quality presets, downscaled/compressed derivative assets and performance
  tuning;
- asset licensing/provenance decisions;
- pinning/reproducing the FNA oracle's sibling dependencies.

## Verified capabilities that must not be reported as missing

- partial XNB support, including model/texture/cube readers;
- GLB container/accessor/index parsing;
- ordinary TGA/PNG `Texture2D` loading;
- DXT1 cube data route;
- vertex/index buffers, 32-bit indices and custom layouts on the GL path;
- render targets, blend/depth/rasterizer/sampler states and readback in principle;
- public Effect parameter/technique/pass types used by stock effects;
- keyboard, mouse, gamepad and multi-touch input;
- Android Back → Escape mapping;
- Android Accelerometer support (hardware validation still required);
- substantial XACT version-46 parser/playback behavior;
- Emscripten non-blocking Game loop and SDL input bridge.

## Backend findings removed from the primary gap list

Old Bgfx/Vulkan/SDL-renderer-specific custom-effect limitations remain true or need
their own audits, but they do not block the selected first route. The primary
sequence is Linux `OPENGL33`, Windows `OPENGL33`, Android `OPENGLES`, then Web
`WEBGL2`. Adding D3D11, Vulkan, Bgfx, WebGPU, `SDL_RENDERER`, or fixed-function
backends is separate scope.

## Priority after CNA modularization

1. Pin CNA and prove minimal `OPENGL33` lifecycle/render/readback tests.
2. Freeze and implement the CNA-owned portable Effect runtime/description.
3. Add the bounded semantic GLB/model behavior and four-model proof.
4. Fix/test `Rgba64` capability truth and RGB24 DDS cube loading.
5. Run the supplied Racing XACT banks through focused CNA tests.
6. Prove the integrated static scene, then the complete render-target chain.
7. After Linux completion, qualify Windows, Android and Web in that order.
