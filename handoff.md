# CNA Samples: Sequential Review Handoff

Updated 2026-09-25. This is a starting point for the next AI agent, not a
replacement for [`rules.md`](rules.md), [`plan.md`](plan.md),
[`NEXT.md`](NEXT.md), the selected sample's `missing.md`, or the exact XNA
source. Read those before working on a sample. Earlier versions of this handoff
are historical; their old commit snapshots and “next sample” statements must
not be treated as current. Detailed evidence for prior samples remains in their
`missing.md` files, `plan.md`, `NEXT.md`, and retained artifact roots.

## Where to resume

The next sample for a **short, read-only analysis** is **SAMPLE-043,
`Particles3DSample_4_0`**. Its existing `samples/Particles3D/` port and
`✅` plan row are historical results, not a substitute for the owner's new
sequential review. Inspect its exact twelve-source original, custom vertex and
particle systems, compiled content and old comparison evidence before deciding
what work is needed. Do not rebuild or normalize SAMPLE-043 merely because it
is next. Wait for the owner's decision after the short analysis.

SAMPLE-042 ShatterEffect was the last item handled in this sequential pass.
Its unchanged Windows/Reach XNA game and `ShatterProcessor` build and run;
five pinned official XNBs are bit-identical across the XNA and CNA products.
The processor's unseeded randomness changes `tank.xnb` on every fresh build,
so compare both engines on the same pinned one. Fresh current OPENGLES3
frozen-time frames reach 98.11–99.17% within eight RGB levels against XNA at
0/0.5/1/2 s and at least 99.99% after a 4 px blur. The previous 89–97%
figures are historical. Current Release WEBGL2 work and local-gallery copies
pass real Chrome shatter/reversal/asset/error gates; the gallery is prepared
locally with 41 cards but has not been pushed. Original non-Content media and
licence are restored, with no sample-side workaround or CNA/sharp-runtime
source fix. The artifact root is **not newly pruned**; only the owner can
request that after hands-on review. Its retained native product statically
links CNA, loads adjacent SDL libraries, and passed its own render/input/exit
gate. The dry run proposes 578.5 MB of six ordinary intermediates but leaves
the isolated 530.1 MB CNA source checkout as an unusual top-level item. See
`samples/ShatterEffect/missing.md` and
`/rv/tmp/samples/SAMPLE-042-ShatterEffectSample_4_0/evidence/requal-20260925/`.
During this audit the shared `../cna` checkout was occupied on `street-perf`,
so an isolated clone of `next` was used through `CNA_SAMPLES_CNA_ROOT`; recheck
that checkout before touching it.

SAMPLE-041 LensFlare was the preceding item in this sequential pass. It was
published and owner-authorized for pruning on 2026-09-20. Its port has no
sample-side workaround, but OpenGL ES 3 / WebGL 2 show the terrain lighting
without the original's sun glow or ten lens flares. The owner explicitly
accepted this **sample-specific visual limitation** for publication and
deferred the general CNA occlusion-count fallback. The difference, required
future CNA work and actual browser measurement are recorded in
[`samples/LensFlare/missing.md`](samples/LensFlare/missing.md) and
[`diff.md`](samples/LensFlare/diff.md), its `SAMPLE-041` plan row and
`SAMPLES-DEC-010`. The gallery warning is live at
<https://samples.libcna.com/LensFlare.html>. Do not generalize that exception
to later samples or claim that the flares work. SAMPLE-015 was skipped for now;
skipping a sample does not itself mean an owner-approved permanent cancellation.

Current working repositories (verify branch, status and HEAD before edits):

| Purpose | Checkout | Expected branch |
|---|---|---|
| Samples | `/rv/data/development/github.com/libcna/cna-samples` | `develop` |
| XNA runtime | `/rv/data/development/github.com/libcna/cna` | `next` |
| .NET runtime | `/rv/data/development/github.com/libcna/sharp-runtime` | `next` |
| Gallery | `/rv/data/development/github.com/libcna/samples.libcna.com` | `main` |

The upstream inventory is `/rv/tmp/XNAGameStudio/Samples`; the FNA API and
behavior reference is `/rv/data/library/github.com/FNA-XNA/FNA`. Retained
sample artifacts live in `/rv/tmp/samples/SAMPLE-nnn-UpstreamDirectory/`.
Other work may appear in these shared checkouts: preserve unrelated edits and
stage only explicit task files.

## The owner's two-decision workflow

1. **Analyze the next numbered sample briefly.** Treat a request such as
   “analyze 43” as read-only. Inspect its exact upstream directory, all
   products/projects, existing port, `plan.md` row, `missing.md`, and relevant
   retained evidence. Explain in a short report what the sample is, whether
   the old result appears trustworthy, the likely normalization/repair work,
   any framework or content blockers, and what would be needed to verify it.
   Identify uncertainty honestly. Do not edit code, change status, build,
   publish, commit a port, or move on to the next sample during this phase
   unless the owner explicitly asks for more.
