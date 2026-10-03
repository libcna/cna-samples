# Missing / Differences from XNA 4.0 original

## Re-analysis against CNA's standard avatar API — 2026-10-03

**Status: still `⛔` until the owner decides whether to reopen** (owner-requested series
SAMPLE-085/086/094/101). Nothing was ported; no sample, CNA or Sharp Runtime source changed.
Heads: CNA `next fc64a4be3`, Sharp Runtime `next db86514c`. The retained `xna4-original/` is
byte-identical to the physical upstream directory.

**What it is.** An Xbox 360/HiDef-only game (one game unit, 392 lines with metadata). One random
avatar plays Celebrate and Wave at the same time. `FindInfluencedBones(AvatarBone.ShoulderRight,
ParentBones)` collects the right-arm subtree, and each frame the sample copies Celebrate's 71
transforms and then overwrites that subtree with Wave's. It draws the composed list with
`AvatarRenderer.Draw(bones, celebrate.Expression)`. LB cycles Celebrate+Wave / Celebrate / Wave
(shown on screen), RB makes a new random avatar, the right stick and triggers move the camera, and
Back exits. Gamepad only, `GamerServicesComponent`, 1280×720 and multisampling.

**The 2026-09-09 cancellation reasons no longer hold in substance.** They were: both presets were
identical zero poses, the renderer was permanently `Unavailable`, `Draw(bones, expression)` was a
permanent no-op, and the excluded `CNAEXT` route could not take a bone list. In CNA today
(`docs/avatars.md`):
- Celebrate and Wave are real, different CNA clips on the 71-bone skeleton;
- `AvatarRenderer` loads to `Ready` and `Draw(bones, expression)` renders the supplied transforms,
  so the composed pose this lesson is about can now be drawn;
- `ParentBones` is XNA's 71-entry table and is available straight after construction
  (`AvatarRenderer.cpp:274`), as the original's `LoadContent` needs, and `AvatarBone::ShoulderRight`
  is bone 22;
- the EXT renderer is retired, so no substitute route is involved.

**Open owner decisions** are the same as SAMPLE-085's:
1. CNA avatars and CNA's Celebrate/Wave motions instead of Xbox ones, documented as in SAMPLE-087.
2. Gamepad-only input: this machine has no pad, so either a manual gate on a real pad or the
   proposed off-by-default keyboard→GamePad `CNAEXT` emulation shared by the whole series.
3. Browser bundle growth from the ~30 MB of embedded avatar catalogs.

**Estimate if reopened:** about 3 h for the port, content rebuild, native and real-Chrome gates and
gallery. If 085 is done first, its content script and gates largely carry over.

**Status: cancelled by the owner on 2026-09-09 (`⛔`). No C++ port has been started.** This is a
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

There is no Windows project or desktop XNA host. Running the original as shipped requires an
Xbox 360 runtime and its Avatar service. This workspace has no Xbox execution or capture of this
particular sample; no desktop execution or screenshot claim is made.

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

## Historical decision options before owner cancellation

No C++ source, CMake target, substitute mesh, fake pose or other workaround was added. No CNA or
sharp-runtime change was needed. Before the owner cancelled this row under `SAMPLES-DEC-004`, the
available boundaries were:

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
- The local `evidence/` holds no Xbox runtime capture; the only image in the artifact is the
  upstream sample's own documentation picture. A run of this exact sample on a functioning
  console has not been tested here, so this absence is not a claim about every possible machine.

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

No port will be produced for SAMPLE-086, on the measurements above: an Xbox 360 binary with no
working Xbox runtime reference in this workspace, a faithful draw call that is a permanent
no-op, a substitute route that cannot take a composed bone list, and preset animations that are
empty. `SAMPLE-087` and `SAMPLE-101` remain in the same position and keep their own rows.

## Current-head re-analysis — 2026-09-27

SAMPLE-085 remains cancelled; this pass reviewed SAMPLE-086 without changing the owner's decision.
All **nine** physical files in the Microsoft upstream directory match the retained
`xna4-original/` snapshot byte-for-byte. The one game source still has 392 lines; the project has
only Xbox 360/HiDef configurations and references `Microsoft.Xna.Framework.Avatar`. The retained
executable and official `XNBx` Font XNB still have the SHA-256 values recorded above. The exact
file inventory, hashes and repository heads are in
`/rv/tmp/samples/SAMPLE-086-AvatarMultipleAnimationsSample_4_0/evidence/current-head-analysis-20260927/inventory.json`.

When the renderer reports `Ready`, the sample updates both `Celebrate` and `Wave`. In combined
mode it takes Celebrate's 71 bone matrices and replaces the **24 indices** in the right-arm subtree
rooted at `ShoulderRight` (22) with Wave's values; the other two modes use one full pose each.
Its only draw passes those composed matrices and Celebrate's expression to normal
`AvatarRenderer.Draw`.
Current CNA (`next 629554a95`) still provides the exact 71-entry parent table, but normal
`AvatarAnimation` initializes both presets to zero matrices of zero length, normal
`AvatarRenderer.State` remains `Unavailable`, and its 71-matrix `Draw` overload validates then
does nothing. Consequently the source's `State == Ready` guard never runs its composition branch.
`DrawRealEXT` computes bones internally from one substitute clip name and time, so it cannot consume
the composed matrix list. This confirms the distinct API/data gap without adding a workaround.

The local lack of a runtime image does **not** establish that no reference can be captured on a
working Xbox 360. That broader claim in the 2026-09-09 note was unsupported. This analysis did not
run the console build, change CNA or Sharp Runtime, or start a port. Status remains `⛔`.
