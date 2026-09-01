# SAMPLE-109 — WinFormsGraphicsSample_4_0 audit

**Status:** owner decision required under SAMPLES-DEC-005. This is a Windows desktop hosting demonstration composed of real WinForms controls, not an XNA Game. No Game wrapper, merged single viewport, fake controls, or alternate UI was added.

## Classification and exact behavior

The original solution contains a .NET 4 x86 WinExe, an XNA game-library/content-reference project, and an XNA content project. Its 1,414 project/source lines demonstrate two independent GraphicsDeviceControl instances embedded in a split WinForms form:

- SpriteFontControl is event-driven. It constructs ContentManager from the control service container, loads hudFont through the normal XNA content route, and draws the documented multiline message only in response to WinForms paint/invalidation.
- SpinningTriangleControl is continuously animated. Application.Idle invalidates it; Stopwatch supplies time; BasicEffect and DrawUserPrimitives render one yaw/pitch/roll triangle.
- Three DropDownList ComboBoxes select named System.Drawing colors, convert their RGB bytes to XNA Color, and mutate the three live VertexPositionColor values. The initial choices are Chartreuse, Coral, and Cornsilk.
- Both controls share one reference-counted IGraphicsDeviceService. Each sets a control-sized Viewport and calls Present with a source rectangle plus its own HWND.
- The service demand-grows the shared backbuffer, exposes device lifecycle events, handles Reset/status, and disposes the device after the final control releases it.
- GraphicsDeviceControl uses System.Drawing to paint designer/device-loss error text and suppresses normal background paint to avoid flicker.

GraphicsDeviceControl, GraphicsDeviceService, and ServiceContainer are the same stock hosting implementation as SAMPLE-108 except for their namespace. The demonstrated product is nevertheless distinct: SAMPLE-108 is an arbitrary runtime FBX/X compiler/viewer with one graphics control, while SAMPLE-109 specifically demonstrates two controls sharing one device, WinForms repaint versus idle animation, live GDI-to-XNA color selection, and ordinary prebuilt ContentManager content.

The exact source snapshot is retained at:

    /rv/tmp/samples/SAMPLE-109-WinFormsGraphicsSample_4_0/original/

## Authentic Windows/XNA evidence

The unchanged solution built successfully in the offline Windows 7 VM with .NET 4 and XNA Game Studio 4.0:

    MSBuild.exe WinFormsGraphicsDevice.sln /t:Rebuild
        /p:Configuration=Release;Platform=x86 /m:2

The official pipeline produced:

    hudFont.xnb  2932 bytes
    SHA-256      a210c5efa16e0d2888719a054c1e2c3d9355f7173c26f9a91972548cd79849bb

The application launched and the SpriteFont/ContentManager control painted its CornflowerBlue background and original white message. The three original color selectors also appeared with their expected initial values.

The prepared VirtualBox/VBoxSVGA reference did not provide a clean success result for the second multi-control presentation path: after the activation overlays were removed, the application was restarted, left running, foregrounded, and resized, but the right GraphicsDeviceControl remained black instead of showing its expected CornflowerBlue rotating triangle. The process remained healthy. SAMPLE-108's single-control override-window path rendered successfully in the same VM, so this failure is narrowly associated with the two-control/shared-device/override-target path or the VM display driver. It is recorded as a reference-environment limitation, not misreported as a successful triangle render and not treated as authority to remove that behavior.

All VM network adapters remained disabled. The VM was shut down normally and the temporary shared folder was removed.

Evidence:

    /rv/tmp/samples/SAMPLE-109-WinFormsGraphicsSample_4_0/evidence/xna4-build.log
    /rv/tmp/samples/SAMPLE-109-WinFormsGraphicsSample_4_0/evidence/xna4-two-controls.png
    /rv/tmp/samples/SAMPLE-109-WinFormsGraphicsSample_4_0/evidence/xna4-two-controls-resized.png
    /rv/tmp/samples/SAMPLE-109-WinFormsGraphicsSample_4_0/win7-export/hudFont.xnb

## Live CNA and Sharp Runtime boundary

Live CNA already supplies the ordinary XNA rendering/content pieces used by the controls: SpriteFont, SpriteBatch, ContentManager, BasicEffect, DrawUserPrimitives, GraphicsDevice construction, status, Reset, Viewport, events, IGraphicsDeviceService, and PresentationParameters.DeviceWindowHandle.

The faithful hosting product still has major missing boundaries:

- GraphicsDevice exposes only parameterless Present(), not the XNA source/destination/override-window overload used for every control paint.
- SDL3Platform.AdoptWindowHandle accepts only an existing SDL_Window pointer, not an arbitrary native WinForms HWND.
- Sharp Runtime has no System.Windows.Forms or System.Drawing implementation. The form, splitter, control lifecycle, paint/invalidation semantics, idle event, comboboxes, named GDI colors, designer container, and error fallback are therefore absent.
- The mandatory browser target needs an explicit UI/component and multi-surface hosting contract. An HTML canvas rewrite cannot silently be called the original WinForms control sample.

Unlike SAMPLE-108, this row does not need Microsoft.Build runtime APIs or an FBX/X importer: the only content is an ordinary SpriteFont that CNA can represent. That makes the content side bounded, but it does not remove the complete WinForms/native-control hosting decision.

## Decision required

Choose one of these boundaries under SAMPLES-DEC-005:

1. accept an evidence-backed non-port because this is a Windows/XNA WinForms integration sample;
2. authorize a faithful Windows desktop implementation with a real control toolkit, arbitrary native-control graphics hosting, full Present overload, shared-device behavior, and a native-only/browser-gate ruling;
3. authorize an explicitly modernized cross-platform component demo and define its UI controls, event/paint semantics, multi-surface rendering model, and browser acceptance behavior.

A single CNA Game window that draws similar text and a triangle would omit the defining hosting, lifecycle, multiple-control presentation, and WinForms interaction behavior.
