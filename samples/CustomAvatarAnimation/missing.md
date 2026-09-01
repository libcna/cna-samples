# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port has been started.** This is a
complete Xbox 360 game plus a custom avatar-animation content pipeline, not just another built-in
animation example. The exact processor and runtime sources compile, and the official XNA 4.0
pipeline produces every custom animation XNB. The remaining blocker is the defining visual result:
Microsoft's proprietary Xbox Avatar body, appearance service and built-in animation data. CNA's
normal XNA Avatar route deliberately exposes only unavailable/no-op stubs off Xbox, while its
`CNAEXT` character is explicitly a substitute that the campaign rules prohibit without an owner
scope decision.

Retained audit root:
`/rv/tmp/samples/SAMPLE-094-CustomAvatarAnimation_4_0/`.

Exact upstream snapshot:
`/rv/tmp/samples/SAMPLE-094-CustomAvatarAnimation_4_0/xna4-original/`.

## Audited original

The 49 MB snapshot contains 28 files and 1,504 lines of C# across four logical projects:

- a 510-line shared runtime library, built separately for Windows pipeline use and Xbox execution,
  that defines animation/expression keyframes, serialized animation data and the 288-line
  `IAvatarAnimation` player;
- a 446-line custom Content Pipeline assembly whose processor finds `BASE__Skeleton`, removes
  `_END` nodes, verifies the exact `AvatarRenderer.BoneCount`, flattens bones into XNA Avatar order,
  converts FBX transforms to 71 root-relative matrices, merges and sorts animation keyframes and
  imports optional facial-expression CSV keyframes;
- a 548-line Xbox360/HiDef game project that loads four built-in Stand presets and five custom
  clips—Walk, Jump, Kick, Punch and Faint—then draws a genuine Xbox Avatar and ground model;
- the Xbox runtime build of the shared library, using the same sources and public data contract.

The custom player preserves the sample's full contract: exactly 71 bone matrices, forward and
reverse time, looping, clamping at both ends and facial-expression keyframe selection. The game
retains A/B/X/Y action selection, left-stick movement and automatic Walk/Idle transitions, random
and signed-in-gamer avatar replacement, right-stick camera orbit/reset, trigger zoom and Back exit.
Full-body motion and facial expression on the genuine avatar are the advertised output.

The content snapshot includes five large avatar-rigged FBX files (`Faint`, `Jump`, `Kick`, `Punch`
and `walk`), two expression CSV files and the ground model/texture. `Test.fbx` is present upstream
but intentionally is not included by the original content project.

## Authentic build evidence

The retained `scripts/build-original.sh` compiles the exact unchanged sources against the official
XNA 4.0 Windows and Xbox reference assemblies. It builds the Windows shared library needed by the
processor, the processor assembly, all content through official Xbox360/HiDef `BuildContent`, the
Xbox shared runtime library and the exact game executable. The complete route passes; the expected
FBX warnings are the same limitations documented by the sample rather than build failures.

The official pipeline produced seven Xbox-platform, version-5, LZX-compressed XNBs:

| Asset | SHA-256 |
|---|---|
| `Faint.xnb` | `9e570faa519444d738597cb5aaf009902430e64b25137989dfd77d6ec3ed5c4d` |
| `Jump.xnb` | `1f192f89916bcb293c46d8406d9cbd92713635599fb20f6f256dde9e2ae6efd2` |
| `Kick.xnb` | `ee66a487f91271800f3fb58662070bf226c79995d920858616498a8fc858d344` |
| `Punch.xnb` | `f240def39bb6d67eeb3d02e8c7bbd781c3b6a838eac552376a354fa2ff4810d9` |
| `Walk.xnb` | `b580c6be8e5ecdcdb68320ad23b55cfd1766fb30a2377b2525b0c26f451fe1dc` |
| `ground.xnb` | `b322c11053aed6c52e3785ac292aa06b1fa916ecaafeee7f3e55ef7e64816ecd` |
| `ground_0.xnb` | `a542aa2f404961ad3dedc7a0ca0195de9d7e464a67d4eb332a77106b8da252f3` |

The Xbox library and game hashes are respectively
`7e0a6691f8bc7fecba66416ac5e8d2be1677a6697e701c5588934c40f6aa9ed6` and
`4b0c745e75fcf7dc425dc990c27717847fbefafc0a8a5e126abef5f1c33f7499`.
The full compiler/pipeline transcript and hash manifest are retained under `evidence/`.

There is no Windows game project. The executable targets Xbox 360 and depends on that platform's
retired Avatar host/service, so no false local runtime or screenshot claim is made. Crucially, the
successful processor run disproves the historical idea that custom FBX animation processing itself
blocks this sample.

## Live CNA audit

The dependency audit used CNA commit `e5ae0820e`. Its ordinary XNA-shaped Avatar implementation
matches an intentional off-Xbox stub boundary:

- `AvatarDescription::CreateRandom` returns an invalid description;
- every built-in `AvatarAnimation` preset exposes 71 zero matrices, zero duration and a neutral
  expression;
- `AvatarRenderer::State` remains `AvatarRendererState::Unavailable` and `BindPose` is unavailable;
- `AvatarRenderer::Draw` accepts the correct 71-bone input but draws nothing.

The Debug `CnaGamerServicesTests` target built successfully with at most eight parallel jobs. A
focused selection passed **60/60 tests from four suites**, covering animation, description,
expression and the normal renderer surface, including the permanent unavailable state and correct
bone-count/no-op draw behavior. The retained log is
`evidence/cna-avatar-tests.log`.

CNA's content stack can already carry sample-authored reflective data through explicit AOT reader
registration, as demonstrated by earlier custom-animation samples. If an Avatar backend is ever
authorized, this sample must register the exact `CustomAvatarAnimationData` object graph rather
than replace its seven authentic XNBs with loose FBX/CSV data. That bounded integration was not
started because it cannot make the defining renderer/service result visible.

## Why CNAEXT is not a faithful shortcut

`AvatarRenderer::EnableRealRenderingEXT`/`DrawRealEXT` uses CNA-generated male/female substitute
meshes and clips. CNA's own extension documentation says it does not reproduce Microsoft's Xbox
Avatar art or service. Using it would change appearance, built-in preset data and the normal XNA
API path. It would also require defining how this sample's exact custom 71-bone matrices and facial
expressions drive the substitute on native and browser renderers. That is an explicit product and
backend decision, not a sample-local workaround. No substitute body, fake preset, skipped facial
animation or ground-only port was added.

## Current result and resume conditions

No C++ source, CMake target, loose-content replacement, CNA workaround or sharp-runtime change was
added. SAMPLE-094 remains `🛑` under `SAMPLES-DEC-004` until the owner chooses one of these scopes:

1. accept this evidence-backed Xbox-only/non-port result;
2. explicitly authorize CNA's non-authentic Avatar extension as a deliberate rules exception and
   define the required custom-matrix, facial-expression, native and WEBGL2 behavior;
3. supply or authorize a faithfully redistributable body/material/appearance and built-in Stand
   dataset, then authorize a large normal-XNA-API Avatar backend for native and WEBGL2.

If a rendering scope is authorized, resume with the complete 1,504-line translation, retain all
seven exact official XNBs, add sample-owned AOT reader registration for the reflective custom data,
and qualify all nine animations, forward/reverse/loop timing, facial expressions, profile/random
avatar selection, movement and camera controls on native OPENGLES3 and a real WEBGL2 browser.
