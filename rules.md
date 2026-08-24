# CNA Samples Porting Rules

## Mandatory status

This file is the binding working agreement for every change in `cna-samples`. Read it completely
before auditing, porting, building, running, documenting or reviewing a sample. Also read
[`plan.md`](plan.md), the affected sample's `missing.md`, and the instructions of every dependency
repository that will be changed.

If another repository document conflicts with this file, follow this file and `plan.md`, then fix
the stale document without discarding useful historical evidence. A historical status, workaround,
ignore decision or statement that CNA lacks a feature is evidence to re-check, not authority.

## Campaign scope and repository chain

- Audit every physical upstream sample directory. This includes existing ports, placeholders,
  previously ignored samples, tools, libraries, XNA 3.x variants, Windows Phone projects and
  retired-service samples.
- Only the owner may make the final decision to ignore a sample or accept an evidence-backed
  non-port conclusion. Do not infer that decision from an old `ignored.md`, `DEFERRED.md`, plan or
  comment.
- The Racing Game is deliberately last and is governed only by `plan_racing.md`. During normal
  sample work, do not edit its code, tasks, API matrix, feasibility report or special plan.
- Use the active development chain:

  | Repository | Checkout | Branch |
  |---|---|---|
  | samples | `cna-samples` | `develop` |
  | XNA runtime | `../cnanext` | `next` |
  | .NET runtime | `../sharp-runtimenext` | `next` |

- `cna-samples` must build against `../cnanext`, with `CNA_SHARP_RUNTIME_ROOT` selecting
  `../sharp-runtimenext`. Do not silently fall back to the old `../cna` or `../sharp-runtime`
  checkouts.
- A sample session may and often must change all three repositories. Keep the fix in the layer that
  owns the behavior.

## Sources of truth

Use these sources in this order:

1. The exact original sample under `/rv/tmp/XNAGameStudio/Samples`, including every C# file,
   project/configuration, content declaration, processor, asset and documentation file.
2. A real XNA 4.0 build of that unchanged source for visible and observable behavior.
3. `/rv/data/library/github.com/FNA-XNA/FNA` for XNA 4.0 API and behavior not established by the
   sample itself.
4. Local Microsoft XNA documentation/specification material.
5. MonoGame or other ports only as supplemental evidence, never as authority over XNA/FNA.

Do not compare only screenshots or only the main `Game` class. Review the original and C++ port
line by line, including content projects, custom importers/processors/writers, helper libraries,
conditional compilation branches and target-specific projects. Preserve inactive platform/debug
branches in the translation when the corresponding original logic is part of the selected source.

## Fidelity is the primary requirement

The C++ port must be as close as practical to the original XNA 4.0 C# sample:

- Preserve namespaces, type and member names, file/class decomposition, lifecycle, algorithms,
  constants, default values, update/draw order, input mappings, scenes, screens, effects, audio,
  networking behavior and content identifiers.
- Preserve the original visual output, timing, interaction and state transitions. Validate audio
  and non-default paths, not just the first frame.
- Keep original branches and complete features. Do not omit or simplify behavior merely because a
  branch is not active on the current Linux reference target.
- Limit deviations to necessary, lossless C#-to-C++ mechanics such as RAII ownership, value versus
  reference representation, C++ property-call syntax and closed AOT content-reader registration.
- Use XNA-shaped CNA APIs and the established `getXProperty()` / `setXProperty(...)` convention.
  Implement CNA-required runtime type naming without changing the original logical type name.
- Prefer `System::*` types and primitive aliases from `sharp-runtimenext` when the C# source uses a
  .NET concept. Do not replace a missing .NET behavior with an unrelated STL implementation just
  to compile.
- Retain original sample documentation and applicable Microsoft sample license/SPDX information.

## Zero-workaround policy

No sample may compensate for a CNA or sharp-runtime defect. In particular, do not:

- call renderer/backend helpers from sample code;
- use `NOXNA`/CNA extension graphics helpers in place of an XNA API;
- replace `Content.Load<T>()` with raw-mesh, raw-model, direct texture or handwritten loader paths;
- use direct `SetData` or loose sidecars as substitutes for original pipeline content;
- hand-wire a shader/effect's behavior inside one sample because the common effect path is missing;
- add sample-local initialization, state changes, culling changes or lifecycle calls only to make a
  broken framework path appear to work;
