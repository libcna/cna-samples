# Missing / Differences from XNA 4.0 original

## Published and pruned on the owner's request — 2026-10-03

The owner requested "vse pushni prorez 85 86 101 09" and explicitly clarified the fourth number
as SAMPLE-094 CustomAvatarAnimation. CNA `next 2b4ff28d7`, samples `develop 23e8ad7` and gallery
`main 4debda9` were pushed; Sharp `next db86514c` was already synchronized. The complete four-game
gallery is now 89 entries. Initial local-only/no-cleanup statements below are historical.

This sample's exact root `/rv/tmp/samples/SAMPLE-094-CustomAvatarAnimation_4_0/` was pruned with
`tools/prune-completed-sample.sh --apply` after a reviewed dry run. Source snapshots, original
products, official content, native/web products, scripts and all qualification evidence remain.
`evidence/prune-20261003/{retained-before,retained-after,hash-verification}.json` proves all
retained file hashes match except the intentionally stripped native executable:
`9072794e9d97f4cd088a1542ba8ce84475ce218390471ea369ccffcf6b5c49e6` →
`8ab747b408822fdbf2a3dc282797d7c7f6b2137365d6dd56154279ef0206db57`. Its full native interaction/animation/camera/appearance
gate passed again with GamePad Back exit 0; see `native/` and `native-gate.log` in that evidence.
The second dry run reports zero removable paths. The four-root apply freed about 2.7 GiB.

`MANIFEST.md` retains exact restoration commands and links the archived previous manifest.
Both native/web build scripts now restore their own verified FNA3D/MojoShader source archive at
pin `32401479a3ab5bd6b2e7f786e87bf4166aa03b0f`; no rebuild depends on SAMPLE-086's removed build
directory. Complete extraction hashes and the MojoShader submodule pin were roundtrip-verified.
Products and game behavior are unchanged. Publication SHA-256 verification is retained in
SAMPLE-094's `evidence/publication-20261003/hosted-hashes.json`.

The original Windows XNA player/DLLs also remain in `xna4-build/windows-reference/bin/`.
After pruning, the genuine original player again produced the byte-identical 436,852-byte CSV
reference; see `original-player-replay.json`. Xbox products remain retained without an Xbox run.

## Completed full custom animation port — 2026-10-03

**Current status: ✅.** All 28 upstream files are freshly verified against the complete snapshot.
The entire original game, four-unit runtime library and assembly metadata are fully translated;
the design-time processor stays byte-identical to upstream. The original runtime/processor DLLs, Xbox game
and seven Xbox content products rebuild successfully (fresh game SHA-256 `5522fab9acaba2442c9ff0d45a6ff1302ffa1a68b88a9162d8f2c53a879ead6f`).
Xbox execution and original Xbox screenshots are unavailable and are not claimed. The genuine
Windows XNA runtime executes the original animation player as a CPU reference diagnostic.

Five original FBX/CSV clips and the ground model/texture are rebuilt by the original processors,
names and metadata using official Windows/HiDef XNA 4.0, compressed XNB v5/LZX. All seven shipped
XNBs match that output; full sizes and SHA-256 hashes are in `windows-hidef-content.json`. No loose
FBX, alternate importer, substitute motion, font/audio or second runtime product is introduced.
The actual 13-reader table is decoded from every custom XNB and registered through CNA's normal
AOT/ReflectiveReader graph, including nullable reference Lists, inline keyframe structs and the
original AvatarExpression property order (Mouth, LeftEye, RightEye, LeftEyebrow, RightEyebrow).

The port preserves strict step keys, the live 71-bone collection, current-position reset semantics,
forward/reverse, looping/clamping and expressions. Against the unchanged original DLL on the same
five official XNBs, **64,822 assertions / 57 states** pass; maximum serialized matrix delta is
`3.0000000039720476e-08`. Eight original exceptions match, including reverse looping a clip without
expressions and reversing after a key cursor reaches the list end. A further **1,041 game graph
assertions** verify all nine loaded slots, valid Idle4, 1,000 idle choices reproducing Next(3),
0.9 m/s movement, action-key priority and walking overriding an action. These are artifact-only
probes of shipped code, not added gameplay or an Xbox rendering reference.

