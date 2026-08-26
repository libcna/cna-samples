# CNA Samples Master Port and Re-audit Plan

## Authority and scope

This is the only authoritative plan for the sample collection. It merges the former
`cna-samples/PLAN.md` migration list and `cnanext/plans/plan_samples.md` CNA-gap list. Sample
implementation, framework/runtime gaps, original-reference evidence and web delivery are tracked
in one place so that a sample cannot be called complete while a workaround or a hidden CNA bug
remains elsewhere.

The local upstream snapshot is `/rv/tmp/XNAGameStudio/Samples`. A direct inventory on 2026-08-22
found **153 directories after excluding `.idea`**. All 153 have an individual row below. Historical
labels such as `Done`, `Placeholder`, `Ignored`, `permanently out of scope` and earlier owner
declines are evidence to re-check, not current conclusions. Every directory, including all 67
previously ignored entries, receives a fresh source audit.

The large Racing Game remains governed exclusively by [`plan_racing.md`](plan_racing.md). Its
source directory is present in the inventory as `SAMPLE-152`, but this plan must not duplicate,
renumber or modify Racing tasks, `plan_racing.md`, `racing_api_matrix.md` or
`racing_feasibility.md`. **Racing is deliberately scheduled as the final port, after every other
sample row and shared sample infrastructure task has either passed or received an explicit owner
decision.**

## Goal

Port the maximum possible number of original XNA samples to C++ with behavior and structure as
close as practical to the original source, using CNA and sharp-runtime rather than sample-local
substitutes. Every port must work natively and must also produce a tested browser build suitable
for later publication.

The active development dependency chain is temporarily:

| Repository | Required checkout | Current branch at plan creation |
|---|---|---|
| `cna-samples` | this repository | `develop` |
| CNA | `../cnanext` | `next` |
| sharp-runtime | `../sharp-runtimenext` | `next` |

`cna-samples` must set `CNA_SHARP_RUNTIME_ROOT` to `../sharp-runtimenext` before adding
`../cnanext` as a subdirectory. It must no longer build against `../cna` or indirectly select
`../sharp-runtime` during this development campaign.

## Renderer boundary for the sample campaign

EasyGL is the only reference renderer for this campaign:

- Native sample builds use `CNA_GRAPHICS_RENDERER=OPENGLES3`.
- Emscripten sample builds use `CNA_GRAPHICS_RENDERER=WEBGL2`, the browser/WebGL 2 spelling of the
  same EasyGL OpenGL ES 3 implementation. `OPENGLES3` itself is intentionally rejected under
  Emscripten, so `WEBGL2` is not a second renderer scope.
- Do not build, debug, compare or add sample-specific handling for Vulkan, SDL_Renderer, Bgfx,
  WebGPU, desktop OpenGL or any other renderer during this campaign. A CNA/sharp-runtime fix found
  by a sample is implemented cleanly in its owning layer, but sample acceptance and regression
  verification are performed only on `OPENGLES3` plus its `WEBGL2` browser target.

## Sources of truth

Use these in descending order for each audit:

1. The exact original sample under `/rv/tmp/XNAGameStudio/Samples` for source structure, control
   flow, content declarations, assets, controls and visible behavior.
2. A running original XNA build for observable behavior. First rediscover and document the
   previously used direct Linux-side route. If that is unavailable or unreliable, use the local
   Windows 7 VirtualBox VM with Visual Studio 2010 and XNA Game Studio 4.0.
3. `/rv/data/library/github.com/FNA-XNA/FNA` for XNA API and behavioral details not established by
   the sample itself.
4. Local XNA documentation/specification mirrors where runtime observation is impractical.
5. MonoGame ports only as supplemental evidence, never as authority over the original sample or
   FNA.

The machine currently has 268 `.sln` and 359 `.csproj` files under the source snapshot. The VM
definition exists at `/home/robertvokac/VirtualBox VMs/win7/win7.vbox`, but on 2026-08-22
`VBoxManage` reported that `/dev/vboxdrv` was unavailable. This is an environment prerequisite to
resolve, not permission to skip original-reference validation.

## No-workaround and fidelity policy

The following rules apply to existing ports as strongly as to new ports:

- Compare every original C# source file and relevant content declaration line by line with the C++
  port. Preserve class/file decomposition, names, lifecycle, algorithms, constants, default values,
  input mapping and draw/update order wherever C++ permits.
- Use the public XNA-shaped CNA API. Do not call backend helpers, construct replacement runtime
  objects to bypass `Content.Load<T>()`, use raw-mesh loaders because `Model` is broken, add sidecar
  data because buffer readback is missing, or replace a missing property/event/network behavior in
  sample code.
- Do not omit or simplify scenes, screens, gameplay, effects, audio, networking, content processing
  or controls. Do not invent keyboard controls, substitute bodies/services, fake data or a reduced
  demonstration and then call the sample ported.
- Remove legacy sample-side workarounds as their rows are audited. A workaround previously approved
  for expediency is not grandfathered into the new completion definition.
- The existing mandatory F1 help overlay is not part of the original XNA samples. Remove its code
  and porting requirement from every audited sample. Preserve only the historical `help.png` by
  moving it out of `Content` into the sample root beside `CMakeLists.txt`; it must not be loaded,
  copied or preloaded by the sample. Apply the same asset-preservation rule to future audits.
- Prefer `System::*` types and primitive aliases from `sharp-runtimenext` when the C# source uses a
  .NET concept. Do not replace missing runtime functionality with an unrelated STL implementation
  merely to make a sample compile.
- Unavoidable C#-to-C++ mechanics such as ownership/RAII and a lossless offline asset-container
  conversion are allowed. Asset conversion is not a license to change behavior: the port must keep
  the same content identifiers and use the corresponding public CNA load call. Converted output
  must be reproducible from the original source asset.
- `missing.md` is an audit record, not a waiver. A sample with an active behavior difference,
  missing feature or workaround cannot be `✅`.

When the faithful C++ translation exposes a missing or incorrect API, fix the owning repository in
the **same sample session**:

| Gap belongs to | Required action |
|---|---|
| XNA/FNA behavior or API | Implement/fix it in `../cnanext`, add the required CNA tests and follow `../cnanext/AGENTS.md` plus `CHECKLIST.md`. |
| .NET `System.*` behavior or primitive/runtime type | Implement/fix it in `../sharp-runtimenext`, add its required tests and follow that repository's `AGENTS.md`. |
| Sample translation/content | Fix it in `cna-samples`; do not hide a framework/runtime gap here. |

One session may therefore change all three repositories. Use the same sample task ID in their
commit messages and record the resulting commit hashes in the sample row or its audit report.

## Definition of done for one sample

A row may become `✅` only after all applicable gates pass:

1. **Classification:** inspect the physical upstream directory and record whether it is a runnable
   XNA game, tool, library, training sequence, platform variant, asset pack or documentation. Never
   accept the old classification without reopening the files.
2. **Original evidence:** select the correct original project/configuration, build and run it when
   runnable, record exact commands/environment, controls, screenshots or frame captures, audio and
   important state transitions. If it truly cannot execute, record file-level evidence and the
   exact external dependency.
3. **Complete translation:** audit every C# source file and relevant content item. Port the complete
   selected original, including all overload use, screens and optional branches exercised by that
   project.
4. **Zero workarounds:** run a targeted review for bypasses, substitutions, invented behavior and
   stale `missing.md` deviations. Fix CNA or sharp-runtime gaps in their own repositories.
5. **Native verification:** configure/build the sample against `../cnanext` +
   `../sharp-runtimenext` with `CNA_GRAPHICS_RENDERER=OPENGLES3`, run it, and compare it with the
   original reference. Build and run relevant CNA/sharp-runtime tests for every dependency change
   on that renderer only.
6. **Web verification:** produce `.html`, `.js`, `.wasm` and any `.data`/content artifacts with
   Emscripten; serve them over HTTP; run in a real browser; verify rendering, input, audio/content
   loading and a representative interaction path. A successful link or Node-only run is not enough.
7. **Documentation and status:** update the sample's `missing.md` with either `No known differences`
   plus evidence or the still-open differences; update this row and any historical gap documents;
   commit all touched repositories by explicit file list.

Every ported sample must keep its web bundle reproducible. The eventual publication location is not
part of this task, but the generated bundle and instructions must be suitable for copying to the
owner's website without source edits.