- invent keyboard controls, services, fake data, replacement screens, reduced demonstrations or
  visual overlays;
- omit a failing scene, branch, effect, audio path, network path or processor output and mark the
  sample complete.

A construct is not a workaround when it is the direct translation of the original source. For
example, an original C# sample's one-pixel `Texture2D.SetData` call remains valid. Mechanical scans
for suspicious patterns help find bypasses but never replace line-by-line review.

When a faithful translation exposes a defect or missing API:

| Owner | Required action |
|---|---|
| XNA/FNA API or behavior | Fix `../cnanext` generally and faithfully, following its `AGENTS.md` and `CHECKLIST.md`; add the required tests. |
| .NET `System.*` API, behavior or primitive | Fix `../sharp-runtimenext` generally; follow its instructions and add tests. |
| Translation or sample content | Fix `cna-samples`; do not hide a framework/runtime issue here. |

Framework fixes must be valid XNA/FNA reimplementation work, not sample-name checks, special cases
or hacks whose only purpose is to pass one sample.

## Content and XNB policy

- Exact pregenerated XNB files produced by the original Microsoft XNA Content Pipeline are
  explicitly acceptable. Prefer them when they preserve the original `Content.Load<T>()` contract.
- Whenever possible, prove checked-in XNBs byte-identical to the retained official pipeline output
  and record hashes and generation evidence in the sample audit.
- Pregenerated XNB acceptance does not make an unrelated Content Pipeline authoring tool/library a
  port, and it does not authorize invented assets or runtime substitutes.
- Keep the original content identifiers and public CNA load calls. A closed AOT reader may map an
  original reflective runtime type only when it reads the same serialized object and fields.
- If an exact XNB cannot be used, any offline conversion must be lossless for the sample,
  reproducible from the original source asset and documented. It is not permission to alter visual
  or behavioral content.
- Use CNA's shared compiled-effect/content path. Do not translate `.fx` behavior by hand per
  sample. If a concrete effect construct is unsupported, treat it as a CNA gap and use the owner
  decision process below when the required subsystem is large.
- Remove loose PNG/BMP, model, font-sidecar, shader or generated-data substitutes left by older
  ports when exact original pipeline content is available.
- Preserve historical `help.png`, but move it to the sample root beside `CMakeLists.txt`. It must
  not be in `Content`, loaded, copied, preloaded or displayed. The old F1 help overlay is not part
  of XNA and must be removed from every audited sample.

## Owner decision boundary

Continue auditing and making bounded faithful fixes without interrupting the owner. If a concrete
sample proves that completion requires a large new subsystem or a material scope choice:

1. do not introduce a workaround;
2. mark the sample row `🛑` in `plan.md`;
3. describe the exact evidence, affected source, measured scope and realistic options in
   `missing.md` and the owner decision queue;
4. ask the owner when the owner is at the computer before implementing the large subsystem or
   declaring a non-port.

Typical decision-sized areas include raw `.fx` ingestion beyond the proven compiled-effect path,
Content Pipeline authoring/extensibility, retired Xbox LIVE/phone/hardware services, major skeletal
animation work, tool/WinForms/Silverlight/older-XNA scope and a substantial browser-platform gap.
The owner decides whether unusual or duplicate variants make sense to port.

## Required per-sample workflow

1. Locate the `SAMPLE-nnn` row in `plan.md`; change it to `🔎` or `🛠` while active.
2. Inspect and classify the physical upstream directory yourself. Never trust the old status.
3. Create the stable artifact root
   `/rv/tmp/samples/SAMPLE-nnn-UpstreamDirectory/` and retain the exact original snapshot used.
4. Select the correct original project and configuration. Build the unchanged original and, when
   runnable, execute and capture it before judging the port.
5. Audit every relevant original file and C++ file line by line. Record controls, visuals, audio,
   timing, branches and representative state transitions.
6. Remove all old sample workarounds. Implement the complete faithful translation and fix any CNA
   or sharp-runtime defect in its own repository during the same session.
