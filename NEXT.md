# NEXT.md

## Active handoff for Claude Code — read this first (2026-08-30, thirty-second update)

This section is the current operational handoff for the non-Racing sample campaign. It supersedes
contradictory instructions in the legacy appendix later in this file. Before doing any work, read
[`rules.md`](rules.md) completely, then [`plan.md`](plan.md), the selected sample's `missing.md`,
and the `AGENTS.md`/`CHECKLIST.md` instructions in every repository that will be changed.

Work is paused at the owner's request after SAMPLE-053. Once the owner resumes the campaign, the
next agent is expected to continue with exactly one sample: **`SAMPLE-054`** -- the next `⬜` row
in `plan.md`. Do not start it before that explicit resume instruction, and do not start a second
sample in the same task unless the owner later asks for it. The owner-assigned AssemblyInfo
back-fill is done (see below).

### Current repository chain and synchronized baseline

| Layer | Checkout | Branch | Synchronized HEAD at handoff |
|---|---|---|---|
| Samples | `/rv/data/development/github.com/openeggbert/cna-samples` | `develop` | The `SAMPLE-053` task commit containing this handoff |
| XNA runtime | `/rv/data/development/github.com/openeggbert/cnanext` | `next` | `6a85149e2` — dynamic external-XNB concrete types for SAMPLE-053 |
| .NET runtime | `/rv/data/development/github.com/openeggbert/sharp-runtimenext` | `next` | `df1b42ab` — unchanged by SAMPLE-053 |

Everything through `SAMPLE-051` remains on the remote. The SAMPLE-052 and SAMPLE-053 task commits
are local and must not be described as pushed. The SAMPLE-053 artifact root is complete and
unpruned; only the owner may authorize `tools/prune-completed-sample.sh --apply`.

**Build cache.** Use `CCACHE_DIR=/rv/cnaccache` for every build. The owner created that cache on
2026-08-25 because the default shared one was thrashed by several concurrent agent sessions — it
sat at a 21.8% hit rate with 16.3 of 20 GB used. The campaign cache is 40 GB with compression on.
And there is **no CPU-core limit** any more: `-j$(nproc)`. The `cnanext` head also carries an
owner-reported fix unrelated to any sample: `GraphicsDevice.Viewport` and `ScissorRectangle` are
public XNA state in logical space, but `IGraphicsRenderer::SetViewport`/`SetScissorRect` are
drawable-space seams for EasyGL, Magnum and OpenGL2, so a game that assigned either property
bypassed the letterbox/scale placement that `UpdateViewportFromWindow()` applies. Both setters now
map through `CNA::Internal::Graphics::MapLogicalRectToPresentation`, which is the identity for the
renderer family that treats the pushed rectangle as logical, and while a render target is bound.

All three checkouts were clean and exactly synchronized with their corresponding `origin` branch
before this handoff edit. Always re-run `git status --short`, branch checks and the upstream
left/right count because other agents may share the machine. Never discard or absorb unrelated
changes. The active samples CMake project already consumes `../cnanext` and forces
`CNA_SHARP_RUNTIME_ROOT` to `../sharp-runtimenext`; do not redirect it to the old `cna` or
`sharp-runtime` checkouts.

### Open items a new session inherits

None of these blocks starting `SAMPLE-054` after the owner resumes work. They are listed so they
are not rediscovered as surprises, and so nobody closes one by accident.

| Item | Where | State |
|---|---|---|
| `SAMPLE-014` Spacewar | `plan.md` row, `samples/Spacewar/missing.md` | 🛑 — the port replaced the original's `XmlSerializer` settings load with a hand-written parser. The owner chose "mark it and decide later" on 2026-08-28. Needs a ruling: implement an XML serializer in `sharp-runtimenext`, or accept the hand parser on record in `diff.md`. Do not decide this alone. |
| FX-126 | `cnanext plans/plan_fx.md` | Open — a downward-facing surface takes a directional light in EasyGL that Direct3D 9 leaves black. Found by SAMPLE-047 at camera angles the sample does not open at; every other explanation was measured and eliminated. Next step is to dump the interpolated normal for one pixel in both engines. |
| The 22 `.fx` `missing.md` claims | see the `.fx` finding section below | Each has to be retested on its own evidence; the blanket claim is disproved but that is not the same as unblocked. Do not mass-edit them. |
| Final full sample build | all 65 sample targets | Deliberately deferred by the owner: every sample is rebuilt in one pass once porting is finished, Racing separately and last. The `AssemblyInfo.cpp` back-fill in particular has never been compiled or seen on screen. |
| `SAMPLES-INFRA-003/004/007` | `plan.md` foundation tasks | ⬜ — doc reconciliation, an inventory validator, and a mechanical bypass-scan template. `INFRA-005`/`006` are 🛠 and want their reusable workflow extracted, which the techniques section above now partly is. |
| Old capture scripts | pruned artifact roots before 2026-08-25 | They search for the window by `--name '^Game$'` and will not find it now that samples declare their own title. Fix the script, do not retitle the sample. |

### Techniques this campaign has settled into — reuse them, do not reinvent them

Every one of these was paid for by a sample that got it wrong first. They are listed here because
they are now the default way this campaign works, not per-sample trivia.

**1. Decode the `.xnb`'s type-reader table before writing any code.**
`tools/dump-xnb-readers.py <file.xnb>` prints an uncompressed `.xnb`'s header and its full
type-reader table. That table is the contract between the file and the runtime, and it must
resolve **in full** before a single object is read — so one unregistered name fails the whole
asset, not the part that uses it. It named both of SAMPLE-048's framework gaps and both of
SAMPLE-051's in minutes; guessing would have found one of each and missed the other.

**2. A sample with its own `ContentProcessor`s.** Compile the extension assembly with the
in-prefix `csc.exe` first, then hand it to `BuildContent` in `PipelineAssemblies`. If the
processors reference a *game* library for the types they serialize — SAMPLE-051's do — compile
that library first and pass **both** assemblies, because the `.xnb` records the reader against the
library's assembly name. Per-asset processor parameters are item metadata named
`ProcessorParameters_<Name>`. Precedents: SAMPLE-042, 048, 049, 051; copy
`SAMPLE-051-.../scripts/{build-original.sh,XnaPipelineRunner.cs}`, they are the most complete pair.

**3. A `.spritefont` naming a font the prefix does not have.** Install it **and register it** —
Wine's GDI does not enumerate a font that is only dropped into `drive_c/windows/Fonts`:

```text
HKLM\Software\Microsoft\Windows NT\CurrentVersion\Fonts
  "Kootenay (TrueType)" = "Kooten.ttf"
```

XNA Game Studio's own faces live in
`/rv/tmp/samples/_tools/xna-game-studio-4-refresh/admin/Fonts/`. Registered so far: Miramonte,
Pericles, Segoe UI Mono, Segoe UI (owner-supplied), Kootenay. SAMPLE-051's build script does both
steps idempotently — copy it rather than doing it by hand, so the build stays reproducible.

**4. The frozen diagnostic pair.** `cna-diag/<file>.orig` and `.frozen`, `xna4-diag/<file>.cs`,
driven by `scripts/compare-frozen.sh`. The hook pins whatever the sample's only non-determinism is:
`CNA_CURSOR` (047, 048), `CNA_SPHERE`/`CNA_FACING` (049), `CNA_TIME` (050), `CNA_ANIM_TIME` (051).
Two rules make it evidence rather than decoration:

- within a leg both engines must go **byte-identical still** — that is what proves the pin took;
- across legs the hashes, coverage and centroid must **move** — that is what proves the metric can
  tell the legs apart. A number that is identical in every leg proves nothing.

`compare-frozen.sh` restores the port from `.orig` on exit. Update the `.orig` in the same breath
as the port source, or the next comparison silently reverts your change.

**5. Compare the DATA before the pixels, and compare floats as floats.** When a sample carries
values — collision heights (049), a whole animation library (051) — dump them from both engines and
compare them numerically. `SAMPLE-051-.../scripts/compare-dump.py` is the reusable form: it parses
every number back to `float32` before comparing, because C#'s `"R"` and C's `%.9g` spell the same
float differently. On 051 that was the difference between "3060 defects" and the truth, **0 ULP
across all 5388 values**.

**6. The browser can be frozen too.** A tab has no environment, so the frozen build reads the same
instant out of the page's query string with one `emscripten_run_script_string` under
`#ifdef __EMSCRIPTEN__` (SAMPLE-050). That turns the web leg from a statistics check into a
pixel-for-pixel comparison against real XNA — 99.74 % within 8 levels there.

**7. Crop what the browser paints.** Chrome's screenshot clip includes the canvas's own 1 px border
and focus ring, which put non-background pixels in rows 0 and 479 and dragged a blurred agreement
from 100.00 % to 94.73 %. `compare.py --inset 3` drops them.

**8. Hold a key press.** `IsNewKeyPress`/`IsNewButtonPress` need the key down in one polled frame
and up in the previous one. A plain `xdotool key` lasts ~12 ms and can fall entirely between two
polls: SAMPLE-051's first capture recorded A doing nothing while B worked. Use `keydown`, sleep
~200 ms, `keyup`, on both the native and the CDP side.

**9. Calibrate every browser gate by breaking what it watches.** A threshold nobody has seen fail
is a guess. SAMPLE-050 forced `TextureEnabled` off and watched `greenFraction` fall 23.35 % → 5.77 %
while the silhouette barely moved; SAMPLE-051 reverted FX-127 and recorded the abort. Keep that run
under `evidence/sabotage/`. Equally: a gate that fails on a *correct* picture is as wrong as one
that passes on a broken one — SAMPLE-049 had to narrow a fog check for exactly that reason.

**10. A framework change has copies to find.** The CNAEXT engine layer keeps its own copies of
shaders the renderer also has (`ShadowMap`, `DepthNormalPrepass` hold the skinned vertex shader).
When a renderer-side change alters an attribute or a uniform, grep `modules/graphics-ext` for it.
`ShadowVisibilityTest.ASkinnedMeshShadowsItself` is what caught FX-127's missed pair.

### AssemblyInfo.cs: DONE across the repository (2026-08-25)

The owner noticed on 2026-08-25 that every CNA window is titled `Game` where the original's
reads its own name (`FuzzyLogic`, and so on). The framework half is fixed -- `cnanext`'s
`CNA::Internal::GetDefaultWindowTitle()` now follows XNA's chain (declared assembly title →
the executable's own file name → `"Game"` as a last resort) instead of a hardcoded literal,
and SDL's Emscripten backend forwards it into `document.title`, so the browser tab follows.

The other half is per sample: every XNA sample has `Properties/AssemblyInfo.cs` carrying
`[assembly: AssemblyTitle("…")]`, and **no port in this campaign had carried that file**.
SAMPLE-027 and SAMPLE-028 do it, as `src/Properties/AssemblyInfo.cpp` with a
namespace-scope `CNA::AssemblyTitleAttributeEXT`. **Every earlier ported sample still shows
`Game`.**

**The owner-assigned back-fill was carried out on 2026-08-25.** All **64** previously
ported samples now carry `src/Properties/AssemblyInfo.cpp` and list it in their
`CMakeLists.txt`; with SAMPLE-027 and SAMPLE-028 that is every ported sample in the
repository. Per the owner's instruction none of them were compiled -- every sample is
rebuilt in one pass at the end, once all porting is finished (Racing excepted, it is
ported separately and last under `plan_racing.md`). The files were syntax-checked
(`g++ -fsyntax-only`) and every generated title was checked for characters that would
break a C++ string literal; none has any.

The executable keeps its `_cna_samples` suffix by the owner's instruction, so the title
comes from the declaration, not the file name.

**How each title was determined**, because guessing it would have been easy and wrong:

- The upstream directory came from `plan.md`'s own rows where they cite
  `samples/<Name>/missing.md` (26 of 64), and from a normalised name match for the rest;
  `GameStateManagement` → `GSMSample_4_0_WIN_XBOX` and `RolePlayingGame` →
  `RolePlayingGame_4_0_Win_Xbox` had to be resolved by hand.
- The *game* project inside it was chosen by reading each `.csproj`'s `<XnaPlatform>`,
  preferring Windows over Xbox 360 over Windows Phone, and skipping content-pipeline
  extensions. Two earlier rules were wrong and were discarded: picking the shallowest
  `AssemblyInfo.cs` chose library projects (`CardsFramework`, `MapData`,
  `NinjAcademyCommonTypes`), and requiring `OutputType=WinExe` missed 17 samples outright
  because **a Windows Phone XNA game project is `OutputType=Library`** -- it ships as a XAP.
  That same rule is what separates `TicTacToe`/`Yacht`'s XNA game from the plain .NET
  server project sitting beside it.
- Three needed individual evidence, and the titles genuinely differ between the
  candidates:
  - `TransformedCollision` — its upstream holds two games; the name-similarity rule gave
    both ports the *Test* one. Corrected to `"Transformed Collision"`.
  - `CatapultWars` — the port's own `missing.md` cites `Source/EX2_PolishAndMenus/End`,
    whose title is `"Catapult Sample"`; EX1's is `"CatapultGame"`.
  - `HoneycombRush` — no stage named anywhere, settled by comparing the port's `Screens/`
    against both stages: it carries all seven of EX2's (EX1 has one). `"Honeycomb Rush"`,
    not `"HoneycombRush"`.

Titles are copied verbatim, including the two spaces in `"Networking:  Client/Server"` and
`"Networking:  Peer-to-Peer"`, and the fact that several read nothing like their directory
(`CardsStarterKit` → `"Blackjack"`, `ReachGraphicsDemo` → `"MIX10 Graphics Demo"`,
`Graphics3D` → `"Graphics 3D Sample"`).

**Not yet verified:** no window title has been observed for any of these 64. They are files
only, as instructed; the final build pass is where they get checked.

Note for the retained capture scripts: those written before this fix search for the window
by `--name '^Game$'` and will not find it once a sample declares its title.

### Mission and non-negotiable scope

- Freshly audit all 153 physical upstream directories one by one, including existing ports,
  placeholders, previously ignored entries, tools, libraries, XNA 3.x variants, Windows Phone
  projects and retired-service projects. An old status is evidence to retest, not a verdict.
- Only the owner may decide that a sample is ignored or accept an evidence-backed non-port
  conclusion. Present concrete scope/options before such a decision.
- Racing is intentionally last. It is governed by [`plan_racing.md`](plan_racing.md). Do not edit
  Racing code, tasks, matrices, feasibility notes or its special plan during the normal queue.
- The goal is the closest practical C++ translation of the exact Microsoft XNA sample, not merely
  a demo that looks similar on its first screen.
- Every successful runtime sample requires both a native OPENGLES3 build and a complete WEBGL2
  browser build that can later be published on the owner's website.
- EasyGL is the only renderer under test: `OPENGLES3` natively and `WEBGL2` under Emscripten. Do
  not spend time on Vulkan, SDL_Renderer, Bgfx, WebGPU, desktop OpenGL or another backend.

### Sources of truth and audit depth

Use evidence in this order:

1. Exact upstream directory under `/rv/tmp/XNAGameStudio/Samples`.
2. A real unchanged XNA 4.0 build and run of that directory.
3. FNA at `/rv/data/library/github.com/FNA-XNA/FNA` for runtime API/behavior.
4. Local Microsoft XNA documentation/specification material.
5. MonoGame or other ports only as supplemental evidence.

For each sample, review every relevant file, not only `Game1.cs`: solutions, project files,
configurations, all C# helpers, content project declarations, importer/processor/writer projects,
assets, audio banks, effects, documentation, conditional branches, Windows/Xbox/Phone variants,
tests and tools. Preserve inactive source branches when they are part of the original. Record
controls, timing, resolution/fullscreen behavior, visuals, audio, state transitions, cleanup and
non-default paths. Screenshots help, but never replace line-by-line source and content review.

### Fidelity rules

- Preserve original namespaces, logical type/member names, file/class decomposition, constants,
  defaults, lifecycle, algorithms, update/draw order, content identifiers and input mappings.
- Preserve all scenes, screens, modes, effects, audio, networking behavior and target/debug
  branches. Do not silently simplify a branch that is awkward on Linux or Web.
- Limit changes to lossless C#-to-C++ mechanics: RAII ownership, references/value representation,
  established `getXProperty()`/`setXProperty(...)` calls and closed AOT reader registration.
- Use XNA-shaped CNA APIs. Use `System::*` and SharpRuntime primitive aliases for .NET concepts;
  if the runtime concept is missing, implement it generally in `sharp-runtimenext`.
- CNA `GetTypeName()` extensions must report the original fully qualified logical type name.
- Preserve original observable bugs/quirks unless evidence proves XNA itself behaves differently.
  Do not “improve” the sample while porting it.
- Retain original documentation and licenses. `help.png` is historical only: move it to the sample
  root beside `CMakeLists.txt`, never package, load, preload or draw it, and remove the invented F1
  overlay and its input/timer state.

### Zero-workaround rule and layer ownership

A sample must not compensate for a CNA or sharp-runtime defect. In particular, never:

- call renderer/backend helpers from sample code;
- use `NOXNA`/CNA graphics extensions instead of an original XNA API;
- replace `Content.Load<T>()` with `RawMesh`, `RawModel`, direct image/model loaders or manual
  runtime content reconstruction;
