# Missing / Differences from XNA 4.0 original

**Current status: `🛑`, current-head re-analysis on 2026-09-28; owner scope decision required.**
No C++ port has been started. See the current analysis below; earlier build/test results remain
historical rather than new current-head qualification. The sample
teaches how to attach a stock model to the moving `SpecialRight` bone of a genuine Xbox LIVE
Avatar. The baseball-bat content and the sample-owned world-transform algorithm are portable, but
their defining visible result depends on Microsoft's proprietary Xbox Avatar body, bind pose,
appearance service and four built-in animation datasets. CNA's normal XNA API deliberately keeps
the off-Xbox unavailable/no-op contract, while its opt-in `CNAEXT` character is explicitly a
substitute that the campaign rules prohibit without an owner scope decision.

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

## Current result and resume conditions

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


## Current-head re-analysis — 2026-09-28

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
