# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port has been started.** The sample
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
