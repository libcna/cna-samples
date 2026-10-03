# CNA samples handoff — keyboard GamePad emulation, then the avatar samples 085/086/101/094

Updated: 2026-10-03, end of a Claude Code session. Audience: an AI agent (Codex) starting with a fresh
context. This file is a convenience. **`rules.md` is binding**, and nothing here waives it.

---

## 1. What to do next (the short version)

The owner's current plan, in order:

1. **Implement a keyboard → GamePad emulation in CNA** (`../cna`). The owner authorized it on
   2026-10-03: "ano pridej do cna emulaci gamepadu pres klavesnici, podobna tam uz je cna rozsireni
   pro emulaci dotyku, akcelerometru a orientace zarizeni". It is the counterpart of CNA's existing
   off-by-default `CNAEXT` emulations (mouse→touch, keyboard→accelerometer, keyboard→orientation).
   **Not started.** Only read-only design research was done; see §5.
2. **Port the avatar samples on CNA's standard avatar API, one at a time:** SAMPLE-085, SAMPLE-086,
   SAMPLE-101, then the fourth one. The owner wrote "Codex bude pracovat na 85 86 101 **104** a pote
   ty dalsi zbyvajici dle planu". In this context the fourth one is almost certainly **SAMPLE-094**
   (`CustomAvatarAnimation`), the fourth row of the avatar series 085/086/094/101 analyzed this
   session. SAMPLE-104 is the unrelated `🟡` partial PerformanceUtility, whose open work is browser
   networking. **Confirm with the owner whether "104" meant 094** before working on either.
   All four rows are still `⛔` in `plan.md`. Reopen each row (to `🛠`) only when you start it, and
   record the owner's instruction in its `missing.md`.
3. **Then continue with the remaining rows "according to the plan"**: SAMPLE-114 onward (§7).

Pause note: after authorizing step 1 and "then only 085", the owner said "nezacinej 85 pockej co ti ted
napisu" (do not start 85, wait) and asked for this handoff. Nothing of steps 1–2 has been started. When
you resume, briefly confirm the order with the owner if they are present; otherwise follow it as above.

---

## 2. Mandatory reading before any work

1. [AGENTS.md](AGENTS.md), then **all of [rules.md](rules.md)** (binding).
2. [NEXT.md](NEXT.md) → **Active handoff** section, and [plan.md](plan.md): the status table,
   `SAMPLES-DEC-004/005/006`, and the row you work on. NEXT.md below the active section is a historical
   ledger, not instructions.
3. The row's `samples/<Name>/missing.md`. For the avatar rows read the newest section,
   "Re-analysis against CNA's standard avatar API — 2026-10-03", first.
4. Before changing CNA: `../cna/AGENTS.md`, `../cna/CHECKLIST.md`, `../cna/docs/avatars.md`,
   `../cna/docs/keyboard-device-emulation.md` and `../cna/plans/plan_keyboard_device_emulation.md`.
   Before changing Sharp Runtime: its `AGENTS.md`.
5. Machine-wide build rules: `/rv/data/development/github.com/libcna/CLAUDE.md` (one shared ccache,
   closed list of build-directory names, never build in `/tmp`, watch RAM).

---

## 3. Repository snapshot (verify on resume)

| Repository | Branch | HEAD | State at handoff |
|---|---|---|---|
| `cna-samples` | `develop` | `7be1418` + this handoff commit | **11 local commits not pushed** (listed below). |
| `../cna` | `next` | `fc64a4be3` | Equal to `origin/next`. Contains an **untracked `xna-games/` directory from another session; leave it alone.** |
| `../sharp-runtime` | `next` | `db86514c` | Clean, equal to upstream. |
| `../samples.libcna.com` | `main` | `c24e74c` | Pushed and deployed on GitHub Pages (Tilt Perspective added). |

