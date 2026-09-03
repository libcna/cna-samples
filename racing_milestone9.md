# Racing Milestone 9 — feature-complete Linux OPENGL33

## Result

Milestone 9 completed on 2026-09-03. The Linux `OPENGL33` reference port is
feature-complete against the canonical XNA 4 source and the unchanged authentic
XNA Game Studio 4 content products. The final source audit maps all 58 original
C# files, the real Debug and Release executables complete the full screen/race
lifecycle, and no modern-repository, GLB, loose-asset or sample-local framework
substitute participates.

This is a runtime qualification result, not permission to redistribute the
Microsoft content. The separate release gate remains blocked until a canonical
license grant is available.

## Frozen revisions

- cna-samples before Milestone 9: `753e73c`;
- CNA before the Milestone 9 storage repair: `056e57d47`;
- CNA after the repair: `822d3b960`;
- sharp-runtime before the repair: `6baf0925`;
- sharp-runtime after the repair: `c419f477`;
- meta-gl: `20c8b2dc5bb80e32706784066db9fd9e15b3f46a`;
- EasyGL: `deda7a426c3c166c0e03a4790f1ede610e2e46fb`;
- renderer/profile: CNA `OPENGL33`, HiDef, Mesa llvmpipe OpenGL 4.5 core.

## Source closure

The source-to-source audit is retained in
[`racing_source_audit.md`](racing_source_audit.md). Milestone 9 restored the
remaining desktop behaviors found by that audit:

- `ScreenshotCapturer` is an XNA `GameComponent`, detects PrintScreen, preserves
  the original sparse screenshot-number search and writes a real JPEG through
  XNA storage;
- `Log` preserves isolated `Log.txt`, the two-MiB rotation rule, original session
  header, timestamped entries, immediate flush and Debug console output;
- the F1 and gamepad LeftShoulder+Y Debug FPS toggles, one-second counters and
  monotonically decreasing `Model.MaxViewDistance` tiers are active;
- the Release-only render exception budget uses the original post-increment
  boundary;
- the dormant `LineManager3D` source branch retains both color overloads, the
  4,096-line cap, cross-frame collection reuse and the authentic
  `LineRendering3D` effect; the GPU harness proves visible world-space lines;
- a real `DeviceReset` caused by applying the original 640x480 option rebuilds
  shadow, glow and menu render targets before the next complete frame.

`LineManager3D` has no world-space producer in the selected original game source;
the only original `BaseGame.DrawLine` call sites are the 2D highscore separators.
The 3D class is retained and tested rather than inventing a new product use.

## Upstream repair

The new log exposed an actual sharp-runtime host-integration bug: desktop
`IsolatedStorage` used `current_path()/.cna_isolated_storage`, and Android carried
an unrelated hard-coded application identity. That made a correct XNA-style
`Log` write into whichever build or repository directory launched the game.

Sharp-runtime commit `c419f477` adds a thread-safe host override to its internal
`StoragePaths` service. CNA commit `822d3b960` connects
`StorageDevice::SetAppNameEXT` to that service, selecting
`<CNA per-user application root>/.cna_isolated_storage`. Racing still calls the
normal `System::IO::IsolatedStorage` API and contains no path workaround.

Regression evidence:

- sharp-runtime full build: zero errors and warnings, maximum two jobs;
- sharp-runtime complete component gate: 17,939/17,939 across 40 executables;
- focused sharp `StoragePathsTests`: 3/3;
- CNA `CnaStorageTests`: 10/10, including exact application-root correlation;
- Racing desktop probe: `Log.txt` exists in the scoped root and contains the
  original session header plus a flushed entry;
- no `.cna_isolated_storage` directory is left in cna-samples.

## Qualification

All Racing compilation used at most eight jobs. The sharp-runtime gate used its
stricter permanent maximum of two.

- cumulative Debug and ASan/UBSan OPENGL33 harness: 151/151 in both builds;
- the real `RacingGame_cna_samples` product executable links cleanly in Debug,
  ASan/UBSan and Release (the probes use the same `RacingGameApplication` owner);
- post-source-audit base harness rerun: 144/144 in Debug, ASan/UBSan and Release;
- Debug and ASan/UBSan physics oracle: 753 records, maximum camera difference
  16 ULP;
- Debug and ASan/UBSan Track/Replay oracle: 79/79 records;
- complete drivable scene, settings, screen flow, menu screens, natural race
  return, desktop runtime and device reset: PASS in Debug and ASan/UBSan;
- Release versions of every preceding probe: PASS; Release harness 144/144;
- natural 7,200-frame race completion and return to menu: PASS in Debug,
  ASan/UBSan and Release;
- FNA/CNA static-scene comparison: normalized RMSE `0.002230`, `99.0503%` of
  channels within two, maximum channel delta 19;
- LeakSanitizer classification: 100,956 bytes in 449 allocations, with every
  allocation stack wholly rooted in external `libGLX_mesa`; no Racing, CNA or
  sharp-runtime frame;
- Release 7,200-frame llvmpipe run: 2:53.66 wall clock, 705,908 KiB maximum RSS,
  zero swaps, exit 0;
- source manifest: 325/325 hash-verified;
- authentic XNA build manifest: 358/358 hash-verified, including 339 XNBs and
  17 copied/XACT products;
- `git diff --check`: PASS.

Desktop OpenGL does not expose the Direct3D-style lost-device event as a normal
runtime condition. Milestone 9 therefore tests CNA's real reset route and resource
recreation without adding a fake production `LoseDevice` API.

## Release boundary

The canonical snapshot contains no `LICENSE`, `EULA`, `COPYING` or RTF grant and
retains the notice `Microsoft Corporation; All rights reserved`. The exact
inventory and packaging rule are in
[`racing_release_gate.md`](racing_release_gate.md). No canonical content asset is
committed to cna-samples.

## Next milestone

Milestone 10 owns Windows `OPENGL33` build, packaging and runtime qualification.
Linux completion does not imply Windows, Android or Web support.
