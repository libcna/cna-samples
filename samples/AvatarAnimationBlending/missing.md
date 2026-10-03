# Missing / Differences from XNA 4.0 original

## Published and pruned on the owner's request — 2026-10-03

The owner requested "vse pushni prorez 85 86 101 09" and explicitly clarified the fourth number
as SAMPLE-094 CustomAvatarAnimation. CNA `next 2b4ff28d7`, samples `develop 23e8ad7` and gallery
`main 4debda9` were pushed; Sharp `next db86514c` was already synchronized. The complete four-game
gallery is now 89 entries. Initial local-only/no-cleanup statements below are historical.

This sample's exact root `/rv/tmp/samples/SAMPLE-085-AvatarAnimationBlendingSample_4_0/` was pruned with
`tools/prune-completed-sample.sh --apply` after a reviewed dry run. Source snapshots, original
products, official content, native/web products, scripts and all qualification evidence remain.
`evidence/prune-20261003/{retained-before,retained-after,hash-verification}.json` proves all
retained file hashes match except the intentionally stripped native executable:
`347bf347bba81369a56ea61dfac35b0089c82dae92b97dd0cad34e325c929ecb` →
`b0df8e6b8b3c047248050bdd96aa85eedfe2045848f8f1694ea26f953424fcb1`. Its full native interaction/animation/camera/appearance
gate passed again with GamePad Back exit 0; see `native/` and `native-gate.log` in that evidence.
The second dry run reports zero removable paths. The four-root apply freed about 2.7 GiB.

`MANIFEST.md` retains exact restoration commands and links the archived previous manifest.
Both native/web build scripts now restore their own verified FNA3D/MojoShader source archive at
pin `32401479a3ab5bd6b2e7f786e87bf4166aa03b0f`; no rebuild depends on SAMPLE-086's removed build
directory. Complete extraction hashes and the MojoShader submodule pin were roundtrip-verified.
Products and game behavior are unchanged. Publication SHA-256 verification is retained in
SAMPLE-094's `evidence/publication-20261003/hosted-hashes.json`.

## Completed faithful port with owner-approved avatar/input differences — 2026-10-03

**Current status: `✅`.** The owner resumed the handoff, confirmed the series 085 → 086 → 101 →
094 (`CustomAvatarAnimation`), and approved the shared keyboard GamePad layout. The Xbox-only
non-port decision below is superseded. [diff.md](diff.md) records CNA's original avatar artwork/
preset timing, the one-line off-by-default shared input opt-in and C++ ownership/live-array mechanics.

All ten upstream files match the retained `xna4-original/` byte-for-byte. Both original C# game
units and assembly metadata were reviewed against the three C++ units. The unchanged original
build succeeds against genuine Xbox XNA 4.0 references; the fresh executable hash is
`119e1ad688983a7348788b2576c39d0d9cd4db283b09919a164f3ac18ddcf27d`
(the executable metadata changes across builds), and the original Xbox Font remains
`8ae963c642fb23e02907790e9a0bb4186a66b0decc029b5433221e3cecff77fa`.
There is no Xbox runtime/reference capture here. The original preview is supporting documentation,
not a screenshot of a local original run. The sample contains no audio or second product.

The shipped 31,918-byte Windows/HiDef `Font.xnb` is byte-identical to the official XNA pipeline
output from the unchanged font/importer/processor/identifier:
`2552eb4205072d6b3c5f4efff20951338f3dc296b5d30e4a4b6192d57070c81b`.
No loose asset, substitute loader, local renderer state repair or per-game keyboard logic is used.
The four presets, 250 ms bone slerp/translation lerp, expression from the current animation,
input edges and branch priority, camera constants, World rotation, text and lifecycle are original.

Two general CNA fixes are committed on `next`: `df2deb690` (INPUT-EMU-002, keyboard GamePad) and
`4f9b103dd` (GS-009g, public avatar -Z space and animation deltas relative to BindPose).
The latter prevents back-facing avatars and doubled offsets for attached objects without changing
the released catalog/description data. Sharp Runtime stays `next db86514c` unchanged.
Input baseline 528/528 → 560/560, focused runtime 32/32 → 33/33; avatar baseline 91/91 → 94/94.
The added attachment regression covers 71 bones, all 31 presets, both body types and caller World.

Canonical static Release OPENGLES3 and single-threaded WEBGL2 products build with the shared
ccache and all cores. Native and the exact gallery WEBGL2 copy render at 1280×720 and pass all
four animated presets, blending off/on, new random avatar, right-stick orbit/reset, trigger zoom
and Back. Real visible Google Chrome on a private Xvfb over plain HTTP has WebGL2, 600 rAF callbacks,
no exceptions/rejections/HTTP errors, no cross-origin isolation, and live contexts 1→0 on Back.
Pixel measurements prove changing motions, text, camera, appearance and zoom; the artifact-only
probe checks the shipped blend class at 100/150/250 ms (all 71 bones, max delta 0) plus identity,
live collection, playback properties and expression: **377 assertions pass**. The physical pad
path is regression-tested with injected platform fixtures; no real controller is attached here.

