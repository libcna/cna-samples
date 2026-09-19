# CNA Samples Sequential Re-audit Handoff

This document lets a future AI agent resume the owner's current sample-by-sample review without
needing the previous conversation. It is a point-in-time handoff, not a second source of authority.
Read [`rules.md`](rules.md) completely first, then [`plan.md`](plan.md), the selected sample's
`missing.md`, and the instructions in every dependency repository that may be changed. If this file
becomes stale, `rules.md`, `plan.md`, the exact upstream source and observed runtime evidence win.

## Owner's current objective

Review the numbered XNA samples in order, using the numbering and artifact roots under
`/rv/tmp/samples`. For every sample, determine from fresh evidence whether the C++ port is faithful
and workaround-free. A runnable sample normally needs both:

- a native **Release OPENGLES3** build and real input/runtime verification; and
- a static-hostable **Release WEBGL2** build, tested in the system Google Chrome with real input,
  rendering and console/runtime error checks.

Do not accept an old `✅`, audit, workaround, screenshot or build product without checking it against
the current source and dependency chain. Fix framework defects in their owning repository, never in
sample code. Publish each newly requalified playable web sample to `samples.libcna.com`.

The owner clarified an important content rule: CNA's newer XNA Content Pipeline functionality is
available when a sample genuinely needs it, as demonstrated by SAMPLE-004, but it must not be forced
onto ordinary runtime samples. Most samples should continue to consume exact pregenerated XNB files
made by the original Microsoft XNA 4.0 pipeline.

## Workspace and branches

The table is the baseline before the 2026-09-19 SAMPLE-016 requalification.
SAMPLE-014 had already been requalified and its temporary artifacts pruned at
the owner's request. The new SAMPLE-016 outcome is in the review table below:

| Purpose | Checkout | Branch | Snapshot commit |
|---|---|---|---|
| Sample ports | `/rv/data/development/github.com/libcna/cna-samples` | `develop` | `e28acad` (SAMPLE-014 artifact-prune follow-up) |
| XNA runtime | `/rv/data/development/github.com/libcna/cna` | `next` | `40e330fcd` |
| .NET runtime | `/rv/data/development/github.com/libcna/sharp-runtime` | `next` | `cb8fd7f8` (SAMPLE-014 XML serializer fixes) |
| GL abstraction | `/rv/data/development/github.com/libcna/meta-gl` | `develop` | `20c8b2d` |
| GL implementation | `/rv/data/development/github.com/libcna/easy-gl` | `develop` | `deda7a4` |
| Published samples | `/rv/data/development/github.com/libcna/samples.libcna.com` | `main` | `3a940dd` (SAMPLE-014 gallery publication) |
| Main project site | `/rv/data/development/github.com/libcna/libcna.com` | `develop` | `833ee91` |

All related working trees were clean before the SAMPLE-013 work began. Recheck status and branch
names before editing; other agents may have advanced them. Preserve unrelated user/agent changes
and stage files by exact path rather than using `git add .` or `git add -A`.

The authoritative upstream XNA source inventory is `/rv/tmp/XNAGameStudio/Samples`. FNA at
`/rv/data/library/github.com/FNA-XNA/FNA` is the API/behavior reference when the sample and a live
XNA run do not answer a question.

## Sequential review completed so far

