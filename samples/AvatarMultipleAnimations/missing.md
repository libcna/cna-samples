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

---

## Re-audited 2026-09-09: same boundary as SAMPLE-085, and one obstacle further

Measured rather than restated. Everything that closed `SAMPLE-085` holds here identically:

- **One project**, `AvatarMultipleAnimationsSampleXbox.csproj`, `XnaPlatform` **Xbox 360**,
  `XnaProfile` HiDef, and one solution named `(Xbox)`. No Windows project.
- The built `xna4-build/bin/AvatarMultipleAnimationsSample.exe` is a PE32 .NET assembly on
  **CLR v2.0.50727**, the Xbox 360 Compact Framework, referencing
  `Microsoft.Xna.Framework.Avatar`. It cannot start on Windows, on Wine, or on the XNA 4.0 runtime
  installed here.
- `evidence/` holds no capture and never could; the only image in the artifact is the upstream
  sample's own documentation picture.

**What makes this one further from portable than SAMPLE-085.** The lesson here is composing one
pose out of two: take Celebrate's bones for the body, then overwrite the right-arm subtree with
Wave's. The XNA call that draws such a composition is
`AvatarRenderer::Draw(const std::vector<Matrix>& bones, AvatarExpression)`, and that overload is a
permanent no-op by design. The `CNAEXT` route that does render is
`DrawRealEXT(const std::string& animationClipName, System::TimeSpan position, bool loop)` — it takes
**a clip name and a playback position, not a bone list**. So even the substitute path the campaign
rules exclude could not express this sample's output; it can only play a whole named clip.

**And the input is empty as well.** `AvatarAnimation`'s constructor allocates 71 bones and sets
`length_` to `TimeSpan::Zero`; the bone matrices are default-constructed, which in this project is
all zeros rather than identity. Celebrate and Wave are therefore the same zero pose of zero length,
so the blend has nothing to blend even before the draw discards it.

**The portable half is genuinely portable, and alone.**
`AvatarRenderer::getParentBonesProperty()` exposes the authentic 71-entry parent table decoded from
the reference assembly, so the sample's right-arm subtree discovery would work exactly as written.
It would walk a real hierarchy to select bones from empty animations and hand the result to a
no-op.

## CANCELLED by the owner, 2026-09-09

No port will be produced for SAMPLE-086, on the measurements above: an Xbox 360 binary for a runtime
that shipped only on that console, no obtainable reference, a faithful draw call that is a permanent
no-op, a substitute route that cannot take a composed bone list, and preset animations that are
empty. `SAMPLE-087` and `SAMPLE-101` remain in the same position and keep their own rows.