Unpushed `cna-samples` commits (the owner has not yet answered whether to push them):
`74d5f69` (108 re-analysis), `5682b09` (108 ⛔, 109), `b3a586f` (109 ⛔, 110), `3282d4f` (110 ⛔, 111),
`19db1d9` (111 ⛔, 112), `49e5018` (112 ⛔, 113), `7fb0baf` (113 ⛔, 085 re-analysis), `4d8b9e8` (086),
`f93760b` (094), `7be1418` (101), plus the commit that adds this handoff. **Push only when the owner
explicitly asks.** Earlier in this session the owner explicitly asked to push 106 and 107; that
authorization does not extend to these commits.

Plan counts at handoff: ✅ 86, 🛑 38, ⛔ 24, ⏸ 2 (100, 106), 🛠 1 (148), 🟡 1 (104), ↗ 1 (152 Racing).

Gamer Services is owner-managed. Rows 075/087/096 say "ported on branch `feature/gamer-services-samples`,
status changes when merged into develop", yet their code is already on `develop` and the rows show ⛔.
That tracker inconsistency belongs to the owner's Gamer Services work. Do not "fix" it unasked; mention
it if relevant.

---

## 4. What this session did (2026-10-03)

| Row | Result | Where recorded |
|---|---|---|
| 106 `SavingEmbeddedImages_4_0` | Re-analyzed, then **⏸ deferred** by the owner with a CNA work list (6–10 h) the owner wants done later: (a) Guide dialogs presented without `GamerServicesComponent` (Windows Phone contract), (b) blocking `End*` modal frames in the browser via Asyncify, (d) `MediaLibrary.SavePicture` throwing `InvalidOperationException`. (c), the browser media-save contract, was not selected. **Do not start without the owner.** | `samples/SavingEmbeddedImages/missing.md` → "Deferred work". Pushed. |
| 107 `TiltPerspective_4_0` | **✅ completed**: static native and web products, a native gate and a visible-Chrome gate that measure recalibration from the box's back-wall offset, a gallery entry (new `page-8.html`, sample 85 of 85), pushed, deployed (byte-verified) and pruned (1.3 GB → 167.8 MB). | `samples/TiltPerspective/missing.md`, artifact `MANIFEST.md`. |
| 108 `WinFormsContentSample_4_0` | ⛔ (WinForms tool). Evidence kept: CNA's native content pipeline rebuilds the original runtime `Model.xnb` byte-identically. | its `missing.md` |
| 109 `WinFormsGraphicsSample_4_0` | ⛔ (WinForms hosting demo). Evidence kept: the unchanged original, compiled with `mcs` against the genuine XNA 4.0 assemblies and run in Wine, renders both controls and exits 0. | its `missing.md` |
| 110 `WP7MusicManagement_4_0` | ⛔ (Phone tasks and music ownership). | its `missing.md` |
| 111 `XnaGraphicsProfileChecker_4_0` | ⛔ (C++/CLI D3D9 diagnostic, outside EasyGL scope). | its `missing.md` |
| 112 `AvatarAnimPack_4_0_BIN` | ⛔ (`STRB` Xbox asset data; CNA reads no Xbox avatar data by policy). | its `missing.md` |
| 113 `AvatarAnimPack_4_0_FBX` | ⛔ (source FBX with no app). A later preview could build on a 094 port. | its `missing.md` |
| 085, 086, 094, 101 | **Re-analyzed**: their 2026-09 cancellation reasons no longer hold, because CNA now implements the XNA avatar API with real behaviour. Rows still ⛔ until started. | each `missing.md`, `plan.md` rows |

Key CNA findings recorded this session (useful beyond these rows):

- **CNA has a native C++ XNA content pipeline** (`modules/content-pipeline`: FBX/X importers,
  `BuildContent`, the `cna-content` CLI; parity campaign in `../cna/plans/plan_xna_sample_xnb_sweep.md`).
- **CNA's Guide** is a real overlay with blocking `End*` (modal frames), but the overlay is installed
  **only** by `GamerServicesDispatcher::Initialize`. Phone games, which have no `GamerServicesComponent`,
  therefore get invisible dialogs. By code inspection this also affects the high-score name prompts of
  ✅ SAMPLE-061 MarbleMaze and SAMPLE-063 HoneycombRush; that observation was not reopened. SAMPLE-065
  and 071 work around it with documented `CNAEXT RenderPending*EXT` lines.