7. Build with at most six parallel jobs (`-j6`) when supported.
8. Build and run the native OPENGLES3 version. Compare it with the real original and exercise
   representative input and behavior, including clean exit.
9. Build the complete WEBGL2 bundle, serve it over local HTTP and test it in the system Google
   Chrome launched from the terminal. Verify real rendering, content, input, audio where relevant,
   browser console/runtime errors and representative interaction. A successful link or Node-only
   execution is insufficient.
10. Run focused regression tests for every changed CNA or sharp-runtime component.
11. Run targeted no-workaround scans, then manually review every hit against the original source.
12. Update the sample's `missing.md`, `plan.md` row and any affected decision/history document.
13. Commit each completed task in every changed repository, staging only explicit files. Use the
   same `SAMPLE-nnn` identifier in related commits. Do not push unless the owner explicitly asks.

## Original XNA execution on this Linux host

- Prefer the retained per-sample build and capture scripts. Do not assume plain `wine Sample.exe`
  uses the correct XNA environment.
- The established prefix is
  `CNA_XNA40_WINEPREFIX=/home/robertvokac/.wine-cna-xna40` (the scripts use this path by default).
- The verified original-game capture path normally uses WineD3D with
  `WINEDLLOVERRIDES=d3d9=b`; default Wine/DXVK behavior may fail even when the original is valid.
- Record the exact prefix, DLL override, configuration, build command and run command in evidence.
- If the direct Linux route is genuinely unavailable, the Windows 7 VirtualBox VM with Visual
  Studio 2010 and XNA Game Studio 4.0 is the fallback. An environment failure is not permission to
  skip comparison.
- Other Codex sessions may share the desktop. Use isolated Xvfb displays or otherwise identify the
  exact process/window before attributing visible output, input or a forced close to the sample.

## Renderer boundary

EasyGL is the only renderer in this campaign:

- Native: `CNA_GRAPHICS_RENDERER=OPENGLES3`.
- Browser: `CNA_GRAPHICS_RENDERER=WEBGL2`, the Emscripten spelling of the same EasyGL OpenGL ES 3
  path.

Do not build, debug, compare, claim support for or add sample-specific behavior for Vulkan,
SDL_Renderer, Bgfx, WebGPU, desktop OpenGL or other renderers during sample audits.

## Artifact policy

All generated artifacts must live below the sample's `/rv/tmp/samples` root, not in the source
repository or an ad-hoc `/tmp` directory:

| Path | Required contents |
|---|---|
| `xna4-original/` | Exact original source/configuration used for comparison. |
| `xna4-build/` or a documented equivalent | Original XNA executable and official pipeline output. |
| `cna-native-opengles3/` | Reusable native CMake tree and executable. |
| `cna-web-webgl2/` | Reusable Emscripten tree and complete `.html`, `.js`, `.wasm`, `.data`/content bundle. |
| `scripts/` | Reproduction/build/run/capture helpers for the audit. |
| `evidence/` | Original/native/web captures, logs, hashes and useful before-fix evidence. |

The sample's `missing.md` must name the exact artifact root and important outputs. Keep build trees
reusable and web bundles suitable for later copying to the owner's website without source edits.

## Documentation and completion gate

`missing.md` is an evidence record, never a waiver. It must state what was compared, the original
configuration, exact content provenance, native/web results, commands or scripts, known
differences, framework/runtime fixes and artifact paths. Stale claims must be retested and either
removed with evidence or kept as active gaps.

A sample may be marked `✅` only when all applicable requirements are true:

- physical directory and every relevant source/content file were freshly audited;
- original behavior was built/run where possible and recorded;
- the C++ translation is complete, including relevant conditional branches;
- no sample workaround, substitute, invented behavior or undocumented difference remains;
- exact XNA content or a documented faithful conversion is used;
- native OPENGLES3 builds, runs and matches the original;
- the complete WEBGL2 bundle runs in real system Chrome and passes representative behavior;
- all cross-repository fixes and focused tests pass;
- `missing.md` and `plan.md` contain current evidence;
- all task changes are committed by explicit file list.

If any active behavioral/visual/audio difference, workaround, unverified large gap or required
browser failure remains, the row is not `✅`.