The original empty-avatar startup, signed-in PlayerOne event/callback, random/profile selection,
world/camera, frame-based camera increments/clamps, ground-before-avatar order, four Stand idles
and five custom actions are retained. [diff.md](diff.md) records approved CNA bodies/proportions/
BindPose and built-in idle timing, shared keyboard emulation, AOT registration and C++ ownership.
Custom matrices are not retargeted for the replacement art; Xbox pose/appearance parity is not
claimed. The source Next(3) bug and stale eight-animation comment over nine slots are retained.

A consumer gate exposed a general CNA Guide defect: sign-in toasts left SpriteBatch's LinearClamp,
AlphaBlend, depth None and CullNone active in the next title frame. GS-009h restores the original
four state references in the system overlay. The existing Guide baseline was 34/34; both new
regressions fail before correction, then 36 Guide + 94 avatar checks pass (**130/130**). CNA is
`next 2b4ff28d7`, also containing keyboard `df2deb690` and GS-009g `4f9b103dd`; Sharp remains
`next db86514c`. No sample-side graphics-state repair is added. Before/after evidence is retained;
native walking ground change above eight channel values is 0 → 66,823 pixels.

Final static Release OPENGLES3 and single-threaded WEBGL2 builds use shared ccache/all cores and
the verified offline FNA3D pin `32401479a3ab5bd6b2e7f786e87bf4166aa03b0f`. Native and exact-gallery
visible system Chrome exercise all custom actions, moving/looped Walk, return to idle, profile and
random avatar, camera orbit/reset, trigger zoom and clean Back. The fixtures use a real CNA offline
profile through its normal auto-sign-in environment, including the actual sign-in callback/toast;
they do not change the shipped game's empty-profile startup or synthesize an avatar backend.
Chrome reports 1280×720 WebGL2, 600 rAF, plain HTTP without isolation, no exceptions/rejections/HTTP
errors and contexts 1→0. Image measurements prove each custom action changes, walking moves the
ground, camera/avatar branches change and zoom grows the body. Final captures are visually reviewed.

A separate native and exact-gallery Chrome gate verifies the ordinary **empty-profile** startup:
only the original ground is drawn, Q leaves the frame byte-identical with no signed-in player,
E renders a new random avatar and Back exits cleanly (web contexts1→0, no runtime/HTTP errors).
The `native-empty` and `gallery-web-empty` images/results retain this branch independently from
the signed-in fixture. Background variation initially exceeded the diagnostic's empty-body mask
threshold; the corrected analysis excludes it and also checks exact pre/post-Q image equality.
Only the fixture was adjusted; shipped code and bundle bytes did not change.

All four gallery files match build hashes; WASM is 40,099,266 bytes with no DWARF and
JS has no shared-memory/thread path. All 89 cards are unique, local links resolve and final desktop/
mobile UI passes. Scripts, full original products, XNB tables, original/CNA numeric CSVs and final
evidence are retained under `/rv/tmp/samples/SAMPLE-094-CustomAvatarAnimation_4_0/`, especially
`evidence/qualification-20261003/{native,gallery-web,gallery-ui}/`, comparison/inventory/hash JSON
and GS-009h logs. Earlier mechanical compile and fixture-hash invocation failures remain history.
Initial qualification was local and without cleanup; the later explicit push/prune request
above supersedes that instruction.

## Reopened by the owner — 2026-10-03

**Current status: ✅.** Owner confirmed SAMPLE-094 as fourth in series 085 → 086 → 101 → 094,
approved standard CNA original-art avatars/idles and shared keyboard GamePad layout. Original game,
runtime library and processor reviewed; full translation, official content and native/web qualification
are complete. Custom clips/expressions remain the original FBX/CSV data, including step timing,
loop/reverse/clamp quirks and the original Next(3) idle selection. Older cancellation is history.

## Re-analysis against CNA's standard avatar API — 2026-10-03

**Status: still `⛔` until the owner decides whether to reopen** (owner-requested series
SAMPLE-085/086/094/101). Nothing was ported; no sample, CNA or Sharp Runtime source changed.
Heads: CNA `next fc64a4be3`, Sharp Runtime `next db86514c`. The retained `xna4-original/` is
byte-identical to the physical upstream directory.

