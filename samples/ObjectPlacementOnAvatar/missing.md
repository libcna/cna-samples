# Missing / Differences from XNA 4.0 original

## Completed faithful attachment port — 2026-10-03

**Current status: ✅.** All nine upstream files match the retained snapshot. Both source units
and original metadata are completely translated. Unchanged Xbox source/content rebuild succeeds;
no Xbox runtime/capture is available or claimed. The fresh executable is
`12175077103310e7cb66454611ebe844f3301a67d1a3cfe4d75066bda1d28418`; original Xbox bat is
`5f7debfcaf19f38b7a58b44fa275610356b8d3ea96a3bb13818ac226dead0432`.
The unchanged FbxImporter/ModelProcessor/name produces official Windows/HiDef bat XNB (21,666
bytes), SHA-256 `0134a8d36120d0660f89c1977197548cf957a446278d66e56f5881bb6d409996`.
No loose model, local renderer repair, extra text, audio or second product is added.

The original four presets, world/camera, input edge priority, `animation × bind × parent`,
SpecialRight attachment, -20° Y/+(.01,.05,0) bat offset, default BasicEffect lighting and bat-before-
avatar draw order are retained. Fingers do not grip the bat, exactly as upstream documents.
[diff.md](diff.md) records approved CNA art/preset timing and shared keyboard opt-in plus mechanical
C++ resource/collection adaptations. CNA remains `next 4f9b103dd` (keyboard input df2deb690 and
GS-009g standard avatar contract); Sharp stays `next db86514c`. The old re-analysis's claim that
composition already matched was incorrect: GS-009g removed preset bind translations and aligned
public coordinate space generally before this port. No sample-specific offset calibration.

The artifact-only probe calls the shipped BonesToWorldSpace method for all four presets, three
times, both body types and two caller World matrices. **3,460 assertions pass**, including 3,408
complete bone matrices and 48 original bat-offset matrices against the actual asset skeleton;
maximum matrix delta `4.76837158e-07`. Native and exact-gallery visible Chrome WEBGL2 render the
bat at the animated hand in all four presets, return to Stand, create another avatar, orbit/reset,
zoom and exit through Back. Pixel series prove all presets move. Chrome: 1280×720 WebGL2,
600 rAF, plain HTTP/nonisolated, no exceptions/rejections/HTTP errors, live contexts 1→0 on exit.
No physical pad is attached; shared GamePad paths already have CNA regression coverage.

Both canonical static Release OPENGLES3 and single-threaded WEBGL2 builds use shared ccache/all
cores and sibling dependencies. A stalled configure-time FetchContent clone was replaced with
the already verified local FNA3D pin `32401479a3ab5bd6b2e7f786e87bf4166aa03b0f`; the final build
passes. WASM contains no DWARF; JS contains no thread/shared-memory path. Four exact gallery files
match build hashes; all 88 cards are unique, local links and desktop/mobile controls/layout pass.
Reproducible scripts and final evidence: `/rv/tmp/samples/SAMPLE-101-ObjectPlacementOnAvatarSample_4_0/`,
especially `evidence/qualification-20261003/{native,gallery-web,gallery-ui}/`, attachment probe,
inventory and gallery hash/link JSON. Earlier mechanical compile failures remain as history.
Work is committed locally, without push or pruning; owner requested continuation without cleanup.

## Reopened by the owner — 2026-10-03

**Current status: ✅.** Authorized series 085 → 086 → 101 → 094, approved CNA original art/presets
and shared keyboard layout. Original source reviewed and fully translated; pipeline/native/web and
actual hand-attachment gates pass. Earlier cancellation sections are historical.
The old re-analysis claimed attachment composition already matched; that was incorrect because
preset matrices included bind translations. General CNA GS-009g (`4f9b103dd`) now provides animation
deltas and consistent public -Z space, making the original equation valid without a sample repair.
See [diff.md](diff.md) for approved differences.

## Re-analysis against CNA's standard avatar API — 2026-10-03

**Status: still `⛔` until the owner decides whether to reopen** (owner-requested series
SAMPLE-085/086/094/101). Nothing was ported; no sample, CNA or Sharp Runtime source changed.
Heads: CNA `next fc64a4be3`, Sharp Runtime `next db86514c`. The retained `xna4-original/` is
byte-identical to the physical upstream directory.