All original-reference, native, web and comparison artifacts produced by this campaign are retained
under `/rv/tmp/samples/SAMPLE-nnn-UpstreamDirectory/`, never in an ad-hoc `/tmp` directory. Use the
following stable layout so every completed audit can be reopened and rebuilt:

| Directory | Contents |
|---|---|
| `xna4-original/` | The exact original source/configuration used for comparison and its compiled executable. |
| `cna-native-opengles3/` | Reusable CMake build tree and resulting native EasyGL/OPENGLES3 executable. |
| `cna-web-webgl2/` | Reusable Emscripten CMake build tree and complete `.html`/`.js`/`.wasm`/`.data` bundle. |
| `evidence/` | Original/native/web screenshots, captures and relevant run logs, including failed-before-fix evidence when useful. |

Per-sample `missing.md` files must record their exact artifact root and build/output paths. Browser
profiles and shared CNA regression build trees are temporary infrastructure rather than sample
artifacts and are not copied into each sample directory.

Use up to six parallel CPU jobs for future sample and dependency builds (`-j6` and the equivalent
vendored-build limit) when the selected build tool supports it.

## Status legend

| Symbol | Meaning |
|---|---|
| ⬜ | Fresh audit not started; historical status is untrusted. |
| 🔎 | Source/reference audit in progress. |
| 🛠 | Port or cross-repository fix in progress. |
| 🛑 | Audited far enough to require an owner decision before a large new subsystem or scope expansion. |
| ✅ | All native, fidelity, no-workaround and real-browser gates passed. |
| ↗ | Managed by a separate owner-approved plan; do not modify it here. |

## Baseline artifact inventory

This describes only what exists before the fresh audit; it is not a completion count.

| Existing artifact state | Count |
|---|---:|
| Has sample source and `CMakeLists.txt` | 63 |
| Placeholder directory without source/build target | 23 |
| No `samples/` directory | 67 |
| **Total upstream directories** | **153** |

## Foundation tasks

| Task | Status | Work |
|---|---|---|
| SAMPLES-INFRA-001 | ✅ | Merge the two former plans into this lowercase `plan.md`, move all 153 upstream directories into individual rows and remove the duplicate plan from `cnanext`. |
| SAMPLES-INFRA-002 | ✅ | Changed the development build to `add_subdirectory(../cnanext CNA_BUILD)` with `CNA_SHARP_RUNTIME_ROOT=../sharp-runtimenext`, removed the monolithic-target and obsolete-renderer assumptions, restricted native presets to `OPENGLES3`, and proved the configuration with `SAMPLE-001`. |
| SAMPLES-INFRA-003 | ⬜ | Reconcile `README.md`, `CLAUDE.md`, `NEXT.md`, `DEFERRED.md`, `ignored.md`, root `missing.md` and per-sample guidance with this plan: new dependency paths, no permanent-ignore authority, no F1 requirement, no workaround acceptance and lowercase plan links. Preserve useful historical evidence. |
| SAMPLES-INFRA-004 | ⬜ | Add an inventory validator that compares the 153 physical upstream directories with exactly 153 unique `SAMPLE-nnn` rows and reports added, removed, renamed or duplicate sources. |
| SAMPLES-INFRA-005 | 🛠 | Proved the direct Linux-side XNA 4.0 compiler route and the official `BuildContent` task under an isolated Wine prefix. `SAMPLE-003` covers the stock FBX, texture, font and effect pipeline; `SAMPLE-007` additionally covers a sample-owned processor assembly, reflective custom runtime type and live original-window capture; `SAMPLE-010` and `SAMPLE-011` prove that the Miramonte and Segoe UI Mono faces distributed inside XNA Game Studio can be installed into the isolated prefix for exact original font builds. Still extract the reusable workflow, Win7/VS2010 VM fallback and shared capture checklist from the per-sample evidence. |
| SAMPLES-INFRA-006 | 🛠 | Qualified `CNA_GRAPHICS_RENDERER=WEBGL2` with `SAMPLE-001`, `SAMPLE-003`, all seven `SAMPLE-005` screens, all five original `SAMPLE-006` sprite-effect modes, `SAMPLE-007`'s official custom-processor atlas, `SAMPLE-008`'s BasicEffect line primitives, `SAMPLE-009`'s complete standard-gamepad report, all nine `SAMPLE-010` buffered moves and `SAMPLE-011`'s 1280x720 title-safe/camera behavior in real Chrome. These gates cover compiled effects, models, fonts, cubemaps, skinning, signed textures, secondary samplers, custom XNB object graphs, line primitives, input and a large browser canvas. Still add reusable local-HTTP/browser smoke automation before making this the shared per-sample web gate. Do not test or claim another renderer. |
| SAMPLES-INFRA-007 | ⬜ | Add a per-sample audit template and mechanical scans for known bypass patterns (`RawMesh`/`RawModel`, direct `SetData` content substitutes, NOXNA graphics helpers, sidecars, invented input, omitted/simplified branches). Scans support, but do not replace, line-by-line review. |

## Owner decision queue — ask when the owner is at the computer

Auditing and small fixes continue without asking. When one of these is confirmed by a concrete
sample, stop before implementing the large subsystem, mark the affected row `🛑`, present measured
scope/options and ask the owner. Do not silently reintroduce a sample workaround.

| Decision | Current evidence to verify | Owner choice needed |
|---|---|---|
| SAMPLES-DEC-001 — Native/custom `.fx` path | `SAMPLE-003` proves that an official XNA 4.0 Effect XNB with 13 techniques loads and renders through the compiled-effect path on OPENGLES3 and WEBGL2. `SAMPLE-006` adds four original pixel-only effects, parameter updates, secondary sampler slots and a signed normal map. Neither audit required hand translation. Audit later shader samples for constructs outside this proven subset before escalating a large missing subsystem. | If a concrete later effect remains unsupported after retest, choose a reusable faithful ingestion/translation strategy versus the exact bounded alternative supported by evidence. Do not hand-wire effect behavior inside each sample. |
| SAMPLES-DEC-002 — XNA content-pipeline extensibility | `SAMPLE-004` confirmed the first concrete case: it has no `Game`, but a 369-line `CompileEffect` CLI using `EffectImporter`/`EffectProcessor` plus a 2,680-line six-effect wrapper library and 2,013 lines of HLSL. The unchanged original source builds and generates all six blobs. CNA OPENGLES3 parses the exact blobs and renders the BasicEffect blob, but CNA/sharp-runtime have no design-time Content Pipeline authoring API or effect-source compiler. `SAMPLE-012` additionally proves an unchanged sample-owned processor assembly can generate exact runtime content outside CNA. Evidence: `samples/StockEffects/missing.md` and `samples/GeneratedGeometry/missing.md`. | Resolved for `SAMPLE-004` on 2026-08-23: the owner selected the evidence-backed non-game/non-port boundary (option 1), with no alias, invented game or web demo. On 2026-08-24 the owner accepted byte-identical pregenerated XNB output as the faithful boundary for runtime samples such as `SAMPLE-012`; this does not declare a standalone design-time tool/library ported. Keep this decision open only for later rows that concretely require Content Pipeline authoring. |
| SAMPLES-DEC-003 — Skeletal/animation completeness | Four placeholders historically cite partial skeletal animation support. `SAMPLE-005` retested the current stack and restored `ReachGraphicsDemo::SkinnedDemo` with official XNB content through bounded reader/Tag fixes, so it no longer supports the old omission claim. | After each remaining placeholder is retested, approve any genuinely large model/content/animation work if it is more than a bounded bug fix. |
| SAMPLES-DEC-004 — Retired platform services and hardware | `SAMPLE-015` is the first measured case: its defining behavior is a 976-line WP7 XNA client plus a 547-line generated WCF proxy and an 818-line WCF service/host. The client registers an MPNS callback URI, sends one-way service calls and receives authoritative game state through XML push messages. The unchanged service builds on Mono; the current reference prefix lacks the WP7 SDK/phone assemblies, while CNA/sharp-runtime lack WCF and `HttpNotificationChannel`. Evidence: `samples/TicTacToe/missing.md`. | Resolved for `SAMPLE-015` on 2026-08-24: the owner accepted an evidence-backed CNA non-port boundary and declined WCF/MPNS emulation. The existing local game is retained only as an explicitly labeled free reimplementation, never as an XNA port. Later Avatar/Xbox LIVE, Bing Maps, invites/presence, GPS and WP7 media cases still require their own audits and decisions. |
| SAMPLES-DEC-005 — Tools, WinForms, Silverlight, older XNA and duplicate variants | Forty-two directories were previously grouped away, and several numbered entries are tools/libraries rather than games. They now receive individual audits. | Decide after audit whether the project expands beyond XNA 4.0 games to faithful tool/library/older-version ports; never reject them merely because the old plan did. |
| SAMPLES-DEC-006 — Web feature gaps | Every successful port now requires a real browser build. XACT, microphone/network services, large content, persistent storage or a WebGL2 limitation may expose substantial work. | Choose the browser support boundary or authorize the measured CNA/sharp-runtime work; native-only completion is not allowed by default. |
| SAMPLES-DEC-007 — Original-reference environment | The local source has hundreds of projects and prior real-XNA evidence, but the earlier Linux route is not documented and the Win7 VM driver is currently unavailable. | Confirm the preferred reference route and any host/VM changes once the discovery task reports concrete options. |