- **CNA avatars** (`../cna/docs/avatars.md`) implement `AvatarDescription`/`AvatarAnimation`/
  `AvatarRenderer` with original CNA bodies. Key facts:
  - valid `CreateRandom`;
  - 31 real preset clips (Stand0–7, Celebrate, Clap, Wave, …);
  - `Ready` state, `ParentBones`, `BindPose` (pure translations; the rig root is at the origin);
  - `Draw(IAvatarAnimation)` and `Draw(bones, expression)`, which renders the bones and maps the
    expression to mouth/eye/eyebrow states;
  - `BeginGetFromGamer`;
  - browser loading (CBIND-143);
  - all catalogs (~30 MB) embedded in the library.

---

## 5. Step 1 in detail: keyboard → GamePad emulation in CNA (authorized, not started)

**Why.** SAMPLE-085/086/094/101 (and other Xbox-only ports) are gamepad-only. This machine has no gamepad
(`/proc/bus/input/devices`). CDP cannot emulate one in Chrome. A virtual `uinput` pad was deliberately
**not** used: `/dev/uinput` is writable, but the device would be visible system-wide and could press
Back in other sessions' games. Without emulation these samples are untestable here and only show an
idle avatar in the gallery.

**Follow the existing pattern exactly** (task `INPUT-EMU-001`, commit `31a560af9`):

- `modules/platform/include/CNA/Platform/Input/KeyboardAccelerometer.hpp` and
  `modules/platform/src/KeyboardAccelerometer.cpp`: a process-wide software source with
  `IsEnabled()`, `SetEnabled()` and `Update(const KeyboardSnapshot&, bool focused)`.
- `modules/runtime/src/Game.cpp` (around line 1398, the event pump): after `keyboard->Update()`, it calls
  `KeyboardAccelerometer::Update(snapshot, getIsActiveProperty())`, and `Update({}, false)` when there
  is no keyboard.
- Public opt-ins are static `CNAEXT` members on the XNA type, for example
  `Accelerometer::setKeyboardEmulationEnabledEXT(bool)` and `getKeyboardEmulationEnabledEXT()`.
- Tests: `modules/devices/tests/Microsoft/Devices/Sensors/KeyboardAccelerometerTests.cpp` drives
  `Update({{KeyCode::Right, KeyCode::Up}, 0}, true)` directly.
- Documentation: `docs/keyboard-device-emulation.md`, `plans/plan_keyboard_device_emulation.md`,
  `CHECKLIST.md` deviations table and `AUDIT.md`. No C ABI addition was needed for INPUT-EMU-001.

**Proposed design** (my research; the owner has not reviewed the key layout, so show it to them):

- Public API: `CNAEXT static void GamePad::setKeyboardEmulationEnabledEXT(bool)` and
  `CNAEXT static bool GamePad::getKeyboardEmulationEnabledEXT()`, off by default and process-wide.
  A sample enables it with one `CNAEXT` line in its constructor, documented in its `diff.md` as an
  owner-requested input addition (precedent: mouse→touch in `samples/TiltPerspective/diff.md`).
- Platform piece, e.g. `CNA/Platform/Input/KeyboardGamepad.{hpp,cpp}`: builds one
  `CNA::Platform::GamepadSnapshot` for **PlayerIndex.One** from the frame's `KeyboardSnapshot`. It is
  neutral when the game is not focused. Its packet number increments when the state changes.
- `modules/input/src/Xna/GamePad.cpp`:
  - in `ReadState(..., game == true)`, for slot 0 while enabled, merge with the physical pad: buttons
    OR'ed, and each axis taken from the keyboard when it is non-zero, else the physical value. Return a
    connected state even when there is no physical pad or platform gamepad service;
  - respect `CNA::Internal::Input::systemOwnsInput()`: while the Guide owns input, the game reads a
    neutral pad. **Do not feed the emulated pad into the system/Guide read path**
    (`SystemInputAccess::read`, `game == false`), because the Guide already reads the keyboard itself,
    and keys would act twice, e.g. typing in a Guide keyboard dialog;
  - in `GetCapabilities(PlayerIndex.One)` while enabled: connected, `GamePadType::GamePad`, every
    mapped button and axis present, OR'ed with a physical pad's capabilities.
