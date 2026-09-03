# Racing Milestone 7 — complete world and in-race rendering

## Result

Milestone 7 completed on 2026-09-03. The complete race scene now renders through
the original XNA 4 content graph: replay ghost, authored/procedural world, shadows,
city plane, brake tracks, lens flare, post-processing and desktop in-race HUD. No
GLB, loose substitute, hand-translated effect or sample-side framework workaround
is present. Non-race menus and their lifecycle remain Milestone 8 work.

## Frozen revisions

- CNA: `056e57d478f8e6accfa9124337803e735b39f1e4`
- cna-samples pre-HUD revision: `ed16a3818d0b330f50da9c678f7d47f28e0341a9`
- sharp-runtime: `6baf0925e0bf7805a363836deed90543d26f6d28`
- meta-gl: `20c8b2dc5bb80e32706784066db9fd9e15b3f46a`
- EasyGL: `deda7a426c3c166c0e03a4790f1ede610e2e46fb`
- renderer/profile: CNA `OPENGL33`, HiDef, Mesa OpenGL 4.5 core

## Rendering closure

The product draws the generated best replay through the authentic
`LightingShader.xnb` `ShadowCar` technique. It resolves all 53 original landscape
models and 1,252 seeded Beginner objects, including all ten combination files and
source-ordered procedural placement. Authentic `ShadowMap.xnb` and
`PostScreenShadowBlur.xnb` run the caster, receiver and two-pass blur sequence in
real `Rgba64` render targets. The original city-ground geometry, capped/overlap-
filtered brake tracks, seven-texture/17-record lens flare and five-pass
`PostScreenGlow.xnb` composition are active.

The renderer work exposed two general engine defects rather than accepting sample
fallbacks. CNA/meta-gl now provide truthful desktop RGBA16 UNORM render targets,
and CNA executes legacy compiled-Effect pass assignments. Their focused tests and
the authentic Racing effect fixtures passed before the dependent sample work was
committed.

## HUD closure

`UIRenderer`, `Texture`, `TextureFont`, `TextureFontBigNumbers` and the original
color/coordinate helpers load the authentic `Textures/Ingame.xnb` and
`Textures/GameFont.xnb`. The port preserves the original atlas rectangles,
1600x1200 and 1400x1050 mapping, timing/ranking strings, lap/speed/gear digits,
tachometer rotation and checkpoint/lap fade-up duration.

The focused real-GL probe checks more than visible white glyphs: the XNB retains a
known black panel texel with alpha 154, and drawing it over `(200,200,200,255)`
produces `(79,79,79,255)`. This caught an initially reversed alpha/additive batch
begin order in the C++ translation; restoring the exact source order fixed the
panel without changing CNA or the asset. A fixed 146 mph scenario submits 15 atlas
sprites, 13 text entries and 67 glyphs; the final product frame naturally submits
14 sprites because its speed has two digits, plus the same 67 glyphs.

## Qualification

`samples/RacingGameHarness/scripts/qualify-milestone1.sh` passed with at most eight
parallel build jobs:

- physics/player/camera oracle: 753 records, with all non-camera records exact and
  the established camera bound at 16 ULP;
- Track/Replay oracle: 79/79 exact records;
- real OPENGL33 harness: 146/146 in Debug and ASan/UBSan;
- 420-frame product scene: PASS in Debug and ASan/UBSan, including 14 HUD atlas
  sprites and 67 glyphs on the final frame;
- standalone HUD captures: identical Debug/ASan/classified-LSan SHA-256
  `c1859ce2fdeb5fc41f637b9b659197be288f563ce09ae96e2ecb8f318ed58364`;
- complete product captures: identical Debug/ASan SHA-256
  `a38811ade71f9460abd32d1129877b98cc3c94585ecc86ff763d5ddceb2bfa1c`;
- FNA/CNA static scene: RMSE `0.002230`, 99.0503% of channels within 2;
- LeakSanitizer: 100,956 bytes / 449 allocations rooted wholly in external
  `libGLX_mesa` frames;
- `git diff --check`: pass.

Evidence is retained under
`/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/evidence/`, particularly
`cna-opengl33/milestone5/`, `cna-opengl33/milestone7/` and
`fna-postprocess-oracle/`.

## Next milestone

Milestone 8 owns the original splash/menu/options/car and track selection/help/
highscore/race-return stack, supplied XACT banks, settings/highscores/replay
persistence and complete launch-to-menu lifecycle.