2. **Wait for the owner to choose “do it” or “skip.”** If the owner authorizes
   implementation, perform the full faithful re-audit and normalization below.
   If the owner skips it, record only what the owner's words actually decide:
   a temporary skip is not `⛔` and is not evidence that the sample is impossible.
   Do not implement a large subsystem, invent an alternative product or accept
   a visual/behavioral deviation without an explicit owner choice.
3. **Finish the authorized sample and hand it back for human review.** Report
   exact changes, original/native/browser results, remaining differences,
   commits and artifact locations. Then stop. The owner will inspect it
   manually and may request corrections. Do not take silence or a successful
   automated gate as approval to prune.
4. **Prune only after a separate, explicit owner instruction.** A `✅` row is
   the agent's completion claim, not the owner's confirmation. Run
   `tools/prune-completed-sample.sh SAMPLE-nnn-UpstreamDirectory` as a dry run
   first, check its exact targets and retained products, and pass `--apply`
   only when the owner specifically authorizes pruning that sample. Recheck
   retained products, evidence, manifest and a second dry run afterward.

Do not automatically begin the following sample when a sample is done. The
owner normally requests its short analysis as a separate step.

## Full workflow after an explicit “do this sample” decision

- Follow [`rules.md`](rules.md) and the selected row in [`plan.md`](plan.md)
  in full. Existing `✅` claims, screenshots, generated assets and old build
  outputs are hypotheses to verify, not authority. Keep the complete exact
  upstream snapshot, including documentation and license, in the sample's
  artifact root. Build the unchanged original XNA project and run/capture it
  when possible. Audit every relevant original and C++ source, content
  project, processor, library, conditional branch, input and state transition.
- Make the port complete and faithful. Preserve XNA names, project profile,
  content identifiers, `Content.Load<T>()`, original controls and behavior.
  Prefer exact official XNA 4.0 XNBs. No sample-side renderer bypass, raw
  asset loader, handwritten replacement shader/XML path, invented feature,
  scene omission or other workaround. Fix XNA defects generally in `../cna`
  and .NET defects in `../sharp-runtime`, with focused tests and their repo
  instructions. If a substantial subsystem or scope choice is needed, record
  measured evidence and ask the owner before implementing it.
- Use the active `../cna` and `../sharp-runtime` checkouts. Build native
  Release `OPENGLES3` and non-threaded Release `WEBGL2`, using **at most four
  CPU compile jobs** (`--parallel 4` or fewer). This latest owner instruction
  overrides the older unbounded-build sentence still present in `rules.md`.
  Keep `CCACHE_DIR` on the shared `~/.cache/ccache` installation and
  `CCACHE_BASEDIR=/rv`; never create a separate campaign cache. Test native
  rendering, input and exit; serve the complete web bundle over HTTP and
  drive it in real system Chrome, checking visuals, controls, all assets and
  console/runtime errors. A successful build or HTTP 200 alone is not enough.
- Record evidence and exact commands under the sample artifact root. Update
  its `missing.md`, `plan.md` row and applicable decision/history records;
  use `diff.md` for owner-approved differences or required C++/AOT mechanics.
  Run focused dependency tests and `git diff --check`. Commit completed task
  changes in each affected repository, staging only exact paths and naming
  `SAMPLE-nnn` in related commit messages. **Push only when the owner asks.**
- For a playable web sample, prepare the usual gallery detail/card and a
  verified Release `.html`/`.js`/`.wasm`/`.data` copy as part of the authorized
  sample work, unless the owner narrows scope. Keep page navigation and the
  12-samples-per-page gallery order current. Test the copied bundle, disclose
  any owner-approved limitation visibly, and deploy/push only when authorized.
  A non-game sample or a temporarily skipped sample must not acquire an
  invented gallery game.

## Reporting and safety reminders

Use the original/XNA run to distinguish translation bugs from framework
defects. For Linux original execution, retained scripts normally use the
isolated XNA 4 Wine prefix and WineD3D (`WINEDLLOVERRIDES=d3d9=b`); an
unavailable reference environment is not proof of parity. Keep builds and
captures inside the named `/rv/tmp/samples/SAMPLE-nnn-*` root, not the source
repository. Do not replace canonical retained products with an unverified
work-tree build. Never delete an upstream snapshot, official content,
canonical native/web product, reproduction script or evidence when pruning.

The final report for an implemented sample should distinguish observed facts
from inference and explicitly state: source/content provenance; exact build
configurations and test results; original/native/browser comparisons; any
CNA/sharp-runtime fixes and tests; remaining errors or approved differences;
gallery and push status; and whether pruning is still awaiting the owner's
manual decision.