## Per-directory audit queue

`Existing` means only the artifact state observed on 2026-08-22: `port` has a sample
`CMakeLists.txt`, `placeholder` has a directory but no implementation, and `absent` has no sample
directory. `Focus` preserves useful findings from the old plans without accepting their verdicts.

### SAMPLE-001–030 — foundation, 2D and gameplay

| Task | Upstream directory | Existing | Fresh audit focus | Status |
|---|---|---|---|---|
| SAMPLE-001 | `PrimitivesSample_4_0` | port | Complete Windows XNA source/runtime comparison, zero-workaround review, OPENGLES3 native run and WEBGL2 Chrome run. Removed the non-original F1 overlay; fixed one-pixel primitive rasterization in CNA `76f1f6ebe` and the faithful first-frame gamepad query's Linux startup delay in CNA `5b9287a41`. Default Wine/DXVK exits during original-XNA startup; the retained reference wrapper uses verified WineD3D. Evidence: `samples/PrimitivesSample/missing.md`. | ✅ |
| SAMPLE-002 | `Primitives3DSample_4_0` | port | Audited against the live XNA 4.0 original; restored the exact Position+Normal vertex, default lighting, HUD, input and disposal paths; fixed CNA generic custom-vertex upload/EasyGL declaration selection. Native OPENGLES3 and real-Chrome WEBGL2 rendering plus A/B/Y/Escape input gates pass. Evidence: `samples/Primitives3D/missing.md`. | ✅ |
| SAMPLE-003 | `TexturesAndColorsSample_4_0` | port | Audited line by line against the XNA 4.0 C# source; replaced every primitive/BasicEffect/help-overlay workaround with the five original models, Clouds texture, DebugText font and official 13-technique compiled effect. All eight XNBs are byte-identical outputs of the official XNA pipeline. Fixed CNA startup XNB-reader registration and its public Emscripten stack contract. OPENGLES3 native and real-Chrome WEBGL2 runs render all 13 techniques and five models and accept Escape. Evidence: `samples/TexturesAndColors/missing.md`. | ✅ |
| SAMPLE-004 | `StockEffectsSample_4_0` | absent | Confirmed as a non-game authoring package: a Content Pipeline effect-compiler CLI plus six educational stock-effect wrappers. Original compiler/library builds pass; all exact blobs parse in CNA and BasicEffect draws on OPENGLES3. On 2026-08-23 the owner selected the evidence-backed non-port boundary: retain the original build, do not alias CNA built-ins, and do not invent native/web behavior that upstream lacks. Evidence: `samples/StockEffects/missing.md`; decision: `SAMPLES-DEC-002`. | ✅ |
| SAMPLE-005 | `ReachGraphicsDemo_4_0` | port | Line-by-line audit complete; all six demos plus title restored with the 22 official XNA-pipeline XNBs and no sample workaround. Fixed CNA base `TextureReader` registration, custom Model Tag ownership, EasyGL semantic vertex binding/VAO lifetime and GLES/WebGL base-vertex fallback. Original and OPENGLES3 native runs pass; system Chrome renders all seven WEBGL2 screens with working menu/back input, WebGL error 0 and no wasm exception. Evidence: `samples/ReachGraphicsDemo/missing.md`. | ✅ |
| SAMPLE-006 | `SpriteEffectsSample_4_0` | port | Audited line by line against the live XNA 4.0 original; removed the handwritten GLSL/JSON effects, direct image substitutes and F1 overlay, and restored the eight byte-identical official-pipeline XNBs. Fixed CNA `NormalizedByte4` XNB/upload support, stock-vertex-shader inheritance for pixel-only SpriteBatch effects and unassigned secondary texture slots. All five animated modes pass in the original, native OPENGLES3 and real-Chrome WEBGL2 with Space/Escape input. Evidence: `samples/SpriteEffects/missing.md`. | ✅ |
| SAMPLE-007 | `SpriteSheetSample_4_0` | port | Audited against the live XNA 4.0 game, runtime type and custom processor; removed runtime repacking, loose images, omitted text and F1 overlay, then restored the three byte-identical official-pipeline XNBs. Fixed CNA's XNB reference dispatch for `System.String` collections, nullable `SpriteBatch.Begin` blend-state defaults and the Linux vendored-SDL cache key that had stranded existing builds on X11-only SDL. The live original, native-Wayland OPENGLES3 and real-Chrome WEBGL2 versions render both labels, rotating cat, animated glow, checkerboard and the exact 198x264 atlas; Escape passes and no sample workaround remains. Evidence: `samples/SpriteSheet/missing.md`. | ✅ |
| SAMPLE-008 | `ShapeRenderingSample_4_0` | port | Audited line by line against the live Debug XNA 4.0 original; removed the F1 overlay, restored the exact game/renderer names, overloads, one-time guard, shared buffers, input and Debug-only call-site semantics. The original Wine/WineD3D, native Wayland OPENGLES3 and system-Chrome WEBGL2 versions render the same five colored line shapes on the rotating camera and accept Escape. No CNA/sharp-runtime defect or sample workaround remains. Evidence: `samples/ShapeRendering/missing.md`. | ✅ |
| SAMPLE-009 | `InputReporter_4_0` | port | Audited line by line against the live XNA 4.0 original; removed the DejaVu font sidecars, loose texture substitutes and F1 overlay, then restored all 15 byte-identical official-pipeline XNBs and the exact C# control/draw flow. Fixed CNA's XNA backslash and case-insensitive content-path behavior generally. XNA and native OPENGLES3 baseline/Circular captures are pixel-identical; real-Chrome WEBGL2 verifies the complete browser gamepad mapping, two-second START/BACK charge switches, Space/Escape and faithful rendering with no runtime error. Evidence: `samples/InputReporter/missing.md`. | ✅ |
| SAMPLE-010 | `InputSequenceSample_4_0` | port | Audited line by line against the live XNA 4.0 original; removed the loose PNGs, DejaVu font sidecars and F1 overlay, restored all 15 byte-identical official-pipeline XNBs including Miramonte Bold, and corrected enum labels, stable longest-first shallow move ordering, logical buffer capacity and original type names. The original and native OPENGLES3 captures are byte-identical for the baseline and all nine moves; system-Chrome WEBGL2 reproduces every move with pixel-identical game rendering and no runtime error. No CNA/sharp-runtime defect or sample workaround remains. Evidence: `samples/InputSequence/missing.md`. | ✅ |
| SAMPLE-011 | `SafeAreaSample_4_0` | port | Audited line by line against the live Windows XNA 4.0 original; removed the loose texture/font substitutes and F1 overlay, restored the three byte-identical official-pipeline XNBs, and retained the original `XBOX && DEBUG` component branch instead of an always-on overlay. The original, native OPENGLES3 and system-Chrome WEBGL2 versions render the same 1280x720 title-safe labels, tiled background, cat and scrolling camera; A is correctly inert on current reference targets and Escape exits. The WEBGL2 baseline is pixel-identical to XNA, and no CNA/sharp-runtime defect or workaround remains. Evidence: `samples/SafeArea/missing.md`. | ✅ |
| SAMPLE-012 | `GeneratedGeometrySample_4_0` | port | Audited line by line against the live XNA 4.0 game and both custom processors; removed runtime geometry, loose BMP, F1-overlay and `CullNone` workarounds, restored the three byte-identical official-pipeline XNBs, and retained the original inactive `WINDOWS_PHONE` timing/fullscreen branch. A closed AOT reader loads the original reflective `GeneratedGeometry.Sky` object (processor-built Model + Texture2D), while terrain uses CNA's stock ModelReader and external texture path. The owner accepted exact pregenerated XNB output as the faithful runtime boundary. The real XNA original, native OPENGLES3 and system-Chrome WEBGL2 builds render the same animated lit/fogged terrain and far-plane skydome at 800x480 and no CNA/sharp-runtime gap remains. Evidence: `samples/GeneratedGeometry/missing.md`; decision: `SAMPLES-DEC-002`. | ✅ |
| SAMPLE-013 | `Platformer_4_0` | port | Complete XNA 4.0 source/content audit: restored every gameplay, validation, touch/accelerometer and target branch; replaced loose substitutes with faithful XNB/audio products; removed the ContentManager workaround after the general CNA fix; real XNA, OPENGLES3 and system-Chrome WEBGL2 runs match. Evidence: `samples/Platformer/missing.md`. | ✅ |
| SAMPLE-014 | `Spacewar_4_0` | port | Complete source/content audit against the live XNA 4.0 Windows original: ported both Retro and Evolved modes plus every screen, upgrade/victory path, scene, effect, model, cubemap, render-target and XACT path. All 157 checked-in runtime assets are byte-identical official-pipeline/XactBld output; no raw-model, loose-content, shader, input or help-overlay workaround remains. General CNA repairs cover XNB/cache/reference semantics, case-insensitive content, XACT banks including FACT-faithful complex-track cursor progression, and faithful EasyGL compiled-effect null/cube sampler behavior. Native OPENGLES3 and system-Chrome WEBGL2 builds render and play through original inputs without runtime errors; isolated and real-device native checks confirm the original title music. Evidence: `samples/Spacewar/missing.md`. | ✅ |
| SAMPLE-015 | `TicTacToe_4_0` | port | Complete directory audit established that upstream has two separately deployed parts—a WP7 XNA client and a WCF/MPNS server—not a standalone local game. The unchanged server builds; the current reference lacks the WP7 SDK for the client, and MPNS is retired. On 2026-08-24 the owner declined WCF/MPNS emulation and accepted an evidence-backed CNA non-port conclusion. `samples/TicTacToe` remains only as a prominently labeled free reimplementation and is a port of neither original part. Its OPENGLES3 native and system-Chrome WEBGL2 builds under `/rv/samples` render and accept input, but are explicitly not original-parity claims. Evidence: `samples/TicTacToe/missing.md`; decision: `SAMPLES-DEC-004`. | ✅ |
| SAMPLE-016 | `BounceSample_4_0` | port | Complete line-by-line audit of the WP7-only original restored the exact 100-sphere physics, accelerometer/emulator input, orientation handling, fullscreen 30 Hz settings, procedural Position/Normal geometry, lighting, shadows and original update quirks. Removed the invented tilt/help overlays, persistent keyboard tilt, alternate vertex format, reordered collision logic and sample-local light workaround. General CNA fixes restore the shipped XNA 4.0 DirectionalLight defaults and faithful deferred browser fullscreen. Native OPENGLES3 and system-Chrome WEBGL2 Release builds render and respond without runtime errors; the original cannot be rebuilt on this host because its XNA/WP7 targets and the VM driver are unavailable, so original evidence is source/project/document/asset based. Evidence: `samples/Bounce/missing.md`. | ✅ |
| SAMPLE-017 | `CollisionSample_4_0` | port | Complete source/content/test audit restored the FPS font, gesture/phone branches, original type/method surfaces, disposal and named colors with no help/workaround path. The unchanged XNA game and its 420,000-case test suite run; the faithful C++ test port also passes all 420,000 cases. A general CNA fix restores the empty XNA `ContentManager(IServiceProvider)` root while preserving the CNAEXT convenience default. Original XNA, native OPENGLES3 and system-Chrome WEBGL2 captures cover all five groups plus orthographic projection and Escape; no runtime error remains. Evidence: `samples/CollisionSample/missing.md`. | ✅ |
| SAMPLE-018 | `PerPixelCollisionSample_4_0` | port | Audited the whole physical directory against a live XNA 4.0 build of the unchanged Windows project. Removed the loose colour-keyed PNGs, the invented F1 help overlay and the RGBA literals; restored the two byte-identical official-pipeline XNBs, `Color::Red`/`CornflowerBlue`/`White`, the original type/member surface and the reference build's own safe-area arithmetic. Two general CNA fixes: `Color` gained the default constructor every XNA value type has (`new Color[n]`), and `SpriteBatch` stopped quantising sprite destinations to whole pixels -- XNA/FNA keep them in floats, so the falling blocks now land between pixels and their edges filter exactly as the original's do (0 blended pixels before the fix, 8475 after, against the original's 9384 over the same 60 frames). Person position is identical to XNA at the start and both clamps in the original, native OPENGLES3 and system-Chrome WEBGL2 builds, and the sprite is byte-identical there; recorded runs show real per-pixel hits plus rectangle overlaps that correctly do not hit. Evidence: `samples/PerPixelCollision/missing.md`. | ✅ |
| SAMPLE-019 | `RectangleCollisionSample_4_0` | port | Audited the whole physical directory against a live XNA 4.0 build of the unchanged Windows project. The old port drew both sprites as solid magenta squares -- its PNGs were raw BMP conversions with no colour key at all -- so they were replaced by this sample's own byte-identical official-pipeline XNBs, proven by running its own content project rather than copying SAMPLE-018's. Also removed the invented F1 overlay and the RGBA literals, and re-derived the reference build's extended-precision safe-area arithmetic. `Rectangle::Intersects` is already character-for-character FNA's, so no CNA or sharp-runtime change was needed. Person position matches XNA at the start and both clamps in the original, native OPENGLES3 and system-Chrome WEBGL2, with a byte-identical sprite; and 180 s recordings confirm this tutorial's own behaviour -- every unambiguous rectangle overlap turns the background red even when the drawn pixels are clear of each other, the exact opposite of SAMPLE-018. Evidence: `samples/RectangleCollision/missing.md`. | ✅ |
| SAMPLE-020 | `TransformedCollisionSample_4_0` | port | Audited the whole physical directory — **both** upstream solutions — against live XNA 4.0 builds of the unchanged Windows projects. The primary game gained `Block.hpp`, both `IntersectPixels` overloads and `CalculateBoundingRectangle`, its three byte-identical official-pipeline XNBs (the loose PNGs and the invented F1 overlay are gone) and the reference build's extended-precision safe-area arithmetic. The sample's **second** product, `TransformedCollisionTest`, was ported for the first time as its own sample directory — it has its own solution, `Program.cs` and content project, so unlike SAMPLE-017's unit tests it cannot be a second target beside the first. One sharp-runtime fix: `List<T>` could not be instantiated at all for an element type without `operator==`, because `Contains`/`IndexOf`/`Remove` are overrides; `findValue` now refuses at the call with `NotSupportedException` instead of at instantiation, as C#'s `EqualityComparer<T>.Default` has no such restriction (7 tests; 17847/17847). `Vector2` gained the CNAEXT `+=`/`-=` pair. Person position is identical to XNA at the start and both clamps in the original, native OPENGLES3 and system-Chrome WEBGL2 (343/40/727 at y=399), and 180 s recordings show ~50 frames per run where the rotated spinner's arm sits well inside the person's rectangle with the background still blue. The mouse-driven second product takes identical input in all three builds and turns red at exactly the same nine of seventeen drag positions in every one of them. Evidence: `samples/TransformedCollision/missing.md`, `samples/TransformedCollisionTest/missing.md`. | ✅ |
| SAMPLE-021 | `PathDrawing_4_0` | port | Audited the whole physical directory against a live XNA 4.0 build of the unchanged sources. Upstream is Windows-Phone-only, but unlike SAMPLE-016 it builds and runs here: everything it uses exists in the desktop profile, so the original's four unmodified files link into a Windows executable with only the entry point the WP7 targets generate. Removed all five of the previous port's documented deviations -- the invented mouse controls (CNA has the whole `TouchPanel` surface), the omitted `SpriteFont`/`DrawString`, the manual ground tiling, the reversed tank/path draw order and the dropped 30 Hz + fullscreen settings; four of its five stated root causes were Vulkan claims that do not hold on EasyGL. Content is this sample's own official-pipeline output built from the real Segoe UI Mono, shipped as the **WindowsPhone** XNBs the only upstream platform produces -- the desktop build differs solely in the container platform tag and the Silverlight mscorlib named by the SpriteFont's reader. The faithful translation needed **no framework change at all**, and the native OPENGLES3 frame is byte-identical to the XNA original's, 384000 of 384000 pixels. No host touch source exists in either build (measured: Wine gives the original no `WM_TOUCH`, SDL3's X11 backend synthesises no fingers even with its own hint set), so at the owner's request `TouchPanel` gained a CNAEXT opt-in that reports the left mouse button as a touch -- off by default, routed through the same entry points a real finger uses, ten tests, and enabled by one marked line here so a touch-only sample is playable with a pointer; the deviation is recorded in `samples/PathDrawing/diff.md`. Interaction is verified both ways: the tank drives 631.6 px along a path drawn with real touch events in system Chrome, and (102,109) -> (650,439) along one drawn with the mouse natively. Also fixed a Paeth bug in the browser harness's PNG decoder that had travelled since SAMPLE-018 and corrected SAMPLE-020's record accordingly. Evidence: `samples/PathDrawing/missing.md`. | ✅ |
| SAMPLE-022 | `Pathfinding_4_0` | port | Audited all 42 upstream files -- two solutions and three projects -- against a live XNA 4.0 build of the unchanged Windows configuration. Removed all five of the previous port's documented deviations: the omitted HUD text, the omitted touch path, the **hand-written XML parser** standing in for `Content.Load<MapData>()`, the six SpriteBatch passes merged into one on a Vulkan claim that does not hold on EasyGL, and the RGBA literals replacing named colours. The `PathfindingData` class library is a third case beyond SAMPLE-004's non-port and SAMPLE-020's second product -- a runtime type the game cannot run without -- so the **type** is ported and the project file is not, recorded explicitly. The four maps now load from official XNBs through a closed AOT reader for `ReflectiveReader<PathfindingData.MapData>`, whose layout was pinned by decoding the container: a reflective value-type field is inlined with no reader index, so the two Points are read raw and only the barrier list carries one. All 13 XNBs are byte-identical to this sample's own pipeline output, including the campaign's first `.tga` assets. One sharp-runtime fix: `Dictionary<TKey,_>` now accepts a key that carries `GetHashCode()` without a `std::hash` specialization -- narrowly, so an existing selector contract test still stands (6 tests; 17853/17853). Native frames are byte-identical to XNA's at 384000/384000 on the start, mid-search and second-map frames; the three later frames differ only inside one box containing the moving tank, with 0 differing pixels outside it. Evidence: `samples/Pathfinding/missing.md`. | ✅ |
| SAMPLE-023 | `WaypointSample_4_0` | port | Audited the whole physical directory against a live XNA 4.0 build of the unchanged Windows configuration. This is the sample SAMPLE-021 and SAMPLE-022 both borrowed `Tank.cs` and `WaypointList.cs` from, so all three were diffed first: 192/262 differing lines in `Tank.cs` and 82/94 in `WaypointList.cs`, three different files, nothing copied. Only this version's `Tank` is a real `DrawableGameComponent` added to `Game.Components`, and only this one has the `Behaviors/` hierarchy the later two dropped -- all of it restored, as `.hpp`/`.cpp` pairs mirroring the original decomposition (the SAMPLE-013/014 arrangement, which is what lets `Behavior` and `Tank` reference each other). The old port had no font and no `blank` asset at all, so the HUD line and the four-line help text were simply absent, and it carried a `TankBehaviorImpl.hpp` with no upstream counterpart; all 5 XNBs are now byte-identical to this sample's own pipeline output. **No framework change was needed.** The start frame is byte-identical to XNA's at 409440/409440, and every other frame differs only inside one box containing the time-integrated cursor, with 0 differing pixels outside it. The timing-independent HUD band is byte-identical in all six frames, and cycling the behavior with B changes exactly the same 188 pixels in the original, the native port and the browser. Evidence: `samples/WaypointSample/missing.md`. | ✅ |
| SAMPLE-024 | `FlockingSample_4_0` | port | The row's flag resolved: the old port had **deliberately not reproduced** an upstream defect -- `Bird.Update` applies the Y component of movement twice, so birds drift about twice as fast vertically -- and had ported the "evidently-intended" single application instead. The rule is not ambiguous and the owner confirmed it: if the XNA 4.0 original has bugs, CNA has them too. The quirk is reproduced line for line, and so are two more the old record never mentioned -- `Flock.FlockParams`'s getter returns the property rather than the field (infinite recursion, never called) and `SliderInputHelper` measures the separation slider from the detection bar's X. All five other documented deviations are gone: the entire HUD (both sliders, both labels, the pulsing red tint, three button glyphs and their text) was simply absent because the port had no font and no glyph assets at all; the touch path, the analog-trigger slider fallback and `TitleSafeArea` bounds are restored. All 6 XNBs byte-identical, four from `.tga`. One cnanext fix: `Vector2` gained `*=` and `/=`, the same C#-synthesised-operator gap SAMPLE-020 found for `+=` (6 tests). The flock is randomly seeded and the label pulses on total game time, so no whole frame can match; the slider bars are drawn after the flock and are byte-identical in 4 of 5 frames, and the exactly-orange pixel count is 800 in the original, the native port and the browser in every frame. Evidence: `samples/FlockingSample/missing.md`. | ✅ |
| SAMPLE-025 | `ChaseAndEvadeSample_4_0` | port | Audited the whole physical directory against a live XNA 4.0 build of the unchanged Windows configuration. The old port was 258 lines against the original's 717 -- a paraphrase. Both documented deviations are gone: the HUD state lines are restored, each drawn twice for its black drop shadow exactly as upstream does, and all three `#if WINDOWS_PHONE` regions are back, the campaign requiring inactive platform branches to be preserved rather than dropped as "out of scope". Neither the old port nor its record mentioned that **the mouse pointer controls the cat in the original**, `smoothStop` easing and all; that is upstream behaviour and is translated. One upstream quirk kept in place: a local in `HandleInput` shadows the field holding the mouse sprite's position. All 4 XNBs byte-identical, three from `.tga`, and the port loads `"Tank"` against an asset named `tank` through CNA's case-insensitive resolution. **No framework change was needed.** Both characters wander on a time-seeded `Random`, so no whole frame can match; the static half of the two HUD lines hashes `2ccb55c0` in the original, the native port and the browser alike, and on the frames where both builds were in the same AI state the state words are byte-identical too. Evidence: `samples/ChaseAndEvade/missing.md`. | ✅ |
| SAMPLE-026 | `AimingSample_4_0` | port | Audited the whole physical directory against a live XNA 4.0 build of the unchanged Windows configuration. The old port was 174 lines against the original's 357 and carried four deviations, all now gone. The largest was the **omitted additive spotlight pass**: its own record admitted a faithful two-pass port "would render correctly on EasyGL" and kept the single-block workaround "for Vulkan-backend safety" -- EasyGL is this campaign's only renderer. Restored, along with all three `#if WINDOWS_PHONE` regions, the viewport X/Y offsets the original reads in five places, and the official `.tga`-sourced XNB in place of a hand-converted PNG (the record claimed CNA "does not support .xnb/TGA source assets", stale since SAMPLE-022). `TurnToFace`/`WrapAngle` were diffed against SAMPLE-025 first, as the handoff required: identical code, only the comment diagram differs. **No framework change was needed.** Nothing in this sample is random, so it is the first of six where whole frames match: `start` and `settled` are byte-identical at 409440/409440 and every pixel of all five frames is within 2. The additive pass is measured, not assumed -- the game clears to black, so lit-pixel counts are countable, and they are exactly equal to the original's in all four frames (63373 / 40050 / 61084 / 61148). Evidence: `samples/AimingSample/missing.md`. | ✅ |
| SAMPLE-027 | `FuzzyLogicSample_4_0` | port | Full original/native/web parity. Whole sample re-ported from the 1298 unchanged C# lines as `.hpp`/`.cpp` pairs (the old port was 518 lines in one header); the four claimed omissions -- SpriteFont labels, touch input, gamepad weight selection and the `Mouse`->`MouseEntity` rename -- were all false and are gone. `System::TimeSpan::operator+=`/`-=` added to sharp-runtime with 6 tests. All 4 XNBs byte-identical to the official pipeline. HUD gate: the three weight bars saturate to 42/42/42, 85/42/42, 85/0/42, 85/0/85 in the XNA original, the native OPENGLES3 port and WEBGL2 in Chrome alike, and the label glyph cores are pixel-identical. | ✅ |
| SAMPLE-028 | `ColorReplacementSample_4_0` | port | **The `.fx` blocker was stale.** CNA loads compiled XNA Effect bytecode through MojoShader on EasyGL (`CNA_EASYGL_COMPILED_EFFECTS`), so the sample is ported whole with its own unmodified `ReplaceColor.fx` compiled by the official pipeline -- nothing rewritten by hand. Four framework fixes: 3 missing XNB readers (`EffectMaterialReader`, `DictionaryReader<string,object>`, `ExternalReferenceReader`), XNA's `protected Effect(Effect cloneSource)` which CNA lacked (so `EffectMaterial` had ZERO parameters), and custom numeric format strings in sharp-runtime (`ToString("0.000")`), verified differentially against mono 28/28. A fifth framework defect surfaced after acceptance, reported by the owner: MojoShader applies D3D9's clip-space depth conversion and EasyGL's own shaders do not, so compiled-effect geometry was depth-tested on a different scale and swallowed the headlight lens and thin glass edges. Fixed by compensating the GL depth range for compiled-effect draws; the body mean colour now matches XNA to 0-1 of 255 per channel, against 2-9 before. Gate: the 4 overlay lines are byte-identical across XNA, native OPENGLES3 and WEBGL2 in Chrome (48/48), and the car body mean colour tracks `TargetColor` to within 2-9 of 255 per channel. No WindowsPhone target: the official pipeline refuses it. | ✅ |
| SAMPLE-029 | `ParticleSample_4_0` | port | Full original/native/web parity. Whole sample re-ported from the 1130 unchanged C# lines as `.hpp`/`.cpp` pairs. **Five of six recorded deviations were false** -- `DrawableGameComponent` + `Components` + `DrawOrder`, the tap gesture, the verbatim status text, the `WINDOWS_PHONE` branch, and above all the three separate `Begin/End` pairs with `BlendState.Additive`, whose own record admitted a faithful port works on EasyGL and kept the workaround anyway. The sixth was real: `font.spritefont` asks for Segoe UI, which the machine did not have (only the redistributable Segoe UI Mono); the owner supplied `segoeui.ttf` and it was registered with Wine, after which all 3 XNBs build byte-identical to the official pipeline. Gate is narrow by nature -- particles are random and overdraw the overlay -- but two full lines of Segoe UI text are byte-identical across 9600 px, and the browser gate pins the one line the plume never reaches against the original. | ✅ |
| SAMPLE-030 | `CameraShake_4_0` | port | Compare tank-family mesh winding, bone transforms and depth behavior with real XNA. | ⬜ |

