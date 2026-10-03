# SAMPLE-111 — `XnaGraphicsProfileChecker_4_0` audit and owner decision

## Owner decision — 2026-10-03: `⛔` cancelled

After the re-analysis below, the owner chose option 1 ("ponech 111 cancelled"). SAMPLE-111 is an
evidence-backed non-port: a legacy Windows Direct3D 9 C++/CLI WinForms diagnostic outside the
EasyGL-only campaign. No CLI, boolean probe or capability-viewer substitute was added, and none will be.

## Current-head re-analysis — 2026-10-03

**Status at analysis time: `🛑` owner decision pending under `SAMPLES-DEC-005`.** Nothing was ported,
modernized or cancelled; no sample, CNA or Sharp Runtime source changed. Heads: CNA `next
fc64a4be3`, Sharp Runtime `next db86514c`.

**What it is.** Not a game. A Windows diagnostic utility written in C++/CLI (1,103 lines) that
explains why `GraphicsAdapter.IsProfileSupported` accepts or rejects Reach and HiDef. It
independently re-checks every XNA profile requirement against the native Direct3D 9 driver
(`Direct3DCreate9`, `GetDeviceCaps`, `CheckDeviceFormat`: shader models, limits, caps bits,
formats, render targets), compares that with XNA's own answer (flagging disagreement as `Yikes!`),
and shows the result as an HTML report in a WinForms `WebBrowser`, with a Copy-to-Clipboard text
export. The retained `original/` snapshot is byte-identical to the physical upstream directory.

**Rechecked; nothing material changed since 2026-09-01:**

- **The original still cannot be built.** C++/CLI (`/clr`) needs Microsoft's Visual C++ (the
  documentation requires VS2010 with C++ and the DirectX SDK). The offline Win7 VM was measured
  without any VC++ toolset; this host has a DirectX SDK June 2010 under `/rv/tmp/samples/_tools/`
  but no Visual C++ 2010, and no other compiler accepts C++/CLI. The VM was not booted again.
- **CNA unchanged in substance.** `GraphicsAdapter::IsProfileSupported`
  (`modules/graphics/src/Xna/GraphicsAdapter.cpp:373`) asks the active renderer's adapter query;
  only the Windows DirectX9 renderer has a real D3D9 probe, and EasyGL truthfully answers `true`
  rather than inventing a caps table. `CNA::RendererCapabilityProfile` remains the separate,
  cross-renderer capability report. Sharp Runtime still has no WinForms/System.Drawing (nor a
  `WebBrowser` control or clipboard).
- **Campaign boundary.** `rules.md` limits sample work to EasyGL (OPENGLES3/WEBGL2) and forbids
  building or claiming support for other renderers. A faithful D3D9 tool is therefore outside the
  campaign unless the owner makes an explicit exception. A browser cannot query a D3D9 driver at all.

The earlier 33/33 focused test run is historical and is not repeated as a current-head gate.

**Options (updated).**

1. **⛔ cancel** as a legacy Windows/D3D9 C++/CLI diagnostic, outside the EasyGL campaign,
   consistent with the WinForms tool cancellations (SAMPLE-090/093/108/109).
2. **Faithful Windows D3D9 tool**, with an explicit renderer-boundary exception: install VC++ 2010
   to capture the original, then port the C++/CLI checker, the WinForms/WebBrowser/clipboard shell
   and the DirectX9 path, with a native-only ruling. Large.
3. **A new CNA capability viewer** on `RendererCapabilityProfile` (native OPENGLES3 + WEBGL2),
   recorded as a new tool and not the XNA D3D9 profile checker. Rough estimate: native about
   3–5 h, browser another 2–3 h.

## Status

Fresh audit complete enough to require an owner product-scope decision under `SAMPLES-DEC-005`.
No invented game, reduced boolean-only probe, OPENGLES3 report relabelled as Direct3D 9, browser
caps substitution or CLI/UI rewrite was added. The row remains `🛑`; only the owner may select a
legacy Windows tool non-port, a native-only exception or an explicit cross-renderer modernization.

## Classification and complete source inventory

The physical upstream directory is a standalone Release/Win32 C++/CLI WinForms diagnostic utility,
not an XNA `Game`. The audit covers all 19 files: solution/project/filter metadata, icon/resources,
WinForms resources, HTML documentation and licence, plus 1,103 lines across all C++/CLI/native
headers and implementation files.

The documented product explains *why* `GraphicsAdapter.IsProfileSupported` accepts or rejects
XNA's Reach and HiDef profiles. It deliberately uses C++/CLI so one process can compare XNA's
managed answer with the native Direct3D 9 capability queries that XNA itself used:

- the 784x562 form embeds an HTML `WebBrowser` report and a **Copy To Clipboard** button that emits
  the same data as plain text;
- it reports adapter description plus vendor, device, subsystem and revision identifiers in the
  original hexadecimal widths;
- for both `GraphicsProfile.Reach` and `GraphicsProfile.HiDef`, it constructs the exact profile
  requirement table, runs the independent native checker, then compares that result with
  `GraphicsAdapter.IsProfileSupported` and visibly flags any disagreement as `Yikes!`;
- Reach requires shader model 2.0, 65,535 primitives, 16 streams, 2,048 textures, 512 cube maps,
  one render target, no volume/vertex textures and its exact restricted format lists;
