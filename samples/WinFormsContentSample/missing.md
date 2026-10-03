# SAMPLE-108 — WinFormsContentSample_4_0 audit

## Owner decision — 2026-10-03: `⛔` cancelled

After the re-analysis below, the owner chose option 1 ("ponech 108 cancelled"). SAMPLE-108 is an
evidence-backed non-port: a Windows design-time WinForms tool, cancelled like SAMPLE-090 and
SAMPLE-093. No port, CLI, substitute viewer or modernized tool was added, and none will be. The
retained evidence stays as recorded, including CNA's byte-identical `Model.xnb` rebuild of the
original's runtime output. Cancelling the row does not close the CNA gaps it measured: rendering
into a foreign window through `Present(..., overrideWindowHandle)` and adopting a native control's
window.

## Current-head re-analysis — 2026-10-03

**Status at analysis time: `🛑` owner decision pending under `SAMPLES-DEC-005`.** Nothing was
ported, modernized or cancelled; no sample, CNA or Sharp Runtime source changed. Heads: CNA `next
fc64a4be3`, Sharp Runtime `next db86514c`. Evidence:
`/rv/tmp/samples/SAMPLE-108-WinFormsContentSample_4_0/evidence/current-head-analysis-20261003/`.

**What it is.** A Windows-only XNA *tool*, not a game: one .NET 4 x86 WinForms application (25
upstream files, 1,497 lines of C# including designer code). `MainForm` opens a "Load Model" file
dialog on startup and from File → Open, accepting any FBX/X file. `ContentBuilder` writes a
temporary XNA content project through the Microsoft.Build object model (Windows, Reach, Release,
stock importers), builds it with `BuildManager`, and an MSBuild `ILogger` collects errors, which
appear in a `MessageBox`. `ModelViewerControl` then loads the temporary `Model.xnb` through
`ContentManager` and draws it continuously rotating and automatically framed. It is a WinForms
`Control` that hosts a reference-counted shared `GraphicsDevice`, is repainted from
`Application.Idle`, and presents into its own HWND with `Present(sourceRectangle, null, Handle)`.
A System.Drawing fallback draws error text. The retained `original/` snapshot is byte-identical
to the physical upstream directory. The 2026-09-01 VM evidence of the unchanged original
(Release/x86 build, dialog, Cats.fbx build and render) stands; the original was not rerun.

**What changed since 2026-09-01.**

1. **CNA now has a native C++ XNA content pipeline** (`modules/content-pipeline`: `FbxImporter`,
   `XImporter`, `ModelProcessor`, the `BuildContent`/`ContentProject` tasks and the `cna-content`
   tool). It was qualified against genuine XNA output by `plans/plan_xna_sample_xnb_sweep.md`,
   whose corpus includes this sample's two references. **Fresh diagnostic:** `cna-content` from
   CNA's shared Release `build-probe` tree (built 2026-10-03 from `next`; the only later commits
   change effect sampling at draw time) built the original `Cats.fbx`/`cat.tga` with the
   `ContentBuilder` settings (`--xnb-platform windows --xnb-profile reach --xnb-version 5
   --xnb-compress none --build-configuration Release --xna-compatible`):
   - `Model.xnb` is **byte-identical** to the genuine VM output
     (`0f24e88b856754724278c8db14fd57bc5bb407d1a669f0ec88e7c32bebf81228`);
   - `cat_0.xnb` has the same size (43,923 B), format (DXT1), dimensions (256×256) and mip count
     (9), but different compressed blocks (`204a37a7…` against `3b0e62a6…`). Decoded, each level
     differs by RMSE ≈ 5–9 of 255, and at level 0 both encoders are about 4–5 RMSE from the
     source TGA. This is the sweep's documented accepted class "two block compressors choose
     different endpoints for the same pixels" (263 references), not a defect.
   The content-pipeline library also compiles for Emscripten (`libcna_content_pipeline.a` in
   CNA's WEBGL2 trees). It was **not** run in a browser.
2. **`GraphicsDevice::Present(sourceRectangle, destinationRectangle, overrideWindowHandle)` now
   exists** (XNA-MISSING-011), but its documentation says no renderer honours a destination
   rectangle or a foreign window. The sample's `Present(sourceRectangle, null, Handle)` into a
   WinForms HWND would therefore throw `NotSupportedException`.
3. CNA has a native open-file dialog (`CNA::Devices::FileDialog` over SDL3). It is not
   `System.Windows.Forms.OpenFileDialog`.

**Still missing.** `Sdl3Platform::AdoptWindowHandle` still adopts only SDL windows, so CNA cannot
render into an arbitrary native control. Sharp Runtime still has no `System.Windows.Forms`,
`System.Drawing` or `Microsoft.Build` (its modules end at component-model/collections/io/net/xml).
A browser has no equivalent of the local Windows dialog, the WinForms control/HWND or the
System.Drawing fallback, so the browser product boundary is still undefined.

**Assessment.** The XNA half of the tool (runtime FBX/X → XNB → `ContentManager` → `Model` draw) is
now essentially covered by CNA natively; the evidence above is one byte-identical model plus a
documented compressor difference. What remains is the Windows desktop shell the sample exists to
demonstrate: WinForms, System.Drawing, the Microsoft.Build object model and hosting in a foreign
native window. The sibling WinForms tools SAMPLE-090 and SAMPLE-093 were cancelled (`⛔`) for the
same kind of scope.

**Options (updated).**

1. **⛔ cancel** as a Windows design-time tool, like SAMPLE-090/093. The pipeline result above
   stays recorded as CNA capability evidence.
2. **Faithful Windows desktop tool:** a WinForms + System.Drawing + Microsoft.Build subset in Sharp
   Runtime, foreign-window hosting and the override `Present` in CNA, and a ruling that the browser
   gate does not apply. This is very large, weeks rather than hours.
3. **Explicitly modernized cross-platform viewer**, recorded in `diff.md` as not WinForms parity:
   a CNA window with `CNA::Devices::FileDialog`, an in-process CNA content build, `ContentManager`
   and the original rotating, auto-framed viewer logic. The browser variant would add a file
   input and the WASM pipeline, unverified. Rough estimate: native about 4–6 h, browser another
   3–5 h with real uncertainty (WASM size and performance of the pipeline, where built content
   lives).

The rest of this file is the 2026-09-01 audit, kept as history. Its statements that the Present
overload and FBX/X import are absent are superseded above.

**Status:** owner decision required under SAMPLES-DEC-005. This is a complete Windows desktop authoring/viewer application, not an XNA Game that can be faithfully represented by adding a normal CNA sample target. No reduced Game, CLI, precompiled-model viewer, alternate UI, or FBX-to-glTF workaround was added.

## Classification and inventory

The original package contains one .NET 4 x86 WinExe project, WinFormsContentLoading, plus four FBX models and two TGA textures. Its 1,659 project/source lines include:

- a MainForm with File/Open and File/Exit menus plus an automatically opened OpenFileDialog;
- a reusable GraphicsDeviceControl hosted directly inside a WinForms Control;
- a reference-counted IGraphicsDeviceService shared by controls;
- device-status, demand-grow Reset, viewport, paint, and device-event handling;
- Present(sourceRectangle, destinationRectangle, overrideWindowHandle) into the control HWND;
- a System.Drawing fallback renderer for designer/device-loss errors;
- a ModelViewerControl that invalidates from Application.Idle and draws a continuously rotating, automatically framed Model;
- a runtime ContentBuilder built on Microsoft.Build;
- an MSBuild ILogger implementation and service container;
- generated WinForms resources/settings and designer code.

ContentBuilder creates a temporary XNA content project in memory, registers the stock FBX/X/Texture/Effect pipeline assemblies, adds the file chosen by the user, executes an asynchronous Microsoft.Build request, waits for it, and then lets ContentManager load the resulting temporary Model.xnb. Supporting only the four bundled models is not the original contract: the dialog accepts arbitrary local FBX and X files.

The exact source snapshot is retained at:

    /rv/tmp/samples/SAMPLE-108-WinFormsContentSample_4_0/original/

## Authentic Windows/XNA evidence

The unchanged solution built successfully in the offline Windows 7 VM with .NET 4 and XNA Game Studio 4.0:

    MSBuild.exe WinFormsContentLoading.sln /t:Rebuild
        /p:Configuration=Release;Platform=x86 /m:2

The produced WinForms application ran on the interactive VM console. Its Shown event opened the real Load Model dialog in the original Content directory. Selecting Cats.fbx exercised the actual runtime ContentBuilder and produced:

    Model.xnb  1937 bytes
    SHA-256    0f24e88b856754724278c8db14fd57bc5bb407d1a669f0ec88e7c32bebf81228

    cat_0.xnb  43923 bytes
    SHA-256    3b0e62a61f8ba3652fd1c560861d35b978c0d6384342c9880b61c1cdba9301f9

The model loaded through ContentManager and rendered as the original continuously rotating, lit, textured cat cube inside the WinForms control. All VM network adapters remained disabled, GuestInfo reported zero network interfaces, the VM was shut down normally, and the temporary shared folder was removed.

Evidence:

    /rv/tmp/samples/SAMPLE-108-WinFormsContentSample_4_0/evidence/xna4-build.log
    /rv/tmp/samples/SAMPLE-108-WinFormsContentSample_4_0/evidence/xna4-open-dialog.png
    /rv/tmp/samples/SAMPLE-108-WinFormsContentSample_4_0/evidence/xna4-cats-model.png
    /rv/tmp/samples/SAMPLE-108-WinFormsContentSample_4_0/win7-export/

## Live CNA and Sharp Runtime boundary

Live CNA already supplies meaningful pieces of the XNA side:

- GraphicsDevice(adapter, profile, PresentationParameters);
- PresentationParameters.DeviceWindowHandle;
- GraphicsDeviceStatus, Reset, Viewport, IGraphicsDeviceService, device events, Model, BasicEffect, and ContentManager;
- a CNA content-pipeline architecture with a ModelProcessor and runtime Model loaders.

Those pieces do not complete this product:

- GraphicsDevice exposes only parameterless Present(). The XNA overload with optional source/destination rectangles and an override window handle is absent.
- SDL3Platform.AdoptWindowHandle currently accepts only an already existing SDL_Window pointer, not an arbitrary native WinForms HWND. It therefore cannot host CNA rendering in this original control as written.
- CNA's current model source pipeline supports glTF/glb, CNJ, and XNB routes, but not the original runtime FBX/X import contract. Converting chosen files through a different format would be a modernization, not source parity.
- Sharp Runtime has no System.Windows.Forms, System.Drawing, or Microsoft.Build object model. Its existing System.ComponentModel and IServiceProvider pieces are far smaller than the required UI/tooling stack.
- A browser cannot silently inherit the original local Windows file dialog, arbitrary FBX/X compiler, native control HWND, or System.Drawing fallback semantics.

The missing GraphicsDevice Present overload is a genuine bounded CNA API gap already identified by the engine audit, but implementing it alone cannot make this sample portable. The dominant scope is the complete desktop UI/hosting/content-authoring workflow.

## Decision required

Choose one of these explicit product boundaries under SAMPLES-DEC-005:

1. accept an evidence-backed non-port because this is a Windows/XNA design-time tool;
2. authorize a faithful Windows desktop-tool program, including an appropriate native UI/control stack, arbitrary native-window hosting, the full Present contract, FBX/X runtime content import, dialogs, error presentation, and a ruling that the mandatory browser gate is not applicable;
3. authorize an explicitly modernized cross-platform tool and specify its UI, browser file-input, supported source formats, content output, and native/browser hosting contracts.

A normal CNA Game that merely displays a precompiled bundled model, a command-line converter, or a glTF-only viewer would each omit the defining behavior and must not be called a port of SAMPLE-108.