| Number | Upstream / port | Result of this sequential pass | Web publication |
|---|---|---|---|
| SAMPLE-001 | `PrimitivesSample_4_0` / `samples/PrimitivesSample` | Requalified; original, Release OPENGLES3 and static Release WEBGL2 pass. | `PrimitivesSample.html` |
| SAMPLE-002 | `Primitives3DSample_4_0` / `samples/Primitives3D` | Requalified; official XNA font restored and native/web passes repeated. | `Primitives3D.html` |
| SAMPLE-003 | `TexturesAndColorsSample_4_0` / `samples/TexturesAndColors` | Requalified; public `IDisposable::Dispose()`, mipmapped XNB and runtime package corrected. | `TexturesAndColors.html` |
| SAMPLE-004 | `StockEffectsSample_4_0` / `samples/StockEffects` | Owner-approved Content Pipeline-only port. The upstream package has no game, so there is intentionally no native game demo or web publication. | None by design |
| SAMPLE-005 | `ReachGraphicsDemo_4_0` / `samples/ReachGraphicsDemo` | Requalified with the title plus all six demos in original XNA, native OPENGLES3 and WEBGL2. | `ReachGraphicsDemo.html` |
| SAMPLE-006 | `SpriteEffectsSample_4_0` / `samples/SpriteEffects` | Requalified with eight byte-identical XNBs and all five modes in original XNA, native OPENGLES3 and WEBGL2; original namespace restored. | `SpriteEffects.html`, pushed at `485c0d7` |
| SAMPLE-007 | `SpriteSheetSample_4_0` / `samples/SpriteSheet` | Requalified from the exact 35-file source through the custom processor and original/native/browser gates; static regions are pixel-identical and no code change was needed. | `SpriteSheet.html`, pushed at `e807c3b` |
| SAMPLE-008 | `ShapeRenderingSample_4_0` / `samples/ShapeRendering` | Requalified from the exact 16-file source through XNA Debug/IL, native Debug/Release/Phone and Chrome Debug/Release/site-Release gates; omitted Phone setup restored. Pushed and owner-authorized work trees pruned. | `ShapeRendering.html`, pushed at `0a068c8` |
| SAMPLE-009 | `InputReporter_4_0` / `samples/InputReporter` | Requalified from the exact 31-file source; original/native captures are pixel-identical and Chrome passes keyboard plus the complete standard-gamepad report. Pushed and owner-authorized work trees pruned. | `InputReporter.html`, pushed at `c2ba204` |
| SAMPLE-010 | `InputSequenceSample_4_0` / `samples/InputSequence` | Requalified from the exact 30-file directory; original/native and all rendered web game pixels are identical over the baseline and all nine moves. The old incomplete snapshot was corrected; no code change or workaround was needed. Pushed and owner-authorized work trees pruned. | `InputSequence.html`, pushed at `9c768a6` |
| SAMPLE-011 | `SafeAreaSample_4_0` / `samples/SafeArea` | Requalified from the corrected exact 16-file root; XNA, Release OPENGLES3 and non-threaded Release WEBGL2 have a pixel-identical 1280x720 baseline and matching input/camera behavior. Current-host Segoe UI Mono rasterization is documented as host-sensitive; no code change or workaround was needed. Pushed and owner-authorized work trees pruned. | `SafeArea.html`, pushed at `8f54f97` |
| SAMPLE-012 | `GeneratedGeometrySample_4_0` / `samples/GeneratedGeometry` | Requalified from the exact 25-file source through both custom processors, exact XNB comparison, original XNA, Release OPENGLES3, Phone compile and non-threaded Release WEBGL2. Public `Sky` fields and documented `CNAEXT` reader registration now match the source/language boundary; no workaround or dependency change remains. Its reproducible work trees were owner-authorized for pruning and pruned on 2026-09-19. | `GeneratedGeometry.html`, local commit `9b28663` |
| SAMPLE-013 | `Platformer_4_0` / `samples/Platformer` | Fresh exact 77-file source audit, XNA rebuild and original/native/web/site runtime gates. C# rounding, enemy division and empty-level failure restored. CNA `e3c14545e` corrects authored Reach NPOT DXT XNB loading with 28/28 focused tests; no runtime sample workaround or sharp-runtime change. | `Platformer.html`, commit `95873e1` |
| SAMPLE-014 | `Spacewar_4_0` / `samples/Spacewar` | Requalified after replacing the handwritten Settings XML layer with SharpRuntime's generic stream `XmlSerializer`, plus CNA Keys metadata, FNA-faithful Reach render targets and a general MojoShader centroid-color fix. Exact original XML round-trips; native OPENGLES3 and system-Chrome WEBGL2 render title, Retro and Evolved with original assets and XACT banks. Owner-authorized temporary artifacts were pruned on 2026-09-19, leaving the canonical products and evidence. | `Spacewar.html`, commit `3a940dd` |
| SAMPLE-016 | `BounceSample_4_0` / `samples/Bounce` | Owner skipped SAMPLE-015 and requested Bounce. Exact WP7-only source was re-audited; the C# double-precision tessellation intermediate was restored. Fresh Release OPENGLES3 rendered, responded to held Up and exited on Escape; non-threaded Release WEBGL2 and the byte-identical gallery copy passed real Chrome error and interaction gates. No new CNA or sharp-runtime change. The original cannot be run here because WP7/XNA targets and the VirtualBox driver are unavailable. The owner authorized pruning of the fresh work trees and temporary Chrome profile, leaving the source, evidence and canonical products. | `Bounce.html`, pushed at `ecb8125` |
| SAMPLE-017 | `CollisionSample_4_0` / `samples/CollisionSample` | Exact 28-file source and unchanged Windows XNA Debug/Reach game rebuilt; original and C++ suites each pass 420,000 checks. C# double-to-float math, camera lerp order, grid division and default zero quaternion restored in the port; no CNA/sharp-runtime fix or workaround. Fresh Release OPENGLES3 and non-threaded WEBGL2 pass all five views, orthographic mode, pause/step/rotation and clean browser error gates. Canonical products retained; all three duplicate/reproducible 2026-09-19 work trees were owner-authorized for pruning and removed. | `CollisionSample.html`, pushed at `ea9f3d7` |
| SAMPLE-018 | `PerPixelCollisionSample_4_0` / `samples/PerPixelCollision` | Exact 16-file source, unchanged XNA game and official XNBs rechecked. Fresh 180-second original/native recordings prove both opaque-pixel collisions and blue rectangle-only near misses; Release OPENGLES3 and non-threaded WEBGL2 pass real input and error gates. The byte-identical gallery bundle passed the same Chrome gate. Existing assembly-title support fixes the historical window-title difference; no new sample/CNA/sharp-runtime code change or workaround. Owner-authorized pruning removed three reproducible work trees; canonical products and evidence remain. | `PerPixelCollision.html`, pushed at `bf5c2e9` |
| SAMPLE-019 | `RectangleCollisionSample_4_0` / `samples/RectangleCollision` | Exact 19-file source, unchanged XNA game and byte-identical official XNBs freshly rebuilt. Original and native 180-second captures show 34/54 red rectangle hits, including 15/26 frames with no estimated opaque-pixel contact; no unambiguous rectangle overlap stays blue. Release OPENGLES3 and non-threaded WEBGL2 plus the exact gallery copy pass input, collision and error gates. Player position and opaque sprite pixels match XNA at start and both clamps. No sample/CNA/sharp-runtime code fix or workaround; the old title discrepancy is resolved by existing assembly metadata. Owner-authorized pruning removed the two CMake work trees and the duplicate XNA work tree; canonical products and evidence remain. | `RectangleCollision.html`, pushed at `c544390` |
| SAMPLE-020 | `TransformedCollisionSample_4_0` / `samples/TransformedCollision` and `samples/TransformedCollisionTest` | Both runnable products freshly requalified from the exact 36-file upstream source, unchanged original XNA builds and six byte-identical official XNBs. Original/native 1800-frame recordings show red transformed-pixel hits and blue rectangle-only near misses. All 17 drag frames plus start/final from the second game are pixel-identical between XNA and native, and Chrome has the same nine red steps. Release OPENGLES3, non-threaded WEBGL2 and both exact gallery copies pass browser/input/error gates. No new source fix or workaround. At the owner's request, all three reproducible work trees were pruned after canonical products and evidence were retained. | `TransformedCollision.html` and `TransformedCollisionTest.html`, pushed at `f8a3848` and served by GitHub Pages |
| SAMPLE-021 | `PathDrawing_4_0` / `samples/PathDrawing` | Exact 18-file Phone source, unchanged original C# and three byte-identical Phone XNBs rebuilt. Corrected `PrimitiveBatch.Dispose` and current CNA pass-pointer syntax in the port, with no framework/runtime change or workaround. Original/native start frames match at 384000/384000 pixels; native mouse-as-touch draws a path, moves the tank and closes cleanly. Release WEBGL2 and its gallery copy each pass Chrome touch/path/error gates (16 touch events; 631.6 px travel). Owner-approved mouse-touch opt-in is documented in `diff.md`; four-job compile cap observed. The owner authorized pruning: both CMake work trees and the byte-checked duplicate XNA work tree were removed, retaining canonical products and evidence. | `PathDrawing.html`, pushed at `d97ef2d`, GitHub Pages built and public files verified byte-identical |