### SAMPLE-031–060 — effects, 3D, models, animation and audio

| Task | Upstream directory | Existing | Fresh audit focus | Status |
|---|---|---|---|---|
| SAMPLE-031 | `BloomSample_4_0` | placeholder | Port the complete effect pipeline through public CNA APIs; prove multi-pass output and web parity. | ⬜ |
| SAMPLE-032 | `DistortionSample_4_0` | placeholder | Reuse already-proven CNA shader capability without sample-local wiring; port all used techniques. | ⬜ |
| SAMPLE-033 | `NonPhotoRealisticSample_4_0` | placeholder | Port every technique and post-process stage; compare exact selectable modes. | ⬜ |
| SAMPLE-034 | `NormalMappingSample_4_0` | placeholder | Port the original tangent-space vertex/content path and effect. | ⬜ |
| SAMPLE-035 | `PerPixelLightingSample_4_0` | placeholder | Port every lighting mode/effect combination and validate selection behavior. | ⬜ |
| SAMPLE-036 | `VertexLightingSample_4_0` | placeholder | Port both original effects/techniques and validate flat/directional lighting. | ⬜ |
| SAMPLE-037 | `RimLighting_4_0` | port | Remove direct `TextureCube`/`Model` construction bypasses; use `Content.Load<T>()`. | ⬜ |
| SAMPLE-038 | `ShadowMappingSample_4_0` | placeholder | Port both shadow-map passes through the normal effect/render-target API. | ⬜ |
| SAMPLE-039 | `BillboardSample_4_0` | placeholder | Port custom vertex layout, processor output, wind and alpha-test behavior. | ⬜ |
| SAMPLE-040 | `InstancedModelSample_4_0` | placeholder | Port actual hardware-instancing behavior and compare counts/motion/performance semantics. | ⬜ |
| SAMPLE-041 | `LensFlareSample_4_0` | port | Verify occlusion/color-write behavior and remove any stale model/tool bypass. | ⬜ |
| SAMPLE-042 | `ShatterEffectSample_4_0` | placeholder | Port processor-generated per-triangle data and full shatter effect. | ⬜ |
| SAMPLE-043 | `Particles3DSample_4_0` | placeholder | Port the complete GPU particle system, including lifecycle and custom vertex data. | ⬜ |
| SAMPLE-044 | `Particles2DPipeline_4_0` | port | Audit content-pipeline semantics and generated particle settings, not just rendering. | ⬜ |
| SAMPLE-045 | `XmlParticles_4_0` | placeholder | Port XML content loading plus the shared 3D particle system. | ⬜ |
| SAMPLE-046 | `Graphics3DSample_4_0` | port | Remove port-only component initialization if absent in C#; fix CNA lifecycle only if behavior differs. | ⬜ |
| SAMPLE-047 | `PickingSample_4_0` | port | Audit component lifecycle, model loading and picking against the original. | ⬜ |
| SAMPLE-048 | `TrianglePickingSample_4_0` | port | Remove the generated picking sidecar; use faithful buffer/content APIs. | ⬜ |
| SAMPLE-049 | `HeightmapCollisionSample_4_0` | port | Verify terrain generation, texture/model path and collision values. | ⬜ |
| SAMPLE-050 | `SimpleAnimation_4_0` | port | Pixel/frame compare with real XNA; recheck tank winding, bones, default depth/blend and asset conversion. | ⬜ |
| SAMPLE-051 | `CustomModelAnimation_4_0` | placeholder | Re-test current skeletal/model support; route a confirmed large gap to DEC-003. | ⬜ |
| SAMPLE-052 | `CustomModelClassSample_4_0` | port | Check tank/model-family winding, near-plane and model-loader bypasses. | ⬜ |
| SAMPLE-053 | `CustomModelEffectSample_4_0` | placeholder | Audit its chained content processors; route systemic work to DEC-002. | ⬜ |
| SAMPLE-054 | `SkinningSample_4_0` | placeholder | Canonical end-to-end skeletal animation reference; route remaining large work to DEC-003. | ⬜ |
| SAMPLE-055 | `SkinnedModelExtensions_4_0` | placeholder | Port complete extension/content/animation behavior after canonical skinning support. | ⬜ |
| SAMPLE-056 | `CPUSkinningSample_4_0` | placeholder | Port the CPU skinning algorithm and exact data pipeline; do not replace with GPU skinning. | ⬜ |
| SAMPLE-057 | `InverseKinematics_4_0` | port | Remove raw model/avatar substitutes and verify the full original IK paths. | ⬜ |
| SAMPLE-058 | `ChaseCamera_4_0` | port | Remove raw model bypasses; verify camera dynamics and original input paths. | ⬜ |
| SAMPLE-059 | `Audio3DSample_4_0` | port | Compare positional audio, looping and listener/emitter behavior; pass browser audio gate. | ⬜ |
| SAMPLE-060 | `SoundAndMusic_4_0` | port | Compare sound/music state transitions and browser user-gesture behavior. | ⬜ |

