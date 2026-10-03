# CNA samples handoff — 106 deferred, 107 complete, 108–113 cancelled, avatar programs under re-analysis

Updated: 2026-10-03. Audience: an AI agent starting with a fresh context.

## Next task and authorization

**SAMPLE-106 is `⏸` deferred by the owner (2026-10-03).** The owner selected three general CNA
fixes for later: (a) Guide without `GamerServicesComponent`, (b) browser modal frames through
Asyncify and (d) the `SavePicture` failure exception. The estimate is 6–10 h, and the complete
list is in [samples/SavingEmbeddedImages/missing.md](samples/SavingEmbeddedImages/missing.md)
→ "Deferred work". Do not start it until the owner asks.

**SAMPLE-107 `TiltPerspective_4_0` is `✅` (2026-10-03).** Native and real-Chrome gates measure
the recalibration from the back wall's position: ±136 px without input, about ±10 px while the
mouse or a touch is held. Native exits with code 0. The gallery entry (detail page, bundle, shot, new
`page-8.html`) was pushed (`c24e74c`), and the artifact root was pruned on the owner's request
(1.3 GB → 167.8 MB, hashes verified). See
[samples/TiltPerspective/missing.md](samples/TiltPerspective/missing.md).

**SAMPLE-108 `WinFormsContentSample_4_0` is `⛔` cancelled by the owner (2026-10-03).**

**SAMPLE-109 `WinFormsGraphicsSample_4_0` is `⛔` cancelled by the owner (2026-10-03).**

**SAMPLE-110 `WP7MusicManagement_4_0` is `⛔` cancelled by the owner (2026-10-03).**

**SAMPLE-111 `XnaGraphicsProfileChecker_4_0` is `⛔` cancelled by the owner (2026-10-03).**

**SAMPLE-112 `AvatarAnimPack_4_0_BIN` is `⛔` cancelled by the owner (2026-10-03).**

**SAMPLE-113 `AvatarAnimPack_4_0_FBX` is `⛔` cancelled by the owner (2026-10-03).**

**Current task: re-examine the cancelled avatar programs SAMPLE-085, 086, 094 and 101** against CNA's
standard avatar API, one at a time, then continue with SAMPLE-114. All four are done (see their `missing.md`): their cancellation reasons no longer hold. The owner decides
all four together (reopen, CNA avatars, input emulation, browser), then SAMPLE-114 follows.

## Mandatory reading

Before any repository work, read these in order:

1. [AGENTS.md](AGENTS.md), then **all of [rules.md](rules.md)**. Rules are binding; this handoff
   is a convenience, not a waiver.
2. [NEXT.md](NEXT.md)'s **Active handoff** section and [plan.md](plan.md), especially SAMPLE-106
   and `SAMPLES-DEC-004`. NEXT contains a large historical ledger; old handoffs are not current
   instructions.
3. [samples/SavingEmbeddedImages/missing.md](samples/SavingEmbeddedImages/missing.md), whose
   2026-10-03 section is the current evidence.
4. Before changing either dependency, its applicable `AGENTS.md` and, for CNA, `CHECKLIST.md`.

Old assertions that an API is missing, a sample is complete or a browser is unavailable must be
rechecked. Preserve useful historical evidence while clearly separating it from fresh results.

## Repository snapshot (2026-10-03, before the commit recording this analysis)

| Repository | Actual branch | Observed HEAD | State |
|---|---|---|---|
| `cna-samples` | `develop` | `e7723c0f893b` | Clean, equal to `origin/develop`. |
| `../cna` | `next` | `fc64a4be3339` | Clean, equal to `origin/next`; the earlier unrelated D3D edits are gone. |
| `../sharp-runtime` | `next` | `db86514c5bb8` | Clean, equal to its upstream; `feature/gamer-services-collections` is no longer checked out. |
| `../samples.libcna.com` | `main` | `8e48825c0033` | Unchanged. |

This analysis adds one local samples commit. **No push was requested.** Recheck with
`git status` and `git log origin/develop..HEAD`. Commits `3fb4799`…`e7723c0` (Gamer Services ports of
SAMPLE-075/087/096 plus an achievements program) arrived from the owner-managed Gamer Services work;
do not reopen them. Separate CNA worktrees under `/rv/data/development/github.com/libcna/cnawork`
and the shared service/server track remain owner-managed.

## SAMPLE-106: current findings

Upstream `/rv/tmp/XNAGameStudio/Samples/SavingEmbeddedImages_4_0/`. Artifact root
`/rv/tmp/samples/SAMPLE-106-SavingEmbeddedImages_4_0/`, new evidence
`evidence/current-head-analysis-20261003/`, helpers `scripts/build-current-head-analysis-20261003.sh`
and `scripts/run-current-head-analysis-20261003.sh`. `cna-native-opengles3-analysis/` (653 MiB)
is a removable intermediate.