**What it is.** Microsoft's own custom-avatar-animation lesson and the consumer of exactly this kind
of data: a 548-line Xbox game, a 510-line runtime library (`CustomAvatarAnimationData`,
`AvatarKeyframe`, `AvatarExpressionKeyframe` and the `IAvatarAnimation` player with forward/reverse,
looping, clamping and expression keyframes), and a 446-line Content Pipeline processor that turns
71-bone avatar FBX plus expression CSV into those data. The game plays four built-in Stand idles and
five custom clips (Walk, Jump, Kick, Punch, Faint) on a random or signed-in gamer's avatar over a
ground model. A/B/X/Y pick actions, the left stick walks, the right stick and triggers move the
camera, and Back exits. Gamepad only.

**All three 2026-09 blockers are resolved in CNA's standard avatar API** (`docs/avatars.md`):

1. *Proprietary body/appearance service* → `AvatarDescription.CreateRandom` gives valid CNA
   avatars, and `BeginGetFromGamer` returns a signed-in gamer's avatar from the CNA service or a
   local offline profile.
2. *Four built-in Stand clips* → `Stand0`–`Stand7` are real CNA clips with real lengths, so the
   recorded "idle re-rolls every frame because `Length == 0`" degeneration no longer happens.
3. *Draw of custom matrices and expressions* → `AvatarRenderer::Draw(bones, expression)` renders
   the 71 supplied transforms and maps the expression to mouth/eye/eyebrow states
   (`AvatarRenderer.cpp:351` onwards). `Draw(IAvatarAnimation)` forwards the animation's bones and
   expression. The `CNAEXT` substitute route is retired.