### SAMPLE-061–083 — full games, UI and advanced samples

| Task | Upstream directory | Existing | Fresh audit focus | Status |
|---|---|---|---|---|
| SAMPLE-061 | `MarbleMaze_4_0` | port | Audit the selected original exercise endpoint, all game states, model conversion and controls. | ⬜ |
| SAMPLE-062 | `NetRumble_4_0` | placeholder | Port full networking plus all four effects; verify multi-peer native behavior and web feasibility. | ⬜ |
| SAMPLE-063 | `HoneycombRush_4_0` | port | Full game/original/native/web parity. | ⬜ |
| SAMPLE-064 | `HoneycombRushTrainingKit_4_0` | absent | Reinspect every exercise/starter/end project; old “redundant” verdict is not accepted. | ⬜ |
| SAMPLE-065 | `NinjAcademy_4_0` | port | Full game/original/native/web parity. | ⬜ |
| SAMPLE-066 | `ShipGame_4_0` | placeholder | Port the whole game and all distinct effects, including point sprites; supplemental MonoGame only. | ⬜ |
| SAMPLE-067 | `CatapultWars_4_0` | port | Full game/original/native/web parity. | ⬜ |
| SAMPLE-068 | `CatapultWarsTrainingKit_4_0` | absent | Reinspect all exercises and advanced project; old “redundant” verdict is not accepted. | ⬜ |
| SAMPLE-069 | `CardsStarterKit_4_0` | port | Audit the complete game/framework, AI, screens and rules; no omitted modes. | ⬜ |
| SAMPLE-070 | `RolePlayingGame_4_0_Win_Xbox` | port | Restore simplified combat/screens and any animated content; compare the complete game. | ⬜ |
| SAMPLE-071 | `Yacht_4_0` | port | Full game/original/native/web parity. | ⬜ |
| SAMPLE-072 | `GSMSample_4_0_WIN_XBOX` | port | Audit the complete Game State Management sample and original project selection. | ⬜ |
| SAMPLE-073 | `SoccerPitchSample_4_0` | port | Full original/native/web parity. | ⬜ |
| SAMPLE-074 | `TankOnAHeightMapSample_4_0` | placeholder | Port multi-part tank bones and terrain; compare winding/depth with `SAMPLE-030/050/076`. | ⬜ |
| SAMPLE-075 | `NGSMSample_4_0` | absent | Reinspect lobby/session code and its documented empty gameplay path; do not dismiss before audit. | ⬜ |
| SAMPLE-076 | `SplitScreenSample_4_0` | placeholder | Port multi-part tank bones and split viewports; remove historical asset-generation gaps. | ⬜ |
| SAMPLE-077 | `DynamicMenu_4_0` | port | Full menu/navigation/original/native/web parity. | ⬜ |
| SAMPLE-078 | `LocalizationSample_4_0` | port | Verify cultures/resources/font/text behavior across native and browser builds. | ⬜ |
| SAMPLE-079 | `GesturesSample_4_0` | port | Verify every gesture and browser pointer/touch mapping. | ⬜ |
| SAMPLE-080 | `TouchThumbsticksSample_4_0` | port | Verify multi-touch controls natively and on real touch-capable browser input. | ⬜ |
| SAMPLE-081 | `PerformanceMeasuringSample_4_0` | port | Preserve timing/query semantics; compare displayed measurements structurally. | ⬜ |
| SAMPLE-082 | `UISample_4_0` | port | Full UI/control/original/native/web parity. | ⬜ |
| SAMPLE-083 | `SnowShovelSample_4_0` | port | Full original/native/web parity. | ⬜ |