- use loose PNG/font/shader/JSON/bin sidecars or direct `SetData` as substitutes for original XNA
  pipeline content (an original sample's own `SetData` call remains valid);
- translate or hand-wire an `.fx` effect independently inside each sample;
- add sample-local render-state, culling, initialization or lifecycle calls merely to hide a
  framework bug;
- invent controls, fake services/data, replacement screens, reduced modes or diagnostic overlays;
- omit a failing feature and mark the sample complete.

Fix behavior in the layer that owns it during the same session:

- XNA/FNA API or behavior → `../cnanext`, with its instructions, Doxygen, focused tests and a
  general implementation with no sample-name special case.
- .NET/System API or primitive behavior → `../sharp-runtimenext`, with focused tests.
- Translation/content issue → `cna-samples`.

If the faithful path exposes a genuinely large subsystem or material scope decision, do not add a
workaround. Mark the row `🛑`, document exact evidence and measured options in `missing.md` and the
owner decision queue, then ask the owner before implementing it. Likely decision areas are a new
raw `.fx` ingestion class, broad Content Pipeline authoring APIs, retired Xbox LIVE/WP7 services,
large skeletal-animation work, unusual tools/older-XNA scope or a substantial browser platform
gap. Bounded runtime bugs are fixed without interrupting the owner.

### Content policy

- Exact pregenerated XNBs produced by the official Microsoft XNA 4.0 Content Pipeline are
  accepted and preferred. Preserve `Content.Load<T>()`, original logical names and serialized
  object behavior.
- Build the official output when possible, retain it under the artifact root, prove checked-in
  XNBs byte-identical with SHA-256 and record the generation command/evidence.
- Pregenerated runtime XNB acceptance does not declare a standalone importer/processor/tool
  project ported. Audit such projects separately and route scope decisions to the owner.
- A closed AOT reader is acceptable only when it reconstructs the exact original runtime type and
  fields. Any offline conversion must be lossless, reproducible and documented.
- Remove old loose content substitutes when official XNB content is available.
- Use CNA's common XNB/compiled-effect path. Unsupported effect constructs are CNA gaps, not a
  reason for a handwritten per-sample shader.

### Required workflow for every `SAMPLE-nnn`

1. Read the binding documents and inspect all three worktrees. Locate the exact `plan.md` row and
   set it to `🔎`/`🛠` while active.
2. Inspect the physical upstream directory yourself and inventory all files. Do not trust old
   `missing.md`, `ignored.md`, `DEFERRED.md`, `NEXT.md` appendix text or an old port status.
3. Create `/rv/tmp/samples/SAMPLE-nnn-UpstreamDirectory/` with `xna4-original`, original build,
   native build, web build, scripts and evidence subtrees. Copy the exact original snapshot used.
4. Build the unchanged original in its correct configuration. Run and capture it when runnable,
   including representative controls/states and clean exit. Retain failed build/run evidence too.
5. Audit original versus C++ line by line. Identify every omission, workaround, substitute,
   renamed API, incorrect default and stale missing claim before implementation.
6. Restore the complete translation and exact content. Fix CNA/sharp-runtime generally as needed.
7. Build with the whole machine — `-j$(nproc)`. There is no CPU-core limit; the old `-j6`
   ceiling was removed by the owner on 2026-08-25. Watch memory, not core count.
8. Build and run native OPENGLES3. Capture representative states, exercise input/audio and prove
   clean exit. Compare against the original.
9. Build the complete WEBGL2 bundle. Serve over local HTTP and launch the system
   `/usr/bin/google-chrome` from the terminal. Check real pixels, content requests, controls,
   audio where relevant, rejected promises, exceptions, browser console and WebGL errors. A link,
   Node run or successful compile alone is not a browser test.
10. Run focused regression tests for every runtime change and any original sample-owned tests.
11. Run mechanical scans for `RawMesh`, `RawModel`, suspicious `SetData`, NOXNA helpers, sidecars,
    F1/help logic, invented controls and omitted branches; manually evaluate every hit against the
    original.
12. Rewrite the sample's `missing.md` as current evidence, update its `plan.md` row and any owner
    decision/history entry. `missing.md` is not a waiver.
13. Run `git diff --check`; stage only explicit task files; commit one task per repository with the
    same `SAMPLE-nnn` ID. Never use `git add .` or `git add -A`. Do not push unless the owner asks.

A row becomes `✅` only after the full applicable original/native/web/content/test/documentation
gate passes and no active gap or workaround remains.

### Artifact and original-XNA conventions

All generated material belongs under the stable sample root, never in the source tree or an
unrelated `/tmp` directory:

```text
/rv/tmp/samples/SAMPLE-nnn-UpstreamDirectory/
  xna4-original/          exact source snapshot
  xna4-build/             original executable and official pipeline output
  cna-native-opengles3/   reusable native Release CMake tree
  cna-web-webgl2/         reusable Emscripten Release tree and publishable bundle
  scripts/                build/run/capture/browser helpers
  evidence/               logs, captures, hashes, console results, failure evidence
```

The established XNA 4.0 Wine prefix is `/home/robertvokac/.wine-cna-xna40`. Prefer retained
per-sample scripts. A typical real-display original run is:

```bash
cd /rv/tmp/samples/SAMPLE-nnn-Name/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine Sample.exe
```

WineD3D (`d3d9=b`) is verified; default DXVK may fail during XNA startup. When other agents share
the desktop or XWayland windows are unmapped, use an isolated Xvfb display for deterministic
capture and input:

```bash
xvfb-run -a -s '-screen 0 1280x720x24 +extension GLX' scripts/capture-original.sh
```

Identify the exact process/window before sending keys or killing anything. Do not attribute a
flashing or invisible window to the current sample without process evidence. The Win7/VS2010/XNA
4.0 VirtualBox environment is the fallback when the direct Linux route is genuinely unavailable;
an environment failure is not permission to skip comparison.

Typical native and web build templates are:

```bash
cmake -S /rv/data/development/github.com/openeggbert/cna-samples \
  -B /rv/tmp/samples/SAMPLE-nnn-Name/cna-native-opengles3 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build /rv/tmp/samples/SAMPLE-nnn-Name/cna-native-opengles3 \
  --target SampleTarget_cna_samples -j$(nproc)

/home/robertvokac/emsdk/upstream/emscripten/emcmake cmake \
  -S /rv/data/development/github.com/openeggbert/cna-samples \
  -B /rv/tmp/samples/SAMPLE-nnn-Name/cna-web-webgl2 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build /rv/tmp/samples/SAMPLE-nnn-Name/cna-web-webgl2 \
  --target SampleTarget_cna_samples -j$(nproc)
```

The root project forces the correct reference renderer for native versus Emscripten and uses the
next-generation siblings. For a CNA runtime change, build the affected modular target and
`CnaTests`; this build currently has no aggregate target named `CNA`. Under a restricted Codex
sandbox, set `CCACHE_DISABLE=1` if ccache cannot write its external cache. Run the focused test
filter from `cnanext/cmake-build-debug/CnaTests`.

### Work completed in the current 153-directory campaign

`SAMPLE-001` through `SAMPLE-017` are complete in [`plan.md`](plan.md). Their individual
`missing.md` files are the detailed evidence. Important outcomes to preserve:

- `SAMPLE-001` Primitives: removed F1 overlay; compared real XNA via WineD3D; fixed CNA primitive
  rasterization and the Linux multi-second first-frame gamepad initialization delay.
- `SAMPLE-002` Primitives3D: restored exact Position+Normal vertex, lighting, HUD, input and
  disposal; fixed generic custom-vertex upload/declaration behavior.
- `SAMPLE-003` TexturesAndColors: restored five models, texture, font and official 13-technique
  compiled effect using eight exact XNBs; fixed CNA reader startup/web stack behavior.
- `SAMPLE-004` StockEffects: proved it is a Content Pipeline compiler/library, not a game. The
  owner accepted the evidence-backed non-port boundary; do not invent a game or alias built-ins.
- `SAMPLE-005` ReachGraphicsDemo: restored title plus all six demos and 22 official XNBs; fixed
  model reader/Tag ownership and EasyGL semantic binding, VAO lifetime and base-vertex fallback.
- `SAMPLE-006` SpriteEffects: restored all five modes and eight exact XNBs; fixed NormalizedByte4,
  pixel-only compiled-effect inheritance and secondary texture-slot behavior.
- `SAMPLE-007` SpriteSheet: removed runtime repacking/loose content; restored custom processor XNB
  output; fixed XNB string reference dispatch, nullable SpriteBatch defaults and stale SDL cache.
- `SAMPLE-008` ShapeRendering: restored exact debug-only renderer/game structure, overloads,
  guards, shared buffers and controls.
- `SAMPLE-009` InputReporter: restored 15 exact XNBs and original input/draw flow; fixed XNA
  backslash and case-insensitive content resolution generally.
- `SAMPLE-010` InputSequence: restored 15 exact XNBs, labels, ordering, capacity and type names;
  native/web inputs and rendering match the original.
- `SAMPLE-011` SafeArea: restored three exact XNBs and original conditional debug component; XNA,
  native and web baseline rendering was verified at 1280x720.
- `SAMPLE-012` GeneratedGeometry: restored processor-built geometry and three exact XNBs rather
  than runtime substitutes. The owner explicitly accepted exact pregenerated XNBs as the faithful
  runtime boundary; this does not port the design-time pipeline API.
- `SAMPLE-013` Platformer: restored the full gameplay, validation, touch/accelerometer and target
  branches, faithful content/audio and native/web behavior; no content workaround remains.
- `SAMPLE-014` Spacewar: restored both Retro/Evolved games, all screens, upgrades, scenes,
  effects/models/render targets and 157 exact assets. CNA fixes cover XNB/cache semantics,
  case-insensitive content, compiled-effect samplers and FACT-faithful complex-track XACT playback,
  including the previously missing title music.
- `SAMPLE-015` TicTacToe: upstream is a WP7 XNA client plus WCF/MPNS server. The owner declined
  WCF/MPNS emulation and accepted a non-port conclusion. The retained local game is explicitly a
  free reimplementation, not a port of either original part; its weak AI is accepted as such.
- `SAMPLE-016` Bounce: audited the WP7-only game; restored 100-sphere physics,
  accelerometer/emulator input, orientation, 30 Hz fullscreen, custom procedural geometry,
  lighting/shadows and original quirks. CNA fixes restore shipped DirectionalLight defaults and
  deferred browser fullscreen.
- `SAMPLE-017` Collision: restored the FPS font/text, gestures, phone branch, exact named colors,
  original class/API surfaces, `IDisposable`, and the complete randomized UnitTests port. Both
  original and C++ tests report `Passed: 420000 Failed: 0`. XNA, native OPENGLES3 and Chrome
  WEBGL2 captures cover Sphere, Ray, Frustum, AABB, OBB, orthographic projection and Escape.
- `SAMPLE-018` PerPixelCollision: removed the loose colour-keyed PNGs, the invented F1 overlay and
  the RGBA literals; restored the two byte-identical official-pipeline XNBs, the named colours, the
  original member surface and the reference build's own safe-area arithmetic. Two general CNA
  fixes: the XNA value-type default for `Color`, and sub-pixel sprite destinations in `SpriteBatch`
  (XNA/FNA keep them in floats; CNA had quantised them, so no sprite drawn at a fractional position
  ever had a filtered edge). XNA, native OPENGLES3 and Chrome WEBGL2 agree on the person's start
  and both clamps with a byte-identical sprite, and all three show real per-pixel hits alongside
  rectangle overlaps that correctly do not hit.
- `SAMPLE-019` RectangleCollision: the old port drew both sprites as solid magenta squares -- its
  PNGs were raw BMP conversions with no colour key at all. Replaced by this sample's own
  byte-identical official-pipeline XNBs, proven by building its own content project rather than
  copying SAMPLE-018's; F1 overlay and RGBA literals removed; safe-area arithmetic re-derived.
  `Rectangle::Intersects` is already character-for-character FNA's, so no framework change was
  needed. 180 s recordings confirm tutorial 1's own behaviour: every unambiguous rectangle overlap
  turns the background red even when the drawn pixels are clear of each other.
- `SAMPLE-020` TransformedCollision: tutorial 3, and the first sample in this campaign whose
  upstream directory needed **two** ports. The primary game gained `Block.hpp`, both
  `IntersectPixels` overloads and `CalculateBoundingRectangle`; the second product,
  `TransformedCollisionTest`, was ported for the first time into its own `samples/` directory
  because it has its own solution, `Program.cs` and content project. One sharp-runtime fix:
  `List<T>` was uninstantiable for an element type without `operator==`. Both products match the
  original in all three builds, and the mouse-driven one matches it exactly, position by position.
- `SAMPLE-021` PathDrawing: the campaign's second Windows-Phone-only sample, and the first that
  could actually be built and run here. Removed all five of the previous port's documented
  deviations, four of whose stated root causes were Vulkan claims that do not hold on EasyGL.
  Ships this sample's own **WindowsPhone** official-pipeline XNBs, built from the real Segoe UI
  Mono. The faithful translation needed **no framework change**, and the native frame is
  byte-identical to XNA's, 384000 of 384000 pixels. Then, at the owner's request, `TouchPanel`
  gained a CNAEXT opt-in reporting the left mouse button as a touch, so a touch-only phone game
  is playable with a pointer -- off by default, enabled by one marked line in the sample, and
  recorded in `samples/PathDrawing/diff.md`. Also fixed a Paeth bug in the shared browser
  harness's PNG decoder.
- `SAMPLE-022` Pathfinding: 42 files, two solutions, three projects. Removed all five documented
  deviations, of which the substantial one was a **hand-written XML parser** standing in for
  `Content.Load<MapData>()`; the four maps now come from official XNBs through a closed AOT reader
  for a type the sample itself declares. The `PathfindingData` class library became a third
  precedent -- the type is ported, the project file is not. One sharp-runtime fix: `Dictionary`
  now accepts a key with `GetHashCode()` and no `std::hash`. Native frames are byte-identical to
  XNA's on every frame where nothing is moving.
- `SAMPLE-023` WaypointSample: the sample SAMPLE-021 and SAMPLE-022 borrowed from. All three
  versions of `Tank.cs`/`WaypointList.cs` were diffed before writing — 192/262 and 82/94 differing
  lines — and nothing was copied. Only this version's `Tank` is a real `DrawableGameComponent`,
  and only it has the `Behaviors/` hierarchy. The old port had no font and no `blank` asset at
  all. **No framework change was needed**; the start frame is byte-identical and the HUD band
  matches in all six frames across all three builds.
- `SAMPLE-024` Flocking: the plan row's odd wording turned out to mean the old port had
  **deliberately not reproduced an upstream defect** — `Bird.Update` applies the Y movement twice.
  It is reproduced now, along with two more the old record never mentioned. The whole HUD was
  absent because the port had no font and no glyph assets. One cnanext fix: `Vector2` gained
  `*=` and `/=`.
- `SAMPLE-025` ChaseAndEvade: the old port was 258 lines against the original's 717. Both
  documented deviations are gone -- the HUD state lines and all three `#if WINDOWS_PHONE`
  regions. Neither the old port nor its record mentioned that the **mouse pointer controls the
  cat in the original**. No framework change was needed.
- `SAMPLE-026` Aiming: four deviations gone, the largest being an **omitted additive spotlight
  pass** whose own record admitted a faithful port would work on EasyGL and kept the workaround
  "for Vulkan-backend safety". Nothing in this sample is random, so it is the first in six where
  whole frames match byte for byte. No framework change was needed.
- `SAMPLE-027` FuzzyLogic: 518 lines in one header replaced by the full 1298-line original as
  nine `.hpp`/`.cpp` pairs. All four claimed omissions were false, including the one that sounded
  like a real language constraint -- the `Mouse` -> `MouseEntity` rename. One sharp-runtime
  addition: `TimeSpan::operator+=`/`-=`.
- `SAMPLE-028` ColorReplacement: recorded as **not portable at all**, blocked by its custom `.fx`.
  Stale: CNA loads compiled XNA Effect bytecode through MojoShader on EasyGL. Ported whole with
  the unmodified shader. Four framework fixes, the sharpest being XNA's
  `protected Effect(Effect cloneSource)`, absent in CNA, which left `EffectMaterial` with zero
  parameters behind a test file that only checked the type name.

### Fonts: which Segoe faces exist here, and which do not

`SegoeUIMono-Regular.ttf`/`-Bold.ttf` are **redistributable** -- Microsoft shipped them with XNA
Game Studio itself (`Samples/RedistributableTTFs_ARCHIVE_3_1/` in the upstream repo), and they are
already in the Wine prefix. That is why every sample so far whose `.spritefont` asks for
**"Segoe UI Mono"** built without trouble.

The proportional **"Segoe UI"** is a Windows system font and was not on this machine at all;
SAMPLE-029 asks for it and both platform targets refused with *"The font family ... could not be
found"*. The owner supplied `segoeui.ttf` on 2026-08-26. Dropping it into
`~/.wine-cna-xna40/drive_c/windows/Fonts/` was **not enough** -- Wine's GDI did not enumerate it
until it was also registered:

```text
HKLM\Software\Microsoft\Windows NT\CurrentVersion\Fonts
  "Segoe UI (TrueType)" = "segoeui.ttf"
```

Check the produced `.xnb` against a sibling sample's before trusting it: all three of 027, 028 and
029 pack the same glyph range into the same-sized atlas, so equal file size proves nothing and
equal bytes would mean a silent fallback. Theirs differ, so Segoe UI really was used.

### THE `.fx` FINDING, AND WHAT IT DOES AND DOES NOT UNBLOCK

`DEFERRED.md` item #11 says custom shaders need hand-rewriting to GLSL `.shader.json` because
"no tooling exists". That describes CNA's *other* effect route. `plans/plan_fx.md` built the real
one: the compiled Effect Framework bytecode the official `EffectProcessor` emits, run through
MojoShader. `docs/fx-compiled-effects.md` §10 lists FNA3D (always on), SDL_GPU and the whole
EasyGL family including `WEBGL2` (both opt-in) as supporting it.

SAMPLE-028 is the first end-to-end proof in this campaign: the sample's own `ReplaceColor.fx`,
compiled by the official pipeline, loaded and ran on `OPENGLES3` and on `WEBGL2` in Chrome, with
all seven of its declared parameters reachable by name.

**22** ported samples' `missing.md` files blame a custom shader for missing behaviour, and **30**
upstream directories ship `.fx`. Their stated reason is now disproved -- but that is not the same
as being unblocked, and no one should mass-edit those records on this finding alone:

- SAMPLE-053 proved the `CustomModelEffect` case end to end: its unchanged three-processor chain
  runs in the official XNA toolchain, the exact resulting XNBs can be retained, and CNA now loads
  their compiled effect plus concrete `TextureCube` external reference. No replacement processor
  or shader translation is required.
- `docs/fx-compiled-effects.md` records the real remaining limits: volume sampling is
  renderer/profile-dependent, vertex-stage texture sampling is absent, and EasyGL effects need
  recreation after a GL context loss. Cube sampling itself is supported on every compiled-effect
  backend.
- The option is **off by default**, so every sample that needs it must configure with
  `-DCNA_EASYGL_COMPILED_EFFECTS=ON` and point `FETCHCONTENT_SOURCE_DIR_FNA3D` at `~/deps/FNA3D`
  (already at the pinned commit) so MojoShader is not re-cloned into the build tree.

Each of the 22 has to be retested on its own evidence, and `DEFERRED.md` #11 rewritten against
this measurement.

### `cnanext` `next` is already red: 15 pre-existing failures, plus 2 more since the `feature/bindings` merge (re-measured 2026-08-28)

A full `CnaTests` run on the SAMPLE-041 head is **8566 passed, 15 failed**. All 15 were confirmed
to fail identically with that day's two commits reverted to `db86bed20`, so they belong to the
branch, not to this campaign's work. Do not spend a session blaming your own change for them:

| Suite | Tests | What it asserts |
|---|---|---|
| `VertexDeclarationLayoutTest` | 6 | declared vertex layouts bind the bytes they declare |
| `DeclarationGuardTest` | 4 | `REMED-GFX-DECL-GUARD` REFUSES a declaration that collides with the renderer's inferred byte-stride table — it is currently **accepting and rendering** one |
| `GpuTimerTest`, `ShadowVisibilityTest`, `TwoProcessLoopbackTest`, 2x `Gltf*` | 5 | unrelated; timing/doc/registry checks |

**Re-measured on 2026-08-28, after `feature/bindings` was merged into `next`.** Two more now
fail and they are NOT from this campaign's work — both were confirmed to fail with every
SAMPLE-048 change reverted:
`GltfRendererPbrFallbackPolicy.EveryPbrVertexPathConsumesWorldViewProjection` and
`.EverySkinnedPbrShaderInverseTransposesTheJointMatrix`. Both assert on a **WGSL** string
(`output.position=u.mvp*`), i.e. the WebGPU renderer's own shader source.

**And six others are pure interference, not failures.** A full-binary run reported
`GltfDrawParamsOracleL6.EveryDrawablePartOfEveryFixtureIsCaptured` and five `GltfConformanceL6.*`
as failed; run with a filter that selects only those suites, **all six pass**. Re-run a suspect set
in isolation before believing a full-run count, exactly as `--rerun-failed -j1` does for ctest.

The `DeclarationGuardTest` arm is the interesting one — its own message says an arm that starts
rendering again means either the real translator landed and the arm is stale, or the guard
regressed. Worth its own ticket; it is not a sample-campaign defect.

**Run the suite with its own display.** A backgrounded `Xvfb` does not survive the tool call that
started it, and when it dies mid-run every remaining test fails with
`AcquireSubsystem(Video) failed: x11 not available` — 1168 such "failures" in one attempt here,
none of them real. Start `Xvfb` and run `CnaTests` inside the **same** command, and check the log
for that string before believing any failure count.

**Run it from the repository ROOT, not from the build directory** (re-learned 2026-08-28). Every
fixture-loading test resolves its `.xnb` path relative to the current directory, so running
`cmake-build-debug/CnaTests` from inside `cmake-build-debug` turns them all red at once: one
filter reported **39 failures** from the build directory and **6** from the repo root, and the 33
difference was entirely `LoadRealMonoGameFixture…`-shaped path failures. The command that works:

```bash
cd /rv/data/development/github.com/openeggbert/cnanext
Xvfb :231 -screen 0 1280x1024x24 +extension GLX >/dev/null 2>&1 & xv=$!
sleep 2
DISPLAY=:231 SDL_VIDEODRIVER=x11 ./cmake-build-debug/CnaTests --gtest_filter='…'
kill $xv
```

**Use a filter; the whole binary does not finish.** An unfiltered run was still going after 3000 s
and had to be killed, hanging inside `MediaLibraryTestFixture` (which also fails on its own here —
it scans the real device's media library). Select the suites your change touches.

**Re-measured on 2026-08-28 at the SAMPLE-051 head**, on the pristine tree with every FX-127 file
reverted, so a new session does not chase them: the 6 `VertexDeclarationLayoutTest` failures are
still there, plus `ShadowVisibilityTest.TheFilterRadiusChangesHowSoftTheEdgeIs`,
`RendererStrideConformance.NoPbrOrSkinnedRecordIsEverReadThroughAnIncompatibleLayout` and three
`GltfRendererPbrFallbackPolicy.*`. **`ShadowVisibilityTest.ASkinnedMeshShadowsItself` is NOT among
them** — it is green on the pristine tree, and that is exactly why it was able to catch FX-127's
missed CNAEXT shader copies. When a renderer change makes a test go red, revert your files and
re-run before assuming it was already broken; both answers happened in that one session.

### Runtime-API sweep across the campaign (2026-08-28)

A second reviewer flagged sample-local reimplementations of runtime functions. Five claims were
checked one by one; **four held, one did not** — the "stale lifecycle diagnosis in Graphics3D's
`missing.md`" was read from the file *before* the SAMPLE-046 commit rewrote it. The measured scope
was larger than the three files named: 10 `std::sto*` parse sites in 5 samples and 90
`std::to_string` sites in 44 files across 22 samples. All of it is now converted.

**Two real sharp-runtimenext defects came out of it, and both were found by measuring, not by
reading.** Converting `snprintf("%02d:%02d")` to the faithful `String::Format("{0:00}:{1:00}")`
would have been a **regression**, because:

1. **`String::Format` silently dropped the format specifier.** It carried a second numeric
   formatter that knew only `X/x/D/d` for integers and `F/G/E` for doubles, so every *custom*
   numeric format fell through to a plain decimal: `Format("{0:00}:{1:00}", 3, 7)` returned `"3:7"`
   where .NET returns `"03:07"`. A wrong answer with no diagnostic.
2. **`Format("{0}", float)` widened to double**, printing `"59.400001525878906"` where .NET prints
   `"59.4"` — the float overload forwarded to the double one.

`Int32::ToString` also lacked the custom-numeric path `Single` and `Double` already had, so
`int.ToString("00")` threw. All three are fixed in `../sharp-runtimenext` with four tests, each
confirmed to fail with its fix reverted; the full suite is **17875/17875**.

**The lesson worth keeping: probe the runtime API before converting to it.** Three assumptions died
in ten minutes here — that `Single::ToString(v,"F2")` would differ from `%.2f` (it does not, on
every midpoint tested), that `ColorReplacement`'s `ToString(x,"0.000")` throws (it does on the older
`sharp-runtime` checkout, but the samples link `sharp-runtimenext`, where it works), and that
`String::Format` honoured its specifiers. A ten-line probe against the tree the samples actually
link settled each one.

**Two traps to avoid repeating.** A sabotage run that does not compile leaves the *previous* binary
in place and the test "passes" — check the build's exit code and the binary's checksum, not just
that a test ran. And `std::to_string` → `Int32::ToString` is behaviour-identical for integers but
**silently truncates a float**; `Spacewar`'s `Add(..., float)`/`Add(..., double)` overloads took
exactly that hit and had to be corrected to `Single`/`Double::ToString`.

**Deliberately left alone.** `PerformanceMeasuring`'s `snprintf` calls stand in for the sample's own
`GameDebugTools/StringBuilderExtensions.AppendNumber`, a helper in the XNA sample itself rather than
a .NET API, so porting it belongs to that sample's own audit (its row is still `⬜`).
`RolePlayingGame`'s `ContentLoader.hpp` is port-only placeholder code for the still-unaudited
SAMPLE-070.

### `xna4-original/` MUST be the whole upstream directory (owner correction, 2026-08-28)

The owner caught this mid-session: *"proc v /rv/tmp/samples \*original\* uz nedavas ty html
soubory?"*. I had been copying only the code and content subdirectories, so **SAMPLE-040 through
SAMPLE-046 all shipped snapshots missing the sample's `.htm` documentation page, its `.sln` files,
`Microsoft Permissive License.rtf` and the upstream screenshot** — 041 also lost a whole
`Documentation/` directory, 040 lost `SourceContent/`, 045 lost `XmlParticles/`. All seven roots
were repaired in place and `rules.md`'s artifact-root table now says so explicitly.

Copy the whole directory and verify it as a set difference, never by eye:

```bash
U=$(find /rv/tmp/XNAGameStudio -maxdepth 3 -type d -name "<Upstream_4_0>" | head -1)
cp -a "$U/." "$ROOT/xna4-original/"
for f in "$U"/*; do [ -e "$ROOT/xna4-original/$(basename "$f")" ] || echo "MISSING $f"; done
```

The `.htm` is easy to overlook because the *port* in `samples/<Port>/` ships it too — check the
artifact root, not the port. And a prune freezes whatever gap exists, so check before pruning.

### Most recent completed sample: SAMPLE-051 CustomModelAnimation

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-051-CustomModelAnimation_4_0
```

- **Read what the old note says the sample NEEDS, not what it says is missing.** This one was filed
  as blocked twice over by DEFERRED #13 for lacking an `AnimationClip`/`AnimationPlayer`
  equivalent. CNA never needed one: those classes are the *sample's own* 400 lines of game code, in
  its own library project, ported like any other game code. A framework-gap claim that names types
  the game itself defines is a category error worth checking for.
- **Decode the reader table before writing a line.** `scripts/dump-xnb-readers.py` (new, reusable —
  it prints an uncompressed `.xnb`'s header and full type-reader table) named both framework gaps in
  ten minutes, the same way SAMPLE-048's two were named.
- **A reflective reader's SHAPE is part of the wire format.** `ReflectiveTypeReaderBuilder` could
  register only the value shape, which is correct for an `.xnb`'s root asset and wrong everywhere
  else: XNA writes a reference type with its own 1-based reader index in front, and `ListReader`,
  `DictionaryReader` and `ModelReader::ReadTag` all branch on whether `T` is `shared_ptr`-shaped.
  The value-shaped reader reads one index short and desynchronises the rest of the payload.
  `RegisterShared()` is the fix; `RegisterShared<System::Object>()` for a `Model.Tag`.
- **A vertex element format describes BYTES, not the shader register.** EasyGL refused a `Vector4`
  BLENDINDICES that real XNA renders — this sample's own processor writes exactly that. FX-127
  unified the read mode: `in vec4 aBoneIndices` on every profile, `Byte4` read as floats. **The
  CNAEXT engine layer keeps its own copies of the skinned vertex shader** (`ShadowMap`,
  `DepthNormalPrepass`); a renderer-side attribute change has to update them too, and
  `ShadowVisibilityTest.ASkinnedMeshShadowsItself` is what says so.
- **Compare the data before the pixels.** `CNA_DUMP` prints every value both engines read out of
  the two `Tag`s — 5388 lines a side — and all of them are bit-identical as `float32`, 0 ULP. 4210
  of those lines differ as *text* (C#'s `"R"` versus C's `%.9g`), so a textual diff would have
  reported 3060 defects that do not exist. Parse floats back to `float32`; never diff them as text.
- **Hold a key press.** `IsNewKeyPress` needs the key down in one polled frame and up in the
  previous one; a 12 ms `xdotool key` can fall entirely between two polls. The first capture here
  recorded A doing nothing while B worked — that is the race, not a bug in the port.

### Previously completed sample: SAMPLE-050 SimpleAnimation

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-050-SimpleAnimation_4_0
```

- **Second sample in a row that needed nothing added to CNA**, and the first with no pipeline
  extension either: one listed asset, `tank.fbx`, through the stock `FbxImporter`/`ModelProcessor`.
- **Dump the structure the old notes described, do not argue with them.** The 2026-07-11 pass built
  a hand-computed `ApplyRestTransforms()` offset table because it believed the reader gave every
  mesh its own bone directly under a synthetic root with an identity transform. A ten-line probe in
  `Load` settled it: the pipeline's `.xnb` carries **12 bones, four levels deep**
  (`tank_geo`→`l_engine_geo`→`l_steer_geo`→`l_front_wheel_geo`), every one with a real translation
  and the tank body itself as `Root`. The probe was run, recorded in
  `evidence/probe/cna-bones.txt`, and removed. Prefer that to reasoning from a symptom.
- **A browser can be pinned to the same instant as a native run.** Everything in this sample
  animates from `gameTime.TotalGameTime`, so the diag hook is a single `CNA_TIME`. A browser tab has
  no environment — so the frozen build also reads the instant out of the page's query string, via
  one `emscripten_run_script_string` under `#ifdef __EMSCRIPTEN__`. That turns the web leg from a
  statistics check into a **pixel-for-pixel comparison against real XNA**: 99.74 % within 8 levels,
  100.00 % after a 4 px blur, centroids within 0.05 px. Reuse this whenever a sample's only
  non-determinism is the clock.
- **Crop what the browser paints.** Chrome's screenshot clip includes the canvas's own 1 px border
  and focus ring, which put non-background pixels in rows 0 and 479 and dragged the blurred
  agreement to 94.73 %. `compare.py --inset 3` drops them; 100.00 % is the real number. A whole-frame
  metric will blame the renderer for the browser's chrome if you let it.
- **Calibrate a gate by breaking what it watches.** The web gate's `greenFraction` is the pin for
  the two FBX-implicit textures. Forcing `TextureEnabled` off in a sabotage build takes it from
  23.35 % to 5.77 % while the silhouette fraction barely moves (22.84 % → 22.56 %) — so the > 15 %
  threshold fails for exactly that defect, and the silhouette check alone would not have caught it.

### Previously completed sample: SAMPLE-049 HeightmapCollision

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-049-HeightmapCollisionSample_4_0
```

- **The first sample that needed NOTHING added to CNA.** Three in a row had found framework gaps;
  this one, with a custom `ContentProcessor`, a custom `ContentTypeWriter` and a game-supplied
  `ContentTypeReader`, ran on what was already there. SAMPLE-048's `Model.Tag` work is why.
- **Compare floats as floats, never as printed text.** The collision-value probe logs differed by
  up to 3e-05 — and that was C#'s `"R"` and C's `%.9g` printing the *same* float with different
  digit counts. One ULP at that magnitude is 6.1e-05, so the "difference" was smaller than a
  representable step; parsed back to `float32` all 81 probes are identical, 0 ULP. A textual diff
  would have reported a defect that does not exist.
- **A game-supplied ContentTypeReader is registered by name, not found.** XNA reflects over the game
  assembly for the name the `.xnb` records. C++ cannot, so the game calls
  `ContentTypeReaderManager::AddTypeCreator("<the name from the .xnb>", ...)`. The hook already
  existed; this is simply the first sample to use it, and it is recorded in the sample's `diff.md`.
- **Do not let a threshold pass by luck.** The browser gate demanded visible black fog in every
  frame; the driven-forward frame legitimately has 0.12 %, because the camera is at the foot of a
  hill that fills the screen. The check now applies to the frames that look at the horizon and says
  why — a gate that fails on a correct picture is as wrong as one that passes on a broken one.

### Previously completed sample: SAMPLE-048 TrianglePicking

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-048-TrianglePickingSample_4_0
```

- **Decode the `.xnb`'s reader table before writing any code.** Both framework gaps this sample
  found were named in ten minutes by dumping the type-reader table out of the built `Sphere.xnb`
  and diffing it against what CNA registers. Guessing would have found the first and missed the
  second.
- **A capability can exist and still never have been exercised.** `Model.Tag` looked supported:
  the property is there, `ModelReader` reads it, there is a test. But `ReadTag` accepted only a
  `std::shared_ptr<System::Object>`, and grepping the whole tree for a reader producing that shape
  turned up exactly one — the test's own fixture. **When a feature's only producer is a test, the
  feature has never run.** Worth checking whenever a "supported" path is about to be relied on.
- **Pick a metric that changes between legs.** Frame agreement was 98.2 % at all four cursor
  positions, which on its own could have meant the hook did nothing. The discriminators were the
  eight distinct frame hashes and the white/magenta pixel counts, which differ per leg
  (1048/0, 973/326, 401/0, 985/27) and match between engines to the pixel. That is what proves both
  engines pick the same triangle; the percentage alone would not have.
- **`compare-frozen.sh` restores the port from `cna-diag/*.orig`.** When a port source changes,
  update the `.orig` in the same breath or the next comparison silently reverts the change.

### Previously completed sample: SAMPLE-047 PickingSample

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-047-PickingSample_4_0
```

- **Third sample running, third time the old notes' "CNA gap" was the bypassed pipeline.** This one
  claimed CNA's model schema could not bind a texture at all, and that every mesh therefore rendered
  as a flat saturated white shape. The official `ModelProcessor` binds the FBX materials' own
  textures — three of which are **not rows in the content project**, the importer resolves them —
  and the table comes out wood-grained. Stop re-diagnosing these; run the pipeline first.
- **A change that moves a number NOT AT ALL is a signal, not a null result.** Adding vertex-colour
  support to the lit shaders left the frame byte-identical, which read as "no effect". It was an
  attribute bound to the wrong slot: the location is the element's **index in that program's own
  `StockProgramInput` table**, not a number the shader may pick. Correcting it took the sphere from
  46.94 % to 99.76 %. When a fix appears to do nothing, check that it is reachable before believing
  the measurement.
- **Isolate until one term is left, then stop guessing.** The residue at other camera angles was
  narrowed to the directional diffuse term on a downward-facing surface by excluding, each with a
  measurement: geometry (coverage/centroid identical to the digit), the ambient term (ambient-only
  agrees on 99.99 %), which light (a single light still diverges), the near plane, the cull mode and
  a mirrored transform (determinants identical and positive). It is filed **open** as `plan_fx.md`
  FX-126 rather than guessed at — the next step is to dump the interpolated normal for one pixel in
  both engines.
- **A shader that fails to compile still links "successfully" enough to keep the old binary.** The
  message is `linking with uncompiled/unspecialized shader`, with the real error a few lines above
  under `VS failed:`. Grep the sample's `run.log`; compile failures print to stderr unconditionally
  even though `CNA_RENDER_LOG` is off.
- **A uniform declared in both shader stages needs the same explicit precision.** The vertex stage
  defaults to `highp`, the fragment stage to `mediump`, and GLSL ES 3.00 refuses the mismatch. That
  file already documents the trap for `uDiffuseColor`; it cost a build here anyway.

### Previously completed sample: SAMPLE-046 Graphics3D

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-046-Graphics3DSample_4_0
```

- **A blocker attributed to a framework bug was, again, the bypassed pipeline.** The 2026-07-09
  notes said `spaceship.fbx` was unreadable binary FBX 6000 and that the ship's invisibility was
  "the pre-existing EasyGL near-plane-clipping framework bug", isolated over a session. The official
  `FbxImporter` reads the file first try, and the ship then renders. That is the second time
  (SAMPLE-041 was the first) that a long, careful isolation of a "renderer bug" turned out to be a
  hand-converted asset. **Run the real pipeline before believing any such note.**
- **Isolate the variable the sample gives you.** This sample's entire state is four checkboxes and
  nothing moves on its own, so a hook per checkbox turns it into a controlled experiment. With any
  ONE of its three lights the frame matched real XNA to 99.99 %; with all three, 90.31 %. That table
  named the defect (a missing [0,1] clamp on an accumulated vertex colour) in one run, where the
  all-three frame alone would only have said "the model is too bright".
- **A uniform signed error across R, G and B is a lost achromatic TERM, not a scaling error.** The
  per-pixel leg was −20.1/−20.1/−20.1: that is a grey specular contribution going missing, which
  pointed straight at `mediump` overflowing on a world-space view vector. With all three lights on,
  the diffuse saturates and hides it (92.40 %) — the **single-light** leg is what makes it legible.
- **Two fixes, and both were the built-in twin of a compiled-effect fix already recorded.** FX-123 is
  FX-122's semantic (D3D9 saturates `oD0`/`oD1`) in EasyGL's own shaders; FX-124 is FX-121's
  (`mediump` is fp16 range) in the same place. When a compiled-effect defect is found, **check the
  built-in path for the same hole** — twice now it was there.
- **A black capture is not always a black frame.** CNA maps `IsFullScreen` to SDL's *exclusive*
  fullscreen where FNA asks for the desktop mode; with no window manager on Xvfb the switch times
  out twice at ~5 s and SDL reverts. An 8 s capture landed inside that stall and produced a solid
  black PNG that looked exactly like "the sample draws nothing" — which is the shape of the old
  port's own report. Wait past it, and check `run.log` for `Time out elapsed after mode switch`.

### Previously completed sample: SAMPLE-045 XmlParticles

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-045-XmlParticles_4_0
```

- **A sibling sample can be mostly a copy — check first.** This is SAMPLE-043's engine with
  `ParticleSystem` made concrete and settings moved to XML. Three upstream files are **byte-identical**
  (`diff` them before porting anything) and the port reuses them unchanged.
- **The reflective wire order is serialized PROPERTIES first, then public fields.** SAMPLE-044's
  type had no serialized properties so "declaration order" held there; this one marks its
  `BlendState` `[ContentSerializerIgnore]` and serializes a private `[ContentSerializer]` string in
  its place, which comes out **ahead of every field**. Decoding with the wrong assumption produced
  a plausible-looking mess (`MaxParticles` = 1768293378); with the right one it lands on the file's
  last byte. **Decode one real file before writing the member list** — it is ten minutes and it is
  the difference between a reader that works and one that reads the wrong bytes.
- **The game-clock change is visibly doing its work.** At update 180 all five systems now hold
  identical queues AND identical clocks to the digit; before it, the trail differed by 6 particles
  in 806 at the same point.
- **State can be exact while frames still differ, and that is worth saying out loud.** 80.08 %
  within 8 levels at 180 updates, but a blur does not help, the differing pixels are *less*
  edge-prone than average, the median difference is one level and 98.5 % are within 32. That is
  accumulated blending across dozens of translucent sprites — not a simulation difference. Use the
  blur test and the edge-enrichment ratio to tell the two apart before reaching for a cause.

### THE GAME CLOCK CHANGED (2026-08-27) — read this before comparing any sample

On the owner's decision that **the XNA 4.0 C# original is authoritative over FNA**, `cnanext`'s
`Game::Tick()` now follows XNA's clock instead of FNA's:

1. the game's **first** update runs with `ElapsedGameTime = TimeSpan.Zero`;
2. `TotalGameTime` is the time **before** the step, so it advances once `Update` returns — in the
   fixed AND the variable path.

Both were measured on the real XNA runtime through SAMPLE-044's `CNA_PROBE`/`CNA_VARIABLE` hooks,
in both timing modes, not inferred. FNA does neither (`FNA/src/Game.cs:475`), which had left a CNA
game two fixed steps ahead of XNA's at the same update index.

**What this means for you:** any earlier sample whose comparison was limited by accumulating
timing drift may now agree better — SAMPLE-044 went from a broken comparison to **100.00 %**, and
SAMPLE-043's 87.7 % at 180 updates should be re-measured if its root is ever rebuilt. Of the two
rules the `TotalGameTime` lag is the one that moves a simulation: reverting only the first-update
rule still scored 99.99 % on SAMPLE-044.

`modules/runtime/tests/.../GameClockFirstUpdateTests.cpp` pins all of it.

### Previously completed sample: SAMPLE-044 Particles2DPipeline

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-044-Particles2DPipeline_4_0
```

- **The row's subject was a content-pipeline bypass, and the previous port admitted it.** Its own
  notes said the four settings XML files were "hand-translated to C++ construction code" because
  "CNA has no general content-pipeline deserializer for custom types". They are now built by the
  real pipeline and loaded from `.xnb`.
- **A reflectively-written `.xnb` IS readable, and the format is simple.** Value-type fields inline
  in declaration order; a reference-type field preceded by the 1-based index of its own type
  reader. Decoding `ExplosionSettings.xnb` by hand ended exactly on its last byte with every value
  matching the XML — do that first, then write the reader against what you measured.
- **CNA now has a reflective-reading layer — use it, do not hand-write a reader.** On the owner's
  decision, `cnanext` gained `Microsoft::Xna::Framework::Content::ReflectiveTypeReaderBuilder<T>`:
  declare the type's fields once, in declaration order, and it builds the reader, derives the
  canonical reader name (CNA strips the assembly qualifiers, so nobody has to guess the string
  again) and registers the `EnumReader`s the `.xnb`'s table names. That last part is the one
  easiest to miss: **the type-reader table must resolve in full before any object is read**, even
  for readers your own code never dispatches to.
- **The field list still comes from the game.** This is not reflection — C++ cannot introspect a
  type at run time. What it removes is the *duplication*: the order lives in the type's
  declaration and in one `.Field(...)` chain, instead of in a hand-written reader that can drift.
  Real reflection would need a libclang generator over the headers.
- **Check a hook is really in BOTH sources before believing a bad number.** The first frozen
  comparison scored 38.66 %, and the cause was a `CNA_FRAMES` edit that had silently failed to
  land in the XNA diagnostic source: the original was running unfrozen against a frozen CNA.
- **The browser harness now polls for `#canvas`** instead of sleeping a fixed 15 seconds. A bigger
  content bundle takes longer to instantiate, and the fixed sleep turned that into a crash in the
  first `evaluate()`.

### Earlier completed sample: SAMPLE-043 Particles3D

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-043-Particles3DSample_4_0
```

The biggest sample so far, and the one that found something that reaches past its own row:

- **XNA and FNA advance the game clock differently, and CNA follows FNA.** XNA's **first** update
  runs with `ElapsedGameTime = 0`, and its `TotalGameTime` lags its update count by one further
  step: after N updates XNA has accumulated N-2 fixed steps where CNA has accumulated N. FNA sets
  `ElapsedGameTime = TargetElapsedTime` for every update including the first
  (`FNA/src/Game.cs:475`) and advances `TotalGameTime` before calling `Update`, and CNA's
  `Game.cpp` is a faithful port of that loop. **Left unchanged**: `CLAUDE.md` names the FNA tree as
  the authoritative behavioural reference, and this is the core game loop, so changing it would
  shift the timing of every sample already verified. It is recorded because **it applies to any CNA
  game whose state accumulates over frames**, and it is what limits this sample's comparison.
- **A blur that does not help is the signal that a difference is real.** SAMPLE-042's residue rose
  from 89 % to 98.6 % under a 4 px blur, which said "boundary noise". Here 87.7 % goes only to
  88.7 % and the differing pixels show **no** edge enrichment (20 % against 21 % of the frame) --
  which said "the particles are genuinely elsewhere" and sent the investigation to the clock.
- **Probe the state, not just the picture.** Printing each system's queue (`active`/`new`/`free`/
  `retired`) every 60 draws showed four of five systems identical and the trail differing by 6
  particles in 806 -- exactly two frames of its 200/second rate. That is what turned a vague 12 %
  pixel difference into an exact two-frame clock offset.
- **Three framework gaps, all "XNA API shapes CNA had not needed yet":** the windowed
  `DynamicVertexBuffer.SetData<T>(offsetInBytes, …, SetDataOptions)`; `using VertexBuffer::SetData`,
  because C++ name lookup stops at the first scope that declares the name and the derived class's
  own overloads were hiding every inherited one; and `Vector3::operator*=`/`/=`, which `Vector2`
  already had. Expect more of this shape in the remaining samples.
- **Packed vector types must never sit in a vertex struct.** `Short2` and `Color` inherit CNA's
  polymorphic `IPackedVectorT`, so an object of either carries a vtable pointer. Hold the raw
  32-bit packed value and use the type only to compute it -- and `static_assert` the struct's size
  and every member offset, so the build breaks instead of the picture.

### Earlier completed sample: SAMPLE-042 ShatterEffect

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-042-ShatterEffectSample_4_0
```

The port needed no framework change; four things are worth carrying forward:

- **Another stale placeholder blocker.** It claimed `ShatterEffect.fx` had to be hand-translated to
  GLSL plus a `.shader.json` descriptor. Nothing is hand-translated — the official pipeline
  compiles the `.fx`. That is now three consecutive samples whose recorded "CNA gap" was obsolete.
- **A content processor can be the whole difficulty, and this one was handled already.**
  `ShatterProcessor` splits every triangle into a disconnected copy and adds two per-triangle
  channels (`TriangleCenter`/TEXCOORD1, `RotationalVelocity`/TEXCOORD2). CNA read the resulting
  five-channel declaration and bound every channel by semantic into a compiled custom effect with
  no changes.
- **Blur before concluding that a low percentage means a wrong picture.** The unblurred agreement
  here is 89-97 %, the lowest of the campaign. A **4 px Gaussian blur removes sub-pixel boundary
  noise but preserves any real displacement**, and under it the same frames agree to 98.3-99.7 %.
  Combined with coverage tracking to 0.6 %, an identical row span, a median difference of 0 and
  95-99 % of differing pixels on an edge, that is a rasterizer disagreeing about boundary pixels on
  thousands of *disconnected* triangles — not a maths difference. The differing/covered ratio
  saturating (17 % → 30.6 % at `time = 0.02` → ~43 %) rather than growing says the same thing.
- **Measure a share, not a count, when the thing you are watching changes size.** The WEBGL2 gate's
  first version counted model pixels in the lower half of the screen and failed on a working
  sample: a collapsed pile covers a third of the pixels the standing tank does, so the count moves
  the wrong way at the end of the animation. The share (0.33 → 0.58 → 0.96) is monotonic.
- **`hold`, not `tap`.** This sample advances only while a key is held, so the capture scripts grew
  a `hold <key> <seconds>` helper; a tap moves its clock by about one frame.

### Earlier completed sample: SAMPLE-041 LensFlare

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-041-LensFlareSample_4_0
```

This was a **re-port**, and the most useful thing it produced is a warning about the old ports:

- **Two of the previous port's three recorded framework bugs were not real.** It was built on a
  hand-converted `terrain.model.json` rather than the official pipeline, and from that bypass it
  concluded that EasyGL had a near-plane clipping bug (filed as confirmed against "a second,
  independent asset") and that `.model.json` could not carry a texture. Through the real pipeline
  the terrain renders correctly and `ground.png` is bound by the FBX material — six XNBs come out
  of five listed assets. Its third finding, EasyGL ignoring `ColorWriteChannels`, had been fixed
  since. **A bypassed content pipeline manufactures framework bugs that are not there**, and an
  old `missing.md` is evidence to retest, never a verdict.
- **The real finding: `OcclusionQuery.PixelCount` is a boolean on OpenGL ES 3.0 and WebGL 2.**
  Their core occlusion target is `GL_ANY_SAMPLES_PASSED`, which answers 0 or 1 however much was
  covered. This sample divides `PixelCount` by an area to get a coverage ratio, so it got
  `1/10000` and faded its glow and flares to nothing. Everything else in the frame was correct.
- **The way that was established is worth copying.** Not by reading code: by measuring
  `occlusionAlpha` (exactly `1/queryArea`), removing the depth test (no change, so not the depth
  comparison), then drawing the query quad **visibly** — 9788 pixels, exactly where the sun is, so
  geometry and projection are right and only the query answers wrongly. Then feeding the component
  the ratio those 9788 pixels represent: the frame agrees with the original to **99.74 %**, mean
  0.58/255, median 0. That last step is what separates "the port is wrong" from "this renderer
  cannot answer the question", and it is cheap.
- **A test that asserts only `> 0` cannot tell a count from a flag.** Both existing EasyGL
  occlusion tests do exactly that, which is why `docs/occlusionquery-support.md` called EasyGL
  "fully correct" for two months. The new test requires the value and the precision claim to agree.
- **Prove the arm that never runs here.** EasyGL now asks the driver for `GL_SAMPLES_PASSED` and
  falls back; on `OPENGLES3` that arm never executes, so it would have been unproven dead code.
  `spikes/occlusion-count-spike/` measures the same Mesa accepting it under desktop GL 4.5 and
  reporting 4096 fragments for a covered 64x64 viewport.
- **`cna-samples` FORCEs its renderer** (`CNA_GRAPHICS_RENDERER` is set with `FORCE` in the root
  `CMakeLists.txt`), so `-DCNA_GRAPHICS_RENDERER=OPENGL33` on the command line is silently ignored
  and you get another OPENGLES3 build. Check the renderer banner in `run.log` before believing a
  second-profile build did anything.

### Earlier completed sample: SAMPLE-040 InstancedModel

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-040-InstancedModelSample_4_0
```

Five things to carry forward:

- **A placeholder's `missing.md` is stale evidence, not a verdict.** This one said the sample was
  blocked on its custom `InstancedModel.fx`. Compiled custom effects have worked since SAMPLE-032,
  and the port rendered correctly on its first run. Retest the claim before believing it — the
  actual gap was somewhere else entirely.
- **The gap was `DynamicVertexBuffer.SetData<T>` with `SetDataOptions`.** CNA had it for the four
  built-in vertex types only, each of which packs the C++ object into a compact GPU stream first. A
  game's own element type — here an array of plain `Matrix` for the per-instance stream — had no
  streaming upload path at all. Added along with `VertexBuffer::SetDataRawWithOptions`; an
  application-defined type has nothing to pack, so the declaration must describe exactly
  `sizeof(TVertex)`, which the existing raw-upload validation already enforced.
- **Hardware instancing through a compiled effect already worked.** `VertexBufferBinding` with an
  instance frequency, `SetVertexBuffers`, `DrawInstancedPrimitives`, and EasyGL binding the
  effect's `BLENDWEIGHT0..3` attributes to the second stream at divisor 1 (`plan_fx.md` FX-082) all
  behaved correctly with no change.
- **`System::Random` matches .NET's sequence, and this sample proves it.** sharp-runtime's is a
  byte-for-byte port of the Knuth subtractive generator, so seeding both engines identically places
  all 1000 instances in the same spots. That is what made a pixel comparison possible at all — and
  it is worth remembering as a tool: when a sample randomizes at RUN time, seed it rather than
  giving up on comparing.
- **A falling agreement percentage is not automatically drift.** 99.26 % at 2 s, 95.24 % at 8 s,
  93.46 % at 20 s looks like divergence and is not: coverage tracked to 0.16 %, the centroid of
  everything drawn stayed under half a pixel apart, and the share of differing pixels on an edge
  stayed pinned at 99.6 % while edge density tripled as the spiral spread out. Measure coverage and
  centroid before concluding that a number falling over time means the maths is drifting.

### Earlier completed sample: SAMPLE-039 BillboardSample

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-039-BillboardSample_4_0
```

The port needed no framework change at all. Everything below is about the investigation, which
went wrong for a long time for one reason worth carrying forward:

- **A content processor may be non-deterministic, and then the two engines MUST be handed the
  byte-identical XNB.** `VegetationProcessor` scatters ~41 300 grass billboards and 77 trees from
  an unseeded random source. The port had been built from its own pipeline run, so CNA and the
  original were rendering *different landscapes*, and every measurement taken against them was
  noise: 74.9 % whole-frame agreement variously "explained" by wind, the alpha test, the depth
  test and billboard sizing, plus an apparent `squishFactor` defect (XNA's trees tall and slender,
  CNA's short and fat) that was simply two different sets of `Random` values. **Before comparing
  anything, `md5sum` the content both binaries actually load.**
- **The tell was in the model, not the picture.** The mesh parts held 165 296/300/4 vertices in
  XNA and 165 276/308/16 in CNA -- same total, different split. That is not something a renderer
  can cause. Dump the structural numbers early; they falsify a content mismatch in one step, while
  pixel percentages will happily support any hypothesis you bring them.
- **`mesh.Effects` does not enumerate the parts in the same order in the two engines.** A
  diagnostic that hides "all parts except index N" therefore hides a *different* part in each, and
  it reported 99.95 % agreement between two frames that visibly showed different objects. Select
  by a value the part carries itself (here `BillboardWidth`), never by position. The ordering does
  not affect the sample, which sets the same value on every effect.
- **Pin the animation in BOTH engines, and check that you did.** The `CNA_WIND` hook existed only
  on the XNA side for most of this work; CNA was still running live wind, which alone accounted
  for 8.6 points of disagreement and made CNA look non-deterministic (96.4 % against itself). With
  the hook on both sides, both engines are bit-exact run to run and agree to 95.6 %.
- **A gate whose number never moves is not measuring anything.** The WEBGL2 cutout check scored an
  identical 47 076 on all five frames because the captured clip starts on a non-sky row, so the
  "vegetation edge" was found at row 0 in every column. Calibrate a new metric against a frame you
  already trust -- both engines' native frames score 169 and 197 -- and confirm it moves.
- **HiDef is enforced twice.** `BuildContent` refuses the 82 668-triangle mesh part under Reach,
  and the loader independently refuses HiDef content unless the executable's embedded
  `Microsoft.Xna.Framework.RuntimeProfile` resource says `Windows.v4.0.HiDef`.
- **Mono's `mcs` emits `Array.Empty<T>()` for an empty `params` argument**, which .NET 4.0 does not
  have. The pipeline runner uses explicit overloads instead of a `params` array.

### Earlier completed sample: SAMPLE-038 ShadowMapping

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-038-ShadowMappingSample_4_0
```

Three things to carry forward, and **one open decision for the owner**:

- **Direct3D 9 expands a sampled texture's missing channels and OpenGL does not** -- one-channel to
  `(R,1,1,1)`, two-channel to `(R,G,1,1)`. CNA was FNA-faithful here and still wrong against XNA:
  this sample's shadow-map preview was a red square where the original's is white. Fixed in the
  sprite shader, from the bound texture's own `SurfaceFormat`, **not** with
  `GL_TEXTURE_SWIZZLE` -- that is exactly D3D9's rule and covers every sampling path, but WebGL 2
  does not have it (measured: `texParameteri` raises `INVALID_ENUM`), and a swizzle fix would have
  split this campaign's native and web targets. A custom effect still sees GL's expansion, since
  CNA does not author that shader.
- **A uniform location belongs to the program it came from.** Caching the expansion's two
  locations from the sprite program broke `WeightedBlendedTransparencyTest`: a SpriteBatch drawn
  with a custom `ShaderEffect` runs THAT effect's program, and a foreign location is
  `GL_INVALID_OPERATION` -- which surfaced two passes later as "native GL errors were pending
  before MRT setup", nowhere near the cause. Look uniform locations up on the program in use.
- **Test failures in this suite vary run to run.** Four different pairs failed across four runs of
  the same filter, each passing in isolation. Confirm a suspected regression by reverting the
  change and rebuilding -- and check the BUILD exit first, or the run silently uses the previous
  binary.
- **Filtering residue scales with minification, and saying so needs a band breakdown.** This
  sample's frame is mostly a minified checkerboard, so a whole-frame "92 %" reads worse than the
  earlier samples' 99 % while describing the same class of difference. The honest number is the
  gradient: 4.63/255 mean difference in the far third against 1.95 near the camera, where the
  floor's pixels are exactly equal. Measure by band before concluding a renderer is wrong.
- **A content project's processor parameters can be plural.** Each model here carries both
  `CustomEffect` and `Scale`; the runner's `Asset()` helper now takes name/value pairs.

### Earlier completed sample: SAMPLE-037 RimLighting

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-037-RimLighting_4_0
```

Four things to carry forward:

- **A blocker recorded in an old `missing.md` is a claim to retest, never a fact.** This port's
  2026-07-10 notes documented `Content.Load<TextureCube>` and `Content.Load<Model>` as
  unavailable and hand-built both assets. Both readers exist in `cnanext` now; checking took two
  greps. Twenty-two other `missing.md` files still blame custom `.fx` (DEFERRED.md item #11) and
  are due the same treatment.
- **The D3D9 COLOR-register clamp bites CNA's own stock-effect shaders too**, not only
  MojoShader's translations. `EnvironmentMapEffect` carries its fresnel term in `COLOR1` and uses
  it as a `lerp` weight; unclamped it extrapolates past the environment map. That is FX-122's
  distinction in a second place, one sample later. When a stock effect routes a value through a
  `COLOR` semantic in FNA's `.fx`, CNA's GLSL has to clamp it at the vertex.
- **Check whether the original can be driven at all before comparing interactive frames.** XNA
  fills `TouchPanel` from a real digitizer only, so this Windows Phone sample ignores a mouse
  entirely under Wine: seven frames, byte-identical to the start frame, across every click and
  drag. CNA responded to the same script, which reads like a CNA bug until the original's own
  frames are diffed against each other. A diagnostic hook that pins the state in BOTH engines is
  the way to compare something the original cannot be driven into.
- **A content project's processor PARAMETERS are part of the content.** `head.fbx` carries
  `DefaultEffect=EnvironmentMapEffect`; without it the model's materials are `BasicEffect` and
  the game's cast fails. The runner passes them as `ProcessorParameters_<Name>` metadata.

### Previously completed sample: SAMPLE-036 VertexLighting

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-036-VertexLightingSample_4_0
```

Three things to carry forward:

- **A sibling sample can share source files verbatim.** `SampleCamera.cs` and `SampleGrid.cs` are
  byte-identical to SAMPLE-035's except for the namespace line, so the C++ ports were carried
  over with that one line changed -- and the check that it stayed honest was `diff | grep -c
  '^[<>]'` returning 2 per file, matching the originals. Look for this before re-porting 666
  lines by hand; SAMPLE-037 RimLighting may well be another.
- **Fix the framework and the next sample is free.** This one needed no runtime change at all --
  the first compiled-effect sample in the campaign for which that is true. Its `VertexLighting.fx`
  writes a `COLOR0` vertex output summing ambient and diffuse, exactly the shape FX-122 had been
  rendering wrong one sample earlier.
- **Prune with the tool, never by hand** (`tools/prune-completed-sample.sh`). A hand prune keeps
  only the executable and deletes the `Content/` staged beside it, so the binary aborts with
  `ContentLoadException: Cannot open file: Content/...`. If the tool refuses with "plan.md row
  does not name samples/<Name>/missing.md", fix the row -- do not work around it.

### Previously completed sample: SAMPLE-035 PerPixelLighting

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-035-PerPixelLightingSample_4_0
```

Three things to carry forward:

- **Two GL ES translation gaps in a row, both silent, both in the same place.** SAMPLE-034 found
  fragment precision (FX-121); this one found that a vertex COLOR output is not clamped
  (FX-122). MojoShader's desktop GLSL path gets both right by leaning on desktop GL defaults
  (`gl_FrontColor` clamping, `mediump` never being fp16); the GLSL ES profiles get neither. When
  a compiled-effect sample renders *almost* right on `OPENGLES3`/`WEBGL2`, suspect a D3D9
  semantic the ES path drops rather than the sample's own maths.
- **Read what the channels say.** FX-122 was named by noticing that red and green matched the
  original EXACTLY, pixel for pixel, and only blue differed -- and only where blue would exceed
  1. A per-channel dump across a scanline cost one command and pointed straight at a clamp;
  a whole-frame difference count would only have said "the cone is wrong".
- **In MojoShader, `ctx->attributes` does not know about output registers while `main()` is
  still open.** `process_definitions()` fills it, and that runs after `emit_GLSL_end()` has
  closed the function. A first version of the FX-122 patch keyed on `attributes`, compiled,
  ran, and emitted nothing; `used_registers` is filled during parsing and is the list to use.

### Previously completed sample: SAMPLE-034 NormalMapping

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-034-NormalMappingSample_4_0
```

Three things to carry forward:

- **A frame can be lit by nothing and still look lit.** Compiled-effect fragment shaders were
  translated at GLSL ES's `mediump` default, which guarantees only fp16 RANGE, while a Direct3D 9
  shader computes in full 32-bit float. This sample normalizes an interpolated WORLD-SPACE light
  vector in the pixel shader, so `dot(v, v)` reached ~10^6, overflowed to infinity,
  `inversesqrt` returned 0, and `normalize` handed back the zero vector -- taking the diffuse AND
  the specular term to exactly zero. Nothing errored. The model drew, the camera responded,
  `AmbientLightColor` still worked, and the frame read as a dim but plausible render; the tell was
  that a full rotation of the light changed **not one pixel**. Fixed in `cnanext` as a second
  MojoShader patch (`plans/plan_fx.md` FX-121). When lighting looks flat, move the light and
  count changed pixels before believing the render.
- **Bisect a value's journey layer by layer, and instrument each layer where it actually lives.**
  Five successive measurements said the light position was correct -- at CNA's runtime boundary,
  in the vertex register file, in the GL uniform read back with `glGetUniformfv`, unaffected by
  writing the register by hand, and unaffected by swapping the normal map for a texture known to
  sample. Only the last two could point past the uniform and into the fragment shader. Cheap
  probes in the right places beat reading the translator's source, which looked correct
  throughout because it WAS correct.
- **`BuildConfiguration` has no default answer** -- see the SAMPLE-032 entry below, now corrected:
  Debug and Release each fail for a different sample, for opposite reasons.

### Previously completed sample: SAMPLE-033 NonPhotoRealistic

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-033-NonPhotoRealisticSample_4_0
```

Three things to carry forward, all of which cost real time here:

- **The browser gate was blind to WebGL errors.** Chrome delivers driver errors through the CDP
  `Log` domain (`Log.entryAdded`), NOT `Runtime.consoleAPICalled`. Five of six presets rendered a
  black frame on `WEBGL2` and nothing at all reached the evidence until the smoke script
  subscribed to both. It does now (`scripts/chrome-smoke.mjs`); keep that when copying it
  forward, and read those entries before instrumenting the renderer.
- **A native pass says nothing about the web.** Everything here was 97–99 % against XNA on
  `OPENGLES3` from the first run, while `WEBGL2` drew nothing for every preset that used a render
  target. WebGL 2 validates bindings desktop GL leaves undefined -- run the browser gate before
  believing a sample is done.
- **Freeze everything non-deterministic, not just the obvious one.** This sample spins its model
  AND re-offsets its sketch pattern from `Random`. With only the rotation frozen the `Pencil`
  preset compared at 0.2 % of pixels within 8 levels; with the jitter frozen too, 98.8 %. Ask what
  else in the frame comes from an RNG or a clock before capturing.

And one measurement worth remembering when reading a comparison: **toon shading amplifies
sub-pixel differences**. Quantised lighting bands turn a hair's difference in an interpolated
normal into a full band step, so the toon preset carries the most large-delta pixels of the six
while still matching on 97.5 % within 8 levels. Judge those by cluster size, not by the count.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-033-NonPhotoRealisticSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine NonPhotoRealistic.exe
```

Controls: `A` cycles the six presets; Escape or Back exits.

### Previously completed sample: SAMPLE-032 DistortionSample

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-032-DistortionSample_4_0
```

Two things here are new to the campaign and will come up again:

- **A sample can ship its own content pipeline extension.** Three of this one's models are built
  by `DistorterModelProcessor`, which lives in `DistortionPipeline.csproj`, not in XNA. The
  recipe: compile that assembly with the in-prefix `csc.exe` against the pipeline DLLs, then list
  it in `PipelineAssemblies` beside the stock importers, and **run the runner from the content
  directory** -- a processor may name an `ExternalReference` relatively, and the pipeline resolves
  those against the current directory. `scripts/build-original.sh` does all three.
- **`BuildConfiguration` is not cosmetic, and it has no default answer.** `EffectProcessor.DebugMode`
  defaults to `Auto`, which skips shader optimization for a Debug build. That cuts both ways and
  both cases are now on record. This sample's own `Distorters.fx` needs 73 arithmetic slots against
  the 64 its `compile ps_2_0` allows, so **Debug fails and Release compiles it**. SAMPLE-034's
  `NormalMapping.fx` is the reverse: the optimizer folds `pow(rDotV, SpecularPower)`, whose base can
  be zero, through `log(0)` into an infinity literal, so **Release fails with `error X4579` and Debug
  compiles it**. Measure the configuration per sample -- do not carry the previous sample's answer
  forward -- and when a shipped `.fx` will not build, suspect the configuration before the toolchain.

And one lesson about reading a symptom:

- **A frame that is black and stays black may be a dead process.** The Displacement-Mapped map
  view looked like "renders nothing"; it was a segfault, and `coredumpctl info <pid>` gave a
  fully symbolized stack in seconds with no gdb installed. Reach for that before instrumenting.
  What it found was a use-after-free that had been in `EffectMaterialReader` since SAMPLE-028 and
  affected **every** model whose material carries a texture -- `EffectParameter` stores a raw
  `Texture*` and the reader pointed it into a value table that dies with the read.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-032-DistortionSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine DistortionSample.exe
```

Controls: `A` cycles the three distorters, `B` shows the raw distortion map, `X` toggles the
blur, left/right rotate the camera; Escape or Back exits.

### Previously completed sample: SAMPLE-031 BloomSample

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-031-BloomSample_4_0
```

Every claim in the old placeholder record was stale, and the sample found **three** framework
defects, all fixed in `cnanext`. What is worth carrying forward:

- **A contract can pin a behaviour on one route and miss it entirely on another.** EasyGL
  corrects a render target's bottom-up row order twice on the `SpriteBatch` + compiled-`.fx`
  route -- once in the sprite's vertex UVs, once in a row-reversed copy of the source -- and drew
  the whole scene upside down. `FX-099`'s contract had pinned exactly this behaviour for a year,
  through `DrawUserPrimitives`, where the game supplies its own vertices. When a fix is written
  for one draw route, ask which OTHER routes reach the same code.
- **FNA is not the specification when a live XNA build can be asked.** FNA advances a line's
  first glyph by `Math.Abs(kerning.X)`; XNA clamps with `Max(kerning.X, 0)` and applies no
  `Spacing`. Six of this font's 95 characters have a negative left side bearing, and two of them
  start lines of the sample's overlay. `rules.md` ranks a real XNA 4.0 build above FNA for
  observable behaviour -- use that ranking. The probe that settled it is worth reusing: build the
  same `.spritefont` three times with `<Spacing>` 0, 3 and -2, and dump `MeasureString` from both
  engines (`scripts/DiagPipelineRunner.cs`, the `CNA_MEASURE` hook, and
  `scripts/dump-spritefont-xnb.py` for the raw kerning table).
- **A fix applied to one overload can leave its twin behind.** `SAMPLE-018` made `Draw` carry
  sub-pixel destinations; `DrawString` kept rounding glyph positions to whole pixels for another
  eight samples. Grep for the helper a fix removes, not just the call site it changes.
- **Ask whether the frame is deterministic before designing the comparison, twice.** The native
  comparison froze the rotation in both engines and could then compare whole frames. The browser
  run is not frozen, and the first web gate failed on two thresholds copied from the frozen
  numbers -- the fix was to assert on the top eighth of the frame, sunset sky the tank never
  reaches, which measured 131.7 on all four bloomed captures.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-031-BloomSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine BloomPostprocess.exe
```

Controls: `A` cycles the six bloom presets, `B` toggles the postprocess, `X` steps through the
four intermediate buffers; Escape or Back exits.

### Previously completed sample: SAMPLE-030 CameraShake

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-030-CameraShake_4_0
```

All **eight** recorded deviations were gone once the sample was ported faithfully, and four of
the records had already diagnosed themselves as porting shortcuts rather than framework limits.

Two things learned here:

- **A framework bug recorded against converted assets may be a bug in the conversion.** This
  sample's last entry described the whole 3D scene collapsing to "a white stripe on all CNA
  backends", never root-caused, and pointed at near-plane clipping of a ground corner at
  (6554,0,6554). It does not reproduce. The port loads the official `Ground.x` Model XNB instead
  of a hand-converted `ground.model.json`, and the old entry's arithmetic ignored the
  `Matrix.CreateScale(.1f)` the game draws the ground with -- that corner is at 655 units with
  the camera at 1000. Nothing in CNA had to change. Before hunting a renderer for a defect
  recorded against `.model.json`/`.bin` geometry, load the real asset and see whether the defect
  is still there.
- **A deterministic sample is worth recognising.** After two samples where nothing was
  reproducible, this one has a fixed camera and two motionless models: 88.9 % of pixels landed
  within 8 levels of the original with **zero** clusters of >40-level difference, and both
  engines return byte-identically to the idle frame after a shake. Check whether a sample is
  actually deterministic before building elaborate machinery to work around randomness.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-030-CameraShake_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine CameraShake.exe
```

Controls: A is a short camera shake, X a long one (both read on the key-press edge); Escape or
Back exits.

### Previously completed sample: SAMPLE-029 ParticleSample

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-029-ParticleSample_4_0
```

Five of its six recorded deviations were false, including a second instance of the exact excuse
SAMPLE-026 was corrected for -- a record that says in as many words that a faithful port "would
render correctly on EasyGL, only Vulkan would break" and keeps the workaround anyway. When you
meet that sentence again, the answer is already settled.

Two things learned here:

- **Try the measurement, then believe the measurement -- including about your own method.** Three
  ways of comparing this sample's overlay were tried. Per-pixel minimum over a burst, to subtract
  the moving particles, is a good trick and it failed: the plume is *alpha-blended* smoke, so it
  darkens white text instead of only brightening it, and the minimum ate the text too. The
  measurement said so immediately; the mistake was assuming additive behaviour, not the method.
- **A vacuous pass is worse than a failure.** Excluding smoke-covered pixels from the numeric
  columns produced "0 differ" -- out of 0 pixels compared, because all 5500 of them are under
  smoke in both engines. It is written down in that sample's `missing.md` precisely so the shape
  is recognisable: always print how many pixels a comparison actually compared.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-029-ParticleSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine ParticleSample.exe
```

Controls: space or the gamepad A button switches between the explosion and smoke-plume effects
(read on the key-release edge); Escape or Back exits.

### Previously completed sample: SAMPLE-028 ColorReplacement

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-028-ColorReplacementSample_4_0
```

**A fifth defect surfaced after this sample was accepted and pushed**, reported by the owner:
body-coloured outlines showing through the car's glass, and light lenses rendered in the body's
colour. It was real. MojoShader ends every vertex shader it generates with Direct3D 9's
clip-space depth conversion (`z = z*2 - w`); **EasyGL's own stock shaders do not** -- they emit the
XNA projection's D3D-style z unchanged, so ordinary geometry occupies the upper half of the depth
range. Compiled-effect geometry was therefore depth-tested on a different scale and won where it
should have lost. `EasyGLRenderer::SetCompiledEffectDepthRangeEXT` now narrows the GL depth range
to `[(min+max)/2, max]` for the duration of a compiled-effect draw, which makes the two encodings
agree exactly; the derivation is in that method's own comment.

Four things learned here:

- **When two runs cannot be aligned, freeze the thing that moves.** Half a day of this
  investigation produced nothing usable because the car rotates on `TotalGameTime` and two separate
  processes are never in the same phase -- every comparison was contaminated. A temporary
  `CNA_FREEZE` knob in *both* engines, pinning the rotation to a fixed angle, turned a noisy
  6000-pixel difference into exactly four clusters. Do that first, not last.
- **Check that a probe measures what you think.** Two of mine did not: a `BasicEffect` re-draw of
  the body rendered at a completely different scale, and the first inverted-depth probe counted
  green pixels the *normal* pass had already painted, so both engines "agreed" on a number that
  meant nothing. The fix was to give the probe its own colour and to verify the probe's own output.
- **A capability can be present, documented and still never have been reached.** CNA had compiled
  effects, `EffectMaterial`, and a test file for it. What it did not have was XNA's
  `protected Effect(Effect cloneSource)`, so `EffectMaterial` was built on the device-only
  constructor and cloned nothing. The existing test asserted the clone's *type name*. When a type
  exists with tests and still does not work, read what the tests actually assert.
- **Ask the file, not the framework.** "Which readers does this model need" was answered by
  decoding `Car.xnb`'s own type-reader table -- ten entries, seven registered -- not by guessing
  from the crash message, which named only the first one.
- **Differential testing against the reference beats expectations.** The custom-format work was
  checked against mono on 28 value/format pairs. That is what caught `"Fx"`: three pinned
  sharp-runtime tests asserted it throws, and .NET returns `"Fx"`.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-028-ColorReplacementSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine ColorReplacement.exe
```

Controls: hold R, G or B and press Up/Down to change that channel of the target colour; Escape or
Back exits.

### Previously completed sample: SAMPLE-027 FuzzyLogic

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-027-FuzzyLogicSample_4_0
```

Three things learned here:

- **"C++ cannot express this" is a claim to test, not to accept.** The old record renamed the
  sample's `Mouse` entity to `MouseEntity` for a collision with `Input::Mouse` that does not
  happen: unqualified lookup finds `FuzzyLogic::Mouse` in the enclosing namespace before it ever
  reaches the names a `using namespace ...::Input;` directive injects, because those behave as
  members of the nearest namespace enclosing both -- the global one. The port keeps the original
  name *and* that using-directive, and compiles clean on both targets. When a record blames the
  language, write the ten lines that check.
- **A saturating control is the gate a random sample cannot give you any other way.** Every mouse
  here is placed and moves at random, so no two runs of the *original* match either. But each
  weight clamps to 0 or 1, and the bars are drawn after every sprite, so a held arrow key reaches
  a state that does not depend on how many frames elapsed: 42/42/42, 85/42/42, 85/0/42, 85/0/85,
  identical in XNA, native OPENGLES3 and WEBGL2 in Chrome.
- **Reference semantics decide the smart pointer, not tidiness.** The tank keeps chasing a mouse
  through the frame in which the game removes it from the list, then dereferences it to clear its
  highlight. C# keeps it alive; a `unique_ptr` list would make that a use-after-free. The list
  holds `shared_ptr` because the original's list holds a reference type.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-027-FuzzyLogicSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine FuzzyLogic.exe
```

Controls: Up/Down select a weight (on the key-release edge), Left/Right change it, Escape or Back
exits. The tank chases whichever mouse the three weights make the best target.

### Previously completed sample: SAMPLE-026 Aiming

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-026-AimingSample_4_0
```

Three things learned here:

- **A record that argues itself out of its own workaround is still a workaround.** This one said
  in as many words that a faithful two-pass port "would render correctly on EasyGL — only Vulkan
  would break", and kept the single-block version anyway. EasyGL is the campaign's only renderer.
  When an old note reasons its way to the right answer and then does the other thing, do the right
  thing.
- **Design the measurement around what the game already makes countable.** This game clears to
  **black** and draws its cone with `BlendState::Additive`, so "did the second pass run, and did
  it blend additively" is just a count of non-black pixels — 63373 / 40050 / 61084 / 61148,
  exactly equal in the original and the port. No sampling, no thresholds to argue about.
- **Set a gate threshold from a measurement, not from a guess.** The first version required the
  cone to light an eighth of the screen and failed, because one frame's steeper aim puts part of
  the cone off-screen and it only lights 40050 of 409440. The measured low-water mark is what the
  threshold should come from.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-026-AimingSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine Aiming.exe
```

Controls: arrow keys or the left stick move the cat, holding the left mouse button walks it
towards the pointer, Escape or Back exits.

### Previously completed sample: SAMPLE-025 ChaseAndEvade

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-025-ChaseAndEvadeSample_4_0
```

Two things learned here:

- **"Phone-specific code is out of scope" is not a valid reason to drop a branch.** The old
  record used it to justify removing the `#if WINDOWS_PHONE` regions. `rules.md` says the
  opposite: preserve inactive platform branches when the corresponding original logic is part of
  the selected source. Restoring them costs nothing and is what the rule asks for.
- **Check what the original's input actually is before calling a control invented.** This sample
  reads `Mouse.GetState()` and moves the cat towards the pointer with a `smoothStop` easing term.
  Neither the old port nor its `missing.md` mentioned it at all — it was simply missing, and it
  would have been easy to mistake for something the port had added if it had been there.

The measurement shape that worked: both characters wander on a time-seeded `Random`, so nothing
that depends on the simulation can match between runs. The **static half** of the HUD lines can,
and hashes `2ccb55c0` in the original, the native port and the browser alike. Note the assertion
had to become a *dominant* hash rather than an invariant one — the tank wanders freely and passes
behind the text, so a strict "identical in every frame" check fails for a reason that is not a
defect.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-025-ChaseAndEvadeSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine ChaseAndEvade.exe
```

Controls: arrow keys or the left stick move the cat, holding the left mouse button walks it
towards the pointer, Escape or Back exits.

### Previously completed sample: SAMPLE-024 Flocking

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-024-FlockingSample_4_0
```

Three things learned here:

- **"The original looks buggy" is not a licence to fix it.** The old port ported the behaviour the
  upstream code *evidently meant* rather than the behaviour it *has*. The rule is the opposite,
  and the owner restated it directly: if the XNA 4.0 original has bugs, CNA has them too. When you
  find a quirk, reproduce it and put a comment on it saying it is deliberate — do not silently
  correct it, and do not silently keep it either.
- **Read the old `missing.md` for what it does not say.** This one documented one deliberate
  non-reproduction and missed two further upstream defects entirely (`Flock.FlockParams` recursing
  into itself; `SliderInputHelper` measuring one slider from the other's X). Audit the original
  for quirks yourself rather than trusting the previous port's list.
- **Decide what is deterministic before capturing.** A time-seeded flock plus a label pulsing on
  `sin(10 * TotalGameTime)` means no whole frame can ever match. The slider bars are drawn *after*
  the flock, so nothing occludes them, and their orange button position is a pure function of the
  parameter — that made a byte-identical comparison possible in 4 of 5 frames and an exact
  800-orange-pixel match across all three builds.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-024-FlockingSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine Flocking.exe
```

Controls: WASD or the left stick move the cat, Up/Down select a slider, Left/Right or the triggers
move it, B resets the distances, X resets the flock, Y adds or removes the cat, Escape exits.

### Previously completed sample: SAMPLE-023 WaypointSample

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-023-WaypointSample_4_0
```

Three things learned here:

- **`import -window <id>` is not a reliable capture.** It returned an 850x480 image for a window
  `xwininfo` reported as 853x480 at every instant — it trims columns for some window shapes, and
  it looked exactly like the port failing to honour `PreferredBackBufferWidth`. Both captures now
  grab the **root** window at the window's absolute geometry (`import -window root -crop
  WxH+X+Y +repage`), so the two images are the same region by construction. Widen the Xvfb screen
  too, or the crop is clipped at the screen edge and you get the same wrong answer a second time.
- **Find the timing-independent part of the frame and compare that.** Anything driven by
  `elapsedTime` — a cursor moved by held keys, a tank following a path — cannot match to the pixel
  between two runs. The HUD band here is byte-identical in all six frames, and the discrete
  transition (B cycles Linear → Steering) changes exactly the same 188 pixels in the original, the
  native port and the browser. That is worth more than any whole-frame near-match.
- **`.hpp`/`.cpp` pairs are the right shape for mutually referencing classes.** SAMPLE-013 and
  SAMPLE-014 already established it; it is what let `Behavior` and `Tank` translate directly
  rather than needing an invented `TankBehaviorImpl.hpp` like the old port had.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-023-WaypointSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine Waypoints.exe
```

Controls: arrows move the cursor, A places a waypoint, B cycles the steering behavior, X resets,
Escape or Back exits.

### Previously completed sample: SAMPLE-022 Pathfinding

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-022-Pathfinding_4_0
```

Four things learned here:

- **A reflective XNB inlines value-type fields with no reader index.** Only reference-type fields
  carry one. Reading a `Point` field through `ReadObject` instead of raw loaded the first map with
  silently wrong values and only failed on the *second*. When writing a closed AOT reader, decode
  the container by hand first — `evidence/` has the script shape — and pin the layout before
  writing the reader.
- **A third project kind exists.** SAMPLE-004's StockEffects was a pipeline compiler and became a
  non-port; SAMPLE-020's TransformedCollisionTest was a second game and became its own sample
  directory; `PathfindingData` is a runtime class library declaring a type the game needs. Its
  *type* is ported into the consuming sample and its project file is not. Record the choice.
- **Narrow a framework fix to exactly the case that needs it.** The first version of the
  `Dictionary` hash-selector change substituted a `GetHashCode` hasher whenever `std::hash` was
  absent, and broke an existing contract test asserting `DefaultKeyHash<std::tuple<double>>` is
  `std::hash<...>`. Requiring `GetHashCode()` to be *present* as well left every existing contract
  standing.
- **A moving object is not a rendering difference.** Three of six compared frames differed; masking
  the single box containing the differing pixels left zero outside it, and the box held the tank.
  Compare a frame where nothing moves before concluding anything about rendering.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-022-Pathfinding_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine Pathfinding.exe
```

Controls: A starts/stops the search, B resets, X cycles the search method, Y loads the next map,
Left/Right adjust the time step, Back exits.

### Previously completed sample: SAMPLE-021 PathDrawing

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-021-PathDrawing_4_0
```

Four things learned here, and the first two change how a later sample should be approached:

- **A Windows-Phone-only sample is not automatically unbuildable.** SAMPLE-016 could not be built
  and set the expectation that WP7 means "audit by reading". This one links its four unmodified
  sources into a Windows executable, because everything it uses exists in the desktop XNA profile
  (`Microsoft.Xna.Framework.Input.Touch.dll` ships in the Windows references) and the only thing
  missing is the entry point the WP7 targets generate. Check what a phone sample actually
  *references* before concluding it cannot run.
- **Content built for `WindowsPhone` is not the same file as content built for `Windows`.** The
  difference is the container's platform tag and, for a SpriteFont, the Silverlight mscorlib named
  by its reader — payloads identical. Build both, measure, and ship the platform upstream targets.
  CNA reads the phone tag and the Silverlight reader name correctly.
- **The gates' inlined PNG decoder mis-applied the Paeth filter** — it fed the row being decoded
  back in as the up-left sample. Invisible on flat backgrounds, which is why it survived
  SAMPLE-018/019/020; a grass texture exposed it immediately. Fixed in all four retained
  `chrome-smoke*.mjs`, and it is what SAMPLE-020's "partly composited screenshot" really was.
- **When a harness disagrees with the file it just saved, suspect the harness.** Decoding the same
  saved PNG with Python settled in one command what a plausible story about the browser had made
  murky.
- **An owner-approved deviation is not a workaround, and it now has a home.** `rules.md` gained a
  section on it: record it in the sample's `diff.md`, put the mechanism in the layer that owns it,
  keep it **off by default**, and let the sample turn it on with a single `CNAEXT` line rather
  than growing a second code path. The zero-workaround policy binds the agent, not the owner.

To launch the retained original interactively (it renders; its touch cannot be driven under Wine):

```bash
cd /rv/tmp/samples/SAMPLE-021-PathDrawing_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine PathDrawing.exe
```

### Previously completed sample: SAMPLE-020 TransformedCollision

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-020-TransformedCollisionSample_4_0
```

Its `scripts/` are SAMPLE-019's adapted, plus three new ones:
`capture-cna-native-test.sh`, `capture-original-test.sh` and `chrome-smoke-test.mjs` drive the
second product. `smoke-cna-native.sh` and `capture-web.sh` now take the product name as their
first argument. Four things were learned here, and the last two are traps for any future sample:

- **A second upstream product may need its own `samples/` directory.** SAMPLE-017's unit tests
  fitted beside the game because they had no entry point and no content; this one does not.
  `rules.md` now has a section on the choice, and `plan.md`'s row must cite **both** `missing.md`
  files or `tools/prune-completed-sample.sh` deletes the second product as an intermediate. The
  tool now derives every port from the row and takes a comma-separated `--port-name`.
- **A deterministic-input product is worth seeking out.** `TransformedCollisionTest` takes the
  same mouse drag in the original, the native build and the browser, so the comparison is exact
  rather than statistical: red at the same nine of seventeen positions, with identical drawn
  geometry, in all three.
- **One probe pixel does not identify a frame's clear colour**, and **the inlined PNG decoder
  these gates carry mis-applied the Paeth filter**. Together they made the browser gate report
  "no collision" on a run whose own final screenshot was plainly red. The probe half is a sprite
  covering the corner; the decoder half was misdiagnosed here as a partly composited screenshot
  and corrected during SAMPLE-021 — see that sample's record. Every classifier now counts the
  pixels that are exactly one of the game's two clear colours and takes the larger, and the
  decoder fix is in the retained scripts of SAMPLE-018, 019 and 020.
- **`safeBounds` is 719x431 here too**, for the same extended-precision reason as SAMPLE-018, and
  the measured original settles it: person at x=343, clamping at 40 and 727, y=399. All three
  builds agree.

To launch the two retained originals interactively:

```bash
cd /rv/tmp/samples/SAMPLE-020-TransformedCollisionSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine TransformedCollision.exe

cd ../test-bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine TransformedCollisionTest.exe
```

Controls: the game takes Left/Right or the D-pad and exits on Escape or Back. The test product is
mouse-driven — hold the left button to move F, the right to move R, left control to drag the
origin, the wheel to rotate, alt to scale, arrows to rotate and scale.

### Previously completed sample: SAMPLE-019 RectangleCollision

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-019-RectangleCollisionSample_4_0
```

Its `scripts/` are SAMPLE-018's, adapted: `build-original.sh`, `capture-original.sh`,
`capture-original-collision.sh`, `smoke-cna-native.sh`, `capture-cna-native.sh`,
`capture-web.sh`, `chrome-smoke.mjs` and `analyze-frames.py`. Two things were learned here:

- **Adapting a sibling sample's scripts is cheap; adopting its conclusions is not.** The two
  content projects declare byte-identical BMPs with the same stock processor, and the XNBs did come
  out identical — but that was established by running *this* sample's own content project through
  the pipeline with its own `ProjectGuid`, not by copying files across.
- **`analyze-frames.py` now separates `unambiguousOverlapWithoutPixelHit` from
  `marginalOverlapWithoutPixelHit`.** A sprite's position is a float and only its rendered pixels
  are visible to the analyser, so every position inside a one-pixel band rasterises identically and
  a one-pixel overlap is below the measurement's resolution. The native run had exactly one such
  frame; checking it by hand showed the block occupied columns 566-597 with no partial edge and the
  person started at 597, so the game's own `(int)` was 565 and `Intersects` was right to say no.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-019-RectangleCollisionSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine RectangleCollision.exe
```

Controls: Left/Right arrow or gamepad D-pad move the person; Escape or gamepad Back exits.

### Previously completed sample: SAMPLE-018 PerPixelCollision

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-018-PerPixelCollisionSample_4_0
```

Reusable scripts: `build-original.sh` (official `BuildContent` + the original `.exe`),
`capture-original.sh`, `capture-original-collision.sh`, `smoke-cna-native.sh`,
`capture-cna-native.sh`, `capture-web.sh`, `chrome-smoke.mjs` and `analyze-frames.py`. The official
XNB hashes are `b509da3d04de79e10f074a2481f6c1858d1f6814c36e750c505db6b57def667f` (`Block.xnb`) and
`7e9cff8ab0f5a5bc0e06282bed455d7783d7425589fea564400243c70d4c6a72` (`Person.xnb`).

Four things learned here are worth carrying forward:

- **The original's own float arithmetic can differ from a plain C++ translation.** C# may evaluate
  a floating-point expression at higher precision than its type, and the 32-bit reference build
  does: `(int)(800 * (1 - 2 * 0.05f))` is 719 there and 720 in C++. `xna4-build/fp-probe/` holds
  the probe that measured it. Suspect this whenever a ported position is off by one.
- **`SpriteBatch` is now sub-pixel accurate on EasyGL.** `ISpriteBatchRenderer` gained a float
  destination overload whose default still truncates, so other renderers are unchanged; a renderer
  opts in by overriding it. `DrawString` still rounds glyph destinations — untouched deliberately,
  because SAMPLE-009/010/011 have pixel-identical text baselines and no evidence here bears on it.
- **The browser harness must play the game, not wait for luck.** A rare random event (here a
  pixel-perfect hit) will not happen inside a sweep. `chrome-smoke.mjs` reads the canvas back,
  finds the sprites and presses only the sample's own arrow keys, and it must *stop steering* once
  it is lined up — otherwise the target block leaves the search window on its last frames and the
  player walks out from under it.
- **Chrome on Xvfb needs ANGLE/SwiftShader here.** With the real GPU it intermittently loses the
  WebGL context (`Creation of StagingBuffer's SharedImage failed`), after which CDP requests never
  answer; every request now carries a 60 s deadline so that fails fast. Run Chrome on its own X
  display: on the shared `:0` desktop stray external key events reached the sample and moved the
  person. The harness logs every DOM key event so contamination is visible.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-018-PerPixelCollisionSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine PerPixelCollision.exe
```

Controls: Left/Right arrow or gamepad D-pad move the person; Escape or gamepad Back exits.

### Earlier completed sample: SAMPLE-017 Collision

The complete evidence root is:

```text
/rv/tmp/samples/SAMPLE-017-CollisionSample_4_0
```

Useful reusable scripts are `build-original.sh`, `run-original-tests.sh`,
`capture-original.sh`, `smoke-cna-native.sh`, `capture-cna-native-xvfb.sh`, `capture-web.sh` and
`chrome-smoke.mjs`. The official `Font.xnb` hash is
`aad7c770f87443708af6bf7a0c6441d0fdf6a5ea2168e25c9fc16f656e2388ad` in the XNA pipeline output,
repository and native bundle. Browser evidence is in
`evidence/cna-web-webgl2/browser-result.json`.

The framework issue exposed here was general: XNA/FNA `ContentManager(IServiceProvider)` begins
with an empty `RootDirectory`, while CNA had inherited the zero-argument CNAEXT convenience
default `"Content"`. Commit `b06e81203` restores the XNA constructor behavior and keeps the CNAEXT
constructor default. Two focused constructor tests pass. No sharp-runtimenext change was needed.

To launch the retained original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-017-CollisionSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine CollisionWindows.exe
```

Controls: G cycles Sphere → Ray → Frustum → AABB → OBB; arrows rotate; +/- zoom; Home resets; B
toggles perspective/orthographic; O/P select projection; Space pauses; [ and ] single-step while
paused; Escape exits. The original and CNA animation is time-based, so moving secondary shapes
need not occupy identical coordinates in separately timed screenshots.

### Exact next task

**`SAMPLE-046`.** Take the next `⬜`/`🔎` row in `plan.md` in order.

What transfers from SAMPLE-018–038:

- The scripts in `/rv/tmp/samples/SAMPLE-033-NonPhotoRealisticSample_4_0/scripts/` are the current
  generation, and the first whose browser gate reads Chrome's `Log` domain.
- Build the content in **Release**. `EffectProcessor.DebugMode` defaults to `Auto` and skips
  shader optimization for a Debug build; two samples in a row have shipped an `.fx` that then
  misses `ps_2_0`'s 64-instruction limit, one of them by a single instruction.
- Build the content for every target platform the sample declares, and when the pipeline refuses
  one, record the refusal as the measurement rather than working around it.
- Anything that opens a window -- Wine, SDL, Chrome, ctest -- gets an explicit virtual display,
  and for a ctest run check `grep CNA_TEST_DISPLAY <build-dir>/CMakeCache.txt` as well.
- Never rebuild a test binary while a ctest run is using it.
- Use `CCACHE_DIR=/rv/cnaccache`; never let a fetched dependency clone into the build tree when
  `~/deps` already has it.
- Ask what in the frame is not deterministic -- clock, RNG, both -- and freeze all of it before
  capturing. Set every threshold from a measured value, and print how many pixels a comparison
  compared.
- When a frame is wrong rather than merely different: `coredumpctl list` for a native crash,
  Chrome's `Log` entries for the web.

### Legacy appendix boundary

The material below the existing “Sample-plan consolidation” banner is retained only as historical
evidence, including the separate Racing feasibility record and old pre-campaign task lists. It
contains obsolete renderer choices, dependency paths, bypass recommendations, ignore decisions
and prohibitions on editing sibling runtimes. Do not execute its “next tasks”, “do not do yet” or
resume prompt for the active 153-directory campaign. Current authority is `rules.md`, `plan.md`,
the active handoff above and each freshly audited sample's evidence. Racing remains separate and
last under `plan_racing.md`.

> **Sample-plan consolidation (2026-08-22):** the Racing section below and its dedicated
> `plan_racing.md` remain separate and unchanged. For every non-Racing sample, the authoritative
> queue is now [`plan.md`](plan.md). Older counts, `Done`/placeholder/ignored classifications,
> workaround approvals and `../cna`/`../sharp-runtime` dependency assumptions retained later in
> this file are historical evidence only; they do not override the new 153-row re-audit,
> no-workaround policy or the planned `../cnanext` + `../sharp-runtimenext` development migration.

## Racing Game feasibility audit and modern delta (2026-08-09)

The initial audit of the older XNA 4 Racing Game conversion at
`/rv/tmp/XNAGameStudio/Samples/XNA-4-Racing-Game-Kit-master` is complete. A second,
deeper delta audit has now analyzed the already-cloned modern repository at:

```text
/rv/tmp/RacingGame
d8092633e4e43e014ff168d8e913a9373538b851
2025-10-01T12:42:29+07:00 — Moved back onto DigitalRiseModel
```

The modern repository is a direct FNA/MonoGame port of the same game. Its Linux/FNA
build is user-confirmed running, so a modern executable oracle is available. No
Racing C++, translated shader, converted asset, content package, or CNA change was
created by this audit.

### Current conclusion

**Verdict: FEASIBLE WITH SUBSTANTIAL, NOW WELL-BOUNDED CNA WORK; medium-high
feasibility confidence.** Select **Option 3, a hybrid hierarchy**, with modern
`rds1983/RacingGame` as the primary implementation/content/runtime reference and the
older XNA 4 source as historical/original-behavior validation.

The old realistic estimate was **930–1,250 h, central about 1,100**. The revised
one-backend Linux `OPENGL33` estimate is **690–880 h, central about 780**: realistic
bands of **480–600 h Racing port/tooling**, **190–240 h reusable CNA work**, and
**20–30 h remaining baseline/reference work**. This reduction removes obsolete
content archaeology; it does not assume the ~23.6k-line C# translation is trivial.

If Windows, Android and Web are all required, qualify them only after Linux:
Windows `OPENGL33` +50–90 h, Android `OPENGLES` +180–300 h, and Web `WEBGL2`
+250–420 h. With shared work de-duplicated, all three add about 430–700 h, for a
realistic all-four-platform program of approximately **1,120–1,580 h**.

### Major superseded risks/work packages

- All 57 old `.x` models have checked-in `.glb` equivalents; no custom `.x`
  conversion tool is planned.
- 57 JSON `.material` files already hold effect/technique/parameter/texture and
  ordered mesh-part metadata; no monolithic `RacingPackage` or manual material
  reconstruction is planned.
- Real runtime-used version-46 `.xgs/.xsb/.xwb` banks exist; acquisition/generation
  is no longer a task, though CNA compatibility still needs testing.
- A Linux/FNA build runs; “no runnable oracle” is no longer a blocker.
- Modern license files substantially reduce code uncertainty; asset-level
  provenance remains a release gate.

### Remaining CNA work and platform decisions

The critical framework work is a CNA-owned portable arbitrary `Effect` runtime
(named parameters, techniques, passes, `CurrentTechnique`, `Apply()` and backend
shader modules), a bounded node/part/material/tangent/bounds-preserving glTF model
path, truthful `Rgba64` render-target behavior, RGB24 loose DDS cube support, and
supplied-bank XACT validation. Do not load FNA DX9 binaries, MonoGame `MGFX`, or
implement a full FX compiler merely for Racing.

The canonical content strategy is **GLB + `.material` + raw assets**. XNAssets and
DigitalRiseModel are evidence/adapters, not dependencies to port wholesale.

Android requires a new game-side multi-touch overlay: analog steering plus throttle,
brake/reverse, handbrake and pause. CNA already has `TouchPanel` and an Android
Accelerometer; tilt steering should be optional and calibrated, while pedals remain
touch-controlled. Windows/Linux keep desktop/gamepad providers. Web reuses touch and
requires WebGL2, progressive/cacheable content delivery, audio unlock/XACT proof and
persistent storage; its roughly 289 MiB canonical asset set must not become one
unexamined production preload.

### Documents updated

- [`racing_feasibility.md`](racing_feasibility.md) — provenance, architecture delta,
  GLB/material/XNAssets/DigitalRise/effects/XACT/licensing evidence, platform
  feasibility, revised estimates and source authority.
- [`plan_racing.md`](plan_racing.md) — post-modularization plan using the modern
  source and GLB/material/raw route, plus Windows/Android/Web qualification and
  Android touch/tilt design.
- [`racing_api_matrix.md`](racing_api_matrix.md) — modern APIs/dependencies and
  framework/port/platform classifications.
- [`missing.md`](missing.md) — genuine CNA gaps only, with superseded old findings
  and platform validation separated from game-side work.

No separate delta document was needed; the evidence and before/after mapping are
integrated into the existing documents.

### Next action

Complete CNA modularization/stabilization first. Then pin the exact CNA and
dependency SHAs, preserve a reproducible FNA run/capture recipe, and execute only a
minimal `OPENGL33` lifecycle/clear/capture/resize/input/render-target/custom-layout
harness. The next content gate is a car/windmill/alpha/sky GLB/material proof—not
gameplay translation or bulk shader work.

## 1. Project summary

**What this is:** C++ ports of the official Microsoft XNA Game Studio 4.0 sample
collection, running on **CNA** (`../cna`) — a C++ reimplementation of the XNA 4.0
API built on SDL3, itself built on **sharp-runtime** (`../sharp-runtime`), a C++
port of relevant .NET BCL types. `cna` and `sharp-runtime` are independent sibling
git repositories consumed via `add_subdirectory`, not submodules of this repo.

**Main goal:** Port the applicable XNA 4.0 desktop samples (`plan.md` catalogs all
153 directories in the upstream XNA Game Studio archive and classifies each one) to
CNA C++, preserving the original class hierarchy and naming
(`Microsoft::Xna::Framework::*`). The ported samples double as integration tests for
CNA and as a migration reference for anyone porting XNA/MonoGame code to CNA.

**Current phase:** 62 samples are fully ported and wired into the root
`CMakeLists.txt`. **RimLighting (#037) was ported this session** (see section 3)
— the sample DEFERRED.md item #14 and section 8 task 6's retirement note both
flagged as the closest remaining portable placeholder: `EnvironmentMapEffect`-
based rim lighting against a real 6-face `OutputCube.dds`, with both
`Content.Load<TextureCube>` (item #14) and `Content.Load<Model>` (item #26)
bypassed the same way ReachGraphicsDemo's `EnvmapDemo` bypassed its own
cubemap. Found and fixed a second real `tools/fbx_ascii2model.py` bug in the
process (DEFERRED.md item #30, new — the tool assumed `LayerElementNormal` is
always `"ByPolygonVertex"`; it's usually `"ByVertice"`, and a repo-wide grep
found this likely also affects ChaseCamera's `Ship.fbx` and ReachGraphicsDemo's
`saucer.fbx`/`model.fbx`, not re-verified/re-shipped this session). Before
this, **ReachGraphicsDemo (#005) was ported** — a "Phase 1" sample previously
flagged (2026-07-10, earlier in this
same multi-session run) as likely-unblocked-but-unverified: its own `missing.md`
had gone stale, claiming SpriteFont/`Content.Load<Model>`/`EnvironmentMapEffect`/
`DualTextureEffect` were all missing from CNA when in fact all four are
implemented and already proven elsewhere in this repo. Of its own 6 demo
scenes, 5 were ported (`BasicDemo`, `AlphaDemo`, `DualDemo`, `EnvmapDemo`,
`ParticleDemo`, plus the shared `MenuComponent`/`TitleMenu`/`DemoGame`
framework); `SkinnedDemo` was skipped (still genuinely blocked on DEFERRED.md
item #13, skeletal animation — replaced with a clear "not available" message,
matching the same "port the surrounding framework faithfully, guard the
actually-unavailable feature" pattern InverseKinematics' Avatar half already
established). Found and worked around **two new, previously-undocumented CNA
rendering gaps** in the process (DEFERRED.md items #28/#29 — a full-backbuffer
`SpriteBatch` draw before any 3D draw call in the same frame breaks that
frame's 3D rendering entirely; `DualTextureEffect`'s own EasyGL shader
hardcodes a Position+UV-only, no-Normal vertex layout unlike every other stock
effect) plus a real bug in `tools/fbx_ascii2model.py` (silently used the wrong
one of 2 UV layers on a multi-UV-layer FBX mesh, fixed with no regression on
any already-shipped single-UV-layer asset). See section 3 for the full account
of each finding, all confirmed live via screenshot isolation. Before this,
**TiltPerspective (#107) was ported** (see section 3) — the second and last of
the two samples covered by the
2026-07-10 user go/no-go decision (section 8 task 9), completing it. Unlike
its sibling AccelerometerSample (#084, ported in the prior session), this
sample's own re-audit against the "look for a nested `DeviceType` branch
inside `#if WINDOWS_PHONE`" correction (DEFERRED.md item #15) came back
negative — `AccelerometerHelper.cs` has no `#if WINDOWS_PHONE` split at all
and its own no-hardware fallback is a non-interactive, time-driven sinusoidal
wobble, not a keyboard branch of any kind — so this port genuinely had to
invent a keyboard-tilt control scheme from scratch (NOXNA), reusing the same
X/Y-arrow-key shape AccelerometerSample's/Yacht's own *promoted* fallbacks
already established, for consistency. With this sample done, both halves of
the 2026-07-10 user go/no-go (task 9) were complete at the time — see section 8
for how ReachGraphicsDemo (this session's newest port, above) was picked up
next regardless, since it had been separately flagged as likely-unblocked in
the same session, not part of that go/no-go. Before this,
**AccelerometerSample (#084) was ported** — the first of the two samples
covered by that same decision; porting it turned up the correction described
above (its own emulator branch, unlike TiltPerspective's, already had a real
keyboard fallback nested inside `#if WINDOWS_PHONE`, so no invention was
needed there). Before that, **PeerToPeer (#103) was ported** — the third and
final sample in the `NetworkSession`/`GamerServicesComponent` networking
family (after ClientServerSample #091 and NetworkPrediction #100), confirming
a *third* time that all three of ClientServerSample's original networking
workarounds (DEFERRED.md items #19/#20/#21) stay gone for an
independently-written sample, this time one with a genuinely different
topology (full peer-to-peer broadcast, no host authority over simulation at
all). This retired section 8 task 7 and, with it, the original "port more
samples" backlog that session set out to work through — see section 8's
rewritten task list for what a future session should look at next
(re-scanning placeholders for anything DEFERRED item #26's fix pattern newly
unblocks, etc.). The Avatar scope question is settled (2026-07-10, user
go/no-go — see section 8 task 10): the 5 Avatar samples will not be ported.
MarbleMaze (#061), ChaseCamera (#058), and InverseKinematics (#057) (the
original 3-sample lighting-candidate list) were ported in earlier sessions and
remain done. 24 placeholder directories exist for samples still genuinely
blocked on real CNA engine work (custom shaders, skeletal animation, one
content-pipeline gap) — verified by direct count: `ls samples | wc -l` = 86
total sample directories = 62 active `add_subdirectory` lines + 24
still-commented placeholder lines in the root `CMakeLists.txt`, with none
unlisted either way. 67 catalogued directories are permanently out of scope
and listed in `ignored.md` (not XNA 4.0, not a runnable `Game`, redundant
duplicates, or tied to a platform CNA won't target). See `plan.md`'s Sample
Count Summary table for exact per-category counts.

**Update (2026-07-11): the "no further approved/queued porting work" claim
below is now stale — see section 8's new "Superseding update" note for the
full account.** `cna`'s `develop` landed a large batch of fixes (Tasks
927-949) since the previous session, confirmed via direct source read and a
live rebuild+screenshot this session, not just trusted from commit messages.
Most notably, DEFERRED.md item #26 (the vertex-corruption bug behind the
long-tracked "near-plane-clipping"/invisible-model bug family) is fixed, and
so is the multi-bone `ModelBone` gap that blocked SplitScreen (#076),
SimpleAnimation (#050), and TankOnHeightmap (#074) — see section 8 task 11.
**There is new, concrete porting work available now.** Skeletal animation
(item #13) remains only partially done (foundation classes only, no
loader/renderer wiring) — do not start SkinningSample/CustomModelAnimation/
SkinnedModelExtensions/CPUSkinning yet. Custom shader conversion (item #11)
and content-pipeline extensibility (item #18) remain fully open.

**Original 2026-07-10 claim, now superseded (kept for history):** every
remaining placeholder is blocked on either a `cna`-side engine fix (items #11,
#13, #14, #6/#18, or the newer #22–#30) or a new user product-scope decision,
not on porting effort alone. A future session should re-verify this claim (per
section 5's own "DEFERRED.md blockers can go stale" caveat) rather than trust
it indefinitely, and should check with the user for new direction before
assuming there's nothing to do. **(2026-07-11: this re-verification happened —
see above; the claim was wrong, not stale-and-still-right.)**

**Important architectural decisions:**
- One executable per sample; no shared sample library. Each `samples/<Name>/`
  directory is fully self-contained (`src/` header-only except one `Program.cpp`,
  `Content/`, `missing.md`, a verbatim copy of the original `.htm` doc) — even
  where two samples' code ends up structurally similar.
- Assets: `Load<Texture2D>` → PNG; `Load<SoundEffect>`/`Load<Song>` → WAV/OGG;
  `Load<SpriteFont>` → `.font.json` + PNG atlas (`tools/make_font.py`);
  `Load<Model>` → `.model.json` (static/rigid single-bone models only — see
  section 5/6). XNA `.xnb` is never supported. Custom XML content files are
  hand-translated to C++ construction code in small numbers; the one exception is
  RolePlayingGame's 281 data files, which needed a real runtime XML loader
  instead.
- CNA is consumed via `add_subdirectory(../cna)`, so building any sample also
  rebuilds CNA (and transitively sharp-runtime) if their sources changed. Never
  assume `cna`/`sharp-runtime`'s working tree is clean, and never edit them
  without confirming scope with the user first — other work may be happening
  there concurrently (confirmed: this machine runs many concurrent Claude Code
  sessions against these same repos).
- Default graphics backend is **EasyGL** (OpenGL ES); a Vulkan backend also exists
  but has a known bug (section 5).
- Framework-level gaps found while porting a sample get fixed in `cna`/
  `sharp-runtime` directly (after confirming scope with the user) — never worked
  around silently inside a sample. Every workaround applied instead must be
  documented in that sample's `missing.md`.

---

## 2. Current status

### Build
**FIXED (2026-07-10) — the full aggregate build is green again.**
`cmake --build cmake-build-debug -j$(nproc)` builds all ~60 samples together
with **0 errors, 0 warnings**, confirmed via a from-scratch full rebuild
(re-run a second time afterward with no source changes: `ninja: no work to
do`, exit 0 — a clean, stable confirmation, not a fluke).

Root cause (as diagnosed earlier this session): `Viewport` no longer exposes
direct `.x`/`.y` public members upstream in `cna` — same class of drift that
previously broke `InputReporter`'s direct `GamePadCapabilities` field access.
Two samples were affected, not just the one originally found:
- `samples/SafeArea/src/SafeAreaOverlay.hpp` (the originally-diagnosed one —
  8 errors, lines 47/48/50/51/53/56/58).
- `samples/RolePlayingGame/src/{RolePlayingGame.hpp, TileEngine/
  TileEngine.hpp}` (found only once the *full* aggregate build was actually
  re-attempted this session — 10 more errors across both files). This wasn't
  visible in the earlier partial build attempts because `ninja` stops at the
  first failing target (`SafeArea`), so `RolePlayingGame` — later in the build
  graph — was never even reached until `SafeArea` itself was fixed.

Both fixed the same mechanical way: every `viewport.x`/`viewport_.x`/`v.x` (and
the `.y` equivalent) switched to `viewport.getXProperty()`/`getYProperty()`
(see `CLAUDE.md`'s property-access table). A repo-wide grep after fixing both
files (`grep -rnE "[a-zA-Z_][a-zA-Z0-9_]*\.(x|y)\b"` filtered to
viewport-shaped variable names) found no further instances — but this was
grepped against the *currently-active* (uncommented) sample list only; the 28
commented-out placeholder directories were not checked (most have no `src/`
yet anyway, so the pattern can't exist there regardless).

One pre-existing, unrelated warning was noted and deliberately left alone
(out of scope for this fix): `samples/RolePlayingGame/src/GameScreens/
../MenuScreens/LoadingScreen.hpp:51` has an ambiguous-overload warning on a
`Color(255, 255, 255, TransitionAlpha())` call (an `intcs` vs. `bytecs`
constructor ambiguity) — nothing to do with `Viewport`, not touched.

### Tests
No automated test suite exists in this repo. Each sample is its own manual/visual
verification unit: build it, run it under `SDL_VIDEODRIVER=x11`, and (when
synthetic input can be verified — see section 5) interact with it or compare a
screenshot.

### CLI / tools available
- `tools/make_font.py <font.ttf> <size_px> <Content/FontName>` — generates a
  `.font.json` + atlas PNG SpriteFont asset.
- `tools/gen_help_png.py <Sample.htm> <Content/help.png>` — extracts a sample's
  own `.htm` "Sample Controls" table into the mandatory F1 help-overlay PNG. Note:
  hardcodes column index 1 for the keyboard control — samples whose table has
  more columns (e.g. a Windows-Phone column first) need a one-off variant script
  to pick the right column (done for MicrophoneEcho — column 2, "Windows" — and
  AccelerometerSample — column 2, "Windows Phone - Emulator"; not generalized).
- `tools/obj2model.py`, `tools/fbx_ascii2model.py` — convert static, single-bone
  `.obj`/`.fbx` models to CNA's `.model.json` format.

### Recently implemented / working
- **SimpleAnimation (#050) ported** (2026-07-11) — the first of 3 samples
  (SimpleAnimation, SplitScreen, TankOnHeightmap) sharing the same multi-bone
  blocker and the same `tank.fbx` asset, picked to validate the newly-landed
  `cna` multi-bone `ModelBone` fix (Tasks 936/937, see the re-verification
  pass entry below) in isolation before the other two build on top of it.
  Builds 0 warnings (targeted + full aggregate rebuild, `ninja: no work to do`
  on re-run); ran 5+ seconds with no crash across 3 runs.
  **Confirmed empirically (not assumed) that `tank.model.json` did not need
  regenerating** — reused byte-for-byte from `samples/CameraShake/Content/`.
  Found the real remaining gap the fix's own caveat only partially
  described: `tank.fbx`'s node hierarchy is genuinely *nested* (e.g.
  `l_back_wheel_geo` is a child of `l_engine_geo`, itself a child of the root
  `tank_geo`), but `cna`'s reader only ever builds a *flat* one-level bone
  tree. Worked around entirely port-side (NOXNA, no `cna` edit, no bypass
  class): `Tank::Load()`'s `ApplyRestTransforms()` sets each of the 11
  non-root meshes' `ModelBone::Transform` directly via
  `Matrix::CreateTranslation`, to each part's correct **absolute** rest
  offset, computed by composing `tank.fbx`'s own `Lcl Translation` values
  through the real parent chain (a plain vector sum, since every
  rotation/PreRotation in this asset is exactly zero — confirmed by direct
  read of `tank.fbx`). Everything else in `Tank.hpp` is an unmodified, direct
  port of `Tank.cs`'s own technique using the real `Model`/`ModelBone`/
  `CopyAbsoluteBoneTransformsTo` API. Confirmed live via screenshot: all 4
  wheels, both steer pivots, the turret, cannon, and hatch render in correct
  relative position and animate correctly (wheel spin, steering sweep,
  turret swivel, cannon elevation, hatch open/close), stable across multiple
  frames and camera angles — without the fix (tested during development),
  every part rendered piled at the tank body's own local origin, confirming
  the gap was real. Also found and worked around an unrelated, NOXNA-tagged
  finding: with no explicit `PreferredBackBufferWidth`/`Height` set (matching
  the original, which sets none), CNA's `Viewport` still reports XNA's
  800×480 default internally but the actual SDL window this machine created
  was 1740×1044 (~2.175× larger) — set an explicit 1280×720 size (matching
  this repo's own established convention for desktop 3D samples) to sidestep
  it; no DEFERRED.md item filed (not confirmed as a `cna` bug, no other
  sample hits this path since all already set an explicit size). F1 help
  overlay confirmed rendering correctly via this repo's established temporary
  debug-auto-trigger pattern (forced `helpTimer_ = 10.0f`, reverted before
  commit — a different real user window held focus throughout, confirmed via
  `xdotool getactivewindow`). No `SpriteFont`/background/ground plane in this
  port, matching the real original's own minimalism exactly (confirmed by a
  full read of `SimpleAnimation.cs` — no `SpriteBatch`/`SpriteFont` reference
  anywhere in it). See `samples/SimpleAnimation/missing.md` for the complete
  account and DEFERRED.md item #6's own updated multi-bone section for the
  cross-sample implication (SplitScreen/TankOnHeightmap can very likely reuse
  `Tank.hpp`'s rest-transform table directly, same asset, confirmed via
  `md5sum` in TankOnHeightmap's own `missing.md`).
- **DEFERRED.md/NEXT.md re-verification pass (2026-07-11), no sample ported this
  entry** — `cna`'s `develop` had moved substantially since the prior session
  (Tasks 927-949, all landed 2026-07-10) without this repo's own docs being
  updated to match. Per section 5's own standing "DEFERRED.md blockers can go
  stale" caveat, did a full live re-verification rather than trusting either
  the old docs or the new commit messages: rebuilt the full aggregate project
  from scratch against current `cna` `develop` HEAD (0 errors/0 warnings,
  `ninja: no work to do` on re-run), then read `cna`'s own current source for
  every DEFERRED.md item flagged as plausibly affected, and live-tested the
  highest-impact one directly (ran `CameraShake_cna_samples` under
  `SDL_VIDEODRIVER=x11`, screenshot-confirmed its tank — loaded via plain
  `Content.Load<Model>`, no bypass — now renders as a complete solid shape
  instead of the previously-documented thin-line/invisible symptom).
  **Findings, all confirmed via direct source read (not assumed from commit
  messages):** DEFERRED.md item **#26** (`ModelTypeReader` vertex-corruption
  bug, the true root cause of the multi-session "near-plane-clipping" bug
  family) is fixed (`cna` Task 927) — the single biggest fix in the batch.
  Item **#14** (TextureCube) is fixed (Task 934). Item **#24** (`Clear(Color)`
  depth) is fixed (Task 928). Item **#25** (`VertexBuffer`/`IndexBuffer::GetData()`)
  is fixed (Task 930). The multi-bone addendum under item **#6** is fixed
  (Tasks 936/937: `ModelMesh::setParentBoneProperty()` + `ModelTypeReader`
  building one real `ModelBone` per mesh), plausibly unblocking SplitScreen
  (#076), SimpleAnimation (#050), and TankOnHeightmap (#074) — not yet
  independently ported/verified. Item **#23** turned out to have never been a
  real CNA bug (Task 929: FNA has the identical subscribe-after-`Initialize()`
  ordering DEFERRED.md had claimed was CNA-specific) — the existing
  `AddComponent()` workarounds in Graphics3D/PickingSample remain correct and
  should not be removed. Item **#13** (skeletal animation) is only
  **partially** done (Tasks 939/940 added `AnimationClip`/`Keyframe`/
  `AnimationPlayer`/`SkinningData` classes and designed the on-disk schema,
  but `ModelTypeReader` parsing, `Model::Draw` wiring, and a conversion tool —
  Tasks 941-944 — are all still unstarted) — still blocks every
  skeletal-animation sample; do not start those yet. Items #11, #18, #22,
  #27, #28 (investigated by `cna` Task 933, not reproduced — stays open), and
  #29 remain open and unchanged. Also found and fixed a stale project memory
  file (`feedback_cna_multiple_spritebatch.md` claimed the Vulkan
  multi-SpriteBatch-per-frame bug was still open; it was actually fixed by
  `cna` Task 664 on 2026-07-07, three days *before* that memory file was even
  written). Updated DEFERRED.md (per-item status corrections + summary table)
  and this file's own section 1/section 8 closing notes to match — see
  section 8's new "Superseding update" note for the task-list impact. **No
  sample was ported this entry** — this was purely a documentation-accuracy
  and re-verification pass; SplitScreen/SimpleAnimation/TankOnHeightmap are
  now real candidates for the next session (section 8 task 11).
- **RimLighting (#037) ported** (2026-07-10, tenth follow-up session) —
  `EnvironmentMapEffect`-based rim lighting: `World`/`View` swapped
  (`World <- World*View`, `View <- Identity`) so the cube-map lookup happens in
  view space, against a real 6-face `OutputCube.dds` (all faces dark except a
  bright "back" face) for the silhouette highlight. Both
  `Content.Load<TextureCube>` (DEFERRED item #14) and `Content.Load<Model>`
  (item #26) bypassed via direct `TextureCube`/`VertexBuffer` construction,
  the same philosophy as ReachGraphicsDemo's `EnvmapDemo`. `head.fbx` also has
  a real non-identity parent Null bone (translation + 180°-Y rotation), baked
  into the vertex data at conversion time (the group node is static, no
  animation Take references it). Found and fixed a second real
  `tools/fbx_ascii2model.py` bug (DEFERRED item #30, new — assumed
  `LayerElementNormal` is always `"ByPolygonVertex"`; it's usually
  `"ByVertice"`, confirmed via a repo-wide grep to likely also affect
  ChaseCamera's `Ship.fbx` and ReachGraphicsDemo's `saucer.fbx`/`model.fbx`,
  not re-shipped this session). Builds 0 warnings; ran 15+ seconds with no
  crash; screenshots confirm a clean, anatomically-consistent rim-light
  highlight and correct UI. See `samples/RimLighting/missing.md` and section 3
  for the complete account.
- **ReachGraphicsDemo (#005) ported** (2026-07-10, follow-up session) — a
  Phase 1 sample ("MIX10 Graphics Effects Demo"): a menu-driven showcase of 5
  of XNA 4.0 Reach-profile stock effects, each its own demo scene, sharing a
  `MenuComponent`/`MenuEntry`/`TitleMenu`/`DemoGame` framework (crossfade
  transition effects between scenes, "zoomy text" click feedback, idle
  attract-mode auto-cycling). Read `DemoGame.cs`/`MenuComponent.cs`/
  `MenuEntry.cs`/`TitleMenu.cs`/`BasicDemo.cs`/`AlphaDemo.cs`/`DualDemo.cs`/
  `EnvmapDemo.cs`/`ParticleDemo.cs`/`SkinnedDemo.cs`/`Sky.cs`/`Tank.cs`/
  `ContentPipelineExtension/CubemapProcessor.cs` in full before writing any
  code, per this task's own brief. Builds 0 warnings (multiple from-scratch
  rebuilds); ran 10+ seconds with no crash across several runs; attract mode
  observed live (unforced) correctly auto-cycling through demo scenes with
  their own crossfade transition animations playing; F1 help overlay
  confirmed appearing/disappearing correctly.
  **Scope: 5 of 6 demo scenes ported** (`BasicDemo`, `AlphaDemo`, `DualDemo`,
  `EnvmapDemo`, `ParticleDemo`, plus the full shared framework);
  **`SkinnedDemo` skipped** — still genuinely blocked on DEFERRED.md item #13
  (no skeletal animation system in CNA at all); replaced with a clear
  "not available, needs DEFERRED item #13" message, following the same
  "port the surrounding framework faithfully, only the actually-unavailable
  feature is inert" pattern InverseKinematics' Avatar half already
  established — but unlike that case, there was no existing CNA class/state
  to guard against here (the underlying animation types simply don't exist),
  so the guard is a hand-written message rather than a permanently-false
  runtime condition.
  **Correction to this task's own original brief:** the brief assumed
  `TitleMenu.cs` used `Sky.cs`/`Tank.cs` for its own 3D background scene,
  offering a "simplify to a static title screen" fallback if that turned out
  too large — a full read of `TitleMenu.cs` found this premise was simply
  wrong: `Sky.cs` is used only by `SkinnedDemo.cs` and `Tank.cs` only by
  `BasicDemo.cs`/`AlphaDemo.cs`; `TitleMenu.cs`'s own `Draw()` has no 3D
  content of any kind (flat background + rotated title text + floating
  SpriteBatch-only "xna" labels). So TitleMenu needed no scope reduction at
  all and is ported fully and faithfully.
  **Every model in this sample is loaded via the established DEFERRED.md item
  #26 bypass** (`RawMesh.hpp`/`RawMeshPosTex.hpp`/`TankModel.hpp`, applied
  proactively from the start, not re-confirmed against a plain
  `Content.Load<Model>` build first, per this session's now-5+-times-confirmed
  precedent). `grid.x` (converted via `assimp export`, no ASCII-FBX parser
  path for `.x` files) hit the same `assimp`-introduced triangle-winding
  inversion already seen on ChaseCamera's/MarbleMaze's own `.x`-derived
  assets — fixed with a permanent, isolated `RasterizerState::CullNone` for
  just that one mesh's own draw call. `tank.fbx`'s own `Connect: "OO"` lines
  revealed a genuine nested (parent, child) bone hierarchy (unlike every
  other model in this repo, all flat/single-bone) — `TankModel.hpp`
  reimplements this hierarchy directly in C++ (a NOXNA 12-part parent-index
  table + per-frame `local * parentAbsolute` chain, mirroring
  `Model.CopyAbsoluteBoneTransformsTo()` exactly), reading each part's own
  un-baked mesh-local vertex data plus its own rest translation via a new
  one-off Python conversion script (not committed to `tools/`). Confirmed
  live: the tank renders fully textured (2 materials) and shaded, with the
  turret visibly rotating frame-to-frame.
  **Found and fixed a real bug in `tools/fbx_ascii2model.py`:** `model.fbx`
  (DualDemo's model) has 2 UV layers per mesh (a base diffuse-texture UV plus
  a separate lightmap UV) — the tool's own parser silently kept whichever
  `UV:`/`UVIndex:` block it saw *last* (the lightmap layer), instead of the
  intended base-texture layer (layer 0), corrupting texture coordinates.
  Fixed to keep only the first UV layer found; confirmed this does not
  change any already-shipped single-UV-layer asset (`tank.fbx`/`saucer.fbx`
  re-converted byte-identical to their previously-shipped `_verts.bin` files).
  **Found and worked around 2 new, previously-undocumented CNA rendering
  gaps, both confirmed live via careful screenshot isolation (one variable
  changed at a time):**
  1. DEFERRED.md item #28 (new) — a `SpriteBatch` draw that stretches a
     texture to cover the *entire* backbuffer, executed before any 3D draw
     call in the same frame, makes every subsequent 3D draw in that frame
     render nothing at all (confirmed independent of effect type, camera
     distance, cull mode, and source texture format — narrowed specifically
     to "full-backbuffer SpriteBatch draw before 3D content," since this
     sample's own smaller `DrawTitle()` SpriteBatch calls, positioned the
     same way in every other demo scene, are unaffected). Worked around in
     EnvmapDemo by replacing the SpriteBatch background-image draw with a
     hand-built full-screen quad via `BasicEffect` + 
     `DrawUserIndexedPrimitives` (`DrawBackgroundQuad()`, NOXNA).
  2. DEFERRED.md item #29 (new) — `DualTextureEffect`'s own EasyGL shader
     (`EnsureDualTextured3DProgram()`) hardcodes a Position+UV-only (no
     Normal) vertex attribute layout, unlike `BasicEffect`'s/
     `EnvironmentMapEffect`'s Position+Normal+UV lit shaders — uploading the
     usual `VertexPositionNormalTexture` against it silently reads the
     mesh's own Normal.xy as if it were UV, producing a flat/uniform color
     per submesh. Worked around with a new `RawMeshPosTex.hpp` (NOXNA)
     uploading plain `VertexPositionTexture` for DualDemo's meshes only.
  Also applied DEFERRED.md item #24's already-known `Clear(Color)`-never-
  clears-depth gap as a sample-level workaround in EnvmapDemo (2-arg
  `Clear(Color, float)` instead), since that scene's cross-frame depth-buffer
  reuse actually mattered here (unlike most other samples, where it's latent).
  **Visual result, confirmed live via screenshot:** BasicDemo's tank renders
  fully textured/shaded/animated on a checkered floor; DualDemo's 7-part
  scene shows real tiled texture detail plus the lightmap's glowing
  "hotspot" pattern correctly blended in; EnvmapDemo's saucer shows a
  genuinely reflective/chrome cubemap surface (dark blue/black tones with
  warm highlights, consistent with the source coastal photo) over its own
  correctly-rendered full-screen background image; AlphaDemo's 25 imposter
  tank billboards render correctly shaped via real alpha-test discard but
  noticeably dark (a plausible faithful single-directional-light
  characteristic, not conclusively root-caused as a bug — see missing.md).
  Live interactive mouse input was not exercised (same `xdotool`
  shared-desktop focus caveat as every other sample this session) — verified
  instead via this repo's established temporary debug-auto-trigger pattern
  (an env-var-gated `SetActiveMenu()` hook plus a forced `helpTimer_`, both
  reverted before commit, reconfirmed via a clean 0-warning rebuild
  afterward). Screenshot tooling itself was intermittently unreliable this
  session (captured stale frames from a previous demo scene despite the
  running process's own logged internal state being correct at capture time)
  — matches this repo's own already-documented "screenshot tooling has
  intermittently failed" caveat (section 5), not a code defect; re-capturing
  always eventually produced a screenshot matching the real, logged state.
  See `samples/ReachGraphicsDemo/missing.md` for the complete account.
- **TiltPerspective (#107) ported** (2026-07-10, follow-up session) — the
  second and last of the two samples covered by the 2026-07-10 user go/no-go
  decision (section 8 task 9), completing it. A small perspective-shifted 3D
  scene (a textured box, `DebugDraw.hpp`) viewed through a
  `Matrix.CreatePerspectiveOffCenter`-based camera that skews based on
  estimated device tilt, plus a ball-in-a-box physics simulation
  (`BallSimulation.hpp`, 25 balls colliding with the box walls and each other,
  gravity driven directly by the same tilt reading). Read
  `TiltPerspectiveSample.cs`/`AccelerometerHelper.cs`/`BallSimulation.cs`/
  `DebugDraw.cs`/`RandomUtil.cs`/`GeometricPrimitive.cs`/`SpherePrimitive.cs`/
  `VertexPositionNormal.cs` in full before writing any code, per this task's
  own brief. Builds 0 warnings (two from-scratch rebuilds — one before, one
  after removing temporary debug code); ran 8+ seconds with no crash across
  three separate runs.
  **Confirmed, per this task's own brief, that (unlike AccelerometerSample)
  this sample's own fallback genuinely has no interactive branch to
  promote** — a full read of `AccelerometerHelper.cs` found no `#if
  WINDOWS_PHONE` split at all (confirmed by the `.csproj`'s single
  `Windows Phone` configuration) and its own no-hardware path is a
  non-interactive, time-driven sinusoidal wobble
  (`FakeRollTheta += elapsed * FakeRollSpeed`) — no `Keyboard`/`GamePad`
  reference anywhere in the file. So a keyboard-tilt scheme was genuinely
  invented from scratch this time (NOXNA, per the user go/no-go): `Left`/
  `Right` → `X∓`/`X±`, `Up`/`Down` → `Y±`/`Y∓`, `Z` fixed at `-1`,
  `Vector3::Normalize()`'d — reusing the same shape AccelerometerSample's/
  Yacht's own *promoted* (not invented) fallbacks already established, for
  consistency, even though nothing in this sample's own original resembles
  it. Everything downstream (`BallSimulation.hpp`,
  `TiltPerspectiveGame::ComputeEyeVector()`) reads only
  `RawAcceleration`/`SmoothAcceleration` exactly like the original, so none of
  that code needed to change — only the input source did, exactly matching
  this task's design goal.
  **Applied DEFERRED.md item #5's still-open remainder** (the same
  texture-less-procedural-vertex gap Primitives3D's own `missing.md` already
  flagged): `GeometricPrimitive.hpp`/`SpherePrimitive.hpp` assign a dummy
  `(0,0)` UV to every procedurally-generated sphere vertex and use the
  already-proven `VertexPositionNormalTexture` + `BasicEffect` lit path,
  confirmed this does **not** hit DEFERRED.md item #26's vtable/stride bug
  (this sample never goes through `ModelTypeReader`/`Content.Load<Model>` at
  all — the geometry is built procedurally at runtime and uploaded via
  `VertexBuffer::SetData()`'s typed overload, which manually packs a plain,
  vtable-free 32-byte struct, confirmed by direct source read of
  `VertexBuffer.cpp`).
  **Found and root-caused (via the real FNA source, not assumed) a confirmed,
  faithfully-reproduced characteristic, not a CNA gap:** the balls render as
  near-black spheres with only a white specular highlight, because
  `GeometricPrimitive.cs` itself never sets `DirectionalLight0.DiffuseColor`
  (FNA's own `DirectionalLight.cs` confirms it defaults to `Vector3.Zero`,
  and `cna`'s own `DirectionalLight.cpp` matches this exactly) — the
  per-ball `DiffuseColor` tint is genuinely invisible in the **real XNA
  original too**, not just this port. A related dead-code finding in the same
  vein: `TiltPerspectiveSample.cs`'s own accelerometer-driven
  `worldGeometry.BasicEffect.DirectionalLight0.Direction = lightDirection;`
  line is immediately overwritten by `DebugDraw.cs`'s own `Draw()` (which
  unconditionally resets it to `-Vector3.UnitZ`), so the box's lighting never
  actually reflects tilt in the original either — both reproduced faithfully.
  Substituted the original's `TouchPanel.GetState().Count > 0` recalibration
  check with "hold the left mouse button" (continuously re-checked each
  frame, matching the original's own semantics) — confirmed by direct source
  read of `cna/src/CNA/Internal/Input/SdlInputBridge.cpp` that CNA's
  `TouchPanel` is fed only from real SDL finger events, never mouse clicks, so
  a literal port would have been permanently unreachable on this desktop.
  `TiltPerspectiveSample.htm` has no Sample Controls table at all (only a
  descriptive touch/tilt paragraph, the same situation Graphics3D's/
  MarbleMaze's own `missing.md` already documented) — used a one-off script
  (not committed to `tools/`) importing `gen_help_png.py`'s own
  `build_text()`/`render_png()` helpers with hand-written text describing
  this port's actual controls. Confirmed live via this repo's established
  temporary debug-auto-trigger pattern (both patches reverted before commit,
  re-verified with a clean from-scratch rebuild afterward — 0 warnings;
  `xdotool getactivewindow` showed a different real user window,
  `0x400003`, held focus throughout, so no synthetic keypresses were sent):
  forcing the synthesized tilt vector's `X` component confirmed the ball
  cluster visibly shifts/settles toward the tilted wall and the box's
  perspective framing visibly changes, proving the keyboard-tilt vector
  drives both `BallSimulation`'s gravity and the perspective shift, exactly
  mirroring how the original's own fake wobble would have driven both, just
  from keyboard state instead of a sine wave; the F1 help overlay rendered
  and then correctly disappeared after its 10-second timer. Converted
  `stone4.tga` (256×256 RGB Targa) to PNG directly via ImageMagick, the same
  established `.tga`→`.png` path already used by ClientServerSample's/
  AimingSample's own assets. No new DEFERRED.md item filed — the one
  genuinely new-to-this-sample gap (texture-less procedural vertices) is
  already fully covered by item #5's existing remainder, and every other
  finding is either a faithful, source-confirmed reproduction of the
  original's own code or an already-established repo-wide pattern. See
  `samples/TiltPerspective/missing.md` for the complete account. **With this
  sample done, both halves of the 2026-07-10 user go/no-go (task 9) are
  complete — see section 8's closing note for what (if anything) a future
  session should look at next.**
- **AccelerometerSample (#084) ported** (2026-07-10) — the first of the two
  samples covered by the 2026-07-10 user go/no-go decision (section 8 task 9):
  a sprite (an asteroid) slides around a starfield background purely from
  accelerometer/tilt input. Read `Accelerometer.cs`/`Game.cs`/`Program.cs` in
  full before writing any code, per this task's own brief. Builds 0 warnings
  (two from-scratch rebuilds — one before, one after removing temporary debug
  code); ran 7+ seconds with no crash across three separate runs. **The
  keyboard-tilt scheme is the original's own emulator fallback, ported
  verbatim, not invented** — `Accelerometer.cs:117-135` (inside the same
  `#if WINDOWS_PHONE` block as the real-hardware branch, gated at runtime on
  `DeviceType != DeviceType.Device`, i.e. "running in the Visual Studio phone
  emulator, which has no physical sensor") already synthesizes a `Vector3`
  from the arrow keys exactly the way this port's own `Accelerometer.hpp`
  does (`Left`/`Right` → `X--`/`X++`, `Up`/`Down` → `Y++`/`Y--`, `Z` fixed at
  `-1`, `Vector3::Normalize()`'d) — this port's `Accelerometer` class has no
  `#ifdef`/real-sensor branch at all, since that emulator branch is simply
  always taken (no phone hardware, no phone/emulator distinction to make on
  this desktop). This corrects the original scope-decision writeup's premise
  (DEFERRED.md item #15, updated this session) that this sample would need
  "a keyboard-tilt-emulation control scheme from scratch" — a closer reading
  of the original source this session found it already had one, just nested
  one level deeper than Yacht/SnowShovel/Bounce's own non-`#if` fallbacks.
  Confirmed live: with real (unmodified) keyboard state, the asteroid stays
  motionless at its correctly-centered rest position (screenshot). Live
  arrow-key input hit this repo's known `xdotool` shared-desktop caveat
  (`xdotool getactivewindow` showed a different real user window, `0x400003`,
  held focus throughout; a `keydown --window`/screenshot/`keyup` round trip
  produced no visible movement) — worked around with this repo's established
  temporary debug-auto-trigger pattern (forcing `acceleration.X = 0.5f` and,
  separately, `helpTimer_ = 10.0f` on the first frame; both reverted before
  commit): the asteroid visibly slid right and clamped correctly at the
  screen edge (two screenshots ~2 s apart, same clamped position in both —
  matching `Update()`'s own edge-clamp logic), and the F1 help overlay
  rendered and then correctly disappeared after its 10-second timer (two
  screenshots ~7 s apart). `Accelerometer.htm`'s Sample Controls table has 3
  columns (`Action | Windows Phone | Windows Phone - Emulator`); generated
  `Content/help.png` with a one-off variant script reading column 2 (the
  Emulator/keyboard column), the same precedent as MicrophoneEcho's own
  one-off column-2 script. Found (not a new gap — a second confirmed sighting
  of an existing, already-accepted characteristic) that this sample's
  `help.png` (632×192) is wider than its 480px-wide portrait window, so the
  centered overlay text is left/right-clipped — `samples/Yacht/Content/
  help.png` (1472×312) has the identical characteristic in that sample's own
  identically-480px-wide window, using the same unmodified `Draw()`-time
  centering code from CLAUDE.md's F1 pattern; not a regression, not fixed,
  same as Yacht's own unaddressed instance. Converted `asteroid.png`
  (128×128 RGBA) and `space.png` (480×800 RGB) directly — both were already
  PNG in the original's `AccelerometerContent/` directory, no reconversion
  needed. No new DEFERRED.md item filed — every finding is either a faithful
  direct port of the original's own code or an already-tracked, previously
  documented pattern. See `samples/AccelerometerSample/missing.md` for the
  complete account. TiltPerspective (#107, the second half of the same
  go/no-go decision) was ported in the very next follow-up session — see the
  entry above and `samples/TiltPerspective/missing.md`.
- **PeerToPeer (#103) ported** (2026-07-10) — the third and final sample in this
  repo's `NetworkSession`/`GamerServicesComponent` family, and the first with a
  genuinely different network *topology*: full peer-to-peer broadcast with **no
  host authority over simulation at all** — every machine fully simulates its own
  locally-controlled tanks (`UpdateLocalGamer()`) and broadcasts the result to
  everyone via `LocalNetworkGamer::SendData(PacketWriter&, SendDataOptions)`'s
  broadcast overload (no explicit recipient), unlike ClientServerSample's
  single-authority model or NetworkPrediction's prediction/smoothing layered on top
  of that same split. Builds 0 warnings (two from-scratch rebuilds — one before, one
  after removing the temporary debug-auto-trigger code); ran 23-30+ seconds with no
  crash across three separate runs. **Confirmed the "zero networking workarounds
  needed" hypothesis a third time**, on a sample with a real topology difference from
  the other two, not just a different application-level mechanic: the constructor
  adds a real `GamerServicesComponent` exactly like the C# original, `HookSessionEvents()`
  needs no manual `Update()` call, and `networkSession_->getIsHostProperty()`/
  `gamer->getIsHostProperty()` are used directly (here only for a cosmetic "(host)"
  label — this sample's `Update()` loop has no `if (IsHost)` branch anywhere, matching
  the C# original exactly). Confirmed live via this repo's established temporary
  debug-auto-trigger pattern (`CreateSession()` + `helpTimer_` forced on frame 30,
  reverted before commit, re-verified with a clean from-scratch rebuild afterward —
  `xdotool getactivewindow` showed a different real user window, `0x400003`, held
  focus throughout, so no synthetic keypresses were sent): session creation completes
  with no hang, `GamerJoinedEventHandler` fires synchronously (a fully textured tank
  labeled `"Stub Gamer (host)"` renders on the very first frame after the trigger),
  and the F1 help overlay renders correctly and disappears after its 10-second timer
  (confirmed via two screenshots ~6 seconds apart). **Found no new CNA gap** — unlike
  NetworkPrediction, this sample's own C# original never references
  `NetworkSession.SessionProperties` at all, so DEFERRED.md item #27 (found while
  porting NetworkPrediction) simply never comes up here; no `PacketKind`-byte
  workaround was needed since every packet on the wire is always the same shape
  (position/rotation/turret-rotation, nothing else). Confirmed `Tank.cs` is
  byte-identical to ClientServerSample's own `Tank.cs` (only the C# namespace
  differs) via `diff`/`md5sum`, and reused ClientServerSample's/NetworkPrediction's
  already-converted `Tank.png`/`Turret.png`/font assets directly (same underlying
  `.tga`/`.spritefont` source files, confirmed byte-identical). Real tank movement via
  keyboard input and a genuine two-machine broadcast test were not exercised live this
  session (same `xdotool`-focus and two-process-discovery limitations already
  documented for ClientServerSample/NetworkPrediction) — see
  `samples/PeerToPeer/missing.md` for the complete account, including why this
  specific gap in live coverage is a real, not-yet-closed verification gap for this
  sample's own distinctive mechanic (full N-way broadcast sync) specifically, unlike
  the other two samples' single-gamer verification, which covers their own
  distinctive mechanics by analogy to already-proven `Tank.hpp` code.
- **NetworkPrediction (#100) ported** (2026-07-10) — section 8 task 7's recommended
  next candidate: client-side prediction and remote-entity smoothing to hide network
  latency and a throttled packet-send rate (`Tank.hpp`'s `UpdateLocal`/`UpdateRemote`/
  `ApplyPrediction`/`ApplySmoothing`, `RollingAverage.hpp`'s clock-skew compensation,
  both direct ports of `Tank.cs`/`RollingAverage.cs`). Builds 0 warnings (two
  from-scratch rebuilds); ran 9+ seconds with no crash across two runs. **Confirmed
  the "zero networking workarounds needed" hypothesis this task was specifically
  set up to test**: unlike ClientServerSample (#091), which needed all 3 of
  DEFERRED.md items #19/#20/#21 (`GamerServicesDispatcher` hang, `IsHost`/`Id`
  stubs, non-synchronous `GamerJoined`), this sample needed **none** of them — a
  real `GamerServicesComponent` is added in the constructor exactly like the C#
  original, `networkSession_->getIsHostProperty()`/`gamer->getIsHostProperty()` are
  used directly with no locally-tracked bool, and `HookSessionEvents()` needs no
  extra manual `Update()` call. Confirmed live via this repo's established
  temporary debug-auto-trigger pattern (`CreateSession()` + `helpTimer_ = 10.0f`
  forced on the first frame, reverted before commit, re-verified with a clean
  from-scratch rebuild afterward): session creation doesn't hang, `GamerJoined`
  fires synchronously (a fully textured, correctly labeled `"Stub Gamer"` tank
  renders on the very first frame with no manual `Update()` call anywhere), the
  session correctly reports itself as host, and the F1 help overlay renders
  correctly on top. Found **one new, genuine CNA gap**: `NetworkSession::
  SessionProperties` has no mutable accessor (`getSessionPropertiesProperty()` is
  const-only, confirmed via direct source read of both the header and `.cpp`) and
  is never replicated over the wire (`sessionProperties_` is set once at
  construction and never touched again anywhere in `NetworkSession.cpp`) — the C#
  original relies entirely on this for its host-authoritative network-quality/
  prediction/smoothing settings. Worked around (NOXNA, no C# equivalent) with an
  explicit host-broadcast "options packet," distinguished from ordinary tank-state
  packets by a new leading `PacketKind` byte on the same `LocalNetworkGamer::
  SendData`/`ReceiveData` channel — `Tank.hpp` itself is completely unaffected
  (matches `Tank.cs` line-for-line). Filed as new DEFERRED.md item #27. Reused
  ClientServerSample's already-converted `Tank.png`/`Turret.png`/font assets
  directly (confirmed byte-identical source `.tga`/`.spritefont` files via
  `md5sum`/`diff` — no reconversion needed). See
  `samples/NetworkPrediction/missing.md` for the complete account.
- **MarbleMaze (#061) ported** (2026-07-10) — a full "Phase 4 — Full Games"
  title: `ScreenManager`-based menu/gameplay flow (main menu, loading/
  instructions, gameplay, pause, high score) wrapping a marble-in-a-tilting-maze
  physics sim (`Marble.hpp`/`Maze.hpp`/`DrawableComponent3D.hpp`, direct
  `TriangleSphereCollisionDetection.hpp` port). Builds 0 warnings; ran 8+
  seconds with no crash across several runs. Applied DEFERRED.md item #26's
  `RawMesh.hpp` bypass proactively from the start (not re-confirmed
  empirically against a plain `Content.Load<Model>` build this time, given 4
  prior independent confirmations already on record — see missing.md),
  generalized beyond ChaseCamera's/InverseKinematics' single-mesh-per-model
  shape to support the maze's 6 separately-textured sub-meshes (walls, 3 floor
  variants, topWall, floorSides) sharing one `BasicEffect`. Found a **second**
  confirmed instance of the `assimp export`-introduced triangle-winding
  inversion first seen on ChaseCamera's `Ground.x` — this time on an `.FBX`
  source, across 5 of the maze's 6 parts at once (only `walls` was unaffected);
  fixed with a permanent `RasterizerState::CullNone` around the whole maze
  draw, isolated live via screenshot (only `walls` visible before, full maze
  visible after, no other change). No `.htm` exists for this sample (a 101-page
  `.doc` tutorial instead) — used a one-off `gen_help_png.py`-based script for
  the F1 overlay text, describing this port's own direct keyboard-tilt mapping
  (derived algebraically from `Accelerometer.cs`'s own emulator/keyboard
  fallback — the established DEFERRED.md item #15 pattern, not an invented
  scheme; no `CalibrationScreen` ported, since its only call site is
  `DeviceType.Device`-gated dead code on desktop). Confirmed via this repo's
  established temporary debug-auto-trigger pattern (reverted before commit,
  since a different real user window held focus throughout — confirmed via
  `xdotool getactivewindow`): the full 3D scene renders correctly (maze fully
  textured/shaded after the winding fix; marble textured, lit, and resting
  stably on the floor with no falling-through over 11+ seconds of passive
  observation — direct proof the runtime-reconstructed collision triangle
  lists work); maze tilt physics correctly rotates the display while collision
  math stays in the maze's unrotated rest frame, exactly matching the
  original's architecture; F1 help overlay renders correctly. The
  menu→loading→gameplay screen *transitions themselves* were not clicked
  through live this session (same `xdotool` focus caveat) — see missing.md's
  Verification section for the precise scope of what was and wasn't exercised.
  Also found and fixed a stale DEFERRED.md item: #9 (`Viewport.AspectRatio`)
  claimed the property didn't exist, but a live grep of `cna`'s current source
  found `getAspectRatioProperty()` already implemented — marked resolved.
  `Content.Load<Model>`'s custom-`ContentProcessor` gap (item #18,
  `MarbleMazeProcessor`'s build-time triangle-list extraction) was worked
  around by reconstructing the same data at runtime from `RawMesh`'s own
  already-loaded vertex/index buffers (`RawMesh::ExpandTrianglePositions()`) —
  no new sidecar file format needed, unlike TrianglePicking's `--picking` tool
  flag. See `samples/MarbleMaze/missing.md` for the complete account,
  including several smaller documented deviations (synchronous asset loading
  instead of a background thread; `std::fstream` high-score persistence
  instead of `IsolatedStorageFile`; a fixed "Player" high-score name instead of
  `Guide.BeginShowKeyboardInput`).
- **ChaseCamera (#058) ported** (2026-07-10) — a spring-physics chase camera
  (`ChaseCamera.hpp`, pure `Vector3`/`Matrix` math ported directly, no CNA gaps)
  following a ship (`Ship.hpp`, simple flight physics + mouse/keyboard/gamepad
  input) flying over a large checkered ground plane. Builds 0 warnings, runs 8+
  seconds with no crash across three separate runs. **A third and fourth
  independent confirmation of DEFERRED.md item #26's `ModelTypeReader`
  vertex-corruption bug**, per this task's own brief to test empirically rather
  than assume: a temporary test build using plain `Content.Load<Model>("Ship")`/
  `Content.Load<Model>("Ground")` (converted via `tools/fbx_ascii2model.py` and
  `assimp export` + `tools/obj2model.py` respectively — both ordinary stride-32
  `.model.json` files, no load-time error) rendered **nothing at all** — a solid
  CornflowerBlue screen with only 2D HUD text visible, confirmed via two
  screenshots 3 seconds apart, no crash — at this sample's own ~4031-unit initial
  camera distance (even farther than Graphics3D's ~3523-unit spaceship, which
  showed the same "fully invisible" symptom). Worked around with a new
  `samples/ChaseCamera/src/RawModel.hpp` (NOXNA), the same bypass shape as
  InverseKinematics' `CylinderModel.hpp`/HeightmapCollision's `Terrain.hpp`,
  generalized to also bind a real `Texture2D` directly to the `BasicEffect` (the
  same side benefit `Terrain.hpp` already established) — confirmed live via
  screenshot: both the ship (`ShipDiffuse.png`, 32458 vertices — two orders of
  magnitude larger than InverseKinematics' 418-vertex cylinder) and the ground
  (`Checker.png`, only 6 vertices — the smallest mesh yet tested through this
  bug) render correctly, fully textured and shaded, at both size extremes. This
  independently reinforces item #26's hypothesis a third and fourth time, on two
  more assets converted through two different pipelines (FBX and `assimp`
  `.x`→`.obj`), uncorrelated with mesh size/complexity. Separately found (not a
  #26 symptom): `Ground.x`'s `assimp export` conversion re-emits its two
  triangles wound the opposite way from CNA's default
  `RasterizerState::CullCounterClockwise`, so the ground was fully back-face
  culled even after the `RawModel.hpp` fix — isolated live by temporarily
  forcing `RasterizerState::CullNone` around only the ground's draw call, which
  alone made it appear; kept as a permanent, documented per-asset adjustment
  (mirroring the same winding accommodation `HeightmapCollision`'s/
  `GeneratedGeometry`'s own `Terrain.hpp` already needed for their runtime-built
  meshes, just the first time seen on an `assimp`-round-tripped `.x` asset
  specifically). F1 help overlay and ship-movement/camera-spring-physics both
  verified live via this repo's established temporary debug-auto-trigger pattern
  (removed before commit) — `xdotool getactivewindow` showed a different real
  user window had focus throughout, so no synthetic keypresses were sent, per
  this repo's own shared-desktop `xdotool` caveat. See
  `samples/ChaseCamera/missing.md` for the complete account.
- **InverseKinematics (#057) ported** (2026-07-10) — the Cyclic Coordinate Descent
  (CCD) inverse-kinematics algorithm, demonstrated via a 20-link chain of cylinder
  models reaching for a billboarded "cat" sprite (plus a faithfully-ported, but
  legitimately inert, Xbox LIVE avatar IK demo — see below). Builds 0 warnings,
  runs 9+ seconds with no crash. **Found and worked around a major, previously
  undiagnosed CNA bug**, likely the true root cause of the long-tracked
  "near-plane-clipping" bug family (section 4): `ModelTypeReader::Read()`
  (`ContentManager.cpp`) picks a typed `VertexBuffer::SetData` overload by
  comparing a `.model.json`'s declared `"vertexStride"` (always one of XNA's
  clean, unpadded sizes — 16/20/24/32) against `sizeof()` of CNA's own vertex
  structs — but every one of those structs now inherits from the polymorphic
  `IVertexType`, adding an 8-byte vtable pointer that inflates their real sizes to
  40/32/56/40 respectively (confirmed live via a standalone `sizeof()` probe).
  `"vertexStride": 32` — the value every `Content.Load<Model>`-based sample in
  this repo uses — *accidentally* equals `sizeof(VertexPositionTexture)` (also
  32, by coincidence), so the reader always dispatches to the *wrong* overload
  and reinterprets the raw (vtable-free) file bytes as vtable-shifted
  `VertexPositionTexture` objects, reading position/texcoord data from the wrong
  byte offsets — silently corrupting every stride-32 model's vertex data
  repo-wide, not just this sample's. Found via systematic isolation: a
  straightforward `cylinder.model.json` port built and loaded correctly (1 mesh,
  418 vertices, verified via debug instrumentation) but never rendered at any
  camera distance/scale/cull-mode; a hand-built triangle and even PickingSample's
  own already-shipped `Cylinder.model.json` both failed the same way through this
  sample's code, while HeightmapCollision's `sphere.model.json` (3252 vertices)
  rendered correctly at every scale tested — isolating the reader, not any one
  asset, as the cause. Worked around with a new `CylinderModel.hpp` (NOXNA) that
  reads the already-converted `cylinder_verts.bin`/`cylinder_idx.bin` directly and
  constructs real C++ `VertexPositionNormalTexture` objects field-by-field (not a
  `reinterpret_cast` on a raw blob), then uploads via the same typed `SetData`
  overload the reader was trying to reach — confirmed live this renders correctly
  (a lit, visibly-shaded, correctly-curling 20-link chain). Filed as new
  DEFERRED.md item #26, with a strong (but not 100%-confirmed-for-other-samples)
  suspicion this explains the thin-line/invisibility symptom for every other
  affected sample too — see section 4 and item #26 for the full reasoning and a
  recommended next step (try the same bypass on CameraShake's `tank.model.json`
  before assuming the bug is really in clip-space math). The avatar half of the
  sample (`AvatarRenderer`) is ported faithfully but renders nothing — confirmed
  by direct source read that `AvatarRenderer::State` is permanently
  `Unavailable` off a signed-in Xbox LIVE/Games-for-Windows-Live session, exactly
  what the C# original's own `UpdateAvatarIK()`/`DrawAvatar()` already guard
  against — not a CNA gap. See `samples/InverseKinematics/missing.md` for the
  complete account.
- **HeightmapCollision (#049) ported** (2026-07-10) — a rolling ball on a
  procedurally-generated terrain, with the camera and ball both queried against a
  `HeightMapInfo` height-lookup class. Builds 0 warnings, runs 7+ seconds with no
  crash across two separate runs; screenshots show a fully **textured and shaded**
  terrain (unlike every other `Content.Load<Model>`-based sample so far) with the
  ball sitting correctly on its surface. The interesting engineering question was
  the terrain: XNA's original generates it at content-**build** time from
  `terrain.bmp` via a custom `TerrainProcessor`/`HeightMapInfoContent`
  `ContentProcessor` pair, attaching the collision height data to the built
  `Model`'s `Tag`. CNA has neither a `Model.Tag` equivalent nor custom-
  `ContentProcessor` extensibility (item #18), so — after reading this repo's own
  `GeneratedGeometry` sample first and confirming it already ships a *structurally
  identical* `TerrainProcessor` in its own C# original — this port builds the
  terrain mesh **and** the `HeightMapInfo` collision data together at runtime
  (`Terrain.hpp`, NOXNA), the same adaptation `GeneratedGeometry` already
  established, not a new pattern. A second, independent reason favored this over
  `Content.Load<Model>`: `terrain.bmp` is 257×257 = 66049 vertices, over the 65535
  limit of a 16-bit index buffer, and a direct source read confirmed CNA's
  `.model.json` `ModelTypeReader` hardcodes 16-bit indices with no way to request
  32-bit — a genuine, narrower nuance of item #6 not previously documented (added
  as a new addendum this session), even though `IndexBuffer` itself fully supports
  `IndexElementSize::ThirtyTwoBits` end-to-end. Building the terrain directly with
  a real 32-bit `IndexBuffer` sidestepped both gaps at once. A pleasant side
  effect: because the terrain never goes through `.model.json` (which has no
  per-mesh texture field — item #6's PickingSample/TrianglePicking addendum), its
  hand-built `BasicEffect` + real bound `Texture2D` renders **correctly textured
  and shaded** — the sphere, which *is* loaded via `Content.Load<Model>("sphere")`,
  still hits the known flat-white gap as expected. This sample's camera sits only
  ~155 units from the ball (much closer than the ~1000+ unit distances that
  trigger the tracked near-plane-clipping bug elsewhere in this repo) — confirmed
  via screenshot that neither the ball nor the terrain shows that artifact here;
  not claiming the bug is fixed, just that this sample's own geometry doesn't
  trigger it. This sample also adds no `GameComponent`s at all, so item #23 simply
  doesn't apply. See `samples/HeightmapCollision/missing.md` for the full account.
- **TrianglePicking (#048) ported** (2026-07-09) — close sibling of PickingSample
  (#047, byte-identical FBX assets confirmed via `cmp`), replacing its per-object
  `BoundingSphere`-only test with real per-triangle ray intersection
  (Moller-Trumbore). CNA has no `Model.Tag`/custom-`ContentProcessor` equivalent
  (item #18) *and*, confirmed this session, no `VertexBuffer`/
  `IndexBuffer.GetData()` runtime readback either (new DEFERRED.md item #25) — the
  C# original's own per-triangle vertex data (normally attached to `Model.Tag` at
  content-build time by a custom `TrianglePickingProcessor`) is instead produced
  by a new `--picking` option added to `tools/fbx_ascii2model.py`, which emits a
  flat binary sidecar of triangle-expanded vertex positions per model, read back
  directly by the C++ port at `LoadContent()` time. Builds 0 warnings, runs 7+
  seconds with no crash. Confirmed the already-tracked near-plane-clipping
  thin-line artifact on the `Sphere` model (consistent every frame, since this
  sample's camera doesn't auto-rotate unlike PickingSample's port) and the
  already-tracked flat-white-saturation finding (DEFERRED.md item #6 addendum) —
  both immediately recognized as known, not re-diagnosed. Also clarified
  DEFERRED.md item #23 (`Game::DoInitialize()`'s `ComponentAdded` timing gap):
  confirmed this sample's C# original adds its `Cursor` component from the
  *constructor*, not `Initialize()` (unlike PickingSample/Graphics3D), and by
  reading `Game.cpp` directly, confirmed this genuinely does **not** need the
  `AddComponent()` workaround — CNA's base `Game::Initialize()` has its own
  unconditional per-component-initialize pass that already catches
  constructor-time adds regardless of `ComponentAdded` subscription timing.
  Unexpectedly got a live (non-debug-forced) confirmation that real per-triangle
  picking works correctly end-to-end: an earlier `xdotool mousemove --window`
  call (issued while checking this repo's known `xdotool` focus caveat) moved the
  real OS pointer over the window even without confirmed focus, and a later clean
  screenshot shows correct bounding-sphere-list text and a correct per-triangle
  picked-model name label. See `samples/TrianglePicking/missing.md` for the full
  account.
- **PickingSample (#047) ported** (2026-07-09) — `DrawableGameComponent`
  (`Cursor`), stock `Model`/`BasicEffect`, `Viewport::Project`/`Unproject`,
  `Ray`/`BoundingSphere::Intersects` for mouse-ray picking against 5 FBX models
  (table + 4 picked objects). Builds 0 warnings, runs 5+ seconds with no crash.
  All 5 source FBX files were plain ASCII, converted directly with
  `tools/fbx_ascii2model.py` (no binary-FBX workaround needed this time). Hit
  and worked around the same `Game::DoInitialize()` component-lifecycle gap
  Graphics3D found (DEFERRED.md item #23 — `Cursor` is created and added to
  `Components` from inside `Initialize()`, matching the C# original). Also hit
  and worked around a related, previously-only-implicitly-documented gap:
  `ModelMesh::ParentBone` is always `nullptr` for `.model.json`-loaded models
  (DEFERRED.md item #6's existing "multi-bone" note already covered the root
  cause) — added a `BoneIndexOf()` fallback-to-0 helper, the same guard CNA's
  own `Model::Draw()` already uses internally. Found that every model renders
  as a flat, fully-saturated white shape with zero shading gradient (confirmed
  angle-independent via multiple screenshots) — root-caused to the combination
  of CNA's `.model.json` format having no per-mesh texture field (an existing
  gap, first flagged by LensFlare's `ground.png`) and XNA's bright default
  3-point lighting rig clamping to white on an untextured, default-white
  material; added an addendum to DEFERRED.md item #6 documenting this more
  severe consequence (not a new item — same root cause). Also separately
  observed the already-tracked near-plane-clipping thin-line artifact once, at
  one camera angle, confirming it's independent of the flat-white finding. See
  `samples/PickingSample/missing.md` for the full account.
- **Graphics3D (#046) ported** (2026-07-09) — `DrawableGameComponent` (touch
  buttons ported to mouse), stock `Model`/`BasicEffect` with 3 directional
  lights + specular + per-pixel-lighting toggle, sprite-sheet animation. Builds
  0 warnings, runs 6+ seconds with no crash. Converted `spaceship.fbx` (an old
  *binary* FBX 6.1 file, unreadable by `assimp`/Blender/this repo's own
  `tools/fbx_ascii2model.py`) via a one-off `ufbx`-based script → `.obj` →
  `tools/obj2model.py`. Found and worked around a real CNA component-lifecycle
  bug (`Game::DoInitialize()` wires `ComponentAdded` after calling the user's
  `Initialize()`, DEFERRED.md item #23) that segfaulted the port on its first
  frame. The spaceship model itself doesn't render — extensively isolated this
  session to the same near-plane-clipping-family bug already tracked for
  `CameraShake`/`CustomModelClass`/`LensFlare`, now confirmed to also cause full
  invisibility (not just a thin line) at longer camera distances. Also found
  `GraphicsDevice::Clear(Color)` never clears the depth buffer (DEFERRED.md item
  #24, latent, not blocking). See `samples/Graphics3D/missing.md` for the full
  account.
- **LensFlare (#041) ported** (2026-07-09) — `DrawableGameComponent`, stock
  `Model`/`BasicEffect`, and `OcclusionQuery`, all working end-to-end with no
  CNA-side API gaps for the sample's own code. Builds 0 warnings, runs 5+ seconds
  with no crash. Found and fixed a real bug in `tools/fbx_ascii2model.py` along the
  way (see below). See `samples/LensFlare/missing.md` for the full account.
- `tools/fbx_ascii2model.py` now applies each FBX mesh's baked
  `PreRotation`/`LclRotation`/`LclScaling`/`LclTranslation` node transform instead
  of silently ignoring it — `terrain.fbx` (LensFlare's asset) has a `-90,0,0`
  `PreRotation` (a Z-up → Y-up correction) that, unapplied, put the entire terrain
  outside the camera's view volume (rendered as nothing at all, not even the
  near-plane-clipping artifact). Confirmed this does **not** change `tank.fbx`'s
  already-shipped output (all its `PreRotation`s are `0,0,0`) — see DEFERRED.md
  item #6's addendum.
- `Microsoft::Xna::Framework::Audio::Microphone` + `DynamicSoundEffectInstance`
  (MicrophoneEcho, #098) — capture-device enumeration, `BufferReady`/`GetData()`,
  loopback playback. Live-verified: a real capture device was found on this
  machine and the waveform render pipeline works.
- `Microsoft::Xna::Framework::Net::NetworkSession` + `GamerServices` (basic path)
  (ClientServerSample, #091) — session creation, `GamerJoined` handling, packet
  send/receive, all live-verified for a solo (single-gamer) session. Three real
  gaps found and worked around at the sample level (section 5).
- `VertexPositionNormalTexture` lit rendering (`BasicEffect.EnableDefaultLighting`)
  in the EasyGL backend — proven by a live-run integration test
  (`cna_test_easygl_basiceffect_combinations`, case "(e)", exit 0) and by porting
  CustomModelClass (#052), which builds and runs against it.
- `CNA_Net`/`CNA_GamerServices` CMake wiring in this repo (previously the targets
  existed in `cna` but were never enabled/linked here — see section 3).

### Known NOT working
- ~~Full aggregate build fails~~ **FIXED 2026-07-10** — see Build subsection
  above. Both affected files (`SafeArea/src/SafeAreaOverlay.hpp`,
  `RolePlayingGame/src/{RolePlayingGame.hpp,TileEngine/TileEngine.hpp}`) are
  fixed; the full aggregate build is green.
- Visual correctness of tank-model-based lit rendering: CustomModelClass (#052)
  and the pre-existing CameraShake both render `tank.model.json` as a thin
  diagonal line instead of a recognizable model — a real, unfixed EasyGL
  near-plane clipping bug (section 4/5), not specific to either sample. **Now
  confirmed on two further, independent assets:** LensFlare's `terrain.model.json`
  shows the identical thin-line artifact; Graphics3D's spaceship (at a longer
  camera distance, ~3523 vs. ~1059 units) renders as **fully invisible** instead —
  same underlying bug family, a new/different visible symptom depending on camera
  distance (isolated via direct swap-in testing, see
  `samples/Graphics3D/missing.md`).
- **EasyGL backend never applies `BlendState.ColorWriteChannels`** (no
  `glColorMask` anywhere in `EasyGLGraphicsBackend`) — found via LensFlare's
  occlusion-query trick, which relies on `ColorWriteChannels.None` to keep its test
  polygon invisible; it renders as a solid white square instead. DEFERRED.md item
  #22 (new, not started). See `samples/LensFlare/missing.md` for the full account.
- LensFlare's glow/flare sprites (gated on the occlusion query's pixel count) were
  never observed to appear during this session's live verification — not
  root-caused; may be a symptom of the `ColorWriteChannels` gap above, or a
  separate issue. Flagged in `samples/LensFlare/missing.md`, not yet its own
  DEFERRED.md item.
- **New: `Game::DoInitialize()` wires `Components_.ComponentAdded` after calling
  the user's `Initialize()` override**, not before (real XNA/FNA does this in the
  `Game` constructor, before `Initialize()` can run) — a component added to
  `Components` from within `Initialize()` (a pattern the Graphics3D C# original
  relies on) never gets its own `Initialize()`/`LoadContent()` called, since the
  event that would trigger it isn't subscribed yet. Segfaulted Graphics3D's first
  `Draw()` until worked around. DEFERRED.md item #23 (new, not started). See
  `samples/Graphics3D/missing.md`.
- **New: `GraphicsDevice::Clear(Color)` (single-arg overload) never clears the
  depth buffer** — confirmed via direct source read; only the multi-arg
  `Clear(...)` overloads do. Affects every 3D sample in this repo (all use the
  single-arg overload), latent but not currently blocking anything observed.
  DEFERRED.md item #24 (new, not started).
- Multi-gamer `NetworkSession` state routing: `NetworkGamer.Id` is a hardcoded
  stub (always `0`), so `FindGamerById()` always resolves to the first gamer —
  correct for solo sessions (verified), wrong once a second gamer joins
  (unverified — no second instance was tested).
- Interactive keyboard/mouse input verification via `xdotool` on this shared
  development desktop is unreliable (section 5) — blocks live-testing new
  samples' input handling, not the samples' own code.

---

## 3. Recent changes

**Newest session (2026-07-10, tenth follow-up):** Ported **RimLighting (#037)**
— the sample section 8 task 6's retirement note and DEFERRED.md item #14 both
flagged as "the closest remaining portable sample," needing only the same
`TextureCube`-bypass technique ReachGraphicsDemo's `EnvmapDemo` had just proven
a few sessions earlier. Read `Game1.cs`, `Camera/{Arcball,ModelViewerCamera}.cs`,
and `UI/{UIElement,Button,Slidebar}.cs` in full, plus `EnvmapDemo.hpp`'s own
header comment and `samples/ReachGraphicsDemo/missing.md`, before writing any
code, per this task's own brief.

Demonstrates `EnvironmentMapEffect`-based rim lighting: `World`/`View` are
swapped (`World <- World*View`, `View <- Identity`) so the cube-map lookup
happens in view space while screen space stays the same, against a cube map
where every face is dark except a bright "back" face — producing a rim/
silhouette highlight, per `RimLighting.htm`'s own "How the Sample Works"
section. **Both of this sample's two real content-pipeline gaps were
bypassed, not fixed in `cna`:**
- `Content.Load<TextureCube>` (DEFERRED.md item #14): `OutputCube.dds` is a
  real, already-baked 6-face DDS cubemap (uncompressed `xRGB8888`, so CNA's
  own `TextureCube::DDSFromStreamEXT` — checked first — couldn't be used
  either, it only decodes DXT-compressed cube maps). Extracted the 6 faces to
  PNGs via ImageMagick (`convert OutputCube.dds[N] ...`), loaded via
  `Content.Load<Texture2D>`, copied into a real `TextureCube` via `SetData()`
  — the same bypass philosophy as `EnvmapDemo`'s cubemap, applied to a
  differently-sourced asset. Face order confirmed correct on the first try
  (DDS index N ↔ `CubeMapFace(N)`, matching `DDSFromStreamEXT`'s own internal
  `static_cast`), confirmed both by inspecting the 5-black-1-bright face
  layout before writing any C++ and by the final render showing a clean,
  correctly-shaped rim highlight (a wrong mapping would have produced a
  scrambled or mis-shaped one instead).
- `Content.Load<Model>` (DEFERRED.md item #26): `head.fbx` bypassed via a new
  `HeadModel.hpp` (the established `RawModel.hpp`-style pattern), applied
  proactively. This model also has a genuinely non-identity **parent Null
  bone** ("group", real translation + 180°-Y rotation) — baked into the
  vertex data at conversion time by a one-off script (not committed to
  `tools/`) since the group node is static (no animation Take references it),
  mathematically identical to what `Model.CopyAbsoluteBoneTransformsTo()`
  would recompute every frame for this non-animated 2-node hierarchy.

**Found and fixed a second real `tools/fbx_ascii2model.py` bug this
session (see DEFERRED.md item #30, new):** the tool assumed every FBX's
`LayerElementNormal` uses `MappingInformationType: "ByPolygonVertex"` (one
normal per polygon corner) — `head.fbx` actually declares `"ByVertice"` (one
normal per unique vertex, confirmed live: `len(normals) == len(positions) ==
8213`, not `len(poly_indices) == 32752`), and the tool's `build_buffers()`
always indexed by the per-corner flat index regardless, silently corrupting
~75% of this mesh's normals (falling back to a hardcoded "straight up"
default). Found via a temporary debug build forcing `EnvironmentMapAmount` to
`0`, which rendered a perfectly clean silhouette (isolating the defect to the
reflection/normal-dependent term, not raw geometry) — the un-fixed render
showed a recognizable head shape but with severe jagged/scrambled dark
patches wherever the reflection was active. **A repo-wide grep found
`"ByVertice"` is actually the more common mapping mode of the two** —
ChaseCamera's `Ship.fbx` and ReachGraphicsDemo's `saucer.fbx`/`model.fbx`
(all converted via this same shared tool) are `"ByVertice"` too, meaning this
bug likely also affects their own already-shipped `_verts.bin` normals (not
re-verified or re-shipped this session — flagged as a follow-up in DEFERRED
item #30). Fixed the tool to read the mapping mode and index correctly for
both cases; confirmed **zero regression** by re-running `P2Wedge.FBX`/
`Cats.FBX` (both `"ByPolygonVertex"`) through the fixed tool and diff'ing the
output byte-identical to the already-shipped files.

Also confirmed **not** applicable here: DEFERRED.md item #28 (a full-backbuffer
`SpriteBatch` draw before any 3D draw breaks that frame's 3D rendering) doesn't
trigger, since this sample's own `Draw()` order (matching the original
exactly) draws the 3D head model first, then the 2D UI — the opposite order
from what item #28 requires.

**NOXNA input substitution:** CNA's own `TouchPanel` (confirmed via a fresh
read of its current source) only reports real touch hardware, with no
mouse-to-touch synthesis fallback, and this dev machine has no touchscreen —
`RimLightingGame::SynthesizeTouches()` builds at most one `TouchLocation` per
frame from `Mouse::GetState()`'s left-button edge transitions, fed through the
**exact same, unmodified** `Arcball`/`Button`/`Slidebar`/`ModelViewerCamera`
`HandleTouch(TouchLocation)` methods the C# original uses. Also split each UI
element's `Draw()` into a 3D part (`Button::DrawBox()`, before any
`SpriteBatch` `Begin()`) and a 2D part (`DrawText(SpriteBatch&)`, inside this
repo's own established single-`Begin()`/`End()`-block-per-frame convention),
instead of each element opening/closing its own block like the C# original.

Builds 0 warnings (multiple from-scratch rebuilds, the last one after removing
all temporary debug instrumentation — both the `EnvironmentMapAmount`-forcing
build used to isolate the normals bug, and a `helpTimer_`-forcing build used
to verify the F1 overlay without live keyboard input). Ran 15+ seconds with no
crash across several runs; screenshots confirm a clean, anatomically-consistent
rim-light highlight (ears, jaw, chin, nose bridge, mouth outline) and correct
UI (button + both sliders showing live `Amount`/`Thickness` text). Interactive
mouse-drag rotation was **not** exercised live this session (the same
shared-desktop `xdotool` focus caveat as several other samples this session —
a real, unrelated window held actual X focus, confirmed via
`xdotool getactivewindow` before attempting any synthetic input) — the
arcball/camera code itself was reviewed line-by-line against the C# original
instead. See `samples/RimLighting/missing.md` for the complete write-up.

**Previous session (2026-07-10, ninth follow-up):** Ported **ReachGraphicsDemo
(#005)** — see section 2's "Recently implemented / working" entry above for
the complete account (scope decisions, DEFERRED.md items #28/#29, the
`tools/fbx_ascii2model.py` multi-UV-layer bug fix, and the full live
verification account). Summary: this Phase 1 sample's own `missing.md` had
gone stale (flagged earlier this same multi-session run, section 8's prior
closing notes) — a fresh audit confirmed 5 of its 6 demo scenes were
portable with zero `cna` changes (only `SkinnedDemo` remains blocked, on
DEFERRED.md item #13). Ported the shared menu framework plus `BasicDemo`,
`AlphaDemo`, `DualDemo`, `EnvmapDemo`, and `ParticleDemo` faithfully; skipped
`SkinnedDemo` with a clear in-app "not available" message. Builds 0 warnings
(multiple from-scratch rebuilds, the last one after removing all temporary
debug instrumentation); ran 10+ seconds with no crash across several runs.
Found and worked around 2 new CNA rendering gaps (DEFERRED.md items #28, #29)
and fixed a real bug in `tools/fbx_ascii2model.py` (silently used the wrong
one of a mesh's 2 UV layers), all confirmed live via screenshot isolation —
see `samples/ReachGraphicsDemo/missing.md` for the complete write-up. This
was the last remaining item explicitly flagged as "not yet acted on" from the
prior session's own closing notes; per section 8's own restated position,
there is still no further approved/queued porting work beyond this as of this
session's own close (a future session should re-verify this rather than
trust it indefinitely, per section 5's standing staleness caveat).

**Previous session (2026-07-10, eighth follow-up):** Ported
**TiltPerspective (#107)**, the second and last of the two samples covered by
section 8 task 9's 2026-07-10 user go/no-go decision — completing it. Read
`TiltPerspective_4_0/TiltPerspective/{TiltPerspectiveSample.cs,
AccelerometerHelper.cs, BallSimulation.cs, DebugDraw.cs, RandomUtil.cs,
GeometricPrimitive.cs, SpherePrimitive.cs, VertexPositionNormal.cs}` in full,
plus the placeholder `missing.md` already on disk, per this task's own brief,
before writing any code.

**Key finding: unlike AccelerometerSample, this sample's own fallback
genuinely has no interactive branch of any kind to promote — a keyboard-tilt
scheme really did need to be invented from scratch this time.** Per this
task's own brief, `AccelerometerHelper.cs` was re-checked against the exact
same "look for a `DeviceType` branch nested inside `#if WINDOWS_PHONE`"
correction found while porting AccelerometerSample (DEFERRED.md item #15),
rather than assumed identical either way. The result differs from
AccelerometerSample: `AccelerometerHelper.cs` has **no** `#if WINDOWS_PHONE`
split at all (confirmed by `TiltPerspective.csproj`'s single `Windows Phone`
build configuration and a full-file read), and its own no-hardware fallback
(`Sensor == null`, the only reachable path on a desktop build) is a
non-interactive, purely time-driven sinusoidal wobble
(`FakeRollTheta += elapsed * FakeRollSpeed`) — no `Keyboard`/`GamePad`
reference appears anywhere in the file. So, per the 2026-07-10 user go/no-go,
this port genuinely invented a keyboard-tilt scheme (NOXNA): `Left`/`Right`
→ `X∓`/`X±`, `Up`/`Down` → `Y±`/`Y∓`, `Z` fixed at `-1`,
`Vector3::Normalize()`'d — deliberately reusing the same shape
AccelerometerSample's/Yacht's own *promoted* (not invented) fallbacks already
established, for consistency across the repo's tilt-emulation samples, even
though nothing in this sample's own original resembles it. Every downstream
consumer (`BallSimulation.hpp`'s gravity, `TiltPerspectiveGame::
ComputeEyeVector()`'s perspective shift) reads only
`RawAcceleration`/`SmoothAcceleration` exactly like the original, so none of
that code needed to change — only the input source did, exactly matching
this task's design goal.

Ported `GeometricPrimitive.cs`/`SpherePrimitive.cs`'s procedural sphere
generation directly, applying DEFERRED.md item #5's still-open remainder
(the same texture-less-procedural-vertex gap this task's own brief predicted,
already flagged by Primitives3D's own `missing.md`): `GeometricPrimitive.hpp`
assigns a dummy `(0,0)` UV to every vertex and uses the already-proven
`VertexPositionNormalTexture` + `BasicEffect` lit path instead of a new CNA
vertex type. Confirmed (rather than assumed) this does **not** hit DEFERRED.md
item #26's vtable/stride-collision bug: this sample's geometry is generated
procedurally at runtime and uploaded via `VertexBuffer::SetData()`'s typed
overload, which manually packs a plain, vtable-free 32-byte struct before
handing it to the backend (confirmed by direct source read of
`VertexBuffer.cpp`) — the bug is specific to `ModelTypeReader::Read()`'s own
stride-vs-`sizeof()` comparison, which this sample's code never goes through
at all (no `Content.Load<Model>` anywhere in this sample).

**Found and root-caused, via the real FNA reference source rather than
assumed, a confirmed characteristic that is NOT a CNA gap:** every ball
renders as a near-black sphere with only a small white specular highlight —
the per-ball `DiffuseColor` tint (Red/Green/Blue/White/Black) is invisible.
Root cause: `GeometricPrimitive.cs` itself never sets
`DirectionalLight0.DiffuseColor` anywhere (confirmed by a full-file grep);
reading FNA's own `DirectionalLight.cs` confirms a freshly-constructed
light's `DiffuseColor` defaults to `Vector3.Zero`, and `cna`'s own
`DirectionalLight.cpp` matches this exactly — so the diffuse term is
genuinely zero regardless of the ball's own tint, in **both the real XNA
original and this faithful port**. A related dead-code finding in the same
vein: `TiltPerspectiveSample.cs`'s own `worldGeometry.BasicEffect.
DirectionalLight0.Direction = lightDirection;` line (the "light always comes
from the ceiling" comment) is immediately overwritten by `DebugDraw.cs`'s own
`Draw()`, which unconditionally resets it to `-Vector3.UnitZ` — the box's
lighting never actually reflects tilt in the original either. Both are
reproduced faithfully in this port, not fixed (there's nothing to fix — it's
what the original does).

Substituted the original's `TouchPanel.GetState().Count > 0` "touch to
recalibrate level" check with "hold the left mouse button" (re-checked every
frame, matching the original's own continuous-while-held semantics) —
confirmed by direct source read of `cna/src/CNA/Internal/Input/
SdlInputBridge.cpp` that CNA's `TouchPanel` is fed only from real SDL finger
events, never mouse clicks, so a literal port would have been permanently
unreachable on this desktop (no touchscreen). `TiltPerspectiveSample.htm` has
no Sample Controls table at all (only a descriptive touch/tilt paragraph),
the same situation Graphics3D's/MarbleMaze's own `missing.md` already
documented — used a one-off scratch script (not committed to `tools/`)
importing `gen_help_png.py`'s own `build_text()`/`render_png()` helpers with
hand-written text describing this port's actual controls.

Builds 0 warnings (two from-scratch rebuilds — one before, one after removing
temporary debug code). Ran under `SDL_VIDEODRIVER=x11` for 8+ seconds across
three separate runs with no crash. Confirmed live via this repo's established
temporary debug-auto-trigger pattern (both patches reverted before commit,
re-verified with a clean from-scratch rebuild afterward — 0 warnings;
`xdotool getactivewindow` showed a different real user window, `0x400003`,
held focus throughout, so no synthetic keypresses were sent): forcing the
synthesized tilt vector's `X` component confirmed the ball cluster visibly
shifts and settles toward the tilted wall, and the box's perspective framing
visibly changes between the untilted and tilted runs — direct proof the
keyboard-tilt vector drives both `BallSimulation`'s gravity
(`IAccelerometerService::RawAcceleration`) and `TiltPerspectiveGame::
ComputeEyeVector()`'s perspective shift (`SmoothAcceleration`), exactly
mirroring how the original's own fake wobble would have driven both, just
from keyboard state instead of a sine wave. The F1 help overlay rendered and
then correctly disappeared after its 10-second timer. Also confirmed live
(passively, with no forced input) that `BallSimulation`'s gravity/collision
physics runs every frame under level/untilted input: two screenshots several
seconds apart show the 25 balls visibly changed position/clustering.

Converted `stone4.tga` (256×256 RGB Targa, the box's interior wall texture) to
PNG directly via ImageMagick (`convert stone4.tga stone4.png`), the same
established `.tga`→`.png` path already used by ClientServerSample's/
AimingSample's own assets. No new DEFERRED.md item filed — the one genuinely
new-to-this-sample gap (texture-less procedural vertices) is already fully
covered by item #5's existing remainder, and every other finding is either a
faithful, source-confirmed reproduction of the original's own code (including
two inert/dead-code characteristics) or an already-established repo-wide
pattern (fullscreen/screensaver omission, Escape-to-exit, touch-to-mouse
substitution, the F1-overlay-width-vs-portrait-window characteristic, and the
"no Sample Controls table" one-off help-script precedent). See
`samples/TiltPerspective/missing.md` for the complete account.

**With this sample done, both halves of the 2026-07-10 user go/no-go
(section 8 task 9) are complete.** There is no further approved/queued
porting work as of this session — every remaining placeholder sample needs
either a `cna`-side engine fix (DEFERRED.md items #11, #13, #14, #6/#18, or
the newer #22–#27) or a new user product-scope decision to become concrete
work, per section 8's closing note (updated this session).

Commit this session: `70c0c52`, pushed to `develop`.

**Previous session (2026-07-10, seventh follow-up):** Ported
**AccelerometerSample (#084)**, the first of the two samples covered by
section 8 task 9's 2026-07-10 user go/no-go decision. Read
`AccelerometerSample_4_0/Accelerometer/Accelerometer/{Accelerometer.cs,
Game.cs, Program.cs}` in full, plus the placeholder `missing.md` already on
disk, per this task's own brief, before writing any code.

**Key finding: the keyboard-tilt scheme is the original's own emulator
fallback, ported verbatim — not invented, despite how task 9 and DEFERRED.md
item #15 originally framed the decision.** A direct read of
`Accelerometer.cs:117-135` found that `GetState()` already has a keyboard
fallback: gated at runtime on `Microsoft.Devices.Environment.DeviceType !=
DeviceType.Device` (i.e. "the sample is running in the Visual Studio Windows
Phone 7 *emulator*, which has no physical accelerometer, rather than on a
real device"), it synthesizes a `Vector3` from the arrow keys —
`Left`/`Right` → `X--`/`X++`, `Up`/`Down` → `Y++`/`Y--`, `Z` fixed at `-1`,
then `Vector3.Normalize()`'d to mimic a real reading including gravity. This
branch lives *inside* the same `#if WINDOWS_PHONE` block as the real-hardware
branch (not in a separate non-`#if` branch the way Yacht/SnowShovel/Bounce's
own fallbacks do), which is why the earlier 2026-07-05 audit's "no `#if
WINDOWS_PHONE` split" check missed it — that check looked for a branch
*outside* the `#if`, not a second, `DeviceType`-gated branch nested *inside*
it. This port's own `Accelerometer.hpp` has **no `#ifdef`/real-sensor branch
at all** — the emulator branch above is its only implementation, always
taken, since this desktop build has no phone-vs-emulator distinction to make
in the first place. DEFERRED.md item #15 was updated with this correction
(the earlier audit's premise for AccelerometerSample specifically was wrong;
TiltPerspective has not yet been re-checked against the same correction).

Builds 0 warnings (two from-scratch rebuilds — one before, one after removing
temporary debug code). Ran under `SDL_VIDEODRIVER=x11` for 7+ seconds across
three separate runs with no crash. Confirmed live via screenshot that, with
real (unmodified) keyboard state, the asteroid sprite renders correctly and
stays motionless at its correctly-centered rest position. Live arrow-key
input hit this repo's known `xdotool` shared-desktop caveat
(`xdotool getactivewindow` showed a different real user window, `0x400003`,
held focus throughout; a `keydown --window`/screenshot/`keyup` round trip
produced no visible movement) — worked around with this repo's established
temporary debug-auto-trigger pattern (forcing `acceleration.X = 0.5f`, and
separately `helpTimer_ = 10.0f`, on the first frame; both reverted before
commit, re-verified with a clean from-scratch rebuild afterward): the
asteroid visibly slid right and clamped correctly at the screen edge (two
screenshots ~2 s apart, same clamped position in both, matching `Update()`'s
own edge-clamp logic), and the F1 help overlay rendered and then correctly
disappeared after its 10-second timer (two screenshots ~7 s apart).

`Accelerometer.htm`'s Sample Controls table has 3 columns (`Action | Windows
Phone | Windows Phone - Emulator`); generated `Content/help.png` with a
one-off variant script reading column 2 (the Emulator/keyboard column,
matching this port's real controls) — the same precedent as MicrophoneEcho's
own one-off column-2 script. Found (not a new gap — a second confirmed
sighting of an already-accepted characteristic) that this sample's
`help.png` (632×192) is wider than its 480px-wide portrait window, so the
centered overlay text is left/right-clipped, exactly like
`samples/Yacht/Content/help.png` (1472×312) in that sample's own
identically-480px-wide window using the same unmodified `Draw()`-time
centering code — not a regression, not fixed, same as Yacht's own
unaddressed instance. Converted `asteroid.png` (128×128 RGBA) and
`space.png` (480×800 RGB) directly — both already PNG in the original's
`AccelerometerContent/` directory, no reconversion needed. No new
DEFERRED.md item filed — every finding is either a faithful direct port of
the original's own code or an already-tracked, previously documented
pattern (fullscreen/30fps omission, Escape-to-exit, the
`LoadContent()`-time viewport-staleness gotcha already seen in
SoccerPitch/SnowShovel, and the F1-overlay-width-vs-portrait-window
characteristic already seen in Yacht). See
`samples/AccelerometerSample/missing.md` for the complete account.

TiltPerspective (#107), the second half of the same user go/no-go decision,
is queued as the very next follow-up — not started this session; a future
session porting it should re-check its own `AccelerometerHelper.cs` (or
equivalent) against this same "look for a nested DeviceType branch inside
`#if WINDOWS_PHONE`, not just a top-level `#if` split" correction before
assuming its own scheme truly must be invented from scratch.

Commit this session: `81a31c9`, pushed to `develop`.

**Previous session (2026-07-10, sixth follow-up):** With the originally-scoped
porting backlog exhausted (10 samples shipped: LensFlare, Graphics3D,
PickingSample, TrianglePicking, HeightmapCollision, InverseKinematics,
ChaseCamera, MarbleMaze, NetworkPrediction, PeerToPeer), fixed the one
remaining task fully within this session's own authority (no `cna` edit, no
user product decision needed): section 8 task 1, the `SafeArea`
`Viewport.x`/`.y` build breakage. Fixed `samples/SafeArea/src/
SafeAreaOverlay.hpp` (8 errors) as originally diagnosed, then re-ran the full
aggregate build to verify — which surfaced a **second, previously-unseen**
instance of the exact same pattern in `samples/RolePlayingGame/src/
{RolePlayingGame.hpp, TileEngine/TileEngine.hpp}` (10 more errors). This second
instance was invisible in every earlier partial-build attempt because `ninja`
stops at the first failing target (`SafeArea`), and `RolePlayingGame` sits
later in the build graph — a good reminder that "the aggregate build reached
target N before failing" only tells you about targets `1..N`, not `N+1..end`.
Fixed both files the same mechanical way (switch every `viewport.x`/`.y` to
`getXProperty()`/`getYProperty()`); left one unrelated, pre-existing warning in
`RolePlayingGame/src/GameScreens/../MenuScreens/LoadingScreen.hpp` alone (an
`intcs`-vs-`bytecs` `Color` constructor ambiguity, nothing to do with
`Viewport`). Confirmed via a from-scratch full rebuild (0 errors/0 warnings)
and a second immediate re-run (`ninja: no work to do`) that the full aggregate
build is genuinely, stably green — not a one-off.

**Same session, prior follow-up:** Ported **PeerToPeer (#103)**,
section 8 task 7's last remaining candidate — the third and final sample in this
repo's `NetworkSession`/`GamerServicesComponent` networking family. Read
`PeerToPeerSample_4_0/PeerToPeer/{PeerToPeerGame.cs, Tank.cs}` in full, plus
`samples/ClientServerSample/src/*.hpp`/`missing.md` and
`samples/NetworkPrediction/src/*.hpp`/`missing.md`, per this task's own brief,
before writing any code.

**Unlike the other two samples, this one has a genuinely different network
topology, not just a different application-level mechanic layered on the same
client/server split.** `PeerToPeerGame.cs`'s `UpdateNetworkSession()` has **no**
`if (networkSession.IsHost)` branch anywhere: every machine fully simulates its own
locally-controlled tanks every frame (`UpdateLocalGamer()` calls `Tank::Update()`
unconditionally, on every peer, not just the host) and broadcasts the resulting
state to everyone in the session via `gamer.SendData(packetWriter,
SendDataOptions.InOrder)` with **no explicit recipient** — a genuine full broadcast,
distinct from ClientServerSample's client→host-only send or NetworkPrediction's
still-client/server-shaped send. `gamer.IsHost` is read only for
`DrawNetworkSession()`'s cosmetic "(host)" label; it plays no role in who simulates
what. This was ported line-for-line with no adaptation needed for the topology
difference itself.

**The "zero networking workarounds needed" hypothesis held up a third time**, this
time specifically tested against a topology this session's brief flagged as worth
checking rather than assuming identical to the other two: the constructor adds a
real `GamerServicesComponent` exactly like the C# original
(`gamerServices_ = std::make_unique<GamerServicesComponent>(*this);
getComponentsProperty().Add(gamerServices_.get());`); `HookSessionEvents()` needs no
manual `Update()` call afterward; `networkSession_->getIsHostProperty()`/
`gamer->getIsHostProperty()` are used directly with no locally-tracked `bool`.
Confirmed live via this repo's established temporary debug-auto-trigger pattern
(`CreateSession()` + `helpTimer_ = 10.0f` forced on frame 30, reverted before a
subsequent clean from-scratch rebuild that re-confirmed 0 warnings — `xdotool
getactivewindow` showed a different real user window (`0x400003`, decimal
`4194307`) held focus throughout, this repo's known shared-desktop caveat, so no
synthetic keypresses were sent): `NetworkSession::Create()` completes with no hang,
`GamerJoinedEventHandler` fires synchronously (a fully textured tank labeled
`"Stub Gamer (host)"` renders on the very first frame after the trigger, with no
manual `Update()` call anywhere), the session correctly reports itself as host, and
the F1 help overlay renders correctly and disappears after its own 10-second timer
(confirmed via two screenshots roughly 6 seconds apart: the first shows both the
tank and the overlay, the second shows only the tank, overlay gone).

**Found no new CNA gap.** Checked explicitly, per this task's own brief, whether
this sample's C# original touches `NetworkSession.SessionProperties` (the source of
DEFERRED.md item #27, found while porting NetworkPrediction) before assuming either
way: it does not — `PeerToPeerGame.cs` has no host-authoritative game options at
all, so item #27 simply never comes up here, and no `PacketKind`-byte
disambiguation was needed in `Tank.hpp` (every packet on the wire is always the
same shape: position, tank rotation, turret rotation, nothing else). This is
explicitly the "or why not" half of this task's brief, not a hedge: the topology
difference this sample demonstrates turned out to be fully expressible through
already-working `LocalNetworkGamer::SendData()`'s existing broadcast overload
(already used by ClientServerSample's own `UpdateServer()` for its own
host→everyone broadcast) with no new API surface needed at all.

Confirmed `Tank.cs` is byte-identical to ClientServerSample's own `Tank.cs` (only
the C# namespace differs) via direct `diff`, and that `Tank.tga`/`Turret.tga`/
`Font.spritefont` are byte-identical (same `md5sum`) across all three sample
directories — reused ClientServerSample's/NetworkPrediction's already-converted
`Content/Tank.png`/`Turret.png`/`font.font.json`/`font.png` directly, no
reconversion needed. Generated `Content/help.png` via the standard
`tools/gen_help_png.py` path with no one-off variant needed (this sample's own
`.htm` has the standard 3-column controls table).

Builds 0 warnings (confirmed via two separate from-scratch rebuilds — one before,
one after removing the temporary debug-auto-trigger code). Ran under
`SDL_VIDEODRIVER=x11` for 23-30+ seconds total across three separate runs with no
crash. Real tank movement via keyboard input and a genuine two-machine broadcast
test (two independent processes actually exchanging tank state over the real
broadcast `SendData()`/`ReceiveData()` path) were **not** tested live this
session — the same `xdotool`-focus and two-process-`ENetDiscoveryService`-
discovery limitations already documented for ClientServerSample/NetworkPrediction.
Explicitly noted in `samples/PeerToPeer/missing.md` (not glossed over) that this is
the one aspect of this specific sample's own distinctive mechanic (full N-way
broadcast sync, no central authority) that isn't already covered by analogy to the
other two samples' single-gamer verification, since `Tank.hpp`'s movement physics
are proven byte-identical code but the *N-way broadcast* itself is this sample's
one genuinely new claim. See `samples/PeerToPeer/missing.md` for the complete
account.

This retires section 8 task 7 and, with it, the original "port more samples"
backlog this session set out to work through: PickingSample, TrianglePicking,
HeightmapCollision, InverseKinematics, ChaseCamera, MarbleMaze, NetworkPrediction,
and now PeerToPeer are all done, and no further placeholder sample was found to be
cleanly unblocked (zero CNA gap) the way this list's candidates were — see section
8's rewritten task list for what a future session should look at next.

Commit this session: `119694f`, pushed to `develop`.

**Previous session (2026-07-10, fourth follow-up):** Ported **NetworkPrediction
(#100)**, section 8 task 7's recommended candidate — the first attempt at either
NetworkPrediction or PeerToPeer since all three of ClientServerSample's (#091)
original networking workarounds were resolved upstream. Read
`NetworkPredictionSample_4_0/NetworkPrediction/{NetworkPredictionGame.cs, Tank.cs,
RollingAverage.cs}` in full, plus `samples/ClientServerSample/src/*.hpp` and its
`missing.md`, per this task's own brief, before writing any code.

**The "zero networking workarounds needed" hypothesis held up completely.** A real
`GamerServicesComponent` is constructed in the constructor exactly like the C#
original (`Components.Add(new GamerServicesComponent(this));` ported line-for-line,
no "don't add one" workaround); every host/client branch uses
`networkSession_->getIsHostProperty()`/`gamer->getIsHostProperty()` directly with no
locally-tracked `bool`; and `HookSessionEvents()` needs no extra manual `Update()`
call afterward. Confirmed live via this repo's established temporary
debug-auto-trigger pattern (`CreateSession()` + `helpTimer_` forced on the very first
frame, reverted before a subsequent clean from-scratch rebuild that re-confirmed 0
warnings): `NetworkSession::Create()` completes with no hang, `GamerJoinedEventHandler`
fires synchronously (a fully textured tank labeled `"Stub Gamer"` renders on that same
first frame, with no manual `Update()` call anywhere), the session correctly reports
itself as host (`DrawOptions()`'s "(X to toggle)"-style prompts, gated on
`IsHost`, render correctly), and the F1 help overlay renders on top. This is the first
confirmation that ClientServerSample's three fixes (DEFERRED.md items #19/#20/#21)
generalize to a second, independently-written sample's own `Update()`-loop shape, not
just ClientServerSample's own — exactly the test this task was set up to run.

**Found one new, genuine CNA gap** while porting `UpdateOptions()` (this sample's own
distinctive content — client-side prediction/smoothing to hide network latency and a
throttled send rate): `NetworkSession::SessionProperties` has no mutable accessor
(`getSessionPropertiesProperty()` returns `const NetworkSessionProperties&` with no
non-const overload or setter anywhere in `NetworkSession.hpp`/`.cpp`, confirmed by
direct source read) and is never replicated over the wire (`sessionProperties_` is
set once at construction and never read or written again anywhere in
`NetworkSession.cpp`, confirmed by grepping the whole file). The C# original relies
entirely on this — the host writes 4 settings into
`networkSession.SessionProperties[i]` once per frame, and every other machine reads
the same indices back with no explicit packet-send call anywhere in the sample's own
code; real XNA's networking layer silently replicates the list to every peer. Worked
around (NOXNA, no C# equivalent, confined to the outer game class — `Tank.hpp` matches
`Tank.cs` line-for-line with no change) with an explicit host-broadcast "options
packet," distinguished from ordinary tank-state packets by a new leading `PacketKind`
byte on the same `LocalNetworkGamer::SendData`/`ReceiveData` channel already used for
tank state. Filed as new **DEFERRED.md item #27** — effort S/M, since a mutable
accessor alone (no replication) is a few lines, but real wire replication is
comparable in shape to how `GamerJoined` already replicates gamer-roster changes.

Reused ClientServerSample's already-converted `Content/Tank.png`/`Turret.png`/
`font.font.json`/`font.png` directly (confirmed byte-identical source `Tank.tga`/
`Turret.tga`/`Font.spritefont` files via `md5sum`/`diff` — no reconversion needed, per
this repo's own "don't regenerate existing assets unless there's a confirmed bug"
guidance). Generated `Content/help.png` via the standard `tools/gen_help_png.py` path
with no one-off variant needed (this sample's own `.htm` has the standard 3-column
controls table). One faithful, deliberate divergence from ClientServerSample's own
port worth calling out: this sample's `IsPressed()` is **rising-edge** triggered
(`currentState.IsKeyDown && previousState.IsKeyUp`), matching its own C# original
exactly, whereas ClientServerSample's `IsPressed()` is level-triggered, matching *its*
own, genuinely different, C# original — confirmed by direct comparison of both C#
sources rather than assumed, since porting the wrong one would have made the A/B/X/Y
option-cycling keys repeat every single frame instead of toggling once per press.

Builds 0 warnings (confirmed via two separate from-scratch object-file rebuilds — one
before, one after removing the temporary debug-auto-trigger code). Ran under
`SDL_VIDEODRIVER=x11` for 9+ seconds total across two runs with no crash.
`xdotool getactivewindow` showed a different real user window (`0x400003`) held focus
throughout, so no synthetic keypresses were sent — same established fallback used
throughout this session. Two-machine `JoinSession()`/the actual host → client wire
replication path for the new options packet were **not** tested live this session —
the same "no genuine 2-process LAN test" limitation ClientServerSample's own
`missing.md` already documents (a pre-existing `ENetDiscoveryService` two-process
discovery limitation on this container, not a regression). See
`samples/NetworkPrediction/missing.md` for the complete account.

Commit this session: `9619ff8`, pushed to `develop`.

**Newest session (2026-07-10, third follow-up):** Ported **MarbleMaze (#061)**,
section 8 task 6's last remaining candidate from the original 3-sample list
(ChaseCamera and InverseKinematics were ported earlier this session — see
below). Unlike the single-mechanic "Phase 3 — 3D Graphics" samples ported so
far, this is a full "Phase 4 — Full Games" title: a complete
`ScreenManager`-based menu/gameplay flow (`GameStateManagement`'s vanilla
plain-text `MenuEntry`/dynamic `UpdateMenuEntryLocations()` variant, not
HoneycombRush's customized button-texture copy of the same library already in
this repo) wrapping a marble-rolling-through-a-tilting-maze physics simulation.

Read `Source/EX2_Polishing/End/MarbleMazeGame/MarbleMazeGame/`'s full C# source
(4687 lines across `MarbleMazeGame.cs`, `Objects/{Camera,DrawableComponent3D,
Marble,Maze}.cs`, `Misc/{Accelerometer,AudioManager,IntersectDetails,
TriangleSphereCollisionDetection}.cs`, the whole `ScreenManager/` library, and
all 7 `Screens/*.cs` files) plus `MarbleMazePipeline/MarbleMazeProcessor.cs`
(the custom build-time `ContentProcessor` — see below) before writing any code,
per this task's own brief.

**Per this task's own brief, given DEFERRED.md item #26 was already
independently confirmed 4 times across InverseKinematics' and ChaseCamera's own
assets, this session did not re-test a plain `Content.Load<Model>` build before
committing to the `RawMesh.hpp` (NOXNA) bypass** — applied proactively from the
start, generalized beyond ChaseCamera's/InverseKinematics' one-mesh-per-model
shape to support `maze1.FBX`'s 6 separately-textured sub-meshes (`walls`, 3
differently-textured `Floor` sub-parts, `topWall`, `floorSides` — confirmed via
`assimp info` that each maps to exactly one material, no further
multi-material reconciliation needed) sharing one `BasicEffect` (texture
swapped per part before each draw). This decision was independently
overdetermined by DEFERRED.md item #18 too (no custom-`ContentProcessor`
extensibility, needed for `MarbleMazeProcessor`'s own per-mesh triangle-list
extraction) and item #6's "no per-mesh texture in `.model.json`" addendum
(the maze structurally needs 6 different textures, which `Content.Load<Model>`
could never have produced correctly regardless of item #26's status) — even if
item #26 somehow didn't apply here, the other two gaps would still have forced
the same bypass. `MarbleMazeProcessor`'s own build-time triangle-list
extraction (feeding `Maze.cs`'s ground/wall/floor-side collision test) was
reconstructed at runtime instead, from the *same* vertex/index data `RawMesh`
already loads for rendering (`RawMesh::ExpandTrianglePositions()` — no new
picking/collision sidecar file format needed, unlike TrianglePicking's
`--picking` tool flag). `maze1.FBX`'s `Start`/`Finish`/`spawnPt1..4` bone-only
marker nodes (no geometry, pure transform nodes) were extracted once, offline,
via a one-off `pyassimp` script walking the FBX's node hierarchy, then
hardcoded as `Vector3` constants — cross-checked for self-consistency against
the exported mesh geometry's own bounding box.

**Found a second, independent confirmed instance of the `assimp export`
triangle-winding inversion first seen on ChaseCamera's `Ground.x`** (see that
session's own entry below): with CNA's default
`RasterizerState::CullCounterClockwise`, 5 of the maze's 6 parts (everything
except `walls`) rendered as fully invisible; forcing `RasterizerState::CullNone`
around the whole maze draw made every part appear correctly, fully textured
and shaded, with no other change — isolated live via screenshot comparison.
Kept as a permanent fix (not a workaround needing later cleanup), the same
class of per-asset accommodation this repo's own `HeightmapCollision`/
`GeneratedGeometry` terrain and `ChaseCamera`'s `Ground.x` already needed —
not filed as a new DEFERRED.md item, just a second confirmed sighting of the
same tool quirk (now seen on both `.x` and `.FBX` sources).

No `.htm` documentation exists for this sample (a 101-page Word tutorial
instead, `3D Game Development With XNA.doc`) — used a one-off script
(`gen_marblemaze_help.py`, following the established Graphics3D/MicrophoneEcho
precedent for samples whose `.htm` can't be scraped normally) that imports
`tools/gen_help_png.py`'s `render_png()` directly with hand-written text
describing this port's actual control scheme: direct arrow-key maze tilt,
algebraically derived from `Accelerometer.cs`'s own emulator/keyboard fallback
combined with `GameplayScreen.cs`'s `DeviceType.Emulator` branch (the
established DEFERRED.md item #15 "un-`#if` the existing non-phone fallback"
pattern, not an invented control scheme) — confirmed by source read that the
real accelerometer path and the double-tap calibration branch are both
`DeviceType.Device`-gated dead code on any non-phone build, so neither
`Accelerometer.cs` nor `CalibrationScreen.cs` was ported at all.

Several smaller, explicitly-documented simplifications (all in missing.md):
`LoadingAndInstructionScreen`'s background-thread asset load became a
synchronous call (CNA's EasyGL graphics-resource creation isn't confirmed safe
off the GL thread); `HighScoreScreen`'s `IsolatedStorageFile` persistence
became plain `std::fstream` against a local file; `FinishCurrentGame()`'s
`Guide.BeginShowKeyboardInput` on-screen-keyboard name entry became a fixed
`"Player"` string; C#'s `LinkedList<Vector3>` checkpoint traversal became a
`std::vector<Vector3>` + index; C#'s `public new bool IsActive` field-hiding
`GameScreen.IsActive` became the same mechanism in C++ (a same-named field
correctly hides the base method for unqualified access, matching C#'s `new`
without needing an explicit keyword); the original's interleaved
`SpriteBatch.Begin()`/3D-draws/`SpriteBatch.End()` (needing a
`DepthStencilState` re-enable dance) was reordered to 3D-first-then-2D,
matching this repo's other mixed-3D/2D samples and sidestepping that dance
entirely. A new `Screens/ScreensGlue.hpp` (NOXNA, no C# equivalent) resolves
the 6 screen classes' mutual circular references (unremarkable in C#, needing
forward-declaration + deferred-method-body treatment in C++) — the same
technique this repo's own `ScreenManager.hpp`/`MenuScreen.hpp` already use in
miniature for `GameScreen`/`MenuEntry`'s own two-way reference.

Also found and corrected a stale DEFERRED.md entry while writing `Camera.hpp`:
item #9 (`Viewport.AspectRatio`) claimed the property didn't exist, but a live
grep of `cna`'s current source found `getAspectRatioProperty()` already
implemented and used directly — marked resolved (per this repo's own "risky
assumption" caveat, section 5: `cna` is under active concurrent development and
DEFERRED.md blockers can go stale silently).

Builds 0 warnings (verified via a from-scratch object-file rebuild, grepped for
"warning"/"error", none found). Ran under `SDL_VIDEODRIVER=x11` for 8+ seconds
with no crash across several separate runs. Confirmed live via this repo's
established temporary debug-auto-trigger pattern (all reverted before commit,
since a different real user window held focus throughout this session,
confirmed repeatedly via `xdotool getactivewindow`): the main menu renders
correctly; the F1 help overlay renders correctly; the full 3D gameplay scene
(all 6 maze parts plus the marble) renders correctly, fully textured and
shaded, with the marble resting stably on the floor for 11+ seconds of passive
observation with no falling-through (direct proof the runtime-reconstructed
collision triangle lists work); forcing a constant tilt input showed the maze
visibly rotating, correctly clamped at ±30°, with the marble staying correctly
positioned on the tilted floor throughout — confirming the physics-in-
unrotated-frame architecture (only the *display* transform rotates; the
marble's actual position/velocity math is entirely independent of the tilt)
works end-to-end. The menu→loading→gameplay screen *transitions* themselves
were not clicked through with real synthetic input this session (the same
`xdotool` focus limitation) — see `samples/MarbleMaze/missing.md`'s
Verification section for the precise scope of what was and wasn't exercised
live.

Commit this session: `09038ca`, pushed to `develop`.

**Newest session (2026-07-10, second follow-up):** Ported **ChaseCamera (#058)**,
section 8 task 6's next candidate after InverseKinematics — a spring-physics chase
camera (`ChaseCamera.cs`, pure `Vector3`/`Matrix` math with no CNA API surface
beyond what every other 3D sample already uses) following a ship (`Ship.cs`,
simple flight physics driven by keyboard/gamepad/mouse) flying over a large
checkered ground plane. `MarbleMaze` is now the only sample left from the
original 3-candidate list (see section 8).

This session's task brief specifically flagged DEFERRED.md item #26 (the
`ModelTypeReader` vertex-corruption bug found while porting InverseKinematics)
and asked for an empirical test, not an assumption, before treating this
sample's own models as affected. That test was done first: a temporary build
using plain `Content.Load<Model>("Ship")`/`Content.Load<Model>("Ground")` (`Ship.fbx`
converted directly via `tools/fbx_ascii2model.py`; `Ground.x` converted via
`assimp export Ground.x Ground.obj` + `tools/obj2model.py`, exactly the pipeline
this task's brief suggested and already proven for InverseKinematics' own
`cylinder.x`) built and loaded without error but rendered **nothing at all** — a
solid CornflowerBlue screen with only 2D HUD text visible, confirmed via two
screenshots taken 3 seconds apart, no crash. This sample's own initial camera
distance is `sqrt(2000^2 + 3500^2) ≈ 4031` units
(`DesiredPositionOffset = (0, 2000, 3500)`) — even farther than Graphics3D's
~3523-unit spaceship, which showed the identical "fully invisible" symptom.

Worked around with a new `samples/ChaseCamera/src/RawModel.hpp` (NOXNA) — the
same bypass shape as InverseKinematics' `CylinderModel.hpp` and
HeightmapCollision's/GeneratedGeometry's `Terrain.hpp`: reads the
already-converted `_verts.bin`/`_idx.bin` sidecars directly and constructs real,
normally-initialized C++ `VertexPositionNormalTexture` objects (not a
`reinterpret_cast` on a raw byte blob), uploaded through the same typed
`VertexBuffer::SetData` overload `ModelTypeReader` was trying (and failing) to
reach — generalized this time to also bind a real `Texture2D` directly to the
`BasicEffect` (the same side benefit `Terrain.hpp` already established, since
`.model.json` has no per-mesh texture field). Confirmed live via screenshot:
both the ship (`Ship_p1_wedge_geo1`: 32458 vertices, 16118 triangles — two
orders of magnitude larger than InverseKinematics' 418-vertex cylinder) and the
ground (`Ground`: only 6 vertices, 2 triangles — the smallest mesh yet tested
through this bug) now render correctly, fully textured (`ShipDiffuse.png`/
`Checker.png`) and shaded. **This is a third and fourth independent
confirmation of DEFERRED.md item #26's hypothesis, on two more assets converted
through two different pipelines (FBX and `assimp` `.x`→`.obj`), at both a much
larger and a much smaller vertex count than the first confirmation** — stated
explicitly, as this task's brief requested, since it further reinforces that
this bug is a structural reader defect uncorrelated with mesh size, complexity,
or source format, not something specific to InverseKinematics' own cylinder
asset.

A second, separate (non-#26) issue surfaced after switching to `RawModel.hpp`:
the ship rendered correctly immediately, but the ground still didn't appear at
all. Isolated live by temporarily forcing `RasterizerState::CullNone` around
only the ground's draw call — confirmed this alone made the full checkered
ground plane appear, with no other change. Root cause: `assimp export
Ground.x Ground.obj` re-emits the `.x` file's two triangles wound the opposite
way from CNA's default `RasterizerState::CullCounterClockwise`, so the ground
was being fully back-face-culled even with correct, uncorrupted vertex data.
This is the same class of per-asset winding accommodation
`HeightmapCollision`'s/`GeneratedGeometry`'s own `Terrain.hpp` already needed
for their runtime-built terrain meshes — not a new bug, just the first time
it's shown up on an `assimp`-round-tripped `.x` asset specifically rather than
a hand-built runtime mesh. `Ship.fbx` (converted directly via
`tools/fbx_ascii2model.py`, no `assimp` round-trip) needed no such adjustment.
Kept as a permanent, documented `RasterizerState::CullNone`/
`CullCounterClockwise` toggle around only the ground's draw call in the final
port, not a global culling change.

Builds 0 warnings (verified via a from-scratch rebuild — object file removed,
rebuilt, output grepped for "warning"/"error", none found). Ran under
`SDL_VIDEODRIVER=x11` for 8+ seconds across three separate runs with no crash.
F1 help overlay and ship-movement/camera-spring-physics were both verified live
via this repo's established temporary debug-auto-trigger pattern (`helpTimer_`
forced to 10.0f and `Ship::Update()`'s `thrustAmount` forced to 1.0f, both
reverted before commit) — the help panel renders the correct 4-row control
table extracted from `ChaseCamera.htm`, and two screenshots 4 seconds apart show
the ship visibly moving forward with the chase camera visibly lagging/springing
behind it, confirming `ChaseCamera::Update()`'s ported spring-damper math
(`force = -Stiffness*stretch - Damping*velocity`) computes live, correct
results. `xdotool getactivewindow` showed a different real user window had
focus throughout this session (this repo's known shared-desktop caveat — section
5), so no synthetic keypresses were sent to the sample's own window. See
`samples/ChaseCamera/missing.md` for the complete account.

Commit this session: `a8b1dc3`, pushed to `develop`.

**Newest session (2026-07-10, follow-up):** Ported **InverseKinematics (#057)**,
section 8 task 6's last remaining candidate from the original 3-sample list
(ChaseCamera and MarbleMaze are now the only ones left — see section 8). This
session's real engineering content ended up being a significant, previously
undiagnosed CNA bug discovery, not the IK/CCD math itself (which is pure
application-level code, unrelated to any `Model`/bone-hierarchy machinery — the
task brief's concern about DEFERRED.md item #6's multi-bone gap turned out to be
a non-issue: the cylinder chain is 20 draws of one single-bone rigid model with
per-draw world matrices computed entirely in game code, and the sample's second
IK demo, driving an Xbox LIVE `AvatarRenderer`, is a real, working CNA type that
just faithfully never renders off a signed-in Xbox LIVE session, matching real
XNA/FNA exactly, not a CNA gap).

The actual discovery: a straightforward first port (`cylinder.x` →
`cylinder.model.json` via `assimp export` + `tools/obj2model.py`, the exact
pipeline already proven for CameraShake/PerformanceMeasuring/Graphics3D) built
cleanly and loaded without error (confirmed via debug instrumentation: 1 mesh,
418 vertices, 190 primitives, a correctly-linked `BasicEffect`) but **never
rendered**, at any camera distance, object scale, cull mode, or lighting
setting — even reduced to a single full-scale, identity-world, unlit, untextured
triangle drawn through the exact same `Model`/`ModelMesh::Draw()` path used by
every other Model-based sample in this repo. Systematic isolation (documented in
full in `samples/InverseKinematics/missing.md`) ruled out this sample's own code,
camera setup, and asset data one at a time: a hand-built triangle at the same
scale failed identically; **PickingSample's own already-shipped, already-working
`Cylinder.model.json`** failed identically when loaded fresh into this sample's
code; but **HeightmapCollision's `sphere.model.json` (3252 vertices) rendered
correctly** through the exact same code path at every scale tested (including
scaled down to match the failing tests' size) — the key data point that
something in the *reader itself*, correlated with mesh size/complexity, not any
one sample's code or asset, was responsible.

Root-caused by direct source read plus a standalone `sizeof()` probe compiled
against `cna`'s own headers: every CNA vertex struct
(`VertexPositionColor`/`VertexPositionTexture`/`VertexPositionColorTexture`/
`VertexPositionNormalTexture`) now publicly inherits from the polymorphic
`IVertexType` (a virtual destructor plus a pure virtual method), adding an
8-byte vtable pointer XNA's own "clean" layouts never had — confirmed live:
`sizeof(VertexPositionColor)=40` (not 16), `sizeof(VertexPositionTexture)=32`
(not 20), `sizeof(VertexPositionColorTexture)=56` (not 24),
`sizeof(VertexPositionNormalTexture)=40` (not 32).
`ModelTypeReader::Read()`'s `if (stride == sizeof(VertexPositionNormalTexture))
... else if (stride == sizeof(VertexPositionTexture)) ...`-style dispatch
compares the `.model.json`-declared *clean* stride (always 32 for every
`Content.Load<Model>`-based sample in this repo, since `obj2model.py`/
`fbx_ascii2model.py` only ever emit `VertexPositionNormalTexture` data) against
these now-inflated sizes — and `32` *accidentally* equals
`sizeof(VertexPositionTexture)`'s own inflated size (a coincidence of the
specific field-count/padding arithmetic), so the reader always silently
dispatches to the **wrong** overload, `reinterpret_cast`ing the raw (vtable-free)
file bytes as if they were vtable-shifted `VertexPositionTexture` objects and
reading Position/TextureCoordinate from the wrong byte offsets — corrupting the
uploaded vertex data for **every stride-32 `.model.json` in this entire repo**,
not just this sample's.

This is very likely the true root cause of the long-tracked
"near-plane-clipping-family" bug (section 4) — a corrupted, essentially
arbitrary per-vertex byte-offset reinterpretation would produce exactly the two
symptoms already observed (a degenerate thin line, or full invisibility)
without requiring any actual clip-space/projection defect, and no prior session
ever directly inspected the EasyGL clipping code itself before attributing the
symptom to it. **Not re-confirmed with 100% certainty for the other affected
samples this session** (would need re-testing `tank.model.json`/
`terrain.model.json` through the same bypass, out of this task's scope) — filed
as new DEFERRED.md item #26 with a clear recommendation for whoever picks up
section 8 task 2 next: try the bypass on CameraShake (the smallest affected
sample) before assuming the bug is really in clip-space math.

Worked around in this port with a new `samples/InverseKinematics/src/
CylinderModel.hpp` (NOXNA): reads the already-converted `cylinder_verts.bin`/
`cylinder_idx.bin` directly and constructs real, normally-initialized C++
`VertexPositionNormalTexture` objects field-by-field (not via
`reinterpret_cast`), then uploads them through the same typed
`VertexBuffer::SetData(const VertexPositionNormalTexture*, count)` overload
`ModelTypeReader` was trying (and failing) to reach — the same shape of
workaround already established by `HeightmapCollision`'s/`GeneratedGeometry`'s
own `Terrain.hpp` for a different `.model.json` gap, and further proof (since
their terrain already goes through this exact typed overload and renders
correctly) that the typed `SetData` path itself was never the problem.

Confirmed live via screenshot: the 20-link cylinder chain renders correctly,
**lit** (a visible shading gradient along the chain — this sample does not hit
the "flat white, no shading" finding from PickingSample/TrianglePicking/
HeightmapCollision, since `CylinderModel.hpp` bypasses the "no per-mesh texture
in `.model.json`" gap entirely by not using `.model.json` at all), correctly
curling from the origin toward the cat's live position every frame — direct
confirmation the CCD IK algorithm itself computes correct, live bone rotations.
The billboarded cat and HUD text render correctly. Builds 0 warnings; ran 9+
seconds with no crash across two separate runs. F1 help overlay verified via
this repo's established temporary-debug-auto-trigger pattern (`helpTimer_`
forced on, screenshotted, reverted before commit). This sample's own camera
sits only ~5 units from the action (`cameraRadius=5`, `near=1`, `far=1000`) —
much closer even than HeightmapCollision's ~155 units — and shows no near-plane
artifact at all, consistent with (though not proof of) the item #26
re-attribution above. See `samples/InverseKinematics/missing.md` for the
complete account.

Commit this session: `67219d6`, pushed to `develop`.

**Newest session (2026-07-10):** Ported **HeightmapCollision (#049)**, section 8
task 6's recommended next candidate — a rolling ball on a heightmap-generated
terrain, with both the ball and the follow-camera queried against a
`HeightMapInfo` height-lookup class (bilinear interpolation over the same height
grid the terrain mesh itself uses).

The real engineering question was the terrain. `HeightmapCollision.cs`'s own
runtime code is nothing more than `Content.Load<Model>("terrain")` + reading
`terrain.Tag as HeightMapInfo` — all the actual work happens at content-**build**
time, in `HeightmapCollisionPipeline.TerrainProcessor` (a custom
`ContentProcessor<Texture2DContent, ModelContent>` that reads `terrain.bmp` as a
heightfield, builds a grid mesh via `MeshBuilder`, chains to the stock
`ModelProcessor`, and attaches a `HeightMapInfoContent` — the same height data,
computed once — to the built model's `Tag`). CNA has neither a `Model.Tag`
equivalent nor custom-`ContentProcessor` extensibility (pre-existing DEFERRED.md
item #18), so this isn't directly portable. Rather than inventing a new
workaround, this session first read this repo's own `GeneratedGeometry` sample
(per the task brief's suggestion) and confirmed via its `missing.md` that its own
C# original ships a **structurally identical** `TerrainProcessor` — meaning
`GeneratedGeometry`'s existing runtime-mesh-generation approach
(`samples/GeneratedGeometry/src/Terrain.hpp`) is precedent, not just a candidate
idea. This session's `Terrain.hpp` (NOXNA) follows the same shape: builds the
terrain's `VertexBuffer`/`IndexBuffer`/`BasicEffect` **and** its `HeightMapInfo`
together at runtime in `LoadContent()`, replicating `TerrainProcessor.Process()`'s
exact algorithm (`terrainScale=30`, `terrainBumpiness=640`, `texCoordScale=0.1`)
and computing per-vertex normals from the heightfield's gradient in place of
`ModelProcessor`'s automatic normal generation.

A second, independent reason favored this approach over `Content.Load<Model>`,
found by direct source read rather than assumed: `terrain.bmp` is 257×257 =
66049 vertices, exceeding the 65535 limit of a 16-bit index buffer. Real XNA's
`ModelProcessor` automatically selects 32-bit indices for a mesh this large;
CNA's `.model.json` `ModelTypeReader` (`ContentManager.cpp`) hardcodes 16-bit
indices unconditionally, with no way to request 32-bit for any mesh regardless of
size — a genuine, narrower nuance of DEFERRED.md item #6 not previously
documented (added as a new addendum this session). Confirmed this is not a
general CNA limitation, though: `IndexBuffer`/`IIndexBufferBackend` (both EasyGL
and Vulkan) already fully implement `IndexElementSize::ThirtyTwoBits`
end-to-end — the gap is specifically `ModelTypeReader`'s hardcoded assumption.
Building the terrain's `IndexBuffer` directly with the real
`IndexBuffer(device, IndexElementSize::ThirtyTwoBits, ...)` constructor
sidestepped this cleanly, verified working live (the full 257×257 grid renders
with no visible artifacts).

A pleasant, unplanned side effect: because this terrain never goes through
`.model.json` (whose mesh schema has no per-mesh texture field — DEFERRED.md item
#6's PickingSample/TrianglePicking addendum, the "flat white" finding repeated
across multiple prior samples), its hand-built `BasicEffect` gets a real,
already-loaded `Texture2D` (`rocks.bmp`) bound directly via
`setTextureProperty()`. Confirmed live via screenshot: the terrain renders fully
textured, with a clearly visible shading gradient across its hills — the first
sample in this repo's Model-based lighting series to *not* hit the flat-white
gap. The **sphere** (`Content.Load<Model>("sphere")`, a plain ASCII FBX with the
same `-90,0,0` `PreRotation` node-transform shape LensFlare's `terrain.fbx` had —
handled automatically by the already-fixed `tools/fbx_ascii2model.py`) does still
render as a small flat white shape, exactly as expected and not re-diagnosed.

Also confirmed, by direct comparison against `NEXT.md`'s own tracked bug list:
this sample's camera sits only ~155 units from the ball
(`CameraPositionOffset = (0,40,150)`), noticeably closer than the ~1000+ unit
distances that trigger the tracked near-plane-clipping-family bug elsewhere in
this repo — screenshots confirm neither the ball nor the terrain shows that
artifact here. Not claiming the bug is fixed; this sample's own camera geometry
simply doesn't reach the distance where it's been observed to trigger. This
sample also adds no `GameComponent`s at all (unlike PickingSample/Graphics3D/
TrianglePicking's `Cursor`/`Checkbox`es), so DEFERRED.md item #23 is simply not
exercised here — noted in `missing.md` rather than silently skipped.

Builds 0 warnings (verified via the real build after fixing one small C++
language-constraint issue: `Vector3` has no `operator*=`, only
`operator*(Vector3, float)`, unlike C#'s `Vector3 *= float` — a one-line fix, not
a CNA gap). Ran 7+ seconds with no crash across two separate runs. F1 help
overlay uses the standard `tools/gen_help_png.py` path with no one-off variant
needed (`HeightmapCollision.htm`'s table has the standard 3 columns). See
`samples/HeightmapCollision/missing.md` for the complete account.

Commit this session: `e15c287`, pushed to `develop`.

**Previous session (2026-07-09, third same-day follow-up):** Ported
**TrianglePicking (#048)**, section 8 task 6's recommended next candidate — a
close sibling of PickingSample (#047, ported immediately before it this same
session): same original author, same table-of-4-objects scene, and (confirmed
via `cmp`) byte-identical source FBX assets (`Sphere.fbx`, `Cats.fbx`≡`Cats.FBX`,
`Cylinder.fbx`, `P2Wedge.fbx`≡`P2Wedge.FBX`, `table.FBX`). The key difference,
confirmed by reading `TrianglePickingSample_4_0/TrianglePickingSample/Game.cs`
directly rather than assuming: this sample replaces PickingSample's simpler
per-object `BoundingSphere`-only test with a real **per-triangle** ray
intersection (a hand-ported Moller-Trumbore ray-triangle test), a fast
bounding-sphere pre-test only used to decide whether the expensive per-triangle
test is worth running at all.

The real engineering question this session was where the per-triangle vertex
data comes from. XNA's original gets it from a custom content-pipeline
processor (`TrianglePickingProcessor`, a `ModelProcessor` subclass) that walks
the model's node tree at content-**build** time and attaches a flat
`Vector3[]` (plus a precomputed `BoundingSphere`) to `Model.Tag`. CNA has
neither a `Model.Tag` equivalent nor custom-`ContentProcessor` extensibility
(pre-existing DEFERRED.md item #18). The task brief suggested a fallback —
reading the data back from the model's already-loaded `VertexBuffer`/
`IndexBuffer` at runtime instead — but a full read of both classes' headers
(`cna/include/Microsoft/Xna/Framework/Graphics/{VertexBuffer,IndexBuffer}.hpp`)
confirmed **neither has a `GetData()` method of any kind**: every method on
both is a `SetData`/`SetDataRaw`/`SetDataWithOptions` upload path, never a
readback path. This is a real, narrower CNA gap than item #18 (a sample could
in principle re-derive picking data from an already-loaded `Model` with no
custom content pipeline involved at all, and still couldn't) — filed as **new
DEFERRED.md item #25**. Worked around at the tooling level, consistent with
this repo's own "convert once, offline" asset philosophy (item #18's framing):
extended `tools/fbx_ascii2model.py` (which already parses every mesh's raw
triangle/vertex data to build `.model.json`'s own buffers) with an optional
`--picking <output.bin>` flag that, from that same parsed data, additionally
emits a flat binary sidecar of triangle-expanded vertex positions per model —
read back directly by the C++ port (`samples/TrianglePicking/src/
TrianglePickingData.hpp`) at `LoadContent()` time, with
`BoundingSphere::CreateFromPoints()` (already implemented in CNA) computed over
it once and cached, reproducing exactly what `TrianglePickingProcessor` stores
in `Model.Tag` in the original.

A second, useful clarification surfaced while investigating whether this
sample would hit DEFERRED.md item #23 (`Game::DoInitialize()`'s
`ComponentAdded`-after-`Initialize()` timing gap) the same way PickingSample
and Graphics3D both did. Reading `Game.cs` directly showed this sample's
`Cursor` component is added from the **constructor**, not `Initialize()` —
and reading `cna`'s `Game.cpp` directly showed why that matters: the base
`Game::Initialize()` (not `DoInitialize()`) separately, unconditionally loops
over every component already present in `Components_` and calls each one's own
`Initialize()` directly, independent of `ComponentAdded` subscription timing
entirely. A component added in the constructor — long before
`DoInitialize()`/`Initialize()` ever run — is already present in `Components_`
by the time this loop executes, so it initializes correctly with **no
workaround needed**. Confirmed live (built and ran without the
`AddComponent()` pattern PickingSample/Graphics3D both required; `Cursor`'s
texture/`SpriteBatch` all load correctly, cursor renders in every screenshot).
Added this clarification directly to DEFERRED.md item #23's own text, since it
narrows the gap's actual trigger condition (order relative to the override's
own `base.Initialize()` call, not simply "constructor vs. `Initialize()`").

Also unexpectedly got a **real, non-debug-forced** live confirmation that the
whole picking pipeline works correctly end-to-end: an `xdotool mousemove
--window <id> <x> <y>` call issued earlier in the session (while checking this
repo's own known `xdotool` keyboard-focus reliability caveat) evidently moved
the real OS pointer over the window even without confirmed focus — a later,
fully clean screenshot (no debug code active) shows correct
`"Inside bounding sphere: P2Wedge, Cylinder"` text and a correct `"P2Wedge"`
per-triangle-picked name label under the cursor, reproduced identically across
two screenshots taken 4 seconds apart. Separately, a temporary debug
auto-trigger (removed before commit) confirmed the picked-triangle magenta
wireframe outline (`DrawPickedTriangle()`) also renders correctly. Confirmed
the already-tracked near-plane-clipping thin-line artifact on the `Sphere`
model (visible in every screenshot, since this sample's own C# original's
camera doesn't auto-rotate without input — unlike PickingSample's port, which
added its own continuous auto-rotation not present in either sample's actual
source) and the already-tracked flat-white-saturation finding (DEFERRED.md
item #6 addendum) — both immediately recognized as known, per this session's
own briefing, not re-diagnosed from scratch.

Builds 0 warnings (verified via a from-scratch rebuild); ran 7+ seconds with no
crash across multiple runs. F1 help overlay confirmed via a temporary debug
auto-trigger (removed before commit) — this sample's own `.htm` has the
standard 3-column controls table, so (unlike PickingSample) the stock
`gen_help_png.py` needed no one-off column-selection variant. See
`samples/TrianglePicking/missing.md` for the complete account.

Commit this session: `5bd30b0`, pushed to `develop`.

**Newest session (2026-07-09, second same-day follow-up):** Ported
**PickingSample (#047)**, section 8 task 6's recommended next candidate (one
of the 6 remaining unblocked lighting samples, chosen for this continuous
unattended porting session). Source: `Game.cs`/`Cursor.cs`/
`BoundingSphereRenderer.cs` — a table with 4 pickable objects (`Sphere`,
`Cats`, `P2Wedge`, `Cylinder`), mouse-ray picking via `Viewport.Unproject` →
`Ray` → `BoundingSphere.Intersects`, with per-model name labels drawn via
`Viewport.Project` + `SpriteFont.DrawString` when the ray hits. Ported
`Cursor` as a `DrawableGameComponent` (Windows/mouse branch only — Xbox/
Windows-Phone branches dropped, matching every other desktop-only port in
this repo) and `BoundingSphereRenderer` as an ordinary instance class (C#
static-class-with-extension-method → C++ instance member, a natural
language-constraint adjustment, not a behavior change).

All 5 source FBX files (`table.FBX`, `Sphere.fbx`, `Cats.FBX`, `Cylinder.fbx`,
`P2Wedge.FBX`) turned out to be plain ASCII FBX 6.1, converted directly with
`tools/fbx_ascii2model.py` — no binary-FBX/`ufbx` workaround needed this time
(unlike Graphics3D). One asset-naming quirk found and worked around:
`Game.cs` calls `Content.Load<Model>("Table")` (capital T) even though the
source file/content-item name is lowercase `table` — only worked in the
original because Windows content loading is case-insensitive; kept the
literal `"Table"` string in the C++ port and named the converted files to
match (`Table.model.json` etc.) instead of changing the source line. Also
found, via direct `Cats.FBX` inspection, that its only real mesh is a plain
box (`Box01`) — not a detailed cat model — matching the original asset
exactly, not a conversion bug.

Two CNA gaps surfaced, both already covered by existing DEFERRED.md items
(no new items needed), plus one confirmed sighting of the already-tracked
near-plane-clipping bug:

1. **`Game::DoInitialize()`'s component-lifecycle gap (DEFERRED.md item
   #23)** — this sample's original creates `Cursor` and adds it to
   `Components` from inside `Initialize()`, the same pattern Graphics3D hit
   first. Worked around with the identical `AddComponent()` helper pattern
   established there.
2. **`ModelMesh::ParentBone` is always `nullptr` for `.model.json`-loaded
   models** — confirmed via direct source read of `ModelTypeReader::Read()`:
   it builds one synthetic `"Root"` bone but never assigns any mesh's
   `ParentBone` to it (no setter even exists). Dereferencing
   `mesh.ParentBone.Index` directly (mirroring the C# source literally, as
   this sample's `DrawModel()`/`RayIntersectsModel()` both do) segfaulted
   immediately. This is the same root cause DEFERRED.md item #6's existing
   "multi-bone rigid-part" note already describes — not a new gap. Worked
   around with a small `BoneIndexOf(ModelMesh*)` helper that falls back to
   bone index 0, the exact same fallback CNA's own `Model::Draw()` already
   uses internally; correct here since every model in this sample is
   logically single-bone.
3. **New finding: untextured `BasicEffect` + default lighting renders every
   model as flat, fully-saturated white, with zero shading gradient at any
   camera angle** (confirmed via pixel sampling across several screenshots
   at different camera-rotation angles — not a one-off framing coincidence).
   Root-caused via direct source read of `EasyGLGraphicsBackend.cpp`'s
   `EnsureLit3DProgram()`: the lit fragment shader always multiplies by
   `texture(uTexture, vUV)`, falling back to an internal 1×1 *white* texture
   when none is bound — a no-op multiply. Combined with `BasicEffect`'s
   default white `DiffuseColor` (never overridden, since this sample's
   original relies entirely on its FBX-embedded material textures for color)
   and XNA's bright standard 3-point `EnableDefaultLighting()` rig, the lit
   result exceeds `(1,1,1)` for a broad range of normals and clips to solid
   white. This is a direct (if visually dramatic) consequence of the
   already-known "no per-mesh texture in `.model.json`" gap first flagged by
   LensFlare's `ground.png` note, not an independent new lighting bug — added
   the addendum to DEFERRED.md item #6 that LensFlare's own `missing.md`
   predicted a future sample would eventually need.

Also separately confirmed, at one particular camera-rotation angle, the
already-tracked near-plane-clipping thin-diagonal-line artifact
(`CameraShake`/`CustomModelClass`/`LensFlare`/`Graphics3D`'s bug family) on
one of the models — an independent, angle-*dependent* issue from the
angle-*independent* flat-white finding above, not a duplicate observation.

`BoundingSphereRenderer`'s wireframe circles were not confirmed visible in
any screenshot (toggling them off made no visible difference) — flagged in
`samples/PickingSample/missing.md` as unresolved (possibly occlusion/depth-
test related), not assumed to be a bug or assumed to be working.

Builds 0 warnings (verified via a from-scratch rebuild); ran 5+ seconds with
no crash across multiple runs. F1 help overlay verified via this repo's
established temporary-debug-auto-trigger pattern (removed before commit) —
renders correctly, using a one-off `gen_help_png.py` variant (same pattern as
MicrophoneEcho) to pick the "Windows" column instead of the tool's default
"Windows Phone" column from the sample's 4-column controls table. Live mouse-
driven cursor/name-label interaction was not exercised via synthetic input
this session (same `xdotool` reliability caveat noted throughout this repo).
See `samples/PickingSample/missing.md` for the complete account.

Commit this session: `c58652e`, pushed to `develop`.

**Newest session (2026-07-09, same-day follow-up):** Ported **Graphics3D
(#046)**, picked interactively (user asked "which sample next" after LensFlare
shipped; recommended it as the leanest remaining unblocked candidate — one FBX
model, one texture, no `.x`-format blockers). Three real, separate findings
this session, on top of the port itself:

1. **`spaceship.fbx` is an old *binary* FBX (v6.1/6000)** — unreadable by
   `assimp` 5.4 and Blender 4.3.2's FBX importer (both explicitly refuse
   anything older than FBX 2011) and by this repo's own
   `tools/fbx_ascii2model.py` (ASCII-only). Worked around with a one-off
   `ufbx`-based Python script (installed in a scratch virtualenv, not added as
   a repo dependency) that bakes the mesh's node transform and writes a plain
   `.obj`, then fed that through the existing `tools/obj2model.py` — no new
   converter added to `tools/`, since this is (so far) a one-off asset format.
2. **Found and worked around a real CNA component-lifecycle bug**: this
   sample's C# original creates its 4 `Checkbox` components from inside
   `Initialize()` (not the constructor) — a pattern real XNA/FNA supports
   because `Game`'s constructor subscribes to `Components.ComponentAdded`
   immediately. `cna`'s `Game::DoInitialize()` instead subscribes that event
   *after* calling the user's `Initialize()` override, so components added
   from inside `Initialize()` never get their own `Initialize()`/
   `LoadContent()` called — segfaulted on the very first `Draw()` (dereferencing
   an unset `std::optional<SpriteBatch>`). Worked around with an explicit
   `component->Initialize()` call right after `Add()`. Filed as DEFERRED.md
   item #23 — every other sample in this repo happens to add components from
   the constructor instead, which is why this hadn't surfaced before.
3. **The spaceship model itself doesn't render.** Extensively isolated via
   direct experimentation (hand-computed NDC coordinates confirmed the math is
   correct; swapping in the already-proven `tank.model.json` through the exact
   same drawing code at the exact same ~3523-unit camera distance also
   rendered nothing; the *same* substitution at `CustomModelClass`'s own
   ~1059-unit camera distance reproduced the known thin-line artifact) — this
   is the same near-plane-clipping-family EasyGL bug already tracked for
   `CameraShake`/`CustomModelClass`/`LensFlare` (section 4), now confirmed to
   cause **full invisibility**, not just a degenerate thin line, at longer
   camera distances. Also found — while testing an unrelated hypothesis during
   this investigation — that `GraphicsDevice::Clear(Color)`'s single-argument
   overload never clears the depth buffer (real XNA's does); confirmed this is
   not what's hiding the spaceship, but it's a real, separate gap affecting
   every 3D sample in this repo. Filed as DEFERRED.md item #24.

Also confirmed live via a temporary debug auto-trigger (removed before commit):
starfield background toggle, explosion sprite-sheet animation, all 4 buttons'
icon/tint state, and the F1 help overlay (custom-written control text, since
the original `.htm` has no keyboard/gamepad table — it's touch-only) all render
correctly. Mouse substitutes for the original's touch/gesture input throughout
(drag-to-rotate, wheel-to-zoom, click-to-toggle) — not separately exercised via
synthetic input this session. Builds 0 warnings; ran 6+ seconds with no crash.
See `samples/Graphics3D/missing.md` for the complete account.

**Same-day, earlier:** Ported **LensFlare (#041)**, section 8 task 1 from
the prior session's handoff. Used `DrawableGameComponent` for `LensFlareComponent`
(matching the C# original's own component split), stock `Model`/`BasicEffect` for
the terrain, and CNA's `OcclusionQuery` for the sun-visibility trick — all worked
with no CNA-side gaps for the sample's own code. While converting `terrain.fbx`,
found and fixed a real bug in `tools/fbx_ascii2model.py`: it never applied a mesh's
baked `PreRotation`/`LclRotation`/`LclScaling`/`LclTranslation` node transform, only
its raw vertex/normal data. `terrain.fbx`'s `Plane01` mesh has a `-90,0,0`
`PreRotation` (a standard 3ds-Max Z-up → FBX-declared-Y-up correction); without
applying it, the terrain's height ended up in Z instead of Y and the whole mesh sat
outside the camera's view volume — first screenshot was solid CornflowerBlue with
nothing visible at all. Fixed the converter generically (any mesh's node transform,
not special-cased to this asset) and confirmed live it does not change `tank.fbx`'s
already-shipped conversion output (all its `PreRotation`s are `0,0,0`; regenerated
into a scratch dir and diffed byte-identical against the checked-in
`tank.model.json`/`.bin` files — see DEFERRED.md item #6's addendum for the note
that `tank.fbx`'s *sub-meshes* do have non-zero `Lcl Translation`, relevant only
once a future sample needs independently-posed tank parts).

After the fix, LensFlare's terrain renders with the same thin-diagonal-line
near-plane-clipping artifact already known from `CameraShake`/`CustomModelClass`'s
tank rendering — confirming that bug's cause is shared across two independently-
converted FBX assets, not specific to the tank model. Also found a **new** CNA
rendering gap while verifying: the EasyGL backend never applies
`BlendState.ColorWriteChannels` (`glColorMask` is never called anywhere in
`EasyGLGraphicsBackend` — confirmed via direct grep), so `LensFlareComponent`'s
occlusion-query trick (drawing an intentionally-invisible test polygon via
`ColorWriteChannels.None`) instead renders a fully visible white square on screen.
Filed as DEFERRED.md item #22 (not started — not fixed this session, out of scope
for a porting task). Additionally observed that the glow/flare sprites never
appeared during 5+ seconds of live verification; not root-caused, flagged in
`samples/LensFlare/missing.md` rather than assumed benign.

F1 help overlay verified via this repo's established temporary-debug-auto-trigger
pattern (removed before commit) — `xdotool` reached the window's focus
(`getactivewindow` matched) but a sent `F1` keypress had no observable effect,
consistent with this repo's known `xdotool` reliability caveat, not a code bug.

**Also discovered, unrelated to LensFlare:** a full aggregate build
(`cmake --build cmake-build-debug -j$(nproc)`, no `--target`) now fails with 8
errors in `samples/SafeArea/src/SafeAreaOverlay.hpp` — `Viewport` no longer exposes
direct `.x`/`.y` members upstream in `cna` (same class of drift that previously
broke `InputReporter`). Not fixed this session (out of scope for the LensFlare
task); flagged as a new item in section 8 for whoever picks this repo up next.
`LensFlare_cna_samples` itself was independently confirmed to build clean via its
own `--target`, both before and after this discovery.

Commits this session: not yet committed as of this NEXT.md update — see git status.

**Newest session (2026-07-06, follow-up):** `cna`'s `feature/net` fixed two of the
three DEFERRED.md gaps `ClientServerSample` (#091) worked around — #19
(`GamerServicesDispatcher` hang, commit `08171ac`) and #20 (`NetworkGamer.IsHost`/`Id`
stubs, commit `81f10b5`). Removed both workarounds from `ClientServerSample`: a real
`GamerServicesComponent` is now constructed (matching the C# original exactly), and
the local `bool isHost_` tracking member is gone in favor of
`networkSession_->getIsHostProperty()`/`gamer->getIsHostProperty()` directly. Built
against `cna`'s `feature/net` tip (temporarily checked out in the `../cna`
build-dependency checkout, not merged into `cna`'s own `develop` — that's a separate
decision for whoever manages that branch; **a future session should either merge
`feature/net` → `develop` in `cna`, or re-check-out `feature/net` locally again, before
relying on these fixes being present**). Live-verified with real `xdotool` keypresses
(unlike the prior session, which needed a debug auto-trigger): session creation no
longer hangs, tank spawns labeled `"Stub Gamer (server)"` (the real
`GamerServicesDispatcher`-populated identity, not the old manually-synthesized
`"Player"`), movement works. A genuine two-instance host+client test hit a separate,
pre-existing `ENetDiscoveryService` two-process discovery limitation on this
container (not a regression) — see `samples/ClientServerSample/missing.md`.
Item #21 (`GamerJoined` queued, not synchronous) was investigated in `cna` in depth
and found to require either a `sharp-runtime` change (needs the user's direct
sign-off) or accepting the sample's existing extra `Update()`-after-subscribing call
as the permanent, correct pattern. Updated `DEFERRED.md` items #19–21 and
the summary table. Commit `3197b06`, pushed to `develop`.

**Same-day follow-up: the user approved the `sharp-runtime` change, and it landed.**
`sharp-runtime`'s `EventHandler<T>` gained a generic, opt-in `SetReplayHook()`
(`develop` commit `69661c2`); `cna`'s `NetworkSession` constructor now uses it so
`GamerJoined` replays immediately on subscription, matching real XNA (`feature/net`
commit `ab05395`). Removed `ClientServerSample`'s last remaining workaround too — the
`networkSession_->Update();` call right after `HookSessionEvents()` — confirmed live
it's genuinely unnecessary now. `xdotool` was unreliable on this shared desktop again
this round (tried real keypresses, a fully isolated `Xvfb :77` display with no window
manager, both failed identically — confirmed environmental, not a regression); fell
back to this repo's own established debug-auto-trigger pattern (temporary, removed
before commit) to verify live: session creation → immediate `GamerJoined` → tank
spawn → render, no crash, no manual `Update()` needed. `ClientServerSample` now has
**zero** of its original three DEFERRED.md workarounds. Updated `DEFERRED.md` item
#21 (now ✅ resolved) and `missing.md`. Commit `ef1e930`, pushed to `develop`.

**Not done this session:** re-attempting NetworkPrediction (#100)/PeerToPeer (#103)
— both share the same three gaps and are now easier to port than ever (all 3
workarounds gone), but porting two new samples from scratch is a separate, larger
task than this session's cleanup scope; a natural next step if the user wants it.

Most recent full porting session (2026-07-06), in order:
- Fixed `InputReporter`'s full-repo build failure: it read `GamePadCapabilities`
  fields directly (`cap.HasLeftStickButton`); upstream `cna` made them private
  behind `getXxxProperty()`. Updated all ~23 accesses in
  `samples/InputReporter/src/InputReporterGame.hpp`.
- Added `ignored.md`: lists all 67 catalogued sample directories that will never
  get a `samples/` directory, each with a one-line reason. `plan.md`'s old
  per-category tables for these were collapsed into a single pointer.
- Added 36 placeholder directories (`<Name>.htm` + `missing.md`, no source) for
  every remaining XNA 4.0 sample judged worth tracking once CNA can do more.
  Root `CMakeLists.txt` got matching commented-out `add_subdirectory()` lines.
- Added `DEFERRED.md` items #16–18 (microphone capture, multiplayer networking,
  content-pipeline processor extensibility).
- Audited all 45 (at the time) already-ported samples' `missing.md` against
  their C# originals and current `cna` source. **Incident:** one sub-agent
  misread `git status` (many concurrent Claude sessions touch this repo — normal)
  and ran `git checkout` on 31 files it didn't own, discarding other batches'
  completed work. Caught, confirmed placeholder dirs were untouched, fully
  re-ran the 6 affected batches.
- Corrected DEFERRED.md items #16, #17, and (pre-existing) #5 after finding they
  were stale — live source/build checks showed `cna` already resolved them
  (merged 2026-07-04, two days before item #16/#17 were written this session).
  This unblocked 13 samples outright: MicrophoneEcho (#098), ClientServerSample
  (#091), NetworkPrediction (#100), PeerToPeer (#103) (item #17), and LensFlare
  (#041), Graphics3D (#046), PickingSample (#047), TrianglePicking (#048),
  HeightmapCollision (#049), CustomModelClass (#052), InverseKinematics (#057),
  ChaseCamera (#058), MarbleMaze (#061) (item #5). NetRumble (#062) went from
  double- to single-blocked.
- Ported **MicrophoneEcho (#098)**. Builds 0 warnings; live-verified via
  screenshot.
- Ported **ClientServerSample (#091)**. Surfaced and worked around 3 new CNA gaps
  (DEFERRED.md items #19–21 — see section 5) and fixed a `cna-samples`-side
  build-wiring gap: `CNA_Net`/`CNA_GamerServices` are separate CMake targets in
  `cna`, gated behind `CNA_ENABLE_NET` (off by default) and never linked by
  `cna_add_sample()`. Fixed in this repo's root `CMakeLists.txt` (added
  `set(CNA_ENABLE_NET ON ...)`) and `cmake/SampleHelpers.cmake` (added
  `if(TARGET CNA_Net) target_link_libraries(... CNA_Net) endif()`). Live-verified:
  session creates, tank spawns, renders correctly, no crash over 8+ seconds.
  `JoinSession()`/2-machine path not verified.
- Ported **CustomModelClass (#052)**, using stock `Model`/`BasicEffect` instead of
  the C# original's own `CustomModel` class (CNA has no build-time custom-
  `ContentProcessor` extensibility — item #18). Builds/runs clean, but rendering
  hit the near-plane clipping bug described in section 4 — confirmed via a
  side-by-side screenshot with CameraShake (identical artifact) that this is
  pre-existing, not a new regression.
- Rewrote this file (`NEXT.md`) to the current 10-section template.

Commits this session (newest first): `eee6769`, `155bc18`, `d9a2baf`, `580283c`,
`4dd7ceb`, `d747356`. All pushed to `develop`.

Commits from the newest follow-up session (see the top entry above): `3197b06`,
`ef1e930`. Both pushed to `develop`.

---

## 4. Current blocker / main problem

**There is no failing build right now.** The full aggregate build
(`cmake --build cmake-build-debug -j$(nproc)`, no `--target`) was found broken
on 2026-07-09 (`SafeArea`'s `Viewport.x`/`.y` usage) and **fixed on 2026-07-10**
— along with a second, previously-undiscovered instance of the exact same
pattern in `RolePlayingGame` (only surfaced once the aggregate build actually
got past `SafeArea`; `ninja` stops at the first failing target, so
`RolePlayingGame` — later in the build graph — was invisible until then). Both
are fixed; a full aggregate rebuild is confirmed green (0 errors/0 warnings,
re-run twice, second run `ninja: no work to do`). See the Build subsection of
section 2 for the full account. The most significant *remaining* open problem
is the rendering bug below.

**Update 2026-07-10: likely re-attributed — read DEFERRED.md item #26 before
investigating this as a clipping bug.** While porting InverseKinematics, a
different, concretely-confirmed bug was found: `ModelTypeReader::Read()`
(`ContentManager.cpp`) picks a vertex-upload code path by comparing a
`.model.json`'s declared (clean, XNA-sized) `"vertexStride"` against `sizeof()`
of CNA's own vertex structs — but every one of those structs now inherits from
the polymorphic `IVertexType`, inflating their real sizes by an 8-byte vtable
pointer (confirmed live: `sizeof(VertexPositionNormalTexture)` is 40, not the
clean 32 every conversion tool declares). `"vertexStride": 32` (used by every
`Content.Load<Model>`-based sample in this repo) *accidentally* collides with
`sizeof(VertexPositionTexture)`'s own inflated size (also 32), so the reader
always uploads every model's vertex data through the *wrong* typed overload,
`reinterpret_cast`-reading position/texcoord fields from the wrong byte offsets
— corrupting every stride-32 model's geometry repo-wide. A corrupted,
essentially arbitrary per-vertex reinterpretation is a far more complete
explanation for the exact two symptoms below (thin line / full invisibility)
than an actual clip-space defect, and no prior session investigating this bug
ever directly opened the EasyGL clipping code to confirm the "suspected cause"
bullet below. **Not confirmed with certainty for tank.model.json/
terrain.model.json specifically** (that would need re-running one of them
through the same bypass used in `samples/InverseKinematics/src/
CylinderModel.hpp` — a fast, cheap first step recommended before any further
clip-space investigation). See DEFERRED.md item #26 and `samples/
InverseKinematics/missing.md` for the full write-up. The rest of this section
is kept as originally written, for full historical context on how the symptom
was originally characterized:

The previously-tracked rendering bug (near-plane clipping) is still open and is now
confirmed on a **third** independent asset, and with a **second distinct visible
symptom**:

- **Exact symptom:** a `Model` drawn through a perspective camera renders as a
  thin diagonal line/dashes instead of a recognizable 3D shape — at "moderate"
  camera distance (~1000 units). Originally found on `tank.model.json`
  (CameraShake/CustomModelClass); confirmed on LensFlare's `terrain.model.json`
  too. **At longer camera distance (~3500 units), the same underlying bug
  instead produces full invisibility, not a thin line** — confirmed 2026-07-09
  while porting Graphics3D (#046): its spaceship (own camera distance ~3523)
  renders nothing at all; swapping the already-proven `tank.model.json` into
  Graphics3D's own drawing code at that same ~3523 distance also rendered
  nothing, and swapping it in again at `CustomModelClass`'s own ~1059 distance
  reproduced the familiar thin line — isolating the distance, not the asset or
  the drawing code, as what determines which symptom appears.
- **Failing command (to reproduce):**
  ```
  cd cmake-build-debug/samples/CameraShake
  SDL_VIDEODRIVER=x11 ./CameraShake_cna_samples
  ```
  (or `CustomModelClass_cna_samples`, `LensFlare_cna_samples` — thin line; or
  `Graphics3D_cna_samples` — fully invisible, same bug family, longer camera
  distance.)
- **No failing automated test** — there is no test suite; this was found by
  screenshot comparison.
- **Affected files/modules:** almost certainly
  `cna/src/CNA/Internal/Backends/EasyGL/EasyGLGraphicsBackend.cpp` (clipping/
  projection path) — not yet confirmed by direct inspection.
- **Suspected cause:** near-plane (`w<0`) vertex clipping in the EasyGL backend
  does not match DirectX/real-XNA behavior, so triangles crossing the near plane
  degenerate instead of being clipped correctly. The distance-dependent
  thin-line-vs-invisible split found this session suggests whatever's wrong is
  sensitive to the actual clip-space `w`/depth values involved, not purely a
  binary "does this triangle cross the near plane" check — worth keeping in
  mind once someone actually opens `EasyGLGraphicsBackend.cpp` to fix this.
- **What has been tried:** confirmed (2026-07-06) via a side-by-side screenshot
  that CameraShake and CustomModelClass show the *identical* artifact in the
  *identical* screen position; confirmed again (2026-07-09) that LensFlare's
  independently-converted terrain asset shows the same artifact too; confirmed
  again (2026-07-09) via direct asset-swap isolation testing that Graphics3D's
  full invisibility is the same bug at a different camera distance, not a
  separate defect. Not yet root-caused inside `cna` itself; no fix attempted.
- **Why it matters now:** 1 more sample (MarbleMaze — LensFlare, Graphics3D,
  PickingSample, TrianglePicking, HeightmapCollision, InverseKinematics, and
  ChaseCamera are now all ported, see section 8) is otherwise unblocked and
  portable, but **should not be assumed to render correctly** just because it
  builds — it needs its own screenshot check for this same artifact once
  ported (and, per the above, "renders nothing" is now just as suspect as "shows
  a thin line" — don't assume a blank frame means something else is wrong
  without checking camera distance against this bug first). PickingSample's and
  TrianglePicking's own ports each confirmed the thin-line symptom once more
  (see section 3); HeightmapCollision's own port, by contrast, confirmed neither
  symptom at all — its camera sits only ~155 units from its subject, well under
  the ~1000+ unit distances where the bug has been observed, a useful negative
  data point on the distance-dependence theory (task 2's own reasoning).
  InverseKinematics's own port also confirmed neither symptom (camera only ~5
  units away) but for a different reason than distance alone — see the
  DEFERRED.md item #26 update above; its cylinder chain bypasses
  `ModelTypeReader` entirely via `CylinderModel.hpp`, so it was never exposed to
  the corrupted-vertex-upload bug in the first place regardless of distance.
  **ChaseCamera's own port went further: it empirically confirmed (not just
  assumed) that its own Ship/Ground models hit the exact "fully invisible"
  symptom at its ~4031-unit camera distance, then confirmed the item #26 bypass
  (`RawModel.hpp`) fixes it completely** — a third and fourth independent
  confirmation of item #26's hypothesis, on two more assets at both size
  extremes (32458 vertices and 6 vertices) — see `samples/ChaseCamera/missing.md`.
  Also, PickingSample surfaced a separate, angle-independent
  "flat white, no shading" finding (also confirmed again by TrianglePicking) —
  don't conflate the two; see `samples/PickingSample/missing.md` and
  `samples/TrianglePicking/missing.md`.

**Other rendering/framework gaps found this session (not blocking, tracked
separately):**
- EasyGL backend never applies `BlendState.ColorWriteChannels` — DEFERRED.md
  item #22 (found via LensFlare).
- `Game::DoInitialize()` wires `Components_.ComponentAdded` after calling the
  user's `Initialize()` override, breaking the common "add components from
  `Initialize()`" pattern real XNA/FNA supports — DEFERRED.md item #23 (found
  via Graphics3D; worked around at the sample level).
- `GraphicsDevice::Clear(Color)`'s single-argument overload never clears the
  depth buffer (every 3D sample in this repo uses it) — DEFERRED.md item #24
  (found via Graphics3D; confirmed not the cause of its invisible-model bug
  above, but a real, separate, latent gap).

Secondary, lower-urgency items (not blocking, just open):
- Both pending product/scope decisions are now fully settled (2026-07-10, user
  go/no-go): AccelerometerSample (#084) **and** TiltPerspective (#107) are both
  **done** (the former needed the original's own emulator keyboard fallback,
  not an invented one; the latter needed a genuinely invented keyboard-tilt
  scheme, since its own original's fallback is a non-interactive time-driven
  wobble with nothing to promote — see DEFERRED.md item #15's correction,
  section 8 task 9 now fully done); the 5 Avatar samples will not be ported
  (section 8, task 10, retired).
- The Vulkan backend's multi-`SpriteBatch`-per-frame bug (section 5) is separate
  from the above and only affects the non-default Vulkan backend.

---

## 5. Known bugs and limitations

- **CONFIRMED BUG, unfixed, likely MIS-DIAGNOSED as clipping — read DEFERRED.md
  item #26 first** — EasyGL near-plane clipping renders certain `Model`-based
  geometry (confirmed: `tank.model.json` at CameraShake's and CustomModelClass's
  ~1059-unit camera distance, `terrain.model.json` at LensFlare's) as a
  degenerate thin line instead of the model — **and, confirmed 2026-07-09 via
  Graphics3D, the same bug renders geometry as fully invisible at longer
  (~3523-unit) camera distances**, isolated by direct asset-swap testing to be
  the camera distance, not the asset or drawing code. See section 4 for full
  detail. **Update 2026-07-10:** a concretely-confirmed, different bug was found
  while porting InverseKinematics — `ModelTypeReader::Read()` uploads corrupted
  vertex data for every stride-32 `.model.json` (an `IVertexType` vtable
  inflates every CNA vertex struct's `sizeof()` past the clean XNA size the
  format declares, causing the reader to upload through the wrong typed
  overload) — which is a far more complete explanation for a thin
  line/full-invisibility symptom than an actual clip-space defect that no prior
  session ever directly confirmed by reading the clipping code itself. Not yet
  re-confirmed on `tank.model.json` specifically, but strongly suspected to be
  the same bug — see DEFERRED.md item #26 and try that fix before investigating
  `EasyGLGraphicsBackend.cpp`'s clipping path.
- **CONFIRMED BUG, unfixed** — the EasyGL backend never applies
  `BlendState.ColorWriteChannels` (no `glColorMask` call anywhere in
  `EasyGLGraphicsBackend.cpp`, confirmed via direct grep). Found via LensFlare's
  occlusion-query trick (a `ColorWriteChannels.None` blend state meant to keep its
  test polygon invisible), which instead renders a solid white square on screen.
  DEFERRED.md item #22 (new, not started). See `samples/LensFlare/missing.md`.
- **CONFIRMED BUG, worked around (Graphics3D, #046)** — `Game::DoInitialize()`
  wires up `Components_.ComponentAdded`/`ComponentRemoved` *after* calling the
  user's `Initialize()` override, unlike real XNA/FNA (which subscribes in the
  `Game` constructor, before `Initialize()` can run). A component added to
  `Components` from within `Initialize()` — a pattern real XNA supports and this
  sample's C# original uses — never gets its own `Initialize()`/`LoadContent()`
  called, since the event that would trigger it isn't subscribed yet; segfaults
  on first `Draw()` if that component's `Draw()` depends on `LoadContent()`
  having run. Worked around by calling `component->Initialize()` explicitly
  right after `Add()`. DEFERRED.md item #23 (new, not started). See
  `samples/Graphics3D/missing.md`.
- **CONFIRMED BUG, unfixed (latent)** — `GraphicsDevice::Clear(Color)`'s
  single-argument overload never clears the depth buffer (confirmed via direct
  source read); real XNA's same-signature overload clears color+depth+stencil
  together. Every 3D sample in this repo uses this overload. Confirmed (via
  Graphics3D's investigation) this is *not* the cause of the near-plane-clipping
  bug above, but is a real, separate, currently-latent gap. DEFERRED.md item #24
  (new, not started).
- ~~Build breakage: `SafeArea`/`RolePlayingGame` accessed `Viewport.x`/`.y`
  directly~~ **FIXED 2026-07-10** — both switched to
  `getXProperty()`/`getYProperty()`. Full aggregate build confirmed green.
- **CONFIRMED BUG, workaround applied (ClientServerSample, #091)** —
  `GamerServicesDispatcher::Update()` in `cna` is a no-op, so
  `NetworkSession::Create`/`Find`/`Join`'s synchronous busy-wait loop never
  completes if a `GamerServicesComponent` has been added (which every C#
  original does). Sample-level workaround: don't add one — DEFERRED.md item #19.
- **CONFIRMED BUG, partially worked around (ClientServerSample, #091)** —
  `NetworkGamer.IsHost` and `.Id` are hardcoded stub constants (always
  `true`/`0`), not real per-instance state. `IsHost` worked around with a
  locally-tracked flag; `Id`/`FindGamerById` is not — multi-gamer sessions will
  misroute state to the first gamer. DEFERRED.md item #20.
- **CONFIRMED BUG, workaround applied (ClientServerSample, #091)** — the initial
  `GamerJoined` event is queued for the next `Update()` instead of raised
  synchronously during `Create()`/`Join()` like real XNA. Workaround: call
  `networkSession_->Update()` once right after subscribing to events.
  DEFERRED.md item #21.
- **CONFIRMED, framework gap, unfixed** — the Vulkan backend discards the first
  of two `SpriteBatch.Begin()/End()` blocks issued in the same frame. EasyGL
  (default) is unaffected. Several samples rely on multiple per-frame
  `SpriteBatch` instances and would need this fixed to run correctly on Vulkan.
- **INCOMPLETE** — SplitScreen (#076), TankOnHeightmap (#074), SimpleAnimation
  (#050) need per-mesh `ModelBone` support in CNA's `.model.json` reader
  (`ModelTypeReader::Read()` in `ContentManager.cpp` currently only ever builds
  one flat "Root" bone). User intends to implement this directly in `cna`.
- **INCOMPLETE** — NGSMSample's single-player path and NetRumble both still need
  real gameplay content/shaders respectively even though the underlying
  networking API now exists.
- **NEEDS VERIFICATION** — ClientServerSample's `JoinSession()` and actual
  2-machine LAN discovery/join were never tested (would need two running
  instances).
- **NEEDS VERIFICATION** — RolePlayingGame's (#070) input-driven playthrough
  (portal/chest/NPC/combat) was only exercised via a temporary debug
  auto-trigger, not real keyboard input.
- **NEEDS VERIFICATION** — real hardware accelerometer shake/tilt (Yacht,
  SnowShovel) has never been tested on a device with a physical sensor.
- **NEEDS VERIFICATION** — UISample's `HighScoreScreen` vertical drag-scroll was
  never live-drag-tested (code-reviewed only).
- **ENVIRONMENT LIMITATION, not a code bug** — `xdotool` keyboard/mouse input on
  this shared development desktop intermittently fails to reach sample windows
  even when focus is confirmed via `xdotool getactivewindow` immediately before
  sending. Screenshot tooling (`import`) has also intermittently failed with raw
  X-server errors (`BadMatch`/`X_GetImage`) under heavy concurrent load, unrelated
  to any sample's code. When live input can't be confirmed, this repo's
  established fallback is a temporary, clearly-commented debug auto-trigger
  (removed before commit) plus a screenshot, not blind trust that "no visible
  change" means a bug.
- **RISKY ASSUMPTION, worth rechecking periodically** — DEFERRED.md blockers can
  go stale silently, because `cna` is under active, concurrent development by
  other sessions/branches. A blocker confirmed accurate today is not guaranteed
  accurate next session; re-verify live (grep `cna`'s current source, or build+
  run an existing test) before treating any DEFERRED.md item as gating a real
  decision.

---

## 6. Architecture notes

- **Sample layout:** `samples/<Name>/{src/,Content/,missing.md,<Name>.htm,
  CMakeLists.txt}`. `src/` is header-only except a single `src/Program.cpp`
  containing `main()`. `CONTENT_DIR` in each sample's `cna_add_sample()` call is
  mandatory for any sample loading assets (including the F1 `help.png`) — the
  binary aborts at startup without it. Content is copied next to the built
  binary via a `POST_BUILD` step, so a sample must be **run from its own binary
  directory**, not the repo root.
- **CNA property pattern:** CNA uses a `DEF_PROP` macro generating
  `getXxxProperty()`/`setXxxProperty()` methods, not public members or real
  C#-style properties (see `CLAUDE.md`'s translation table). Every `Game`
  subclass must implement `GetTypeName()`.
- **CMake module split in `cna`:** the main `CNA` static-lib target excludes
  `Microsoft::Xna::Framework::{GamerServices,Net}` sources entirely; those live
  in separate `CNA_GamerServices`/`CNA_Net` targets gated behind the
  `CNA_ENABLE_NET` option (this repo now sets it `ON`; see `cmake/
  SampleHelpers.cmake` for the matching `target_link_libraries` addition). Any
  future sample needing `Net`/`GamerServices` types gets this automatically.
- **GameComponent lifetime invariant (do not break):** a `shared_ptr`-owning
  "screen"/"parent" object that itself owns many `GameComponent`s must never be
  destroyed synchronously from inside `Game::Update()`'s/`Draw()`'s own
  component iteration (`Game` iterates a snapshot of `Game.Components` taken at
  the top of the frame). Established fix: defer the actual `shared_ptr` release
  to the start of the next frame's `Update()` — used in HoneycombRush/
  NinjAcademy/CardsStarterKit/RolePlayingGame's `ScreenManager`s.
- **Input fallback patterns (pick the one matching the interaction shape, don't
  invent a new one):** single discrete tap → synthesize a `GestureType::Tap` on
  a mouse left-click rising edge via `TouchPanel::EnqueueGesture()` (preferred);
  continuous drag → track mouse position while a button is held; no rotation
  sensor → a keyboard key manually resizes the back buffer.
- **NetworkSession usage pattern (established by ClientServerSample, #091):**
  do **not** add a `GamerServicesComponent` (hangs `Create`/`Find`/`Join` — see
  section 5, item #19); track "am I the host" with a locally-scoped bool set at
  the `Create()`/`Join()` call site, never via `gamer.IsHost`/
  `networkSession.IsHost`; call `networkSession_->Update()` once immediately
  after subscribing to session events, to flush the initial `GamerJoined` event
  before the first real per-frame `Update()` reads gamer `Tag` state.
- **Boundaries not to cross:** no shared `samples/common/` library, even between
  structurally similar samples. Do not use CNA's NOXNA `SetBlendEnabled`/
  `SetDepthTestEnabled`/`SetDepthWriteEnabled` helpers — use the real XNA state
  objects (`BlendState`, `DepthStencilState`, `RasterizerState`) directly. Do not
  switch `ScreenManager` screen ownership from `shared_ptr` to raw pointers.
- **`cna`/`sharp-runtime` boundary:** separate, independently-developed sibling
  repos consumed via `add_subdirectory`. Never assume their working tree is
  clean; never edit them without confirming scope with the user first.

---

## 7. Useful commands

Configure (from repo root; only needed once or after a `CMakeLists.txt` change):
```
cmake -S . -B cmake-build-debug
```
If CMake complains about the vendored ENet dependency's minimum version:
```
cmake -S . -B cmake-build-debug -DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

Build one sample (target name is `<SampleDirName>_cna_samples`):
```
cmake --build cmake-build-debug --target CustomModelClass_cna_samples -j$(nproc)
```

Build everything:
```
cmake --build cmake-build-debug -j$(nproc)
```

Run a sample (must `cd` into its own binary directory first — `Content/` is
copied there, not to the repo root):
```
cd cmake-build-debug/samples/CustomModelClass
SDL_VIDEODRIVER=x11 ./CustomModelClass_cna_samples
```

Reproduce the current main problem (section 4) — compare against CameraShake,
which shows the identical artifact:
```
cd cmake-build-debug/samples/CameraShake
SDL_VIDEODRIVER=x11 ./CameraShake_cna_samples
```

Generate a font asset:
```
python3 tools/make_font.py /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf 14 samples/<Name>/Content/font
```

Generate the mandatory F1 help overlay:
```
python3 tools/gen_help_png.py samples/<Name>/<Name>.htm samples/<Name>/Content/help.png
```

No lint/format command and no automated test suite are configured in this repo.

---

## 8. Next smallest tasks

**Recently completed (2026-07-09/07-10):** LensFlare (#041), Graphics3D (#046),
PickingSample (#047), TrianglePicking (#048), HeightmapCollision (#049),
InverseKinematics (#057), ChaseCamera (#058), MarbleMaze (#061),
NetworkPrediction (#100), PeerToPeer (#103), **AccelerometerSample (#084)**,
and **TiltPerspective (#107)**, all screenshot-verified — see section 3 for
the full account of each, including the new DEFERRED.md items (#22–#27), the
item #6/#9/#15 corrections, and item #23's clarification. Tasks 6, 7, and now
9 (below) are all retired/fully done — **this completes both the original
"port more samples" backlog this multi-session porting run set out to work
through, and the 2026-07-10 user go/no-go decision covering these last two
samples.** See task 7's own entry for a full account of the porting backlog,
and this section's new closing note for what a future session's task list
should look like — **as of this session, there is no further approved/queued
porting work; the next concrete step needs either a `cna`-side fix or a new
user product-scope decision.**

**Superseding update (2026-07-11):** the "no further approved/queued porting
work" conclusion below is now **stale** — `cna`'s `develop` picked up a large
batch of fixes (Tasks 927-949, all landed 2026-07-10, discovered and verified
this session) that resolve most of the concrete blockers this list names.
Confirmed via direct source read/live rebuild+screenshot, not just commit
messages: DEFERRED.md items **#26** (the vertex-corruption bug behind the
whole "near-plane-clipping" bug family — the single biggest fix, see item #2
below and DEFERRED.md item #26's own updated write-up), **#14** (TextureCube),
**#24** (Clear depth), **#25** (VertexBuffer/IndexBuffer GetData), and the
**multi-bone addendum under #6** (unblocks SplitScreen/SimpleAnimation/
TankOnHeightmap — see task 11 below) are all now fixed. Item **#23** turned out
to never have been a real bug (FNA has the identical behavior). Item **#13**
(skeletal animation) is only partially done — foundation classes landed but
the model-loader/renderer wiring and conversion tool did not, so it still
blocks every skeletal-animation sample; do not start those yet. Items #11
(shader conversion), #18 (ContentProcessor extensibility), #22
(ColorWriteChannels), #27 (SessionProperties replication), #28 (investigated,
not reproduced), and #29 (DualTextureEffect vertex layout) all remain open,
unchanged. **There IS new approved-shape porting work available now**
(SplitScreen/SimpleAnimation/TankOnHeightmap, task 11) — a future session
should pick this up rather than trust the "nothing left" conclusion below,
which predates this discovery. This repo's own standing caveat (section 5:
"DEFERRED.md blockers can go stale") turned out to cut both ways — a blocker
can also silently *resolve* upstream, not just go stale-and-wrong.

**Backlog-exhaustion note (2026-07-10, PARTIALLY SUPERSEDED — see note above):** every sample this session's own
brief could point to as "confirmed-unblocked, zero CNA gap, just needs the
port written" has now been ported (see the list above). No further placeholder
sample was independently confirmed to be in that same state as of this session —
see task 6's own entry for the research pass that established this for the
graphics/lighting backlog, and task 7's entry for the equivalent conclusion for
the networking backlog. A future session picking this repo back up should NOT
assume the well is dry, though — it should actively re-verify, since two things
can change the picture at any time: (1) `cna`/`sharp-runtime` are under
concurrent development and DEFERRED.md blockers can go stale (this session
independently caught one such stale entry, item #9, in an earlier pass — see
section 5's own standing caveat) — a future session should not trust any
placeholder's `missing.md` blocker write-up without a fresh grep/read of `cna`'s
actual current source, the same standing rule section 5 already documents for
this exact reason; (2) DEFERRED.md item #26's fix, if/when someone applies it
(see task 2 below), removes a real constraint on *how* any future
`Content.Load<Model>`-based sample must be ported (straight loading vs. a
`RawMesh.hpp`/`RawModel.hpp`-style bypass) even though — checked directly this
session, not assumed — **none of the (then-28, now-25) currently-commented
placeholder directories are blocked by item #26 specifically**: every one of
them checked (`ColorReplacement`, `BillboardSample`, `InstancedModel`,
`NetRumble`, `ShipGame`, and the rest of the shader/`Phase 3`/`Phase 4` list) is
blocked by a custom HLSL shader (item #11), skeletal animation (item #13), or
per-mesh `ModelBone` support (item #6/#18) instead — deeper gaps item #26's fix
alone does not remove. **Update (2026-07-10, later the same session):**
`ReachGraphicsDemo` — originally in this same list as "not blocked by item #26
but also not yet independently confirmed portable" — has since been ported in
full (5 of its 6 demo scenes; see this section's newest entry above), applying
item #26's bypass proactively throughout, exactly as this note predicted it
would need. A future session should still re-check the remaining 25 rather
than trust this session's one-time audit, per the same staleness caveat above. Both
previously-open product-scope decisions are now fully settled (2026-07-10,
user go/no-go — see tasks 9/10 below): AccelerometerSample **and**
TiltPerspective are both done (the former shipped with the original's own
emulator keyboard fallback, not invented; the latter needed a genuinely
invented keyboard-tilt scheme, since its own original's fallback is a
non-interactive time-driven wobble with nothing to promote — see DEFERRED.md
item #15's correction); the 5 Avatar samples will not be ported. **With task 9
now fully shipped, every remaining concrete candidate is gated on a `cna`-side
fix (item #11, #13, #14, #6/#18, or the newer #22–#27) or a new user
product-scope decision — there is no more approved/queued porting work as of
this session.** A future session should re-verify this rather than trust it
indefinitely (per section 5's staleness caveat), and should check with the
user for new direction before assuming otherwise.

1. **✅ DONE (2026-07-10): fixed `SafeArea`'s and `RolePlayingGame`'s
   `Viewport.x`/`.y` build breakages — full aggregate build is green again.**
   Fixed both `samples/SafeArea/src/SafeAreaOverlay.hpp` and
   `samples/RolePlayingGame/src/{RolePlayingGame.hpp,TileEngine/TileEngine.hpp}`
   (the second one only surfaced once the first was fixed and the aggregate
   build could reach it). Confirmed via a from-scratch full rebuild (0 errors/0
   warnings) and a second immediate re-run (`ninja: no work to do`). See
   section 2's Build subsection for the full account.

2. **✅ DONE (fixed upstream in `cna` 2026-07-10, confirmed live 2026-07-11):
   the near-plane-clipping bug family was DEFERRED.md item #26 all along, and
   it's fixed.** `cna` Task 927 rewrote `ModelTypeReader::Read()` to build
   vertex objects field-by-field at hardcoded clean XNA offsets instead of
   `reinterpret_cast`-ing raw file bytes against the vtable-inflated
   `sizeof()` of CNA's own vertex structs. Verified this session: rebuilt the
   full aggregate project against current `cna` `develop` HEAD (0 errors/0
   warnings) and ran `CameraShake_cna_samples` under `SDL_VIDEODRIVER=x11` —
   its tank (loaded via plain `Content.Load<Model>`, no bypass) now renders as
   a complete, correctly-shaped solid silhouette, where it previously degraded
   to a thin line or full invisibility. See DEFERRED.md item #26's own updated
   write-up for the full account.
   - **New optional follow-up (not done this session, not blocking anything):**
     the bypass code this repo wrote around this bug while it was still open
     (`InverseKinematics/src/CylinderModel.hpp`, `ChaseCamera/src/RawModel.hpp`,
     `MarbleMaze`/`ReachGraphicsDemo`'s own `RawMesh.hpp`-family files,
     `RimLighting`'s/ReachGraphicsDemo's `TextureCube` bypass now that item #14
     is also fixed) is no longer strictly necessary and could be replaced with
     plain `Content.Load<Model>()`/`Content.Load<TextureCube>()` calls for
     closer fidelity to each sample's C# original, per this repo's own porting
     philosophy (CLAUDE.md: "as similar as possible to the XNA 4.0 C# original").
     Low priority, purely cosmetic/fidelity cleanup on already-working, already-
     shipped samples — a future session should treat this as optional, not
     assume it's expected.

3. **Fix the EasyGL `BlendState.ColorWriteChannels` gap (DEFERRED.md item #22).**
   - Goal: `EasyGLGraphicsBackend` never calls `glColorMask` (or equivalent), so
     `ColorWriteChannels.None`/partial-channel blend states are silently ignored
     — every draw writes all 4 color channels regardless of the active
     `BlendState`. Found via LensFlare's occlusion-query trick, which renders a
     visible white square instead of staying invisible.
   - Files: wherever `EasyGLGraphicsBackend` applies `BlendState` to GL state
     (alongside its existing blend-func/equation setup) — exact location not yet
     confirmed.
   - Verify: run `LensFlare_cna_samples` under `SDL_VIDEODRIVER=x11`; confirm the
     occlusion-query polygon is no longer visible as a white square. Consider
     also re-checking whether this fixes the "glow/flare sprites never appear"
     observation in `samples/LensFlare/missing.md` — not established either way
     yet.

4. **✅ RETIRED (2026-07-10, `cna` Task 929) — not a bug.** Investigated
   directly against real FNA source: FNA has the *identical*
   subscribe-after-`Initialize()` ordering DEFERRED.md item #23 originally
   claimed was a CNA defect. No `cna` change made or needed. Graphics3D's/
   PickingSample's `AddComponent()` workaround remains correct (it's handling
   a real XNA/FNA gotcha, not a CNA-specific one) and should NOT be removed.
   See DEFERRED.md item #23's corrected write-up.

5. **✅ DONE (2026-07-10, `cna` Task 928) — `GraphicsDevice::Clear(Color)` now
   also clears depth/stencil**, matching FNA. Confirmed via `cna`'s own Task
   928 notes the fix is "behaviorally inert" in practice (both backends
   already cleared depth some other way), so no observable regression risk;
   it's a real correctness fix, not yet independently re-verified against
   this repo's own samples but not expected to change any rendering. See
   DEFERRED.md item #24.

6. **RETIRED (2026-07-10) — task 6's original 3-sample list is now fully
   ported.** PickingSample (#047), TrianglePicking (#048), HeightmapCollision
   (#049), InverseKinematics (#057), ChaseCamera (#058), and now MarbleMaze
   (#061) are all done — see section 3 for each. **DEFERRED.md item #26 is now
   confirmed independently across 5 assets** (cylinder, ship, ground, plus
   MarbleMaze's own marble+6 maze parts assumed-but-not-independently-tested —
   see its missing.md) at vertex counts from 6 to 32458 — treat any stride-32
   `.model.json` in this repo as presumptively affected; go straight to a
   `RawMesh.hpp`/`RawModel.hpp`-style bypass (read the converted
   `_verts.bin`/`_idx.bin` directly, construct real `VertexPositionNormalTexture`
   objects field-by-field, upload via the typed `VertexBuffer::SetData`
   overload, bind a real `Texture2D`) for any future sample's model rather than
   assuming `Content.Load<Model>` will render correctly.
   - **What's next, per a dedicated research pass this session:** no further
     placeholder sample was found to be cleanly unblocked (zero CNA gap) the
     way this original 3-sample list was. Every remaining "Todo" placeholder
     needs at least one of: custom HLSL→GLSL shader conversion (item #11, still
     `not started` — see section 9's explicit "do not start" list), skeletal
     animation playback (item #13), or per-mesh `ModelBone` support for
     independently-posed rigid parts (item #6's multi-bone note / item #18).
     **`RimLighting` (#037) was flagged here as the closest to portable, and
     has since been ported (2026-07-10, tenth follow-up — see section 3):**
     the premise below (that it needed a real `cna`-side `TextureCubeTypeReader`
     addition first) turned out to be avoidable, not a hard requirement —
     `OutputCube.dds` was bypassed the same way ReachGraphicsDemo's `EnvmapDemo`
     bypassed its own cubemap (extract faces via ImageMagick, load as
     `Texture2D`, copy into a real `TextureCube` via `SetData()`), with no
     `cna` change needed. DEFERRED.md item #14 remains open (`cna` itself
     unchanged) but no longer blocks any sample in this repo's task list. Kept
     below, struck through in spirit, as a reminder that "needs a `cna` change"
     should be re-checked for a bypass before being treated as a hard blocker:
     ~~needs a small, real `cna`-side addition first (a `TextureCubeTypeReader`
     in `ContentManager.cpp`, effort **S**, item #14) that's out of scope for a
     pure porting session without the user's sign-off to touch `cna`.~~
   - **Recommended immediate next task at the time this note was written:
     section 8 task 7 (NetworkPrediction/PeerToPeer)** — since executed; task 7
     is now also retired (see its own entry below) and the entire networking
     backlog it covered is done. See this section's top-of-section
     "Backlog-exhaustion note" for what a future session should look at next
     instead.
   - Files: N/A (retired).
   - Verify: N/A (retired).

7. **RETIRED (2026-07-10) — task 7's entire networking-family backlog is now
   fully ported: ClientServerSample (#091), NetworkPrediction (#100), and
   PeerToPeer (#103) are all done.**
   - **What was confirmed, across all three samples now:** all three of
     ClientServerSample's original networking workarounds (DEFERRED.md
     #19/#20/#21) stay gone for every independently-written sample tried against
     them so far, including PeerToPeer's genuinely different peer-to-peer
     broadcast topology (no host authority over simulation at all) — not just a
     coincidence of ClientServerSample's or NetworkPrediction's own particular
     `Update()`-loop shape. See section 3's two newest entries and
     `samples/NetworkPrediction/missing.md`/`samples/PeerToPeer/missing.md` for
     the full live verification of each. `../cna`'s current `develop` HEAD has
     all three fixes; no branch switch was needed for any of the three samples.
   - **DEFERRED.md item #27** (`NetworkSession::SessionProperties` has no mutable
     accessor and no wire replication) was found while porting NetworkPrediction
     and worked around there with a `PacketKind`-byte options packet;
     **PeerToPeer confirmed this gap is genuinely conditional on a sample's own
     use of `SessionProperties`, not universal to the API family** — PeerToPeer's
     C# original never references `SessionProperties` at all, so it needed no
     workaround for this gap and no `PacketKind` byte anywhere in its own
     `Tank.hpp`. Item #27 remains open (only NetworkPrediction's workaround
     exists for it) but is now known to be a narrower, sample-specific gap
     rather than something every `NetworkSession`-based sample will inevitably
     hit.
   - **What's next:** no further sample in this networking family remains
     portable today. NetRumble (#062) is the only other sample anywhere in
     `plan.md` that uses this same `NetworkSession` API family, and it's
     documented (`samples/NetRumble/missing.md`) as blocked on custom HLSL
     shaders (item #11), not on networking — see this section's closing note
     above. (NGSMSample, plan.md #075, is permanently out of scope per
     `ignored.md`: its own "Single Player" path is an intentionally empty stub
     in the original documentation itself, so porting it would only ever
     produce a hollow networking-lobby demo with no real gameplay behind it —
     not a candidate regardless of networking-API readiness.) There is no more
     low-hanging networking-only fruit left; any further work in this specific
     area is either fixing DEFERRED.md item #27 for real (mutable
     `SessionProperties` + wire replication in `cna` itself, effort S/M, see
     that item's own writeup) or waiting on item #11 to unblock NetRumble.
   - Files: N/A (retired).
   - Verify: N/A (retired).

8. **✅ RETIRED (found already fixed, 2026-07-11) — the Vulkan
   multiple-SpriteBatch-per-frame bug.** Discovered while cross-referencing
   `cna`'s recent commit history that this was fixed by `cna` Task 664
   (`VulkanSpriteBatchBackend::Begin()` no longer clobbers the prior batch;
   per-cycle `BatchSnapshot` + a real running cursor) on 2026-07-07 — well
   before this task was ever added to this list, and well before this repo's
   own memory file about it was written. Not independently re-verified live
   this session (EasyGL remains the default backend; would need
   `SDL_VIDEODRIVER=x11` + an explicit Vulkan-backend run to confirm visually),
   but confirmed via direct source read that the fix is present. See the
   project's own `feedback_cna_multiple_spritebatch` memory file (updated
   2026-07-11) for the full account.
   - Files: N/A (retired).
   - Verify: N/A (retired; optional live re-confirmation on the Vulkan backend
     specifically, not required).

9. **✅ FULLY DONE (2026-07-10, user go/no-go): port AccelerometerSample (#084)/
   TiltPerspective (#107).** Both samples are **done** — see section 3 for the
   full account of each and `samples/AccelerometerSample/missing.md`/
   `samples/TiltPerspective/missing.md` for the complete per-sample write-ups.
   - **Correction found while porting AccelerometerSample, then explicitly
     re-checked (not assumed) while porting TiltPerspective — the two samples
     turned out to need genuinely different treatment.** The task was
     originally framed as "invent a keyboard-based tilt substitute, since
     neither original has any non-phone code to reuse." That premise was
     wrong for AccelerometerSample: a direct read of
     `Accelerometer.cs:117-135` found the original's own `GetState()` already
     has a keyboard fallback (the Windows Phone 7 *emulator* branch, gated on
     `DeviceType != DeviceType.Device`, nested inside the same
     `#if WINDOWS_PHONE` block as the real-hardware branch) — ported verbatim
     instead of inventing anything. The premise turned out to be **correct**
     for TiltPerspective, though, once actually re-checked rather than assumed
     either way: `AccelerometerHelper.cs` has no `#if WINDOWS_PHONE` split at
     all, and its own no-hardware fallback is a non-interactive, time-driven
     sinusoidal wobble with no `Keyboard`/`GamePad` reference anywhere in the
     file — genuinely nothing to promote. So TiltPerspective's own keyboard-tilt
     scheme really was invented from scratch (NOXNA), reusing
     AccelerometerSample's/Yacht's own promoted-fallback shape for consistency.
     See DEFERRED.md item #15's correction (updated again this session) and
     both samples' own `missing.md` for the exact code/mapping in each case.
   - Files: `samples/AccelerometerSample/src/` and
     `samples/TiltPerspective/src/` (both done).
   - Verify: both done — see section 3/each sample's `missing.md`.

10. **RETIRED (2026-07-10, user go/no-go): the 5 reopened Avatar samples**
    (#085, #086, #087, #094, #101) **will not be ported.** The user decided the
    substitute-body visual (`AvatarRenderer::EnableRealRenderingEXT`) isn't
    faithful enough to the original Xbox Avatar look to be worth the effort.
    Moved back to `ignored.md`'s permanent list (its "reconsider" note removed —
    this is now a settled decision, not an open question).
   - Files: N/A (retired).
   - Verify: N/A (retired).

11. **✅ `cna`-side half DONE (2026-07-10, Tasks 936/937). ✅ SimpleAnimation
   (#050) ported and live-verified (2026-07-11) — SplitScreen and
   TankOnHeightmap remain NEW, UNSTARTED WORK for a future session, but the
   hard part (deriving correct rest transforms) is already solved and
   reusable.** `ModelMesh::setParentBoneProperty()` plus `ModelTypeReader::Read()`
   building one real `ModelBone` per mesh both landed and were confirmed via
   direct source read — exactly the fix this task asked for. The "Identity
   transform" caveat this task originally flagged turned out to need a
   port-side fix, not an asset regeneration: see
   `samples/SimpleAnimation/missing.md` and DEFERRED.md item #6's own updated
   multi-bone section for the full account — `tank.fbx`'s hierarchy is
   genuinely nested (not flat), so `SimpleAnimation/src/Tank.hpp`'s
   `ApplyRestTransforms()` sets each of the 11 non-root meshes' `ModelBone::Transform`
   directly in C++ to its correct absolute rest offset, composed through the
   real parent chain from `tank.fbx`'s own `Lcl Translation` values (`cna`
   Task 938, regenerating the asset, was NOT needed and remains unstarted/
   optional). Confirmed live via screenshot: tank fully assembled, all parts
   in correct relative position, all 5 animations (wheel spin, steering,
   turret swivel, cannon elevation, hatch) visibly active and stable across
   frames.
   - **What's next:** SplitScreen (#076) and TankOnHeightmap (#074) remain
     unported. Both use the exact same `tank.fbx`/`tank.model.json` (confirmed
     via `md5sum` in TankOnHeightmap's own `missing.md`) — a future session
     should copy/reuse `samples/SimpleAnimation/src/Tank.hpp` (or extract it
     into a shared per-sample copy, per this repo's own "no shared
     `samples/common/` library" rule — copy, don't share) rather than
     re-deriving the rest-transform table from scratch. SplitScreen adds
     dual-viewport split-screen camera/rendering logic on top; TankOnHeightmap
     adds a heightmap terrain (can reuse `HeightmapCollision`'s already-proven
     `Terrain.hpp` pattern).
   - Files: `samples/SplitScreen/src/` (currently only `missing.md` + `.htm` —
     needs a full port), likewise `samples/TankOnHeightmap/`.
   - Verify: build each sample, run under `SDL_VIDEODRIVER=x11`, screenshot,
     confirm independently-posed tank parts (wheels/turret/cannon/hatch) render
     in their correct relative positions, not stacked at the mesh's local origin.

---

## 9. Do not do yet

- **Do not start a custom-`.fx`-shader sample** (BloomSample, DistortionSample,
  NonPhotoRealistic, NormalMapping, PerPixelLighting, VertexLighting,
  ShadowMapping, BillboardSample, InstancedModel, ShatterEffect, Particles3D,
  XmlParticles, ShipGame, NetRumble) without first building an HLSL→GLSL
  `.shader.json` workflow in `cna` (DEFERRED.md item #11) — no tooling exists
  yet. This does **not** apply to the lit-`BasicEffect`-only samples (LensFlare,
  Graphics3D, PickingSample, TrianglePicking, HeightmapCollision,
  InverseKinematics, ChaseCamera, MarbleMaze, and now RimLighting are all ported
  — section 8 task 6 is retired) — none of those needed shader work either,
  `RimLighting`'s own `EnvironmentMapEffect` + `TextureCube` cubemap bypassed
  via direct `SetData()` construction instead (DEFERRED.md item #14 remains
  open but no longer blocks it — see task 6's retirement note).
- **Do not start a skeletal-animation sample** (SkinningSample,
  CustomModelAnimation, SkinnedModelExtensions, CPUSkinning) — **update
  2026-07-11: `AnimationClip`/`Keyframe`/`AnimationPlayer`/`SkinningData`
  classes now exist in `cna` (Task 939/940, landed 2026-07-10), but this does
  NOT unblock these samples yet** — `ModelTypeReader` doesn't parse the new
  skeleton/animation fields, `Model::Draw` isn't wired to consume
  `AnimationPlayer`'s output, and no FBX/X → skeletal-`.model.json` conversion
  tool exists (`cna` Tasks 941-944, all still ⬜). `Content.Load<Model>()`
  cannot produce an animated model today. See DEFERRED.md item #13's updated
  write-up before assuming otherwise.
- **Do not port any of the 5 reopened Avatar samples** — decided (2026-07-10)
  not worth the substitute-body visual gap; permanently back in `ignored.md`.
  (AccelerometerSample/TiltPerspective's keyboard-tilt scheme, by contrast, IS
  approved — see section 8 task 9 — no caveat needed there anymore.)
- **Do not assume a newly-unblocked lighting sample renders correctly just
  because it builds** — screenshot it and check for the near-plane clipping
  artifact (section 4) first. Remember it can now manifest as either a thin
  line *or* full invisibility depending on camera distance (confirmed via
  Graphics3D) — don't assume "renders nothing" must be a different, new bug
  without checking this first.
- **Do not add a shared `samples/common/` library**, even where two samples'
  code looks structurally similar.
- **Do not edit `cna`/`sharp-runtime` source** without confirming scope with the
  user first — they are independently active sibling repos.
- **Do not hammer `xdotool` input** without re-verifying window focus
  immediately beforehand, and do not conclude "no visible effect" means a code
  bug without first checking the sample's own state.
- **No broad refactors or unrelated cleanup** while `ColorWriteChannels` (item
  #22, section 8 task 3) is still open. **Update 2026-07-11: the other bugs
  this bullet used to list are now resolved** — near-plane-clipping (item #26,
  task 2), `ComponentAdded`-timing (item #23, task 4 — turned out not to be a
  bug at all), `Clear(Color)`-depth (item #24, task 5), and the Vulkan
  multi-SpriteBatch bug (task 8) — see each task's own retirement note above.
  Regenerating already-shipped bypass code to use item #26's fix directly
  (section 8 task 2's own new "optional follow-up" note) is still explicitly
  optional/low-priority, not required, and should not be done as an
  unscoped drive-by refactor either.
- **Do not regenerate existing font atlases or `.model.json` assets** unless
  there is a confirmed rendering bug — regenerating is cheap but pointless churn
  otherwise.
- **Do not add a directory under `samples/` for anything listed in
  `ignored.md`** without first removing it from `ignored.md` and adding it to
  `plan.md` properly (only if its exclusion reason has genuinely stopped
  applying).
- **Do not trust a DEFERRED.md blocker without re-verifying it live** if a real
  porting decision depends on it (section 5, "risky assumption").

---

## 10. Resume prompt

```
Read NEXT.md first to understand the current project state.
Then inspect only the files needed for the first task in section 8.
Do not refactor unrelated code and do not start any task not listed in section 8.
Make one small, verified improvement, run the verification command listed for
that task, and confirm it actually passes.
After finishing, update NEXT.md (status, recent changes, next tasks) to match
reality.
```