The pre-task heads are in the workspace table. SAMPLE-016 changed only the
`cna-samples` and gallery repositories; CNA and sharp-runtime retain those
heads. The owner requested push after requalification: `0ba6ee6` and
`ecb8125` reached `origin`. The subsequent artifact prune is recorded in its
own SAMPLE-016 follow-up commit.

SAMPLE-013's final audit is
[`samples/Platformer/missing.md`](samples/Platformer/missing.md). Its artifact root is
`/rv/tmp/samples/SAMPLE-013-Platformer_4_0`; the exact upstream snapshot, fresh original XNA,
stripped Release OPENGLES3 product, four-file non-threaded Release WEBGL2 product, exact site-copy
gate, scripts and evidence are current. Its newly recreated work trees remain; SAMPLE-013 pruning
has **not** been authorized. SAMPLE-012's work trees alone were explicitly authorized and pruned,
leaving its source, canonical products, scripts and evidence intact.

## Next ordered item: SAMPLE-022

The owner explicitly chose to skip `SAMPLE-015` for now. `SAMPLE-016` through
`SAMPLE-021` are requalified; the review table above and each sample's
`missing.md` are the current evidence. `SAMPLE-021` has a fresh original,
native and Chrome build, and its exact local gallery copy passes the same
browser gate. Audit `fedee1e` and gallery `d97ef2d` reached origin at the
owner's request; GitHub Pages built the gallery commit and the public bundle
is byte-identical to the Chrome-tested local copy. All three reproducible
SAMPLE-021 work trees were owner-authorized for pruning and removed after the
canonical products and evidence were verified.
The next numbered row for this sequential pass is `SAMPLE-022`.