- Proposed key layout, three "diamonds" mirroring the pad:

  | Pad | Keys |
  |---|---|
  | Left thumbstick | W A S D (W = +Y) |
  | Right thumbstick | Arrow keys |
  | D-pad | T (up) F (left) G (down) H (right) |
  | Face A / B / X / Y | K / L / J / I (diamond: I top = Y, J left = X, L right = B, K bottom = A) |
  | LB / RB | Q / E |
  | LT / RT | Z / C (0 or 1) |
  | Left / right stick click | Left Shift / Right Shift |
  | Start / Back | Enter / Escape |
  | BigButton | not mapped (CNA's Guide uses Home) |

  Stick values are digital (`right − left`, `up − down`) and diagonals are normalized to unit length,
  as the accelerometer emulation does. The `GamePadDeadZone` processing still applies.
- Document conflicts: the arrow keys also drive the keyboard accelerometer and orientation emulations,
  and a game's own `Keyboard.GetState` still sees every key. Keys are not consumed.
- Tests (in `modules/input/tests`):
  - off by default, nothing changes;
  - enabling makes PlayerIndex.One connected, with no other slot affected;
  - each mapping;
  - diagonal normalization and opposite keys cancelling;
  - unfocused input is neutral;
  - packet-number change;
  - merge with a fake physical pad, if a fake platform gamepad exists in the tests;
  - neutral while the system owns input;
  - capabilities.
  Run them through `tools/platform/run_gpu_tests_private.sh --exec <binary> '--gtest_filter=...'` from
  CNA's cwd, and establish a baseline of the existing input/runtime suites first.
- Commit in CNA with a new task ID, for example `feat(INPUT-EMU-002): ...`. Update
  `plans/plan_keyboard_device_emulation.md`, the docs, `CHECKLIST.md` and `AUDIT.md`. Do not push.
- Real qualification happens through SAMPLE-085 natively and in Chrome (CDP can send keys:
  `Input.dispatchKeyEvent`).
- Estimate: 2–3 h.

---

## 6. Step 2 in detail: the avatar samples on CNA's standard avatar API

The precedent is SAMPLE-087 AvatarShadows (`samples/AvatarShadows/missing.md`), ported by the owner's
Gamer Services work:

- an Xbox-only upstream, with content **rebuilt for Windows/HiDef by the official offline XNA pipeline**
  from the unchanged sources (`/rv/tmp/samples/SAMPLE-087-AvatarShadows_4_0/scripts/build-windows-hidef-content.sh`);
- a line-by-line C++ port;
- CNA avatars documented as a difference;
- gamepad-only input;
- native qualification only; **087 has no browser gate yet.**

Your ports need the full `rules.md` workflow, including the real-Chrome gate and the gallery.
Decisions the owner already made or implied: they chose to proceed with 085 using CNA avatars and the
new keyboard emulation. Record CNA avatars instead of Xbox avatars, and the emulation opt-in, in each
sample's `diff.md`.

| Row | Upstream | What it needs | Estimate |
|---|---|---|---|
| **085** `AvatarAnimationBlendingSample_4_0` | Xbox-only, 2 units (533 lines with metadata), `Font.spritefont` | random avatar; presets Stand0/Celebrate/Clap/Wave; the sample's own `AvatarBlendedAnimation : IAvatarAnimation` (250 ms slerp/lerp per bone); `Draw(IAvatarAnimation)`. LB = blending on/off (text on screen), RB = new avatar, A/B/X/Y = animations, right stick/triggers = camera, right-stick click = reset, Back = exit. 1280×720, multisampling, `GamerServicesComponent`. | 3–5 h |
| **086** `AvatarMultipleAnimationsSample_4_0` | Xbox-only, 392 lines | Celebrate on the body and Wave on the right-arm subtree (`FindInfluencedBones(ShoulderRight=22, ParentBones)`), `Draw(bones, celebrate.Expression)`; LB cycles 3 modes. `ParentBones` is available straight after construction. | ~3 h |
| **101** `ObjectPlacementOnAvatarSample_4_0` | Xbox-only, 361 lines, `baseballbat.fbx` | `BonesToWorldSpace` = `anim[i] · BindPose[i] · world[parent]`, bat at `SpecialRight=49`. CNA's renderer composes identically (identity bind rotations, root at the origin, rotation-only non-root tracks; see its `missing.md`), so **verify visually** that the bat sits in the drawn hand for all four presets (Stand0, Celebrate, Clap, Stand5). | ~3 h |
| **094** `CustomAvatarAnimation_4_0` (if that is what "104" meant) | Xbox game (548 lines) + runtime library (510) + custom processor (446) + 5 custom FBX clips + expression CSVs + ground | Translate the library and the game; rebuild the 5 custom-animation XNBs and the ground for Windows/HiDef through the **unchanged** processor and the official pipeline (the retained `scripts/build-original.sh` already builds it for Xbox); register the reflective `CustomAvatarAnimationData` reader with one `CNAEXT` line (rules.md precedent SAMPLE-049/051, in `diff.md`); keep the upstream defect `PlayRandomIdle` = `Next(3)`. Uses `BeginGetFromGamer` for the signed-in gamer. | 5–8 h |

Each artifact root already exists (`/rv/tmp/samples/SAMPLE-0nn-*`) with `xna4-original/`, an Xbox
`xna4-build/`, `xbox-refs/` and evidence. The originals cannot run here (Xbox 360 CLR), so the
comparison basis is the source, the documentation and the official content. Do not claim an original
capture.

Open measurements for the first avatar port (085):

- the WEBGL2 bundle size and load time with the ~30 MB of embedded avatar catalogs;
- that avatars render in real Chrome through the standard API;
- that the emulated pad works in Chrome through CDP key events;
- clean exit (Escape = Back with emulation; window close natively).

Use visible system Chrome on a private Xvfb (§8), not headless.

---

## 7. Step 3: remaining rows after the avatar series

Open rows by status (from `plan.md`, which is authoritative):

- 🛑 owner decision pending (38):
  - 114 AvatarAnimPack Maya, 115 AvatarAnimPack Mod Tool;
  - 116–118 Avatar rigs (Max 2010, Maya 2009, Softimage Mod Tool 7.5);
  - 119 BasicEffectShader 2.0, 120 ButtonImages, 121 CardsStarterKit VB, 122 Catapult 2.0,
    123 ControllerImages, 124 CustomIndeterminateProgressBar;
  - 125–127 GSMSample (Mango, Mango VB, Phone), 128 LevelStarterKit, 129 LobbyChatImages;
  - 130 MaterialsAndLights 2.0, 131 Minjie 2.0, 132 ModelViewerDemo Mango, 133 Movipa,
    134 MultipassLighting 2.0, 135 NonLinear WP SL navigation;
  - 136/137 PaddleBattle (Mango, VB), 138 Pickture 2.0, 139 PushRecipe WP7 SL, 140 RedistributableTTFs,
    141 Riemers, 142 RobotGame 2.0, 143 RolePlayingGame Phone, 144 SilverlightMicrophone, 145 SoundLab;
  - 146 SpaceShooter 3.0, 147 SpriteBatchShader 2.0, 149 TombstoningSample, 150 UnitConverterStarterKit,
    151 VectorRumble 2.0, 153 XNA_XNB_Format.
- 🛠 148 TiledSpritesSample 3.1 (native done; real-Chrome gate pending). Its old note blames an
  extension route; the gate is now system Chrome as for 107.
- ⏸ 100 NetworkPrediction, 106 SavingEmbeddedImages (CNA work list).
- 🟡 104 PerformanceUtility (browser networking incomplete).
- ↗ 152 Racing (last; governed only by `plan_racing.md`).

The pattern this session settled into, which the owner liked: **one row at a time**.
1. Do a short current-head analysis: what the sample is, what changed in CNA/Sharp since the last
   audit, the remaining gaps and realistic options with estimates.
2. Write it at the top of the row's `missing.md` ("Current-head re-analysis — <date>"), add a short
   note to the `plan.md` row, and update `NEXT.md`'s active section and this file.
3. Commit locally and report to the owner in Czech (the owner writes Czech; repository text is English).
4. Wait for the decision. Typical answers have been "ponech NNN cancelled" (record ⛔ with the quote,
   update the counts and the ⛔ list in the summary table) or "udelej NNN" (implement fully).

Rows 114–118 are avatar art/rig deliveries like 112/113 (no application). Expect a quick analysis.

---

## 8. Techniques and recipes from this session

- **Build scripts** follow SAMPLE-102/107:
  - `scripts/build-cna-native.sh` and `build-cna-web.sh` in the artifact root;
  - `cmake [--fresh] -S cna-samples -B <root>/cna-native-opengles3` (or `emcmake ... cna-web-webgl2`);
  - `-DCMAKE_BUILD_TYPE=Release -DCNA_SHARED_LIBRARY=OFF -DCNA_GRAPHICS_RENDERER=OPENGLES3|WEBGL2`;
  - `-DCNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF -DCNA_SAMPLES_ONLY=<Name>`;
  - `-DCNA_SAMPLES_CNA_ROOT=../cna -DCNA_SHARP_RUNTIME_ROOT=../sharp-runtime`;
  - both ccache launchers, `CCACHE_DIR=~/.cache/ccache CCACHE_BASEDIR=/rv`, then
    `cmake --build ... --target <Name>_cna_samples --parallel $(nproc)`.

  With a warm cache the native build takes about 2 min and the web build 2–5 min.
- **Native gate**:
  - an owned Xvfb at the game's size, run with `SDL_VIDEODRIVER=x11 LIBGL_ALWAYS_SOFTWARE=1`;
  - capture with `import -window`, give input with `xdotool`;
  - a bare Xvfb logs an SDL "fullscreen mode-switch timeout", which is only an environment message;
  - close with an X11 `WM_DELETE_WINDOW` client message, because there is no window manager and
    `xdotool` 3.2016 cannot do it: `/rv/tmp/samples/SAMPLE-107-TiltPerspective_4_0/scripts/send-wm-delete.c`.
    Record the exit code.
- **Web gate**:
  - **visible** `/usr/bin/google-chrome` on an owned Xvfb, `--app=http://127.0.0.1:<port>/<page>` over
    plain HTTP, SwiftShader ANGLE flags, a temp `--user-data-dir`;
  - drive it over CDP from a Node script: `/home/robertvokac/emsdk/node/24.19.0_64bit/bin/node`;
  - check WebGL 2, the canvas size, rAF count, and no exceptions, rejections or HTTP errors;
  - give input with `Input.dispatchMouseEvent`, `dispatchTouchEvent` and `dispatchKeyEvent`;
  - wait for Chrome to exit before deleting its profile.

  Templates: `/rv/tmp/samples/SAMPLE-107-TiltPerspective_4_0/scripts/{capture-cna-web-gate.sh,chrome-tilt-gate.mjs}`.
- **Measure behaviour, don't just screenshot it.** For 107 a script found the back-wall edges in each
  frame (`analyze-tilt-frames.py`) to prove the recalibration. For avatars, consider measuring something
  similar: frame differences over time, the blend progressing over about 250 ms, the bat near the hand.
- **Original reference runs**:
  - XNA Windows/Reach originals run under Wine (`/home/robertvokac/.wine-cna-xna40`,
    `WINEDLLOVERRIDES=d3d9=b`, owned Xvfb);
  - WinForms originals can be compiled with Mono `mcs` against
    `/rv/tmp/samples/_tools/xna-game-studio-4-refresh/admin/Program Files/Microsoft XNA/XNA Game Studio/v4.0/References/Windows/x86`
    (SAMPLE-093, SAMPLE-109 scripts);
  - Xbox and Phone originals cannot run here.
- **CNA content builder**: `../cna/build-probe/cna-content build <src> -o <out> --format xnb --xnb-platform windows --xnb-profile reach|hidef --xnb-version 5 --xnb-compress none --xna-compatible`.
  It is a diagnostic only; the campaign prefers official XNA pipeline output for sample content.
- **CNA focused tests**: build test targets (`CnaGamerServicesTests`, `CnaMediaTests`, …) in a
  `cna-native-opengles3-analysis/` tree inside the sample root (SAMPLE-105/106 scripts), then run them
  through `tools/platform/run_gpu_tests_private.sh --exec`.
- **Gallery** (`../samples.libcna.com`):
  - copy the exact four bundle files and verify the hashes;
  - add a 480×800 or 800×480 shot of the running game in `assets/img/` (portrait cards use
    `class="portrait-thumb"`);
  - add a detail page modelled on `TiltPerspective.html` (Previous/Next pager);
  - add a card: 12 per page. `page-8.html` currently holds 1 (sample 85 of 85); update
    "Samples x–y of N" on every page;
  - check every local link, gate the gallery copy in Chrome, and after a push verify the deployed bytes
    (`gh run list -R libcna/samples.libcna.com`, then `curl` and hash).
- **Prune** only on the owner's explicit request:
  - dry run first (`tools/prune-completed-sample.sh SAMPLE-nnn-Dir`);
  - record before/after hashes;
  - `--apply`, verify retained hashes, re-run the stripped native gate, extend `MANIFEST.md`, and
    run a second dry run.

---

## 9. Standing rules and owner preferences (unchanged)

- Use the exact upstream and real XNA as authority, then local FNA at
  `/rv/data/library/github.com/FNA-XNA/FNA`. Review all products, source units, content processors,
  conditional branches and non-default interactions. Retain complete upstream snapshots
  (`xna4-original/`).
- No sample workaround for a framework defect. Fix XNA behaviour generally in CNA, .NET behaviour
  generally in Sharp Runtime, and translation/content in samples. No omitted features, invented
  service, raw content substitute, hand-translated shader or reduced game labelled complete.
- Owner-requested differences go in an **English `diff.md`**, cross-referenced by `missing.md` and the
  plan row. Shared emulation stays off by default and is enabled by a minimal `CNAEXT` call.
  - The owner requires mouse→touch on touch-only ports (`TouchPanel::setMouseTouchEmulationEnabledEXT(true)`).
  - Accelerometer and keyboard orientation emulation are separate opt-ins.
  - The new keyboard→GamePad emulation will be another one.
  - Do not invent per-game key logic.
- Only EasyGL is in scope: native **OPENGLES3** and browser **WEBGL2**, static Release products.
  Web threads stay OFF unless the original source requires threading.
- Use the shared ccache (`CCACHE_DIR=~/.cache/ccache`, `CCACHE_BASEDIR=/rv`, both launchers). Never
  create another cache or change its size. Use all cores and lower jobs only for a measured memory
  problem. Standalone Sharp Runtime builds obey its own two-job policy.
- Keep generated builds, diagnostics, profiles and captures only under the sample's
  `/rv/tmp/samples/SAMPLE-nnn-*` root.
- **Never inject system-wide input devices** (uinput) and never touch the owner's live desktop
  display. Other sessions run on this machine.
- If VirtualBox has lock or hypervisor trouble, stop VM work and tell the owner; do not kill other
  VMs or emulators.
- Run changed-component regressions against a baseline. Do not attribute inherited failures or
  warnings to your change.
- Commit with explicit file lists and the `SAMPLE-nnn` identifier (CNA: its task ID). Preserve
  unrelated edits, for example CNA's untracked `xna-games/`. Push only on explicit request.
- Prune only on explicit request, for that sample only.
- Report to the owner in Czech, concisely. Repository documentation is English.

---

## 10. First actions in the next context

1. Read §2, then check the heads and statuses (§3), preserving concurrent work.
2. Ask or confirm: whether to push the 11 local commits; whether "104" meant 094.
3. Implement §5 (CNA keyboard→GamePad emulation) with tests and documentation, and commit in CNA.
4. Port SAMPLE-085 per §6 and `rules.md`: reopen the row to `🛠`, write `diff.md`, run the native and
   real-Chrome gates, add the gallery entry, mark `✅`, commit, and offer the prune dry run. Then
   SAMPLE-086, SAMPLE-101 and SAMPLE-094 the same way.
5. Continue with the remaining rows (§7), one analysis at a time, waiting for the owner's decisions.
