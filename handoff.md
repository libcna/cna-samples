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

Snapshot on 2026-09-12 after SAMPLE-005 was pushed:

| Purpose | Checkout | Branch | Snapshot commit |
|---|---|---|---|
| Sample ports | `/rv/data/development/github.com/libcna/cna-samples` | `develop` | `2287b6d` |
| XNA runtime | `/rv/data/development/github.com/libcna/cna` | `next` | `0a3a14601` |
| .NET runtime | `/rv/data/development/github.com/libcna/sharp-runtime` | `next` | `0c82d9b8` |
| GL abstraction | `/rv/data/development/github.com/libcna/meta-gl` | `develop` | `20c8b2d` |
| GL implementation | `/rv/data/development/github.com/libcna/easy-gl` | `develop` | `deda7a4` |
| Published samples | `/rv/data/development/github.com/libcna/samples.libcna.com` | `main` | `0337f0c` |
| Main project site | `/rv/data/development/github.com/libcna/libcna.com` | `develop` | `833ee91` |

All seven working trees were clean at this snapshot. Recheck status and branch names before editing;
other agents may have advanced them. Preserve unrelated user/agent changes and stage files by exact
path rather than using `git add .` or `git add -A`.

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

Relevant recent commits:

- `cna-samples`: `af4ad35` (SAMPLE-002), `cfbc182` (SAMPLE-003), `2292384`
  (SAMPLE-004), `2287b6d` (SAMPLE-005).
- `cna`: `fcc9320f5` (SAMPLE-003), `8b4e6ec30` and `e3f6ba420` (earlier SAMPLE-005
  framework fixes), `0a3a14601` (SAMPLE-004).
- `samples.libcna.com`: `38a1968` (SAMPLE-002), `85ff2fe` (SAMPLE-003), `0337f0c`
  (SAMPLE-005).

SAMPLE-005's final audit is [`samples/ReachGraphicsDemo/missing.md`](samples/ReachGraphicsDemo/missing.md),
and its C++-only AOT adaptations are recorded in
[`samples/ReachGraphicsDemo/diff.md`](samples/ReachGraphicsDemo/diff.md). Its canonical 70 MB artifact
root is `/rv/tmp/samples/SAMPLE-005-ReachGraphicsDemo_4_0`; it contains one native product, one web
product, the XNA reference build, reproducible scripts, all seven evidence screens for each runtime,
and no `current` aliases or duplicate build products.

## Next action: freshly re-audit SAMPLE-006

Continue with `SAMPLE-006`, upstream `SpriteEffectsSample_4_0`, port
`samples/SpriteEffects`, artifact root
`/rv/tmp/samples/SAMPLE-006-SpriteEffectsSample_4_0`.

The row is currently `✅` and its `missing.md` claims all five effect modes pass, but the current
campaign requires a fresh check. Treat those claims as useful leads, not proof. In particular, the
artifact root's `MANIFEST.md` and `README.md` still contain stale `openeggbert` checkout paths and old
build commands, so repair them if the new evidence confirms completion.

Recommended first steps:

1. Confirm every related repository is clean, then change only the SAMPLE-006 row to `🔎` or `🛠`.
2. Inventory the exact upstream solution, C# source, content project, custom processors and all
   platform/configuration branches. Compare every file line by line with the current port.
3. Rebuild the unchanged XNA program and all eight XNB files. Verify the byte-identity claims,
   including `NormalizedByte4` normal-map content and the four compiled effects.
4. Rebuild only `SpriteEffects_cna_samples` as current Release OPENGLES3 and exercise all five modes,
   Space/A transitions and Escape/Back exit with real input.
5. Rebuild only that target as non-threaded Release WEBGL2, serve it over local HTTP, and exercise
   the same modes in system Chrome. Check WebGL2, `gl.getError()`, page exceptions, console errors,
   asset requests and clean input transitions.
6. Inspect the runtime package for loose image/shader substitutes, stale help overlay loading,
   handwritten effect replacements, renderer helpers and sample-specific texture-slot workarounds.
7. Update `missing.md`, the row in `plan.md`, reproducible artifact scripts/checksums/manifest and,
   only after all gates pass, publish it as the next playable site entry.

Do not reuse or rename an old build directory as the final product. Rebuild into a disposable work
tree, verify it, replace the single canonical retained product, then remove the work tree.

## Per-sample execution checklist

Use the full checklist in `rules.md`; the condensed sequence is:

1. Mark the row active.
2. Preserve an exact upstream snapshot in `/rv/tmp/samples/SAMPLE-nnn-UpstreamDirectory/xna4-original`.
3. Build and run the unchanged original XNA product when it is runnable.
4. Audit source, helpers, libraries, content declarations, processors and inactive branches line by
   line. Preserve original namespaces and runtime type names.
5. Remove sample-side workarounds and loose/generated replacement assets. If an XNA or `System.*`
   behavior is missing, fix CNA or sharp-runtime generally and add focused tests there.
6. Build native with `CNA_GRAPHICS_RENDERER=OPENGLES3`, `CMAKE_BUILD_TYPE=Release` and a
   sample-only target. Drive real input, capture representative states and verify normal exit.
7. Build web with `CNA_GRAPHICS_RENDERER=WEBGL2`, `CMAKE_BUILD_TYPE=Release` and
   `CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF`. Test the actual bundle from HTTP in Chrome.
8. Run focused dependency tests and `git diff --check`; manually review every no-workaround scan hit.
9. Update evidence, checksums, manifest, `missing.md` and `plan.md`. Use `diff.md` for explicit
   language-required adaptations or owner-approved deviations.
10. Prune only reproducible intermediates after the canonical products and evidence are verified.
11. Commit each changed repository separately with the same `SAMPLE-nnn` identifier. Push only when
   the owner explicitly asks.

For campaign builds, use the shared cache exactly as specified in `rules.md`:

```bash
export CCACHE_DIR="$HOME/.cache/ccache"
export CCACHE_BASEDIR=/rv
```

Use `--parallel` or `-j$(nproc)` unless memory pressure requires less. Configure `cna-samples`
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

The current published order is Primitives, Primitives 3D, Textures and Colors, Reach Graphics Demo.
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