The four gallery files match the built product hashes. WASM is 39,867,492 bytes, without DWARF;
JS has no pthread/shared-memory path. The gallery has 86 unique entries, valid local links,
desktop/mobile layouts, controls and neighbour navigation. The new detail visibly documents the
approved CNA appearance/motion boundary beside Play. At initial qualification, all work was committed locally; publication/pruning happened later
on the explicit owner request recorded above.

Reproducible scripts and evidence: `/rv/tmp/samples/SAMPLE-085-AvatarAnimationBlendingSample_4_0/`,
especially `evidence/{keyboard-gamepad-20261003,avatar-space-20261003,qualification-20261003}/`.
Final products are measured in `qualification-20261003/native-release/` and `gallery-web/`;
`gallery-ui/`, `gallery-copy.json` and `inventory.json` record publication checks and provenance.
Earlier intermediate captures/build failures remain as history; the final gates above pass.

## Reopened by the owner — 2026-10-03

The request to resume `handoff.md` carries forward the owner’s authorization for shared keyboard
GamePad emulation and this complete avatar port. The owner confirmed the series is 085, 086, 101,
then 094, and approved the keyboard layout. Status is `✅`; earlier cancellation sections below
are historical. The complete translation and native/WEBGL2 qualification are finished.
See [diff.md](diff.md) for the approved CNA avatar and input differences.

## Re-analysis against CNA's standard avatar API — 2026-10-03

**Status: still `⛔` until the owner decides whether to reopen.** The owner asked to re-examine
SAMPLE-085, 086, 094 and 101 because CNA now implements avatars. Nothing was ported; no sample,
CNA or Sharp Runtime source changed. Heads: CNA `next fc64a4be3`, Sharp Runtime `next db86514c`.
The retained `xna4-original/` is byte-identical to the physical upstream directory.

**What it is.** An Xbox 360/HiDef-only game (two units, 533 lines including metadata). It shows
one random avatar (`AvatarDescription.CreateRandom`, `AvatarRenderer`) switching between four
built-in animations (Stand0, Celebrate, Clap, Wave). The sample's own `AvatarBlendedAnimation : IAvatarAnimation`
blends the old and the new animation over 250 ms, slerping each bone's rotation and lerping its
translation, and passes that to `AvatarRenderer.Draw(IAvatarAnimation)`. The controls are
gamepad-only: LB toggles blending (shown on screen), RB makes a new random avatar, A/B/X/Y play
the animations, the right stick and triggers move the camera, and Back exits. It uses
`GamerServicesComponent`, 1280×720 and multisampling.

**The 2026-09-09 cancellation reason no longer holds in substance.** It rested on CNA's avatar API
being the Windows-XNA no-op: an invalid random description, zero-length presets, an `Unavailable`
renderer and no-op `Draw`, with only a `CNAEXT` substitute renderer. CNA now implements the API
with real behavior on original CNA avatars (`docs/avatars.md`; the EXT surface is retired):
- `CreateRandom` returns valid descriptions;
- the 31 presets are real CNA clips on the 71-bone skeleton;
- `AvatarRenderer` loads to `Ready`, and `Draw(IAvatarAnimation)`/`Draw(bones, expression)` render;
- avatars also load in a single-threaded browser build (CNA `CBIND-143`).
Every API this sample calls exists. SAMPLE-087 AvatarShadows, an Xbox-only avatar sample of the same
kind, has already been ported on this API (`samples/AvatarShadows/missing.md`). Its approach:
Windows/HiDef content rebuilt by the official offline XNA pipeline from the unchanged sources, a
line-by-line port, and the CNA avatar look documented as a difference.

**What still needs owner decisions before a port:**

1. **CNA avatars instead of Xbox avatars.** The bodies and the four animations are CNA's own
   (different look and timing), as in SAMPLE-087. This is a documented difference, not Xbox
   fidelity.
2. **Input.** The original is gamepad-only. This machine has no gamepad (checked in
   `/proc/bus/input/devices`), CDP cannot emulate one in Chrome, and a virtual uinput pad would be
   visible to every other session's games. Without a pad the faithful port only shows the idle
   avatar. Either accept gamepad-only with a manual gate on a real pad, or have CNA add an
   owner-requested, off-by-default keyboard→GamePad emulation `CNAEXT`. That would be the
   counterpart of its existing accelerometer and orientation keyboard emulation
   (`docs/keyboard-device-emulation.md`); it does not exist today.
3. **Browser.** CNA embeds all avatar catalogs (v1–v3, about 30 MB of assets) into the library, so
   a WEBGL2 bundle grows accordingly. This is untested for this sample; SAMPLE-087's port was
   qualified natively only.

**Estimate if reopened:** port, content rebuild, native and real-Chrome gates and gallery about
3–5 h. A keyboard→GamePad emulation in CNA, if chosen, is another 2–3 h including tests. It would
also serve 086, 094 and 101, which are gamepad-only Xbox samples too.

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
