# Missing / Differences from XNA 4.0 original

**Status: cancelled by the owner on 2026-09-09. No C++ port has been started.** The sample's
defining output is a genuine Xbox LIVE Avatar rendered and animated by Microsoft's Xbox 360 Avatar
service/content stack. CNA's ordinary XNA-shaped Avatar API intentionally preserves the unavailable
off-Xbox reference-assembly behavior, while its opt-in `CNAEXT` renderer draws a documented
substitute body. Using that extension would not be a faithful port and is forbidden without an
explicit owner-approved scope change.

Source: `/rv/tmp/XNAGameStudio/Samples/AvatarAnimationBlendingSample_4_0/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-085-AvatarAnimationBlendingSample_4_0/`.

## Current-head re-analysis — 2026-09-27

All 10 files in the physical upstream directory remain byte-identical to `xna4-original/`.
The source contains one Xbox 360/HiDef project and no Windows project. The retained original
Xbox build and official `Font.xnb` still have SHA-256 values
`1ab1bda55b2bc264ecdce9b96ccf3e12ee84458dbe8b2b2f71d4d1e75e16dec2` and
`8ae963c642fb23e02907790e9a0bb4186a66b0decc029b5433221e3cecff77fa`
respectively.
`evidence/current-head-analysis-20260927/inventory.json` records every upstream hash and the
current dependency heads (CNA `629554a95`, Sharp Runtime `9e58c955`).

The unchanged source still requires a genuine Avatar body, the `Stand0`, `Celebrate`, `Clap`
and `Wave` preset transforms, and the sample-owned 250 ms blend of 71 bone matrices. Current
CNA's ordinary `AvatarDescription::CreateRandom()` still returns an invalid zero description;
`AvatarAnimation` still has zero-valued matrices and duration; `AvatarRenderer::State` remains
`Unavailable`, and its normal `Draw` validates but draws nothing. These are the documented
Windows XNA Avatar defaults, not a functioning Xbox Avatar backend: Microsoft's
[AvatarRenderer](https://learn.microsoft.com/en-us/previous-versions/windows/xna/dd940226(v=xnagamestudio.41))
and [AvatarAnimation](https://learn.microsoft.com/en-us/previous-versions/windows/xna/dd940207(v=xnagamestudio.41))
references explicitly say the Windows methods return defaults and do not render. CNA's separate
`EnableRealRenderingEXT`/`DrawRealEXT` path uses a replacement model and clip dataset, so it
cannot qualify this original sample without an owner-approved scope change. The historical 47
focused tests were not rerun because no dependency or sample source was changed in this audit.

One historical claim below is too absolute: **this workspace has no Xbox 360 runtime/console
reference run**, but that does not prove a capture is impossible on every machine. Microsoft's
[Xbox 360 preservation notice](https://news.xbox.com/en-us/2023/08/17/xbox-360-store-will-close-july-2024/)
states that Xbox 360 games remain playable after the store closure. Whether this particular
development sample and its Avatar content can be run on a working console today was not tested.
The prior owner's non-port decision remains `⛔`: there is still no authentic body/preset dataset
or faithful CNA backend, and no substitute port was created.

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
on Windows/Wine without an Xbox 360 runtime, which is unavailable here. The exact upstream
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
workaround was added. No CNA or sharp-runtime change was needed. At the time of this section,
SAMPLE-085 remained `🛑` under `SAMPLES-DEC-004`; the owner later chose the non-port boundary
and marked it `⛔`. The historical options were:

1. accept this evidence-backed Xbox-only/non-port result;
2. explicitly approve the documented CNA substitute visual as a deliberate rules/scope exception;
3. supply or authorize a faithfully redistributable Avatar body/material/appearance and preset-clip
   dataset, then authorize the large normal-XNA-API backend needed to make random descriptions,
   animation presets, renderer state and drawing meaningful on native and WEBGL2.

If option 3 is selected, resume with the normal `AvatarDescription`/`AvatarAnimation`/
`AvatarRenderer` API rather than adding sample-specific calls, port both original source units,
retain the exact XNB, and qualify the same four animations and 250 ms 71-bone transitions on native
OPENGLES3 and real-browser WEBGL2.

---

## Re-audited 2026-09-09: measured rather than restated

The audit's conclusion holds, and the measurements below make the boundary sharper than "choose one
of three options".

**The sample is Xbox 360 only.** It ships **one** project,
`AvatarAnimationBlendingSampleXbox.csproj`, with `<XnaPlatform>Xbox 360</XnaPlatform>` and
`<XnaProfile>HiDef</XnaProfile>`, and one solution named `(Xbox)`. There is no Windows project.
That is unlike SAMPLE-072, which ships a Windows and an Xbox project over identical sources, and
unlike SAMPLE-084, which has a Windows branch that runs here.

**The built executable is an Xbox binary.** `xna4-build/bin/AvatarAnimationBlendingSample.exe` is a
PE32 .NET assembly on **CLR v2.0.50727** — the Xbox 360 Compact Framework — referencing
the Xbox-targeted `Microsoft.Xna.Framework.Avatar` from the retained `xbox-refs/`.
Microsoft also shipped a Windows assembly of that name, but its Avatar methods are documented
as defaults/no-ops. This Xbox-targeted binary has no supported Windows/Wine execution path in
this workspace.

**No reference capture was obtained in this workspace.** `evidence/` holds no Xbox screenshot:
the required console/runtime is not present here. This does not establish that a working Xbox 360
could never run the sample; see the current-head correction above. Every
other sample audited so far has had a reference, or — as with SAMPLE-082 — a recorded diagnostic
boundary standing in for one. Here there is not even a boundary to record, because the binary will
not start.

**CNA's Avatar API is faithful to the Windows XNA assemblies, which is true and beside the point.**
`AvatarDescription::CreateRandom` returns an all-zero description and `AvatarRenderer` forces itself
to `Unavailable` on every read, both marked in the source as decoded from the real reference
assembly rather than guessed — as is the 71-entry parent-bone table. Windows XNA does the same. But
the sample never ran on Windows XNA either, so matching it proves nothing about this sample's
output.

### The shape of the decision

The defining output — Microsoft's proprietary Avatar body driven by four built-in 71-bone presets —
was supplied by the Xbox 360 runtime, which is not available in this workspace. No Xbox reference
run was obtained here. The only CNA route that draws anything is
the `CNAEXT` substitute body, which is explicitly not the original's output and which the campaign
rules exclude.

`SAMPLE-086`, `SAMPLE-087` and `SAMPLE-101` are in exactly the same position: one project each,
`Xbox 360` only. `SAMPLE-094` is **not** — it ships four projects including a Windows one, so it can
at least be built and run against the Windows Avatar API, and should be judged separately.

## CANCELLED by the owner, 2026-09-09

No port will be produced for SAMPLE-085. The decision follows the measurements above: the sample is
an Xbox 360 binary for a runtime not available in this workspace, no local reference capture was
obtained, and the only CNA route that draws anything is a substitute body the campaign rules
exclude.

`SAMPLE-086`, `SAMPLE-087` and `SAMPLE-101` sit in the same position and keep their own rows.
`SAMPLE-094` does not — it ships a Windows project as well and is to be judged separately.