Do not reuse or rename an old build directory as the final product. Rebuild into a named work tree,
verify it, and replace the single canonical retained product. Leave pruning to the owner; an agent
may only offer or run the dry run unless `--apply` is explicitly authorized.

## Per-sample execution checklist

Use the full checklist in `rules.md`; the condensed sequence is:

1. Mark the row active.
2. Preserve an exact upstream snapshot in `/rv/tmp/samples/SAMPLE-nnn-UpstreamDirectory/xna4-original`.
3. Build and run the unchanged original XNA product when it is runnable.
4. Audit source, helpers, libraries, content declarations, processors and inactive branches line by
   line. Preserve original namespaces and runtime type names.
5. Remove sample-side workarounds and loose/generated replacement assets. If an XNA or `System.*`
   behavior is missing, fix CNA or sharp-runtime generally and add focused tests there.
6. Build native with `CNA_GRAPHICS_RENDERER=OPENGLES3` and a sample-only target. Use Release unless
   the selected original configuration requires another semantic mode; drive real input, capture
   representative states and verify normal exit.
7. Build web with `CNA_GRAPHICS_RENDERER=WEBGL2`, Release code generation and
   `CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF`. Test the actual bundle from HTTP in Chrome.
8. Run focused dependency tests and `git diff --check`; manually review every no-workaround scan hit.
9. Update evidence, checksums, manifest, `missing.md` and `plan.md`. Use `diff.md` for explicit
   language-required adaptations or owner-approved deviations.
10. Offer the owner a prune dry run after the canonical products and evidence are verified; never
    apply it without explicit owner authorization.
11. Commit each changed repository separately with the same `SAMPLE-nnn` identifier. Push only when
   the owner explicitly asks.

For campaign builds, use the shared cache exactly as specified in `rules.md`:

```bash
export CCACHE_DIR="$HOME/.cache/ccache"
export CCACHE_BASEDIR=/rv
```

The owner's latest instruction (2026-09-13) caps compilation at four CPU cores. Use
`--parallel 4` or an equivalent maximum of four until the owner changes it. Configure `cna-samples`
against the current `../cna` and `../sharp-runtime`; do not silently build against another checkout.

