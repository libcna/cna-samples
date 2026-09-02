# Genuine CNA gaps relevant to the modern Racing Game route

## Scope and baselines

This file intentionally lists **framework gaps**, not the work of porting a game.
It uses:

- modern primary source `/rv/tmp/RacingGame` at
  `d8092633e4e43e014ff168d8e913a9373538b851`;
- `../cnanext` at `51d61ef42d1105d97387feeba11eae91a2f3e2e9`;
- the frozen Milestone 0/1 evidence in `racing_baseline.md` and
  `racing_milestone1.md`.

Recheck every row when the pinned CNA revision changes. “Validation gap” means not
proven; it is not yet a defect.

## Changes after the `rds1983/RacingGame` delta audit

| Previous finding | New status |
|---|---|
| CNA needs a route for 57 legacy `.x` models | **Superseded.** The primary source has 57 checked-in GLBs and no `.x`; direct `.x` support is not a Racing requirement |
| A custom Racing model/material package is needed | **Superseded.** GLB + 57 `.material` sidecars + raw assets are the cleaner contract |
| Hierarchy/tangent/material metadata must be reconstructed | **Substantially reduced.** Data is present; CNA must preserve/expose it |
| Authentic generated XACT banks are missing | **Superseded as acquisition risk.** Real runtime-used version-46 banks are present; CNA validation remains |
| No runnable reference exists | **Superseded.** The user confirmed the modern FNA build runs on Linux |
| `Rgba64` may not matter if rendering changes | **Resolved as a capability contract.** Live OPENGL33 reports it unsupported and construction refuses it consistently; Racing needs an explicit `Color` fallback |
| Custom Effect support is important | **Resolved for the reference route.** Live CNA loads all ten exact FNA `.efb` files and applies every reflected pass after `FX-128`; pixel/state fidelity remains an integration gate |
| DDS cube issue | **Still relevant.** The normalization cube is still uncompressed RGB24 |

## Confirmed framework gaps

### 1. Racing's required glTF/Model semantics need a four-asset proof

CNA has a mature GLB/CNB/Model route, and 56 of the 57 supplied GLBs pass its live
converter. It is not yet proven that the current public result preserves every
Racing requirement: unskinned scene nodes/instances, original names and parent
matrices, ordered parts, material identity, DigitalRise-equivalent bounds, and the
authored `_TANGENT`/`_BINORMAL` attributes. The remaining `Cube.glb` is malformed:
its unused VEC3 `TEXCOORD_0` accessor exceeds its buffer view.

This is a **bounded proof with possible general model-module fixes**, not a reason to
port DigitalRiseModel or weaken generic accessor validation. Required proof assets:
car, windmill, one alpha model, and a deterministically normalized sky cube.

### 2. Loose DDS cube loading does not cover the supplied RGB24 normalization cube

`SkyCubeMap.DDS` is DXT1 and fits the current route. `NormalizeCubeMap.DDS` is an
uncompressed 24-bit RGB cube and remains outside CNA's known loose-cube format path.
Add a generic RGB24 cube decode/upload test and support, or an explicitly approved
framework-level conversion to RGBA8. This is a small CNA content/texture gap.

## Useful general additions, not proven blockers

### 3. A modular raw-asset provider/cache/lifetime boundary

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

## Validation gaps — not yet CNA defects

### 4. Authentic Racing XACT behavior has not been qualified end to end

The modern repository supplies real version-46 `.xgs`, `.xsb`, and `.xwb` files and
uses them at runtime. Milestone 0 loaded all three, resolved all 27 cue names,
submitted representative playback through SDL dummy audio, and round-tripped a
long-lived cue variable. Remaining work against these exact inputs is:

- every wave encoding present;
- cue/category/global and per-cue variables;
- RPC/variation/filter behavior used by motor/gear/brakes;
- fire-and-forget and long-lived cue lifetime;
- pause/resume/volume/device teardown.

Repeat first on Linux desktop, then independently on Android and Web. CNA's XACT
demo is currently excluded on Android/Emscripten, and browser audio requires a user
gesture; neither fact alone proves the library cannot work there. Do not file a
framework defect until a focused supplied-bank test fails.

### 5. Racing render-target chains need an integrated proof

Milestone 1 proved one render-target bind/clear/unbind/readback cycle, all six Color
cube faces, exact 44-byte tangent-layout upload/readback and GPU consumption,
resize/fullscreen, capture and three presents. It does not yet prove the complete
shadow → scene → glow/blur/menu → UI chain, authored cube formats/mips, pass-state
transitions or resource restoration across that chain. Build those incrementally.

### 6. `OPENGL33` needs non-Linux qualification; `OPENGLES`/`WEBGL2` need target qualification

The newer `OPENGL33` profile has extensive real Linux/Mesa coverage, not Windows
driver coverage. `OPENGLES` is the Android route and `WEBGL2` builds through
Emscripten, but neither has a complete Racing-class target proof. These are platform
acceptance gaps, not automatically framework bugs.

### 7. Android sensor/touch and Web touch need game-shaped hardware tests

CNA already has `TouchPanel`, gamepad support, Android Back mapping, and an Android
Accelerometer implementation with landscape-axis handling. Racing must test
simultaneous steer/pedal touches, safe-area transforms, controller switching, sensor
calibration/orientation, background/resume, and missing-sensor fallback. Any failure
must be isolated before deciding whether it belongs to CNA or the game overlay.

### 8. Web persistent storage and audio-unlock behavior need a complete-game proof

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
- exact FNA compiled Effects with named parameter/technique/pass collections on
  OPENGL33 (all ten Racing files and every reflected pass);
- truthful OPENGL33 `Rgba64` capability refusal;
- `Game` lifecycle, resize/fullscreen, device events, readback and repeated present;
- `RenderTarget2D`, six-face Color `TextureCube`, and the exact Racing 44-byte
  tangent vertex layout on the GPU;
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

## Current priority

1. Complete the bounded four-model GLB/material proof.
2. Fix/test generic RGB24 loose DDS cube loading.
3. Prove representative compiled-Effect pixels/state in the content scene.
4. Prove the integrated static scene, then the complete render-target chain.
5. Finish supplied-bank XACT behavior and audible-output qualification.
6. After Linux completion, qualify Windows, Android and Web in that order.