**What it is.** An Xbox 360/HiDef-only game (361 lines) that attaches a baseball bat (`baseballbat.fbx`
→ `Model`) to a random avatar's right hand while it plays one of four presets: Stand0, Celebrate,
Clap or Stand5. Each frame `BonesToWorldSpace` builds 71 world matrices as
`BoneTransforms[i] · BindPose[i] · world[parent]` (the root's parent is `renderer.World`). The bat
is drawn with `BasicEffect` at a small fixed offset times the `AvatarBone.SpecialRight` (49) world
matrix, and the avatar with `Draw(BoneTransforms, Expression)`. RB makes a new avatar, A/B/X/Y pick
the preset, the right stick and triggers move the camera, and Back exits. Gamepad only,
`GamerServicesComponent`.

**The 2026-09-28 cancellation reasons no longer hold in substance.** They were: invalid
descriptions, zero presets, an `Unavailable` renderer, a no-op `Draw`, a substitute extension unable
to take caller matrices, and no matching hand/bind pose. CNA today (`docs/avatars.md`):
- gives valid random descriptions and real CNA clips for all four presets;
- loads to `Ready`, exposes XNA's `ParentBones` and a ready avatar's `BindPose`, and draws
  `Draw(bones, expression)`;
- has no substitute route involved any more.

**Will the bat land in the drawn hand?** It does if the sample's composition equals the renderer's,
and the owning code says it does by construction:
- CNA's bind rotations are identity, so `BindPose` holds pure translations (`docs/avatars.md`;
  `AvatarRenderer.cpp:257–261`);
- the rig's root joint is at the origin (`tools/avatar_builder/cna_avatar/rig.py:81`), so
  `BindPose[0]` is the identity;
- CNA clips key translation only on the root track; every other bone is rotation-only
  (`src/Internal/Avatars/AvatarClips.cpp`, `sampleClip`).
The renderer composes `S·R(anim) · T(bind) · parent` with the root taking the animation's
translation (`AvatarRenderer.cpp:351` onwards), which therefore equals the sample's
`anim · bind · parent`. This is a code-level conclusion; a port must still confirm it visually,
with the bat in the hand across all four presets.

**What a port would involve if reopened:** translate the 361-line game line by line; rebuild
`baseballbat` (and its texture, if any) for Windows/HiDef through the official offline XNA pipeline
(the retained build is Xbox-platform). Open owner decisions are those of SAMPLE-085: CNA avatars
and clips instead of Xbox ones, gamepad-only input on a machine without a pad (or the proposed
keyboard→GamePad `CNAEXT` emulation), and browser bundle size. **Estimate if reopened:** about 3 h.

**Current status: `⛔` cancelled by explicit owner decision on 2026-09-28.**
The owner accepted the Xbox-only non-port boundary after the current-head analysis. No C++ port
will be produced for SAMPLE-101. Preserve the exact original, authentic Xbox executable/content
and analysis evidence. The missing general Avatar backend/data and caller-supplied pose/expression
rendering contract remain framework findings; cancellation does not close them.

The sample teaches a baseball bat attached to the animated `SpecialRight` hand of an Xbox Avatar.
The detailed analysis below is retained as historical evidence supporting the owner decision.

Source: `/rv/tmp/XNAGameStudio/Samples/ObjectPlacementOnAvatarSample_4_0/`.

Retained audit root:
`/rv/tmp/samples/SAMPLE-101-ObjectPlacementOnAvatarSample_4_0/`.

## Audited original

The complete 272 KB physical package was retained verbatim. It contains one Xbox360/HiDef game,
one stock content project, the 361-line game source, assembly metadata, solution/project files,
the 140,976-byte `baseballbat.fbx`, the original documentation and preview, and the Microsoft
Permissive License. There is no Windows or Phone game project.

The game retains the following defining behavior:

- create a random Xbox LIVE `AvatarDescription`, a genuine `AvatarRenderer`, and the built-in
  `Stand0`, `Celebrate`, `Clap` and `Stand5` animation presets;
- wait for `AvatarRendererState.Ready`, update the current animation, and calculate all 71 bone
  matrices in depth order as `animationPose * bindPose * parentWorld`;
- select `AvatarBone.SpecialRight` from that result and draw the stock baseball-bat `Model` with
  the original -20 degree Y rotation and `(0.01, 0.05, 0)` translation offset;
- draw the Avatar with the same animation transforms and expression, so the bat follows the
  genuinely animated right hand;
- switch animations with A/B/X/Y, request a new random Avatar with right shoulder, orbit/reset
  with the right stick, zoom with the triggers and exit with Back;
- use the original 1280x720 multisampled presentation, camera limits and
  `GamerServicesComponent` lifecycle.

The documentation explicitly notes that the animated fingers do not grip the bat and describes
overriding the right-finger matrices as a possible extension. The port must preserve that original
limitation rather than inventing a grip pose.