### SAMPLE-084–111 — phone, service, networking and tool candidates

| Task | Upstream directory | Existing | Fresh audit focus | Status |
|---|---|---|---|---|
| SAMPLE-084 | `AccelerometerSample_4_0` | port | Verify the original emulator keyboard branch and real sensor path; remove only port-invented behavior. | ⬜ |
| SAMPLE-085 | `AvatarAnimationBlendingSample_4_0` | absent | Fresh API/content audit; no substitute avatar body. Route retired-service scope to DEC-004. | ⬜ |
| SAMPLE-086 | `AvatarMultipleAnimationsSample_4_0` | absent | Fresh API/content audit; no substitute avatar body. Route retired-service scope to DEC-004. | ⬜ |
| SAMPLE-087 | `AvatarShadows_4_0` | absent | Fresh API/content audit; no substitute avatar body. Route retired-service scope to DEC-004. | ⬜ |
| SAMPLE-088 | `BingMaps_4_0` | absent | Reinspect API/service dependencies and any offline-testable logic; route scope to DEC-004. | ⬜ |
| SAMPLE-089 | `BingMapsPathFinding_4_0` | absent | Reinspect mapping/path logic and service dependencies; route scope to DEC-004. | ⬜ |
| SAMPLE-090 | `BitmapFontMaker_4_0` | absent | Audit as a WinForms tool rather than a game; route faithful tool scope to DEC-002/005. | ⬜ |
| SAMPLE-091 | `ClientServerSample_4_0` | port | Remove GamerServices/network identity/event workarounds now that CNA claims fixes; test two processes. | ⬜ |
| SAMPLE-092 | `ContentManifestExtensions_4_0` | absent | Audit the actual library/tool outputs and processor contracts; route to DEC-002. | ⬜ |
| SAMPLE-093 | `CurveEditor_4_0` | absent | Audit as a WinForms curve tool and identify reusable XNA `Curve` behavior; route to DEC-005. | ⬜ |
| SAMPLE-094 | `CustomAvatarAnimation_4_0` | absent | Fresh API/content audit; no substitute avatar body. Route retired-service scope to DEC-004. | ⬜ |
| SAMPLE-095 | `GeolocationSample_4_0` | absent | Audit platform abstraction and emulator behavior; no invented GPS data. Route to DEC-004. | ⬜ |
| SAMPLE-096 | `InvitesSample_4_0` | absent | Audit the exact Xbox LIVE account/invite/presence contract; route to DEC-004. | ⬜ |
| SAMPLE-097 | `MemoryMadnessLab_4_0` | absent | Inspect every lab stage/document and identify runnable endpoints before classification. | ⬜ |
| SAMPLE-098 | `MicrophoneEchoSample_4_0` | port | Compare microphone buffering/latency and pass real-browser permission/gesture testing. | ⬜ |
| SAMPLE-099 | `ModelImporterSample_4_0` | absent | Audit importer UI/content behavior and route the measured pipeline/tool scope to DEC-002. | ⬜ |
| SAMPLE-100 | `NetworkPredictionSample_4_0` | port | Remove the explicit options-packet substitute by implementing faithful `SessionProperties` behavior in CNA. | ⬜ |
| SAMPLE-101 | `ObjectPlacementOnAvatarSample_4_0` | absent | Fresh API/content audit; no substitute avatar body. Route retired-service scope to DEC-004. | ⬜ |
| SAMPLE-102 | `Orientation_4_0` | port | Confirm the old phone-hardware classification was wrong; full orientation/native/web parity. | ⬜ |
| SAMPLE-103 | `PeerToPeerSample_4_0` | port | Verify genuine peer topology, events and two/multi-process behavior plus browser feasibility. | ⬜ |
| SAMPLE-104 | `PerformanceUtility_4_0` | absent | Audit as a reusable library; identify dependent samples and faithful testable output. | ⬜ |
| SAMPLE-105 | `PushNotificationsSample_4_0` | absent | Audit exact WP7 service/emulator contracts; route faithful platform scope to DEC-004. | ⬜ |
| SAMPLE-106 | `SavingEmbeddedImages_4_0` | absent | Audit exact media-library behavior and assets; route faithful platform scope to DEC-004. | ⬜ |
| SAMPLE-107 | `TiltPerspective_4_0` | port | Remove the invented keyboard-tilt scheme; preserve original sensor/fallback behavior exactly. | ⬜ |
| SAMPLE-108 | `WinFormsContentSample_4_0` | absent | Audit XNA-in-WinForms hosting/content behavior; route faithful tool/hosting scope to DEC-005. | ⬜ |
| SAMPLE-109 | `WinFormsGraphicsSample_4_0` | absent | Audit XNA-in-WinForms graphics hosting behavior; route faithful tool/hosting scope to DEC-005. | ⬜ |
| SAMPLE-110 | `WP7MusicManagement_4_0` | absent | Audit media-library behavior and emulator branches; route platform scope to DEC-004. | ⬜ |
| SAMPLE-111 | `XnaGraphicsProfileChecker_4_0` | absent | Audit the WinForms diagnostic and exact graphics-profile checks; route tool scope to DEC-005. | ⬜ |

