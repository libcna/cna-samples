# SAMPLE-108 — WinFormsContentSample_4_0 audit

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