- HiDef requires shader model 3.0, 1,048,575 primitives, 32-bit indices, 4,096 textures/cubes,
  256-volume extent, four vertex samplers/render targets, unconditional non-power-of-two support
  and the exact expanded format lists;
- `ProfileChecker` calls `Direct3DCreate9`, `GetDeviceCaps` and the relevant `CheckDeviceFormat`
  variants. It checks shader versions, numeric limits, dozens of D3D caps bits, blending, depth,
  stencil, MRT, texture addressing/filtering/mipmapping, vertex declarations, 2D/cube/volume and
  vertex-texture formats, plus mandatory Color/HdrBlendable render targets;
- the no-hardware-transform-and-light path deliberately substitutes XNA's software vertex-shader
  capability floor before continuing the checks;
- failures become an ordered detail list; exceptions become escaped HTML/plain-text report data.

A replacement that prints only Reach/HiDef `true` values omits the entire purpose of the utility.

## Original build evidence

Artifact root: `/rv/tmp/samples/SAMPLE-111-XnaGraphicsProfileChecker_4_0/`.

- `original/` is the complete byte-for-byte upstream snapshot.
- The owner-provided Windows 7 SP1 32-bit VM ran headless with all eight virtual network adapters
  set to `none`; no guest browser or internet access was enabled. The VM was shut down normally and
  the temporary `cna_sample111` share was removed after the audit.
- The original documentation explicitly requires XNA Game Studio 4.0, a Visual Studio 2010 edition
  with C++ support (not C# Express alone), and the native DirectX SDK.
- The unchanged Release/Win32 solution reaches its VCX project but cannot build in this VM because
  the Visual C++ component is absent: `VCTargetsPath` is empty, and exhaustive local searches find
  no `vcvarsall.bat`, `Microsoft.Cpp.Default.props` or `cl.exe`. MSBuild consequently tries
  `C:\Microsoft.Cpp.Default.props` and fails with `MSB4019`.
- This is a measured reference-environment prerequisite, not a source defect and not permission to
  translate the C++/CLI source before its behavior can be captured. No compiler/SDK was installed
  and the offline VM configuration was not broadened.

The reproducible build/probe scripts, MSBuild log and prerequisite transcript remain under
`build-win7.cmd`, `probe-win7.cmd` and `evidence/`. There is no executable or runtime screenshot to
claim from this environment.

## Live CNA capability audit

CNA has meaningful but differently scoped graphics-capability infrastructure:

- the XNA-shaped `GraphicsAdapter` properties, `IsProfileSupported`, render-target/back-buffer
  format queries and display enumeration exist; 27 focused adapter/platform tests pass;
- `CNA::RendererCapabilityProfile` is a modern extension with 30 independently classified
  features, ten numeric limits, per-`SurfaceFormat` usage masks, unknown/unsupported distinctions
  and a generated English report; all six focused tests pass;
- the common OPENGLES3 build truthfully returns `true` for Reach and HiDef because it has no
  `D3DCAPS9` structure and refuses to pretend a hardcoded table is a hardware query;
- CNA's Windows-only DirectX9 renderer does contain real `Direct3DCreate9`/`GetDeviceCaps` profile
  probing, a HiDef floor, profile format tables and several real format/multisample queries. These
  are renderer-internal and cover the runtime decisions CNA needs, not the original utility's full
  per-capability failure report;
- on non-Windows platforms adapter subsystem/revision are unavailable and reported as zero, while
  the active renderer may be OpenGL ES, Vulkan, WebGPU or another family with no truthful mapping
  to a Windows `D3DCAPS9` field.

The clean focused gate is 33/33 with `SDL_VIDEODRIVER=offscreen` and is retained as
`evidence/cna-focused-tests-offscreen.log`. It proves the live CNA APIs above; it does not turn a
WebGL2 capability report into the original Direct3D 9 compatibility diagnosis.

## Why no partial port was added

The exact tool can plausibly remain a Windows/DirectX9 diagnostic, but the campaign's normal
OPENGLES3 plus mandatory WEBGL2 product gates do not describe that product. A browser cannot query
the user's D3D9 driver, shader model or D3D caps bits. Conversely, displaying CNA's existing
`RendererCapabilityProfile` would be a useful new multi-renderer tool, but it answers different
questions with different feature identities and is therefore an explicit modernization, not an
unchanged port.

Replacing the WinForms/WebBrowser/clipboard shell with a CLI or an invented game also changes the
original product. Sharp Runtime has no WinForms/System.Drawing implementation, and adding that
large UI subsystem merely to hide the platform decision is not justified.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port for this legacy XNA/D3D9 diagnostic utility;
2. provide/install the documented Visual C++ 2010 and DirectX SDK prerequisites, then authorize a
   faithful Windows DirectX9 tool with a native-only qualification exception and preserved full
   report/clipboard behavior; or
3. explicitly authorize a distinct CNA multi-renderer capability viewer based on
   `RendererCapabilityProfile`, defining native and WEBGL2 UI/export requirements and recording
   that its report is not the original XNA D3D9 profile checker.

Until that choice, no CNA/Sharp Runtime source change or sample tool is justified. The exact source,
build failure and live capability evidence remain ready for whichever scope the owner selects.
