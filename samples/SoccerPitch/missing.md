# SAMPLE-073 — SoccerPitchSample_4_0 audit

## Result

The sample is completely re-ported from the original XNA 4.0 source with no sample workaround.
It renders the independently tiled base/detail pitch, alpha-blended or alpha-tested markings,
procedural textured ball and flattened shadow, animated camera, FPS counter and original status
text. The original GamePad Back exit and TouchPanel Released toggle are retained exactly; no
Escape, mouse, F1/help-overlay or diagnostic input was added to the game.

All ten original C# source units are represented, including the previously omitted untextured
`PlanePrimitive` helper and the exact 24-byte and 40-byte custom vertex formats. The old merged
header implementation, shared-UV simplification, fixed aspect-ratio shortcut and manually wired
secondary `ContentManager` are gone. `GraphicsDevice.Viewport.AspectRatio`,
`ContentManager(game.Services)` plus literal `content\\Font`, and the original update/draw order now
run through the normal CNA framework paths.

## Original reference

The complete original tree is retained at:

`/rv/tmp/samples/SAMPLE-073-SoccerPitchSample_4_0/xna4-original/`

`scripts/build-original.sh` builds all ten unchanged sources twice with Microsoft XNA 4.0:

- the shipping Windows Phone/Reach code path as `xna4-build/phone-reach/SoccerPitch.dll`;
- an explicitly labelled Windows/Reach diagnostic executable using the same unchanged sources,
  because the upstream project itself is a phone application library rather than a desktop EXE.

The diagnostic host exited 0 and exposed the expected 480-by-800 `SoccerPitch` window. Retained
captures show the wide and near camera positions, independent grass scales, pitch markings, ball,
shadow, Segoe UI Mono FPS text and `Alpha-Blend` label. This is visual/run evidence for unchanged
source; it is not mislabelled as the original Windows Phone application host.

## Content provenance

The retained XNA pipeline runner builds the original `SoccerPitchContent.contentproj` for the
shipping WindowsPhone/Reach target. The six checked-in runtime files are byte-identical to that
output:

| Asset | SHA-256 |
|---|---|
| `Base.xnb` | `d0f4c0029091af03e1e23232deebba07b3c32d57d8ba58598da35621c0e1cfad` |
| `Detail.xnb` | `f54c0a01c1de362debbdc4ac63c2c638451040ebb56f542fc4a2de72b5ca3755` |
| `Font.xnb` | `531d0dd266891b2f3f533740258e9aa476876a867935a1c68d7455b1ecc74d28` |
| `SoccerBall.xnb` | `47f268dbe9ce4c4cddb8dbf141072d83caff038ad16e58997937fd8e62ce5d16` |
| `Stripe.xnb` | `a87517af6efcd96ccd4df3d8ea9ac29ae8b3304414e089cae0bad34c09b3fdda` |
| `Stripe2.xnb` | `d2ed7a0939a5f054939e46d2c110792304e35bbcd556364585574c9bfc4201fd` |

The loose PNG files, generated font atlas/JSON and replacement DejaVu font were removed. The exact
upstream `SoccerPitchOverview.htm` is restored, and `help.png` remains only as a non-runtime sample
document image at the sample root.

## General CNA fix exposed by this sample

The original 40-byte `VertexPositionNormalDualTexture` assigns different data to
`TextureCoordinate` usages 0 and 1. Live EasyGL's built-in DualTextureEffect shader declared only
one UV attribute and sampled both textures through it, while its vertex mapping also ignored usage
1. That was an upstream renderer defect: the earlier port hid it by replacing the original vertex
format with one shared UV.

CNA commit `89024e0d4` fixes the general contract. EasyGL now maps both texture-coordinate semantics
and the DualTextureEffect vertex/fragment stages carry independent `aUV`/`aUV1` values. The native
regression `EasyGL_DualTextureEffect_IndependentUV` uploads the sample's real 40-byte position,
normal, UV0, UV1 layout. On actual OPENGLES3 it rendered `(0,0,0)` before the fix and `(255,0,0)`
after it; the test does not contain a shared-UV fallback. The related static/dynamic/indexed/user
slot-and-sampler regression was also updated to exercise explicit dual semantics.

Qualification of that framework commit included a full Debug CNA build, 121 related graphics
tests, all 12 EasyGL DualTexture tests and the two focused Release regressions. All passed on a real
OpenGL ES 3.2 context.

## Intentional C++ representation

The two custom vertices remain trivially-copyable POD data with exact field offsets and static
`VertexDeclaration`s. They do not inherit CNA's polymorphic `IVertexType`: a C++ virtual base would
insert a vptr and corrupt the original 24-byte/40-byte GPU stream. Compile-time size, offset and
trivial-copy assertions enforce the XNA layout. This is the necessary C++ representation of the
C# value-type contract, not a renderer or sample workaround.

## Qualification

Reproducible scripts and unpruned evidence are retained under:

`/rv/tmp/samples/SAMPLE-073-SoccerPitchSample_4_0/`

- Debug OPENGLES3: the target builds and runs on Mesa OpenGL ES 3.2, loads all six original XNB
  requests (including case-insensitive `Soccerball`), and captures a complete 480-by-800 frame.
- Release OPENGLES3: a separate clean Release tree builds and runs on OpenGL ES 3.2 and captures
  the same full scene at 480 by 800.
- Browser WEBGL2: a clean Emscripten build runs in real system Chrome with a genuine WebGL 2
  context. The canvas and backing buffer are both 480 by 800. A Chrome DevTools touch start/release
  reaches the original TouchPanel path and visibly changes `Alpha-Blend` to `Alpha-Test`, with the
  corresponding line-rendering change. The probe then completes 600 additional
  `requestAnimationFrame` callbacks. It reports zero page exceptions, zero unhandled promise
  rejections and zero relevant HTTP failures.
- Original diagnostic, native Debug, native Release and browser captures all show the same moving
  camera, independently scaled grass layers, markings, ball/shadow and original text placement.

No CNA, Sharp Runtime or sample blocker remains for SAMPLE-073.