### SAMPLE-112–153 — formerly grouped/ignored physical directories

These IDs are newly assigned in bytewise directory-name order. Do not renumber them after
publication; the validator introduced by `SAMPLES-INFRA-004` will pin the mapping.

| Task | Upstream directory | Existing | Historical classification to re-check | Status |
|---|---|---|---|---|
| SAMPLE-112 | `AvatarAnimPack_4_0_BIN` | absent | Avatar animation asset pack (binary). | ⬜ |
| SAMPLE-113 | `AvatarAnimPack_4_0_FBX` | absent | Avatar animation asset pack (FBX). | ⬜ |
| SAMPLE-114 | `AvatarAnimPack_4_0_Maya` | absent | Avatar animation/DCC asset pack (Maya). | ⬜ |
| SAMPLE-115 | `AvatarAnimPack_4_0_Mod_Tool` | absent | Avatar animation/DCC asset pack (Mod Tool). | ⬜ |
| SAMPLE-116 | `AvatarRig_4_0_Max_2010` | absent | Avatar DCC rig. | ⬜ |
| SAMPLE-117 | `AvatarRig_4_0_Maya_2009` | absent | Avatar DCC rig. | ⬜ |
| SAMPLE-118 | `AvatarRig_4_0_SoftImage_Mod_Tool7_5` | absent | Avatar DCC rig. | ⬜ |
| SAMPLE-119 | `BasicEffectShader_ARCHIVE_2_0` | absent | XNA 2.0 shader archive; inspect code/tool relevance. | ⬜ |
| SAMPLE-120 | `ButtonImages` | absent | Shared image resources; identify all consumers and completeness role. | ⬜ |
| SAMPLE-121 | `CardsStarterKit_4_0_VB` | absent | Visual Basic variant; compare for content/behavior absent from the C# port. | ⬜ |
| SAMPLE-122 | `Catapult_ARCHIVE_2_0` | absent | XNA 2.0 sample; compare with, but do not assume duplication of, CatapultWars. | ⬜ |
| SAMPLE-123 | `ControllerImages` | absent | Shared controller resources; identify all consumers and completeness role. | ⬜ |
| SAMPLE-124 | `CustomIndeterminateProgressBarSample` | absent | Silverlight control; inspect runnable/testable scope. | ⬜ |
| SAMPLE-125 | `GSMSample_4_0_Mango` | absent | Mango variant; compare line by line with desktop GSM. | ⬜ |
| SAMPLE-126 | `GSMSample_4_0_Mango_VB` | absent | Mango Visual Basic variant; inspect unique behavior/content. | ⬜ |
| SAMPLE-127 | `GSMSample_4_0_PHONE` | absent | Phone variant; inspect unique behavior/content. | ⬜ |
| SAMPLE-128 | `LevelStarterKit` | absent | Previously considered incomplete/Silverlight; inspect all files and runnable stages. | ⬜ |
| SAMPLE-129 | `LobbyChatImages` | absent | Shared lobby/chat resources; identify consumers and completeness role. | ⬜ |
| SAMPLE-130 | `MaterialsAndLights_ARCHIVE_2_0` | absent | XNA 2.0 graphics sample; inspect API/content value. | ⬜ |
| SAMPLE-131 | `Minjie_ARCHIVE_2_0` | absent | XNA 2.0 sample; inspect complete source and assets. | ⬜ |
| SAMPLE-132 | `ModelViewerDemo_4_0_Mango` | absent | Phone model viewer; inspect unique XNA behavior and controls. | ⬜ |
| SAMPLE-133 | `Movipa` | absent | Third-party video sample; inspect XNA/API/content relevance. | ⬜ |
| SAMPLE-134 | `MultipassLighting_ARCHIVE_2_0` | absent | XNA 2.0 effect sample; inspect reusable effect behavior. | ⬜ |
| SAMPLE-135 | `NonLinear-WP-SLApp-Navigation-Service` | absent | Silverlight navigation sample; inspect runnable/testable scope. | ⬜ |
| SAMPLE-136 | `PaddleBattle_4_0_Mango` | absent | Phone/Mango game; inspect as a distinct port candidate. | ⬜ |
| SAMPLE-137 | `PaddleBattle_4_0_Mango_VB` | absent | Visual Basic phone variant; compare unique content/behavior. | ⬜ |
| SAMPLE-138 | `Pickture_ARCHIVE_2_0` | absent | XNA 2.0 sample; inspect complete source and assets. | ⬜ |
| SAMPLE-139 | `PushRecipe_WP7_SL` | absent | WP7/Silverlight push sample; inspect service and emulator behavior. | ⬜ |
| SAMPLE-140 | `RedistributableTTFs_ARCHIVE_3_1` | absent | Font resource archive; identify sample consumers and licensing/build role. | ⬜ |
| SAMPLE-141 | `Riemers` | absent | Third-party tutorial collection; inventory individual runnable projects before scope decision. | ⬜ |
| SAMPLE-142 | `RobotGame_ARCHIVE_2_0` | absent | XNA 2.0 game sample; inspect complete source and assets. | ⬜ |
| SAMPLE-143 | `RolePlayingGame_4_0_Phone` | absent | Phone variant; compare for unique behavior/content missing from `SAMPLE-070`. | ⬜ |
| SAMPLE-144 | `SilverlightMicrophoneSample` | absent | Silverlight microphone sample; compare with XNA microphone APIs and web needs. | ⬜ |
| SAMPLE-145 | `SoundLab` | absent | Standalone audio authoring tool; inspect formats/workflows used by samples. | ⬜ |
| SAMPLE-146 | `SpaceShooter_ARCHIVE_3_0` | absent | XNA 3.0 game sample; inspect complete source and assets. | ⬜ |
| SAMPLE-147 | `SpriteBatchShader_ARCHIVE_2_0` | absent | XNA 2.0 shader sample; inspect effect/compiler relevance. | ⬜ |
| SAMPLE-148 | `TiledSpritesSample_ARCHIVE_3_1` | absent | XNA 3.1 sample; inspect complete source and assets. | ⬜ |
| SAMPLE-149 | `TombstoningSample` | absent | WP7/Silverlight lifecycle sample; inspect state-persistence behavior. | ⬜ |
| SAMPLE-150 | `UnitConverterStarterKit` | absent | Previously considered empty/incomplete; verify every file before classification. | ⬜ |
| SAMPLE-151 | `VectorRumble_ARCHIVE_2_0` | absent | XNA 2.0 game sample; inspect complete source and assets. | ⬜ |
| SAMPLE-152 | `XNA-4-Racing-Game-Kit-master` | absent | Governed only by `plan_racing.md`; execute it last, after every other sample/infrastructure outcome. Do not change that plan here. | ↗ |
| SAMPLE-153 | `XNA_XNB_Format` | absent | XNB format documentation; assess relevance to faithful content loading/tooling. | ⬜ |

