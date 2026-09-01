# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port has been started.** This sample
teaches a distinct complete render-target technique around 16 genuine Xbox Avatars. Most of its
ground/effect/shadow math is portable, but its two required inputs are not currently available on
the campaign renderers: the normal CNA Avatar path draws no body, and EasyGL truthfully refuses the
requested `SurfaceFormat.Alpha8` render target. Neither gap should be hidden in sample code.

Source: `/rv/tmp/XNAGameStudio/Samples/AvatarShadows_4_0/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-087-AvatarShadows_4_0/`.

## Audited original

The package is Xbox 360/HiDef only. Four runtime source units contain 437 lines; AssemblyInfo adds
34 metadata lines. The exact behavior is:

- create a 4x4 grid of 16 random Xbox Avatars, each with an independently selected one of 30
  built-in animation presets and random Y rotation;
- animate and draw every body normally on a textured 30x30 ground plane;
- compute `Matrix.CreateShadow` from the rotating light and flatten every body onto a plane offset
  by 0.001 to avoid z-fighting;
- draw those flattened bodies into a full-backbuffer `SurfaceFormat.Alpha8`/
  `DepthFormat.None` `RenderTarget2D`, deliberately using one byte per pixel;
- sample that target by screen coordinate in the original `GroundEffect.fx`, darkening ground RGB
  by 50% wherever the sampled alpha is nonzero;
- rotate the camera with the left thumbstick, the light with the right thumbstick, display the
  exact status font and exit only with GamePad Back.

The official documentation explains why all three pieces matter: the Avatar renderer does not
expose body geometry or accept a replacement effect, so the sample applies the planar transform
through `AvatarRenderer.World`, captures the ordinary Avatar draw into Alpha8, and composites it in
the ground shader. A ground-only screen or invented blob shadows would not demonstrate the sample.

The retained `scripts/build-original.sh` uses authentic XNA 4.0 Xbox reference assemblies, runs
the official Xbox360/HiDef content pipeline and compile-checks every exact unchanged source unit.
All three pipeline assets pass, including the original shader and DXT/mipmapped ground texture.
Outputs are Xbox version-5 LZX XNBs. Their SHA-256 values are:

| Output | SHA-256 |
|---|---|
| `AvatarShadows.exe` | `a0a12b358dbfd7ce445cb655feb900f6410cb596ab79e2df3f20ad89861f21d0` |
| `Font.xnb` | `6c121cfcee78c2dbe0a57f360b5d90ac1b8526d5c041df265601fdeffbd449a4` |
| `GroundEffect.xnb` | `d3a398bf6c311c6406d89b238d552ba07d347004baaa5eb59089d14c7501e1dd` |
| `ground.xnb` | `a542aa2f404961ad3dedc7a0ca0195de9d7e464a67d4eb332a77106b8da252f3` |

There is no Windows XNA project/host. Running the original body/shadow result requires an Xbox 360
runtime and retired native Avatar content, so no false desktop execution or screenshot claim is
made. The exact upstream 64x64 preview and full technique documentation remain in `xna4-original`.

## Live CNA audit: Avatar dependency

The audit used CNA commit `35268971c`. CNA exposes all ordinary non-Avatar mechanics used here:
dynamic vertex buffers, triangle strips, `Matrix::CreateShadow`, planes, render targets, compiled
effects/parameters, two sampled textures, wrapped ground sampling, title-safe area, SpriteBatch,
input and the 71-transform Avatar surface.

The normal Avatar path remains the measured off-Xbox reference stub:

- `AvatarDescription::CreateRandom` is invalid;
- all 30 selected animation values produce a zero-length, 71-zero-matrix animation;
- `AvatarRenderer` stays `Unavailable` and normal `Draw` is a no-op.

The same bounded default-surface selection passed 47/47 tests at this HEAD, covering preset-
insensitive animations, random descriptions, renderer state and draw. Consequently both the normal
body pass and the flattened-body render-target pass are empty.

CNA's opt-in `EnableRealRenderingEXT`/`DrawRealEXT` path draws a documented substitute body/clip,
not the original Xbox content. Using it would replace the source's normal XNA API and its random
appearance/preset semantics. The campaign rules require an explicit owner scope decision first.

## Live CNA audit: Alpha8 render-target dependency

The source specifically requests `SurfaceFormat.Alpha8`, not a Color target used only as an
implementation detail. EasyGL's current render-target storage mapping supports Color and half/full
float formats. Alpha8 is unmapped, so `ClassifyRenderTargetFormatEXT` defers to the framework rule
and the public constructor refuses it instead of silently allocating RGBA8.

This was verified on a real OPENGLES3 context, not inferred from source alone:

- `GraphicsCapabilityFloatRenderTargetTest.NonColourNonFloatFormatsAreNotRenderTargets` passed and
  reported only Color plus float render-target formats;
- `EasyGL_SurfaceFormat_Throws` passed 30/30 and separately confirms that EasyGL also refuses
  unsupported Alpha8 `Texture2D` storage rather than widening it silently.

The same EasyGL implementation backs required WEBGL2. A correct general fix is not just adding an
enum case: WebGL2/GLES red-only storage writes/samples the red channel, while XNA Alpha8 must retain
drawn alpha and return it through sampled alpha. The implementation needs truthful attachment
storage, shader/output routing or swizzle semantics, sampling, clear/readback behavior and real-GL
native/browser regressions. Widening to Color would lose the format's stated memory contract and is
not accepted merely to make construction pass.

Because authentic Avatar geometry is independently unavailable, an Alpha8-only fix cannot produce
or fully qualify this sample. The gap is therefore recorded for the eventual authorized Avatar
backend rather than implemented as an unobservable partial detour during this audit.

## Current result and resume conditions

No C++ source, CMake target, Color-target substitution, fake shadow/body or other workaround was
added. No CNA or sharp-runtime source changed. SAMPLE-087 remains `🛑` under `SAMPLES-DEC-004` until
the owner chooses one of these product boundaries:

1. accept this evidence-backed Xbox-only/non-port result;
2. explicitly approve CNA's substitute Avatar visual as a rules/scope exception, plus authorize
   the general normal-transform/render-target integration and truthful Alpha8 support needed to
   retain the actual technique;
3. supply or authorize a faithfully redistributable Avatar body/appearance/30-preset dataset and
   a large normal-XNA-API backend, together with the Alpha8 renderer work for OPENGLES3/WEBGL2.

If a full backend is authorized, resume with all four source units and the three exact XNBs, then
qualify 16 independently animated bodies, a nonempty Alpha8 capture, spatially matching 50%-dark
ground shadows, both camera/light rotations and clean exit on native OPENGLES3 and real-browser
WEBGL2.