## Artifact discipline

The owner explicitly rejected duplicated/stale `current` directories. Each artifact root should
end with only clearly named canonical products, for example:

```text
xna4-original/
xna4-build/
cna-native-opengles3/samples/<Sample>/
cna-web-webgl2/samples/<Sample>/
evidence/xna-original/
evidence/cna-native/
evidence/cna-web/
scripts/
MANIFEST.md
```

Temporary CMake trees should have unambiguous `work-*` names and be deleted only after successful
replacement and verification. Never leave a newer build under `*-current` while an older build
occupies the canonical path. A manifest must describe what actually exists, use current `libcna`
paths, state whether binaries are stripped truthfully, and give commands that recreate deleted
intermediates.

Do not prune broadly or by unresolved glob. Resolve the exact artifact root, inspect it, and remove
only named reproducible directories. Keep original snapshots, canonical products, scripts, logs,
screenshots and checksums.

## Content and pipeline decisions

- Prefer exact Microsoft XNA 4.0 XNB output for ordinary runtime samples.
- Preserve content names and `Content.Load<T>()` calls. Do not use loose PNG/model/font/shader
  sidecars, raw mesh paths or direct `SetData` as replacements for pipeline content.
- A C++ AOT reader registration is acceptable only when it reads the same original serialized
  object graph; mark the call `CNAEXT` and explain it in `diff.md`.
- Keep precompiled Effect XNB input when the original `.fx` requires Microsoft's external compiler.
  Do not hand-translate an effect into a sample-local shader.
- Use CNA's content-pipeline implementation for a design-time/pipeline product when that is the
  actual sample boundary, as with SAMPLE-004. Do not convert every runtime sample to it merely
  because the functionality now exists.
- Never replace an official XNB just because a local font/compiler environment produces a different
  hash. Diagnose and document the environment first. SAMPLE-005 deliberately keeps its original
  font XNBs; its build-only copy substitutes Liberation Sans because Arial is unavailable.

## Publishing to samples.libcna.com

Publish only a verified, non-threaded Release bundle. Copy exactly the `.html`, `.js`, `.wasm` and
`.data` files into `<Sample>/`, then verify the copied hashes against the gated artifact. Required
static-host checks are documented in that repository's `README.md`; at minimum both commands must
report zero:

```bash
grep -ac debug_info <Sample>/<Sample>_cna_samples.wasm
grep -aEc 'PThread|shared:true|emscripten_thread' <Sample>/<Sample>_cna_samples.js
```

For each published sample:

- add a detail page and a gallery card;
- link directly to
  `https://github.com/libcna/cna-samples/tree/develop/samples/<Sample>`;
- make Play open the Emscripten page in a new tab;
- update Previous/Next navigation on both neighboring pages and put the same navigation before
  `About this sample` and at the bottom;
- keep Previous/Next as highlighted centered buttons. Their label and sample-name rows use the same
  visual style, with whitespace between the rows and no arrow or separating line;
- preserve the visible work-in-progress notice and the GitHub progress link on the gallery.

The current local gallery order is Primitives, Primitives 3D, Textures and Colors,
Reach Graphics Demo, Sprite Effects, Sprite Sheet, Shape Rendering, Input Reporter,
Input Sequence, Safe Area, Generated Geometry, Platformer, Spacewar, Bounce, Collision Sample,
Per-Pixel Collision.
SAMPLE-004 is skipped because it intentionally has no web game.

One browser-capture trap from SAMPLE-005: a 480×800 canvas needs a browser viewport taller than the
canvas plus page chrome. A shorter headless viewport produces white lower halves even when the game
is rendering correctly. Use a sufficiently tall viewport and visually inspect every saved image;
never accept a zero-error JSON report without checking the actual frames.

## Completion report expectations

Report changed files and repositories, framework/runtime fixes, added stubs, content provenance,
intentional differences, exact build configurations, tests, original/native/browser results,
remaining errors and artifact cleanup. Distinguish observed facts from inference. A link returning
HTTP 200 is not a browser runtime pass, and a screenshot of the first frame is not evidence that all
sample behavior works.