## Authentic build evidence

`scripts/build-original.sh` compiles the exact unchanged source against the official XNA 4.0 Xbox
reference assemblies and runs `baseballbat.fbx` through the official Xbox360/HiDef
`FbxImporter`/`ModelProcessor`. Both operations pass offline. The resulting files are:

| Product | SHA-256 |
|---|---|
| `xna4-build/bin/ObjectPlacementOnAvatar.exe` | `bcd0c3af5104e4d7d51e7e07ee593b5f0b52e89550ef9690bd219a7b87e63dba` |
| `xna4-build/Content/baseballbat.xnb` | `5f7debfcaf19f38b7a58b44fa275610356b8d3ea96a3bb13818ac226dead0432` |

The 21,666-byte XNB has the Xbox marker, version 5 and the expected six-reader stock graph:
`ModelReader`, `StringReader`, `VertexBufferReader`, `VertexDeclarationReader`,
`IndexBufferReader` and `BasicEffectReader`, with three shared resources. The exact FBX is
byte-identical to the one already processed and rendered by complete SAMPLE-055, so neither model
content nor model attachment as a general technique is the missing subsystem.

The executable references Xbox `Microsoft.Xna.Framework.Avatar` and Xbox `mscorlib` 2.0.5.0.
There is no Windows host, and the defining Avatar body/data came from the Xbox platform service,
so the executable cannot be truthfully run on Wine, the Win7 VM or a current PC without an Xbox
360 runtime and the retired Avatar delivery stack. No desktop screenshot is claimed.

Evidence is retained in `evidence/original-build.log`, `original-output-sha256.txt`,
`baseballbat-xnb-readers.txt` and `original-assembly-refs.txt`.

## Live CNA audit

The dependency audit used CNA commit `7712534d3`. A focused Debug OPENGLES3 run passed **69/69**
tests from `AvatarAnimationTest`, `AvatarDescriptionTest` and `AvatarRendererTest`; its full log is
`evidence/cna-avatar-tests.log`. The tests and implementation establish the current normal API
contract:

- `AvatarDescription::CreateRandom()` returns the reference stub's invalid all-zero 1,021-byte
  description;
- all four requested `AvatarAnimation` presets expose 71 zero matrices, neutral expression and
  zero duration;
- `AvatarRenderer::ParentBones` exposes the real 71-entry hierarchy, including
  `SpecialRight = 49`, but `State` remains `Unavailable` and `BindPose` throws because no ready
  body exists;
- ordinary `AvatarRenderer::Draw` validates the 71-entry input and is otherwise a no-op.

Therefore a literal C++ translation never enters `BonesToWorldSpace`. It would draw only the bat
near the identity pose on Cornflower Blue, without the body or any of the four advertised hand
motions. Calling that result a port would be a reduced demonstration, not XNA parity.

## Why CNAEXT is not a faithful shortcut

CNA offers `AvatarRenderer::EnableRealRenderingEXT`, `DrawRealEXT` and generated
`SkinnedModelEXT` bodies. `docs/avatar-real-rendering-ext.md` explicitly states that these are
CNA-original substitutes, not Microsoft's proprietary body, appearance or preset datasets. The
extension also owns a different skeleton and takes a clip name/time instead of the original
71-entry `BindPose`/`BoneTransforms` contract used to compute `SpecialRight`.

Using it would require alternate body/clip content, non-XNA calls and a new mapping from the
sample's Xbox bone calculation to the substitute skeleton. That is an explicit product/backend
scope choice, not a sample-local repair. No substitute body, fake pose, bat-only port or other
workaround was added.

## Historical result and resume options

No C++ source, CMake target, CNA workaround or sharp-runtime change was added. SAMPLE-101 remains
`🛑` under `SAMPLES-DEC-004` until the owner chooses one of these boundaries:

1. accept this evidence-backed Xbox-only/non-port result;
2. explicitly approve CNA's documented non-authentic Avatar extension plus a truthful
   `SpecialRight` attachment mapping as a deliberate rules/scope exception;
3. supply or authorize a faithfully redistributable Avatar body/material/appearance and all four
   preset datasets, then authorize the large normal-XNA-API backend needed for native OPENGLES3
   and WEBGL2.

If a faithful backend is authorized, resume with the exact 361-line translation and authentic
`baseballbat.xnb`, then qualify all four animation choices, moving hand attachment, random-avatar
replacement and camera controls on native OPENGLES3 and real-browser WEBGL2.


## Historical current-head re-analysis — 2026-09-28