## Execution order

1. Finish `SAMPLES-INFRA-002` through `SAMPLES-INFRA-007` and prove the workflow with
   `SAMPLE-001`.
2. Audit one sample at a time. Prefer small existing ports first to expose systematic workaround
   patterns, then port currently-unblocked placeholders. Do not bulk-mark siblings complete.
3. When a sample exposes a bounded CNA or sharp-runtime defect, fix and test it immediately in the
   same session, then return to the sample and complete all gates.
4. When a genuinely large capability is confirmed, update the sample row with exact evidence,
   mark it `🛑`, add it to the decision queue and ask the owner when available. Continue with other
   independent rows meanwhile.
5. Revisit all historically absent/ignored directories individually. A final non-port conclusion
   requires fresh file-level evidence and, for a scope choice, an explicit owner decision.
6. Keep web verification inside every sample task rather than postponing 153 browser builds to a
   final cleanup phase.
7. Start the separately governed Racing plan only after all other sample and infrastructure rows
   are resolved; Racing is the final port in the campaign.

## Session report and commit contract

At the end of each sample session report:

- upstream projects/files and reference runtime used;
- `cna-samples`, `cnanext` and `sharp-runtimenext` files changed;
- workarounds removed and framework/runtime gaps fixed;
- intentional C++/asset-container deviations still present;
- native builds/tests and real-browser result;
- remaining blocker or owner decision, if any;
- commit hash from every touched repository.

Stage only explicit task files. One sample task uses one commit per touched repository, with the
same `SAMPLE-nnn` identifier. Do not bundle unrelated cleanup and do not push unless the applicable
repository instructions and owner authorization require it.
