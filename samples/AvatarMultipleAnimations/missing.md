# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port has been started.** This is a
distinct sample, not a duplicate of SAMPLE-085: it demonstrates bone-subtree masking by running
two Xbox Avatar preset animations simultaneously. Its sample-owned transform-composition algorithm
is portable, but the visible result still requires Microsoft's genuine Xbox Avatar body and preset
clips. CNA's normal XNA Avatar route intentionally cannot provide those off Xbox, and substituting
the `CNAEXT` character would violate the campaign rules without an owner-approved scope change.

Source: `/rv/tmp/XNAGameStudio/Samples/AvatarMultipleAnimationsSample_4_0/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-086-AvatarMultipleAnimationsSample_4_0/`.

## Audited original

The package is Xbox 360/HiDef only. Its one 392-line game source implements:

- a random `AvatarDescription`, genuine `AvatarRenderer` body and the built-in `Celebrate` and
  `Wave` `AvatarAnimation` presets;
- a 71-matrix mutable output list initialized to identity;
- `FindInfluencedBones`, which starts at `AvatarBone.ShoulderRight` and follows
  `AvatarRenderer.ParentBones` to select the complete right-arm subtree;
- combined playback that copies `Celebrate` over the whole body and overwrites only that subtree
  with `Wave`, plus independent Celebrate-only and Wave-only modes;
- left-shoulder mode cycling, right-shoulder creation of a new random avatar, right-stick
  orbit/reset, trigger zoom and GamePad Back exit;
- 1280x720 multisampled rendering and an exact Segoe UI Mono SpriteFont status legend.

This independently matters because SAMPLE-085 interpolates two whole-pose animations over time,
whereas SAMPLE-086 composes two simultaneous poses spatially by skeleton ancestry. The upstream
documentation explicitly defines that teaching goal; the retained 64x64 preview shows the genuine
Xbox Avatar result.

The retained `scripts/build-original.sh` uses the authentic XNA 4.0 Xbox reference assemblies,
runs the official content pipeline for Xbox360/HiDef, and compile-checks the exact unchanged source.
Both operations pass. The resulting PE32 executable has SHA-256
`4aa3a31ceca106e058cb2186fa53c225d3480534f584e20b018463aaa6c1fc9a`; `Font.xnb` has
SHA-256 `8ae963c642fb23e02907790e9a0bb4186a66b0decc029b5433221e3cecff77fa`, the Xbox platform
marker and the expected `SpriteFontReader` graph.

There is no Windows project or desktop XNA host. Running the original requires an Xbox 360 runtime
and its retired native Avatar delivery stack, so no false desktop execution or screenshot claim is
made.

## Live CNA audit

The dependency audit used CNA commit `35268971c`. The unique sample-owned mechanics can be ported
without a framework workaround: CNA exposes the exact 71-entry `ParentBones` hierarchy (including
`ShoulderRight = 22`), `AvatarBone`, read-only transform collections, mutable matrices, expression,
input, camera and renderer properties. The composition loops and descendant search are ordinary
C++ collection logic.

The actual two poses and visible body are unavailable through the normal XNA route:

- `AvatarDescription::CreateRandom` returns the reference stub's invalid 1,021-byte description;
- `AvatarAnimationPreset::Celebrate` and `Wave` both yield the same 71 zero matrices, neutral
  expression and zero length;
- `AvatarRenderer` stays `AvatarRendererState::Unavailable`, so the source never enters its
  transform update branch;
- `AvatarRenderer::Draw` validates the 71 transforms and then draws nothing.

The focused default-surface regression selection passed 47/47 tests, including exact parent-bone
values, both preset-insensitive animation behavior, permanent unavailable state and no-op draw.
Thus a literal C++ translation would display only the status text on Cornflower Blue and teach none
of the advertised animation-composition behavior.

## Why the extension is not a faithful shortcut

CNA's `AvatarRenderer::EnableRealRenderingEXT`/`DrawRealEXT` path uses substitute CNA-generated
meshes and clips. `docs/avatar-real-rendering-ext.md` explicitly says it is not a reproduction of
Microsoft's Xbox Avatar art or service. More importantly for this sample, `DrawRealEXT` accepts a
clip name/time rather than the original `Draw(finalBoneTransforms, expression)` result, so merely
selecting the extension would also bypass the sample's defining 71-bone subtree composition.

Making the substitute demonstrate the original algorithm would require additional non-XNA
extension work for externally composed bone transforms plus alternate body/clip content. That is a
large platform/backend and scope decision, not a sample-local fix. No such workaround was added.

## Current result and resume conditions

No C++ source, CMake target, substitute mesh, fake pose or other workaround was added. No CNA or
sharp-runtime change was needed. SAMPLE-086 remains `🛑` under `SAMPLES-DEC-004` until the owner
chooses one of these boundaries:

1. accept this evidence-backed Xbox-only/non-port result;
2. explicitly approve the non-authentic CNA Avatar as a deliberate rules/scope exception and
   authorize the additional normal-transform drawing capability needed to retain the sample's
   actual subtree-composition lesson;
3. supply or authorize a faithfully redistributable Xbox-Avatar-equivalent body/material/
   appearance and Celebrate/Wave dataset, then authorize a large normal-XNA-API backend for native
   and WEBGL2.

If a rendering backend is authorized, resume by translating the exact 392-line source and one exact
XNB, then qualify all three playback modes, the right-arm-only overwrite, random-avatar replacement
and camera controls on native OPENGLES3 and real-browser WEBGL2.