**What a port would involve if reopened:**
- translate the runtime library and the game line by line (about 1,060 lines of C#). The processor
  is design-time and stays as the source of the pregenerated XNBs;
- rebuild the five custom-animation XNBs and the ground model/texture for Windows/HiDef through the
  unchanged processor and the official offline XNA pipeline. The retained build already compiles it
  for the Xbox target;
- register the reflective `CustomAvatarAnimationData` reader with one `CNAEXT` line, the
  language-mechanism precedent of SAMPLE-049/051 recorded in `diff.md`;
- keep the upstream defect that `PlayRandomIdle` never picks the fourth idle (`Next(3)`), rather
  than repairing it.

**Open owner decisions** are those of SAMPLE-085: CNA avatars instead of Xbox ones (the custom clips
are the original's own data; the bodies and Stand idles are CNA's), gamepad-only input on a machine
without a pad (or the proposed keyboard→GamePad `CNAEXT` emulation), and browser bundle size.

**Estimate if reopened:** about 5–8 h. It is the largest of the four: more code, the custom
pipeline rebuild and the AOT reader. A finished 094 port would also be the natural base for a later
preview of the cancelled SAMPLE-113 animations.

**Status: cancelled by the owner on 2026-09-09 under `SAMPLES-DEC-004`; re-analyzed on
2026-09-27. No C++ port has been started.** This is a
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

## Re-audit 2026-09-09: two findings the first pass did not record

### The substitute route's *shape* is wrong, not only its art

The section above says using `CNAEXT` "would require defining how this sample's exact custom
71-bone matrices and facial expressions drive the substitute". Measured against the current tree,
that is stronger than a definition problem: `AvatarRenderer` has exactly three draw entry points,
and none of them can carry this sample's product.

| entry point | takes | renders |
|---|---|---|
| `Draw(IAvatarAnimation*)` | the animation, from which it reads 71 matrices + expression | nothing — forwards to the overload below |
| `Draw(const std::vector<Matrix>& bones, AvatarExpression)` | the 71 matrices; the expression parameter is unnamed in the definition and discarded | nothing — validates `bones.size() == 71`, then returns |
| `DrawRealEXT(const std::string& clipName, TimeSpan position, bool loop)` | a **clip name**; it calls `realModel_->ComputeBoneTransformsEXT(...)` and derives the matrices itself | real GPU-skinned geometry |

So the one route that draws cannot be handed caller-computed matrices at all, and has no facial
expression parameter in any form. `CustomAvatarAnimationPlayer` exists to produce exactly those two
things per frame from the authored FBX/CSV keyframes. Authorizing the substitute would therefore
not be only a scope ruling about non-authentic art — it needs a **new `AvatarRenderer` entry point**
that accepts 71 caller-supplied matrices plus an `AvatarExpression`, which is framework work rather
than sample work. (One level down the pieces already exist: `SkinnedEffect::SetBoneTransforms` is
what `DrawRealEXT` itself calls. Reaching for it directly from a port would be writing a different
program, not porting this one.)

Verified in `modules/gamer-services/include/Microsoft/Xna/Framework/GamerServices/AvatarRenderer.hpp:140,150,199`
and `modules/gamer-services/src/Xna/AvatarRenderer.cpp:102,117,178`.

### A faithful port would not merely be invisible; it would spin

`AvatarAnimation` gives every preset `Length == TimeSpan::Zero`, and `Update` clamps
`CurrentPosition` to `Length` when not looping
(`modules/gamer-services/src/Xna/AvatarAnimation.cpp:12-19,46-60`). This is faithful — the real
off-Xbox XNA assemblies do the same, and the CNA source says so where it is implemented.

The game's own end-of-animation test is
`animations[currentType].CurrentPosition == animations[currentType].Length`
(`CustomAvatarAnimationSample.cs:203-208`), and idles are not looped. With a zero-length preset that
equality is therefore true on the *first* idle frame and every frame after, so `PlayRandomIdle()`
fires once per frame forever. The four Stand idles never play; they restart.

This is worth recording because it names what a desktop port using those zero-length presets would
produce: the game would re-roll its idle animation at frame rate. The Xbox executable cannot be
run off Xbox here; no such runtime observation is claimed.

### Upstream defect: `Idle4` is loaded and never played

`PlayRandomIdle()` is `PlayAnimation((AnimationType)random.Next((int)AnimationType.Idle4))`
(`CustomAvatarAnimationSample.cs:421`). `AnimationType.Idle4` is 3, so `Random.Next(3)` returns
0, 1 or 2 — `Idle1`, `Idle2`, `Idle3`. The fourth idle is loaded by the `for (int i = 0; i < 4; i++)`
preset loop at line 135 and is unreachable at runtime. The bound should be `AnimationType.Walk` (4)
for the loop and the selection to agree.

A port must **reproduce** this, not repair it: the campaign rule is that upstream defects are
preserved. Recorded here so a future port does not silently "fix" it and diverge. Related: line 131
comments "We will use 8 different animations" over an array of 9 (`IAvatarAnimation[9]`); nine is
correct and the comment is stale.

## Owner decision and historical resume conditions

No C++ source, CMake target, loose-content replacement, CNA workaround or sharp-runtime change was
added. The owner chose the Xbox-only non-port boundary on 2026-09-09, so SAMPLE-094 is `⛔`.
The historical decision choices were:

1. accept this evidence-backed Xbox-only/non-port result (**selected**);
2. explicitly authorize CNA's non-authentic Avatar extension as a deliberate rules exception and
   define the required custom-matrix, facial-expression, native and WEBGL2 behavior;
3. supply or authorize a faithfully redistributable body/material/appearance and built-in Stand
   dataset, then authorize a large normal-XNA-API Avatar backend for native and WEBGL2.

If a rendering scope is authorized, resume with the complete 1,504-line translation, retain all
seven exact official XNBs, add sample-owned AOT reader registration for the reflective custom data,
and qualify all nine animations, forward/reverse/loop timing, facial expressions, profile/random
avatar selection, movement and camera controls on native OPENGLES3 and a real WEBGL2 browser.

## Current-head re-analysis — 2026-09-27

The retained 28-file source snapshot is byte-identical to the distributed upstream directory.
SHA-256 verification of the unchanged original Xbox game/library and all seven official XNBs
passes. These are retained build results, not a fresh XNA build or a runtime capture: this workspace
still has no Xbox 360 execution path for the Xbox-only game. All seven content products were
previously built with the sample's own processor by the official XNA 4.0 Xbox360/HiDef pipeline;
the five custom FBX animations therefore are not the observed blocker.

Current CNA `next b1e4a2414` still keeps `AvatarDescription::CreateRandom` invalid, every stock
`AvatarAnimation` at 71 zero matrices and zero length, and normal `AvatarRenderer::State` at
`Unavailable`. Its standard `Draw(bones, expression)` validates 71 inputs and then returns without
rendering. `DrawRealEXT` renders a non-authentic substitute from a named clip; it still takes no
caller-supplied matrices or `AvatarExpression`, so it cannot consume this sample's custom animation
player. The current focused Avatar selection passed 81/81 tests across four suites; no production
code was changed. Evidence: `/rv/tmp/samples/SAMPLE-094-CustomAvatarAnimation_4_0/evidence/current-head-analysis-20260927/`.

The owner cancellation remains the correct status. No sample-side workaround, substitute avatar,
ground-only build, native executable or browser bundle was created. Next numbered sample: SAMPLE-095.
