# CNA samples handoff — resume with SAMPLE-106 analysis

Updated: 2026-09-28. Audience: an AI agent starting with a fresh context.

## Next task and authorization

**Resume with analysis of SAMPLE-106, `SavingEmbeddedImages_4_0`.** The owner explicitly selected
this as the next task when requesting this handoff. This session prepared the handoff; it did
not perform a new SAMPLE-106 audit or authorize implementation, modernization or cancellation.

Start with a short analysis against the current source and dependency heads. Explain what the
sample demonstrates, its controls, trustworthy existing evidence, current CNA/Sharp gaps and
realistic implementation choices. Record uncertainty. Do not turn the historical `🛑` row into
cancellation, silently narrow the product or implement a large new subsystem. The owner decides
what to do after the analysis. Do not automatically advance to 107.

## Mandatory reading

Before any repository work, read these in order:

1. [AGENTS.md](AGENTS.md), then **all of [rules.md](rules.md)**. Rules are binding; this handoff
   is a convenience, not a waiver.
2. [NEXT.md](NEXT.md)'s **Active handoff** section and [plan.md](plan.md), especially SAMPLE-106
   and `SAMPLES-DEC-004`. NEXT contains a large historical ledger; old handoffs are not current
   instructions.
3. [samples/SavingEmbeddedImages/missing.md](samples/SavingEmbeddedImages/missing.md).
4. The exact upstream directory, every relevant project/source/content file and retained evidence.
5. Before changing either dependency, its applicable `AGENTS.md` and, for CNA, `CHECKLIST.md`.

Old assertions that an API is missing, a sample is complete or a browser is unavailable must be
rechecked. Preserve useful historical evidence while clearly separating it from fresh results.
The previous handoff's resume-at-44, retired branches, gallery counts and four-job build ceiling
are obsolete. Its history remains in Git and the affected samples' audit records.

## Repository snapshot and concurrent work

Workspace: `/rv/data/development/github.com/libcna/cna-samples`.
These heads were observed **before the commit updating this handoff**; verify them on resuming.

| Repository | Actual branch | Observed HEAD | State |
|---|---|---|---|
| `cna-samples` | `develop` | `bb7c3ffec849be8686859bfea5b6b1edc8758139` | Clean before handoff edits; two local commits ahead of `origin/develop`. |
| `../cna` | `next` | `b1ea16aeb4fd10d436fc07ce482fbc8be6d483fe` | HEAD matches `origin/next`; unrelated D3D edits are present. |
| `../sharp-runtime` | `feature/gamer-services-collections` | `007280bd1cc789f851f7f454a5041c8ce2479e13` | Clean and matches its upstream; preserve this owner-managed branch. |
| `../samples.libcna.com` | `main` | `8e48825c003368dbc24bebd8f024d392379700d6` | Clean and matches upstream; previously deployed and byte-verified. |

Samples' observed remote HEAD is `3f66955ef1b481fe14abf527548593ece48943a6` (105 cancellation).
The pending local commits are `c0df1f0` (104 evidence cleanup) and `bb7c3ff` (105 prune).
This handoff adds another local commit. **No push was requested for these tasks.** Do not infer
push authorization from earlier requests that already completed. Recheck with `git status` and
`git log origin/develop..HEAD` rather than treating this snapshot as permanently current.

CNA's unrelated changes at inspection were:

```text
 M modules/renderers/common/d3d/src/D3DProgramReflection.cpp
 M modules/renderers/directx11/include/CNA/Internal/Renderers/DirectX11/DirectX11Renderer.hpp
 M modules/renderers/directx11/src/D3D11EffectRenderer.cpp
 M modules/renderers/directx12/src/D3D12ComputeShader.cpp
 M modules/renderers/directx12/src/D3D12EffectRenderer.cpp
 M modules/renderers/directx12/src/D3D12StorageTexture2D.cpp
 M modules/renderers/directx12/src/D3D12Texture2DArray.cpp
 M modules/renderers/directx12/src/DirectX12Renderer.cpp
?? modules/renderers/common/d3d/include/CNA/Internal/Renderers/D3DCommon/D3DShaderReflectionIid.hpp
```

