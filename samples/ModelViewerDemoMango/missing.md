# SAMPLE-132 — `ModelViewerDemo_4_0_Mango` audit and owner decision

## Status

Fresh audit complete enough to require a Windows Phone Silverlight/XNA product decision under
`SAMPLES-DEC-005`. This is a complete hybrid application whose defining behavior is the integration
of Silverlight UI and XNA 3D rendering. No standalone `Game`, reduced tank viewer, fake page menu or
HTML/CSS substitution was introduced.

## Complete product inventory

The upstream delivery contains 47 files / 17,950,150 bytes:

- a four-project Visual Studio 2010 solution: Windows Phone 7.1 Silverlight application, XNA Phone
  content library, custom Windows/x86 content-pipeline assembly and content project;
- 21 C# files / 2,077 lines and eight XAML files / 330 lines;
- seven navigable Phone pages plus `App.xaml`, with 12 buttons, 10 checkboxes, six sliders and 16
  two-way data bindings;
- a custom `SkyProcessor`/`SkyContent`, runtime sky class, tank renderer, renderer state, three light
  states, frame-rate counter and shared-graphics lifetime manager;
- one FBX tank, two model textures, sky and ground bitmaps, one SpriteFont description, shell
  artwork, HTML product documentation and the Microsoft Permissive License.

The documentation explicitly calls this a complex Silverlight/XNA minigame/application rather than
a beginner rendering sample. The Phone 7.1 manifest names `MainPage.xaml` as its entry page, declares
Silverlight runtime identity and eleven Phone capabilities.

## Audited application behavior

`App.xaml` creates three ordered lifetime objects: XNA's `SharedGraphicsDeviceManager`, the
sample's `SharingModeManager`, then `Renderer`. Activation/deactivation toggles graphics-device
sharing mode. A separate `GameTimer` pumps `FrameworkDispatcher`, while each active page runs a
333,333-tick update/draw timer.

Every page derives from `PhoneApplicationPage` through `BasePage`. It renders the complete live
Silverlight page into a 480×800 `UIElementRenderer` texture, draws the XNA scene, then composites
that UI texture through SpriteBatch with a half-second fade-in. The seven-page flow is:

1. Main: choose Free Look or Edit Settings.
2. Free Spin: one-finger free-drag orbits the camera; pinch clamps distance to 1–10.
3. Edit: navigate to Rendering, Animation or Lights.
4. Rendering: toggle lighting, texture, wireframe and FPS/UPS display.
5. Animation: independently toggle wheels, steering, turret, cannon and hatch.
6. Lights: select any of three directional lights or reset all defaults.
7. Light Editor: enable/reset one light and edit diffuse/specular RGB with sliders.

Navigating between views drives camera position and target through six dependency-property
animations in two 0.5-second quadratic ease-in/out Storyboards. This is not equivalent to replacing
the pages with hardcoded SpriteBatch text.

The XNA scene draws an uncompressed custom-processor skydome, a 100×100 tiled ground plane and the
tank. Nine named tank bones animate four wheels, two steering pivots, turret, cannon and hatch. The
renderer supports textured/untextured, lit/unlit and solid/wireframe branches, three editable
default lights, five independent time functions and an optional red FPS/UPS overlay.

## Authentic reference and build boundary

The exact `Debug|Mixed Platforms` solution was rebuilt in the owner-supplied offline Win7 VM. The
sample-owned custom pipeline assembly compiled successfully, but the application project stopped on
the absent:

```text
Microsoft.Silverlight.WindowsPhone71.Overrides.targets
```

The exact solution mapping also asks the content project to clean without a target platform when
rebuilding the Phone content library. That project-mapping diagnostic is retained, but is not used
to claim a content failure: the four unchanged content entries were built separately through the
official XNA 4 WindowsPhone/Reach `BuildContent` task. The VM was shut down normally and all eight
network adapters remained `none`.

No Phone 7.1 SDK/application host exists in the Wine prefix or VM, and the archive ships no XAP or
reference capture. The two collection thumbnails are retained as documentation, not presented as a
runtime execution. A full original interaction/visual capture therefore remains unavailable.

## Measured CNA/content boundary

The exact sample-owned `SkyProcessor` is byte-for-byte the already completed SAMPLE-012 processor
after changing only its namespace; its `SkyContent` differs only in namespace/runtime type, and the
input `sky.bmp` is byte-identical. The official Phone/Reach pipeline emits six `XNBm` products:

- `sky.xnb`, containing `ReflectiveReader<ModelViewerDemo.Sky>`, a generated Model and uncompressed
  Texture2D through three shared resources;
- `tank.xnb` plus two external tank texture XNBs;
- `rocks.xnb` and `FpsFont.xnb`.

Live CNA at `7712534d3d22` losslessly converts and validates all five stock Model/Texture2D/SpriteFont
items. Generic CNB transcoding honestly refuses the sample-defined reflective Sky type. This is not
a new design-time or geometry blocker: a faithful port would retain the exact XNB and add the same
closed two-field AOT reader already used for SAMPLE-012. That reader was not added without an
approved runnable product.

The live runtime passes 32/32 focused BasicEffect/default-light tests, 28/28 applicable
rasterizer/wireframe tests (the one opposite-backend refusal case skips on OPENGLES3), and 21/21
focused Model/Texture2D/SpriteFont/reflective/shared-resource tests. CNA and Sharp Runtime contain
none of the product-defining `SharedGraphicsDeviceManager`, `UIElementRenderer`, `SetSharingMode`,
`PhoneApplicationPage`, `PhoneApplicationFrame`, `DependencyObject` or `Storyboard` facilities.
The missing scope is therefore the hybrid application model, not ordinary tank rendering or content.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-132-ModelViewerDemo_4_0_Mango/`.

- `xna4-original/` is the complete byte-for-byte upstream snapshot;
- `evidence/original-sha256.txt` and the audit assertions prove all 47 inputs match upstream;
- `evidence/win7-msbuild.log`, `win7-result.txt` and `win7-final-state.txt` retain the exact original
  build and poweroff/network state;
- `xna4-build/Content-phone/` retains all six official Phone XNBs and
  `evidence/xna4-content-sha256.txt` pins their hashes;
- `cna-diagnostic/` retains the five validated stock CNBs, while
  `evidence/cna-content-transcode.log` retains the exact custom-reader refusal;
- three CNA logs retain the 32 BasicEffect, 28 applicable rasterizer and 21 content passes;
- `scripts/build-original-content.sh` and `qualify.sh` reproduce the offline content and live CNA
  evidence under isolated Xvfb. CNA conversion is capped at eight workers;
- `scripts/win7-build-original.cmd` is the exact credential-independent in-guest build command.

## Owner decision required

Choose one:

1. authorize a faithful Phone 7.1 compatibility product, including the reusable Silverlight/XNA
   shared-device, UI-to-texture, XAML/navigation, dependency-property/Storyboard and lifecycle
   subsystems before translating all seven pages;
2. explicitly authorize a complete native/WEBGL2 modernization and define the replacement UI and
   lifecycle contract, while preserving every page, binding, transition, touch gesture, render
   setting, animation, light editor and exact XNA content behavior;
3. accept an evidence-backed historical Phone application non-port/support boundary, retaining its
   complete source, content processors/assets and documentation.

Until that ruling, a tank-only `Game`, one-screen checkbox demo, removed Silverlight overlay or
manual camera/render controls would be a different product and violate the campaign's fidelity
rules.