The game is a Windows Phone 7 Reach, 480×800, 30 Hz sample. Tapping either image opens
`Guide.BeginShowKeyboardInput` for a file name and then saves it through
`MediaLibrary.SavePicture`: the game-project JPEG stream reopened through `TitleContainer`, or the
content texture through `SaveAsJpeg`. It then calls `BeginShowMessageBox` and **immediately**
`EndShowMessageBox`, and catches only `InvalidOperationException`. Back exits.

- The snapshot is identical to upstream (16 files) and the four official Phone XNBs verify. The
  original still cannot run: no Phone SDK/host/emulator exists, and the VM was not booted.
- CNA now has a real system Guide overlay, and `End*` waits through modal frames (GS-005i). The
  overlay is installed **only** by `GamerServicesDispatcher.Initialize`, which Phone games never
  call. Unchanged, the keyboard prompt is invisible and the blocking message-box wait has no drawer
  (G1). The `RenderPending*EXT` precedent of SAMPLE-065/071 would not help, because modal frames
  skip the game's `Draw`. SAMPLE-061/063 high-score prompts appear affected as well; they were
  reported, not reopened.
- Browser: `runModalFrame` refuses under Emscripten (G2), although sample executables already use
  Asyncify. SDL3 Emscripten has no Pictures folder (M1). CNA's `IOException` escapes the original's
  `InvalidOperationException` catch (M2).
- Fresh native focused gate **123/123**, no skips.

Owner options as presented (decided 2026-10-03: **deferred**, fixes (a), (b), (d) selected for later): (1) ⛔ cancel; (2) authorize (a) Phone-contract Guide presentation without
`GamerServicesComponent`, reconciling SAMPLE-065/071's `RenderPending*EXT` lines, (b) Asyncify
modal frames, (c) a browser media-save contract, (d) the XNA failure exception, then a full port
with native/web gates and the gallery; (3) a native-only or `🟡` narrower scope. Any
implementation follows `rules.md`'s full per-sample workflow. The mouse-to-touch opt-in
(`TouchPanel::setMouseTouchEmulationEnabledEXT(true)`) applies to its Tap input.

## Recently closed work: do not reopen without a request

### SAMPLE-105 — cancelled and pruned

Owner explicitly selected **⛔ cancellation**, then requested pruning. Both original products
(Phone notification receiver and WinForms sender) remain unported.

Root: `/rv/tmp/samples/SAMPLE-105-PushNotificationsSample_4_0/`.
Prune removed four intermediate paths and reduced allocated storage **110.2 → 9.8 MiB**
(115,511,296 → 10,223,616 bytes including receipts). All **75 retained files** match their before
hashes; the manifest was separately updated. Exact originals, both sender/font generations,
Win7 exports, scripts and all existing evidence remain. Repeat prune is empty.

The generic pruner would delete the noncanonical diagnostic tree, including its loose executable.
`cna-native-opengles3-analysis/CnaPhoneTests` was protected during that deletion and restored to
its exact original path. It remains byte-identical at **9,297,568 bytes**, and its loader has no
missing dependency. Do not classify it as a CNA game or MPNS/browser product. Its earlier **9/9**
focused tests established CNA's native loopback backend at CNA `92d23c84d`, not today's complete
Phone/service/shell behavior. Cancellation does not resolve those general framework gaps.

Read [samples/PushNotifications/missing.md](samples/PushNotifications/missing.md) and
artifact `evidence/prune-closure-20260928/`. Restoration commands are in the root manifest;
helpers `build-original-analysis-20260928.py`, `build-content-analysis-20260928.sh` and
`build-phone-analysis-20260928.sh` regenerate the removed intermediates.

### SAMPLE-104 — partial browser release and extra evidence cleanup

Status remains **🟡 partial**. The local profiling utility and original command console work;
authenticated remote networking is unqualified, and browser networking is not implemented.
Original `remote` remains. No fake gamer, account, session or sample-local transport was added.
The complete current WEBGL2 product/gallery entry is published with an explicit limitation.

Root: `/rv/tmp/samples/SAMPLE-104-PerformanceUtility_4_0/`.
After the ordinary prune, the owner approved removal of **eight specific evidence binaries and
objects**, saving 35.74 MiB. Current root allocation is **44,507,136 bytes (42.4 MiB)**.
All **508 retained files** have unchanged hashes; the manifest was separately updated. Current
native/web products, canonical historical products, originals, captures, logs and diagnostic
sources remain. The old `frozen-web` directory is supporting material, no longer a runnable bundle.

Read [samples/PerformanceUtility/missing.md](samples/PerformanceUtility/missing.md),
[diff.md](samples/PerformanceUtility/diff.md) and artifact
`evidence/closure-20260928/owner-evidence-cleanup-20260928.json`. Browser follow-up ownership is
in `../cna/docs/browser-network-readiness.md` and `../cna/plans/plan_gamer_services_server.md`.
Do not extend 104's owner-approved partial release to another sample without a scope decision.

## Continuing campaign rules and owner preferences

- Use exact upstream and real XNA as authority, then local FNA at
  `/rv/data/library/github.com/FNA-XNA/FNA`. Review all products, source units, content processors,
  conditional branches and non-default interactions. Retain complete upstream snapshots.