### Package, behavior and retained products

All **9 physical upstream files / 241,956 bytes** match the retained `xna4-original/` snapshot
exactly, including both compiled C# inputs, Xbox solution/project, content project, FBX, thumbnail,
documentation and license. The earlier 272 KB figure describes allocated package size, not byte
content. The game has 362 logical lines (361 newline characters); no Windows or Phone target,
keyboard/mouse/touch path, audio, network session, custom content reader or XML serializer is present.

This is an **Xbox 360 object-attachment demonstration**. The original 1280×720 multisampled
HiDef game creates a random Avatar and four stock clips (Stand0, Celebrate, Clap, Stand5), waits
for a ready body, and composes 71 world matrices as `animationPose * bindPose * parentWorld`.
The baseball bat uses index `AvatarBone.SpecialRight` (49), with the original −20° Y rotation
and `(0.01, 0.05, 0)` local offset. Both the body and bat must follow the same animated hand.
The original gamepad controls and the original non-gripping finger behavior remain as described
above. Showing only a bat would omit the demonstrated behavior.

The two retained original products still match their original build hashes above: Xbox executable
and 21,666-byte Xbox/version-5 Model XNB. Its reader table was decoded again: six stock readers
and three shared resources. The FBX is byte-identical to SAMPLE-055's successfully supported
input. The content pipeline and ordinary Model/BasicEffect rendering are therefore not the
identified blocker. This analysis does not rebuild the original or claim Xbox execution.
The original Xbox binary cannot establish authentic Avatar behavior on Wine/Win7; an Xbox
reference/runtime and the required Avatar body/animation data would be needed.

### Current CNA boundary

Source inspection is pinned to CNA `next 8d56fa2fa6cbd40b2b99a4178f0510fce45d1043`.
The relevant normal Avatar implementation is unchanged from the already audited `b1e4a2414`
chain. Sharp Runtime independently advanced to clean
`feature/gamer-services-collections 6c4a857d`; its branch/work was preserved and is not a
SAMPLE-101 implementation. No framework source was edited.

Normal CNA still exposes the **off-Xbox reference behavior**, not the required Xbox product:

- CreateRandom returns an invalid all-zero 1,021-byte description;
- preset animations expose 71 zero matrices and zero duration;
- State is forced to Unavailable, so the sample's Ready branch never computes the hand position;
- BindPose throws while unavailable;
- Draw validates the 71 matrices and performs no rendering, discarding the expression.

These facts are not a broken bat transform or an EasyGL-specific failure. The missing product is
a genuine/expressly authorized replacement Avatar body, appearance, animation and rendering
backend behind the normal public API. The recent account-service integration does not supply it.

The opt-in CNAEXT path can draw a substitute skinned character, but still samples its **own**
clip/skeleton through `DrawRealEXT(clipName, position, loop)`. It does not make State ready,
expose the original bind pose, or implement normal `Draw(bones, expression)`. There is no
rendering entry point that consumes the caller's 71 matrices and expression. Reusing it here
would need an owner-approved appearance/clip change plus a defined SpecialRight mapping and
shared pose contract so the bat and body agree; a sample-local guess is not authorized.

### Verification and decision

**74/74** existing AvatarAnimationTest, AvatarDescriptionTest and AvatarRendererTest cases pass
through CNA's mandatory private GPU runner. This is a fresh focused run of the retained Debug
test binary, not a fresh framework build, native/browser sample test or substitute-renderer
qualification. The exact command, binary SHA-256, source heads and full output are retained.
The historical 69/69 result above remains historical.

SAMPLE-101 stays `🛑` under DEC-004. The owner may cancel this Xbox-only product, explicitly
approve and specify a substitute Avatar implementation, or authorize the complete normal-API
backend and suitable body/preset data. A future CNA-original Avatar/server product remains
possible, but has not been authorized as a fidelity exception for this sample. No bat-only port,
fake pose, new controls, workaround, CMake target or native/web release was added.

Evidence under the stable artifact root:
`evidence/current-head-analysis-20260928/{inventory,review,test-run,final-heads}.json` and
`cna-avatar-tests.log`. The inventory pins every upstream/product hash and decodes the XNB.


## Owner cancellation — 2026-09-28

The owner explicitly requested **“keep 101 cancelled”**. SAMPLE-101 is therefore `⛔` under
DEC-004. No substitute Avatar, bat-only demonstration, backend implementation or artifact pruning
was requested. The earlier 74/74 test run and verified original/product hashes remain retained
evidence rather than a claim that this Xbox Avatar product is supported by CNA.
