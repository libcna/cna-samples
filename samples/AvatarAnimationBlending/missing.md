# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port has been started.** The sample's
defining output is a genuine Xbox LIVE Avatar rendered and animated by Microsoft's Xbox 360 Avatar
service/content stack. CNA's ordinary XNA-shaped Avatar API intentionally preserves the unavailable
off-Xbox reference-assembly behavior, while its opt-in `CNAEXT` renderer draws a documented
substitute body. Using that extension would not be a faithful port and is forbidden without an
explicit owner-approved scope change.

Source: `/rv/tmp/XNAGameStudio/Samples/AvatarAnimationBlendingSample_4_0/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-085-AvatarAnimationBlendingSample_4_0/`.

## Audited original

The physical package is Xbox 360/HiDef only. Its two game source files contain 533 lines and retain
the following product behavior:

- create a random `AvatarDescription` and render its real Xbox LIVE Avatar body;
- create the built-in `Stand0`, `Celebrate`, `Clap` and `Wave` `AvatarAnimation` presets;
- blend all 71 bones over 250 ms in the sample-owned `IAvatarAnimation` implementation, using
  `Quaternion.Slerp` for rotation, `Vector3.Lerp` for translation and reconstructed matrices;
- switch animations with A/B/X/Y, blend back to Stand with either shoulder button, and orbit/zoom
  the camera with the right stick and triggers;
- draw the original control legend with its one Segoe UI Mono SpriteFont at 1280x720 with
  multisampling and `GamerServicesComponent`.

The retained `scripts/build-original.sh` copies the authentic XNA 4.0 Xbox reference assemblies,
runs the official content pipeline for Xbox360/HiDef, and performs a compile-only build of the exact
unchanged source. Both operations pass. The resulting PE32 executable has SHA-256
`1ab1bda55b2bc264ecdce9b96ccf3e12ee84458dbe8b2b2f71d4d1e75e16dec2`; the exact `Font.xnb`
has SHA-256 `8ae963c642fb23e02907790e9a0bb4186a66b0decc029b5433221e3cecff77fa` and the expected Xbox
platform marker and `SpriteFontReader` object graph.

There is no Windows project or desktop XNA host for this package. Its executable cannot be run
without an Xbox 360 runtime and the retired native Avatar delivery system. The exact upstream
64x64 preview and documentation remain in `xna4-original/`; they visibly establish that the
demonstrated body is the proprietary Xbox Avatar, not an arbitrary 71-bone model. No false original
runtime or screenshot claim is made.

## Live CNA audit

The dependency audit used CNA commit `35268971c`. CNA already exposes every XNA-shaped API needed
by the sample-owned blending algorithm: `IAvatarAnimation`, the 71-bone collections,
`ReadOnlyCollection.CopyTo`, `Quaternion::Slerp`, `Vector3::Lerp`, matrix translation,
`TimeSpan`, `AvatarAnimation`, `AvatarDescription` and `AvatarRenderer`. The blend algorithm itself
therefore does not justify a sample workaround or a new Sharp Runtime feature.

The ordinary Avatar implementation deliberately represents the unavailable off-Xbox contract:

- `AvatarDescription::CreateRandom` returns the reference stub's invalid 1,021-byte description;
- preset `AvatarAnimation` construction produces 71 zero matrices and a zero duration;
- `AvatarRenderer` remains `AvatarRendererState::Unavailable`, has no bind pose and draws nothing;
- `AvatarRenderer::Draw` validates its inputs but is otherwise a no-op.

The focused default-surface regression selection passed 47/47 tests. This verifies that the
observed result is the current intentional contract rather than an untested missing method. Two
broader extension-constructor cases require a video display and were deliberately excluded from
that bounded headless run; they are unrelated to the default-stub conclusion.

## Why CNA's extension is not the original product

CNA also has `AvatarRenderer::EnableRealRenderingEXT`, `DrawRealEXT`, `SkinnedModelEXT` and an
avatar asset conversion pipeline. `docs/avatar-real-rendering-ext.md` explicitly describes that
route as a CNA-owned replacement using substitute generated male/female meshes and animation
clips, not a reproduction of Microsoft's Xbox Avatar art or service.

Selecting it would require changing the original source to invoke non-XNA APIs, supplying alternate
body/clip content and using a different draw path. It would preserve only the broad idea of a
skinned character. The campaign rules prohibit such a `CNAEXT`/content workaround, and the
historical owner decision recorded in `ignored.md` already rejected this particular substitute as
not faithful enough. The fresh audit records that history but does not infer a new permanent-skip
decision for the current plan.

## Current result and resume conditions

No C++ source, CMake target, substitute mesh, generated clip, fake Avatar service or other
workaround was added. No CNA or sharp-runtime change was needed. SAMPLE-085 remains `🛑` under
`SAMPLES-DEC-004` until the owner chooses one of these boundaries:

1. accept this evidence-backed Xbox-only/non-port result;
2. explicitly approve the documented CNA substitute visual as a deliberate rules/scope exception;
3. supply or authorize a faithfully redistributable Avatar body/material/appearance and preset-clip
   dataset, then authorize the large normal-XNA-API backend needed to make random descriptions,
   animation presets, renderer state and drawing meaningful on native and WEBGL2.

If option 3 is selected, resume with the normal `AvatarDescription`/`AvatarAnimation`/
`AvatarRenderer` API rather than adding sample-specific calls, port both original source units,
retain the exact XNB, and qualify the same four animations and 250 ms 71-bone transitions on native
OPENGLES3 and real-browser WEBGL2.