- No sample workaround for a framework defect. Fix XNA behavior generally in CNA, .NET behavior
  generally in Sharp, and translation/content in samples. No omitted features, invented service,
  raw content substitute, hand-translated shader, source clone left in a final artifact root or
  reduced game labelled complete. Scan results require manual comparison with the original.
- Owner-requested differences belong in **English `diff.md`**, cross-referenced by `missing.md`
  and the plan. Shared emulation stays off by default and is enabled by a minimal CNAEXT call.
  The owner explicitly requires mouse-to-touch on touch-only ports: retain/use
  `TouchPanel::setMouseTouchEmulationEnabledEXT(true)` and test real clicks/gestures in native and
  web. Do not remove it as an unauthorized workaround. For a future implemented 106 this policy
  applies to its Tap input. Accelerometer and keyboard window-orientation emulation are separate
  shared opt-ins; 102 enables `GameWindow::setKeyboardOrientationEmulationEnabledEXT(true)`.
  Do not invent per-game arrow logic or automatically enable both emulators.
- Only EasyGL is in scope: native **OPENGLES3**, browser **WEBGL2**. Release static CNA products
  are the normal deliverables. Web threads are OFF unless original source requires threading;
  SharedArrayBuffer/COOP/COEP cannot be silently assumed on GitHub Pages.
- Export `CCACHE_DIR=~/.cache/ccache` and `CCACHE_BASEDIR=/rv`, and configure both C and C++ ccache
  launchers. `/rv/cnaccache` is a deliberate symlink to the same cache. Never create another cache
  or change the global cache limit. Samples/CNA use all available cores per `rules.md`; reduce
  jobs only for a measured memory problem. **Standalone Sharp builds obey its own two-job policy.**
- Store generated builds, diagnostics, browser profiles and captures only under the selected
  `/rv/tmp/samples/SAMPLE-nnn-*` root. Reuse its scripts/build trees during work; after pruning,
  rebuild through the manifest. Keep original pipeline XNBs and prove provenance/hashes.
  Reader-table inspection, compiling the original custom processors and using the same pinned
  XNB/frozen-time diagnostic input in XNA and CNA are established techniques. Diagnostic patches
  must remain reproduction tools, not sample runtime branches or gallery products.
- For original execution use the established Wine prefix
  `/home/robertvokac/.wine-cna-xna40`, normally `WINEDLLOVERRIDES=d3d9=b`, and owned Xvfb/window
  capture helpers. Plain `wx`/Wine desktop behavior has produced BadWindow/device-name errors;
  these do not establish a game defect. Prefer the sample's verified helper over an old copied
  virtual-desktop command. Win7 with VS2010/XNA4 is available as fallback; the owner repaired
  VirtualBox and reset guest access. If VirtualBox has lock/hypervisor trouble, stop VM work and
  tell the owner, as requested, instead of killing another VM or Android emulator.
- Use real system Google Chrome launched from the terminal over ordinary HTTP for web gates.
  Retained CDP capture helpers are accepted; an old unavailable extension is not a blanket browser
  prohibition. Inspect rendering, content, controls, audio if relevant, runtime errors and clean
  exit. Link success or Node-only tests are insufficient.
- Run changed-component regressions. CNA's private GPU helper is
  `../cna/tools/platform/run_gpu_tests_private.sh --exec <binary> <gtest-filter>` from CNA's cwd.
  Establish current baselines before attributing inherited failures/skips/warnings to a change.
  SAMPLE-104's older receipts include two inherited Sharp module-boundary findings; do not declare
  them newly introduced without comparison. Follow dependency-specific full gates when required.
- When implementation is authorized and browser-playable, update the gallery in
  `../samples.libcna.com`: detail, card, page navigation and complete byte-verified Release bundle.
  Use a screenshot of the **actual running game**, not a menu or unrelated asset. State partial
  or approved limitations visibly. Never forget the gallery while claiming a web sample complete.
- Commit completed tasks with explicit paths and the relevant SAMPLE identifier. Preserve unrelated
  edits and owner branches. Push only when explicitly requested for the current work.
- Prune only the explicitly authorized sample after inspecting a dry run. Use guarded single-root
  `--allow-cancelled`, `--allow-deferred` or `--allow-partial` when applicable. Check products and
  loose diagnostic executables before applying: generic classification can discard an unusual
  diagnostic tree. Keep captures/logs/hashes; compiled diagnostics and old snapshots must not be
  retained thoughtlessly. Any evidence exception needs a concrete owner-approved list, as for 104.
  Verify retained hashes, update the manifest and run a second dry run. Prior deletion approvals
  are specific to their sample and set of files.

## First actions in the next context

1. Read the mandatory documents and inspect repository status/heads, preserving concurrent work.
2. When the owner resumes SAMPLE-106, do its "Deferred work" in `../cna` under CNA's
   `AGENTS.md`/`CHECKLIST.md`, with tests and the listed sample regressions. Port 106 only if the
   owner then decides so.
3. SAMPLE-106's choice is made (deferred). Start its "Deferred work" only on the owner's request.
   SAMPLE-107 is complete, pushed and pruned. Continue with the next row the owner names.