Preserve them. Do not reset, stash, stage or commit another task's files. Separate CNA worktrees
also exist at `/rv/data/development/github.com/libcna/cnawork` (`gamerservicese`) and
`/rv/data/development/github.com/libcna/cnawork/cna-gamer-services`
(`feature/gamer-services-server`). The shared service/server track is owner-managed. Do not
switch Sharp to `next` merely because the general campaign branch table names it. Use and record
actual approved dependency checkouts; ask only if a necessary change genuinely conflicts with
concurrent work.

## SAMPLE-106: historical starting evidence to reassess

Current plan status is **🛑 owner decision pending**, with no C++ sample target.

- Upstream: `/rv/tmp/XNAGameStudio/Samples/SavingEmbeddedImages_4_0/`.
- Artifact root: `/rv/tmp/samples/SAMPLE-106-SavingEmbeddedImages_4_0/` (about 3.4 MiB at inspection).
- Existing helper: `scripts/build-original.cmd` inside that root.
- Existing content: `win7-export/Content/`; full original snapshot and build/test receipts remain.

This is a Windows Phone 7 **Reach**, fullscreen 480×800, 30 Hz game demonstrating **two real
media-library save routes**, with Tap selection and Back exit:

1. `GameProjectImage.jpg` is read via `TitleContainer.OpenStream` / `Texture2D.FromStream`.
   Its save route reopens and passes the original JPEG stream to `MediaLibrary.SavePicture`.
2. `ContentProjectImage` is loaded through `Content.Load<Texture2D>`. Its save route uses
   `Texture2D.SaveAsJpeg`, rewinds a `MemoryStream`, then calls `MediaLibrary.SavePicture`.

Both routes request a name through `Guide.BeginShowKeyboardInput`, then show success/failure
through Guide message boxes.

The historical audit reviewed 16 upstream files and 334 C# lines. The official Phone/Reach
pipeline produced four platform-`m`, XNB-v5 assets, and CNA imported both 480×800 textures and
both SpriteFonts. Exact hashes are in `missing.md`. A focused native test gate passed **77/77**
for XNB/JPEG/saved-picture/media-library/Guide behavior **at that audit's heads**. Do not report
those as a fresh gate at today's `b1ea16aeb` CNA head. The unchanged Phone game build stopped
at missing Phone XNA project extensions in the Win7 installation; no authentic game run was
proved. Content generation success is distinct from application runtime fidelity.

### Questions for the fresh analysis

Recheck the owning code rather than copying these historical conclusions:

- Does browser `MediaLibraryPaths` still lack `UserFolder::Pictures`, causing a real save to fail?
  Native `SavePicture` historically used a real Pictures/Saved Pictures store and collections.
  A hidden transient WASM file does not meet the sample's user-visible media-library contract.
- Does shared Guide now automatically own keyboard/message-box display and completion?
  The source immediately calls `EndShowMessageBox` after `BeginShowMessageBox`; XNA documents
  blocking completion. The historical CNA route threw while pending and required caller-owned
  `RenderPending*EXT` rendering. Adding that rendering only to this game is a forbidden workaround.
- What parts are now available on native and WEBGL2, and which remain platform/product choices?
  Browser downloads, OPFS/IDBFS or a picker need an explicitly approved persistence, collection,
  permission and readback contract; they are not automatically XNA media-library parity.
- Can the unchanged original run with current local tooling or the repaired Win7 VM? Record
  environment failures honestly; do not mistake absent SDK support for a CNA defect.

If these gaps remain, explain the same three broad choices with current evidence: cancel this
Phone-specific sample, authorize reusable media-save plus automatic Guide support, or explicitly
modernize its native/browser product. None has been approved for 106. Shared Gamer Services
progress alone does not prove media-library or modal Guide support.

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
2. Locate the exact SAMPLE-106 original and existing artifact evidence; distinguish historic facts
   from fresh current-head capabilities.
3. Reassess media-save destinations, Guide lifecycle and original reference tooling. Use bounded
   diagnostics when needed, retaining their commands/results under the 106 root. Do not implement
   a sample/framework change or substitute UI merely to obtain a runnable screenshot.
4. Report the brief analysis to the owner, update the appropriate evidence/plan/handoff record,
   and wait for their scope choice if completion still requires a major subsystem or deviation.
