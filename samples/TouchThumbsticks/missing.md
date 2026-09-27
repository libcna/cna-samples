# SAMPLE-080 — Touch Thumbsticks audit

## Current-head completion — 2026-09-27

**Status: complete on CNA `next 5572f3ca1` and Sharp Runtime `next 9e58c955`.**

The current Release OPENGLES3 and nonthreaded Release WEBGL2 products were
built from the active `libcna/cna-samples` checkout. The native game uses its
adjacent `libcna.so` and exited cleanly after a qualification-only SDL quit.
An external two-finger SDL shim exercised simultaneous left movement and
right aim/fire; captures show both thumbsticks, displaced world/player, red
bullets and both rings disappearing on release. The shim is artifact-only and
is neither linked into nor shipped with the sample. The WEBGL2 bundle ran on
ordinary HTTP in Chrome with `crossOriginIsolated=false`, real two-contact
Chrome touch events, WebGL 2, 600 additional animation frames, and no runtime,
rejection or relevant HTTP errors. The exact four files copied to gallery
commit `91e7c75` passed a second, independent two-touch Chrome run.

The owner's requested desktop mouse behavior uses only CNA's general,
default-off `TouchPanel` bridge. The constructor opt-in is the sole sample
deviation and is recorded in `diff.md`. Separate genuine mouse tests for native
and Chrome show that a left-half drag moves the ship and a right-half drag
aims and fires; one mouse contact operates one stick at a time. The original
touch-state/gameplay code remains unchanged. The exact gallery bundle also
passed a separate Chrome mouse run. The real-game gallery screenshot is from
the fresh native two-contact run while the ship is moving and firing; the
detail page explains that the two rings appear only while touched.

All 22 files in the upstream snapshot remain byte-identical, as do the four
official Phone/Reach XNBs. The omitted original licence and three Phone
package images are restored to the port. The original XNA Wine diagnostic
currently fails before drawing with “No suitable graphics card found”; the
older genuine original gameplay capture remains the reference and is not
presented as a fresh run. No CNA or Sharp Runtime code change was needed.
Current logs, image metrics and browser results are under
`/rv/tmp/samples/SAMPLE-080-TouchThumbsticksSample_4_0/evidence/requal-20260927/`.

## Current-head analysis — 2026-09-27 (before implementation)

The physical upstream Phone/Reach directory contains 22 files. All 22 match
the retained `xna4-original/` snapshot byte for byte. The original project
has six gameplay classes plus `Program.cs`, assembly metadata, a Phone project,
content project, manifests, four source textures, documentation, licence and
Phone package images. The four checked-in `alien`, `bullet`, `player1` and
`thumbstick` XNBs remain byte-identical to the retained official Phone content
pipeline output; each XNB occurs once in the retained web `.data`. The HTML
documentation is also byte-identical to upstream.

The original and ported gameplay sources were reopened together. Touch IDs,
earliest contact position, the left/right half split, 60-pixel stick scaling,
0.75 acceleration, 0.98 drag, right-stick firing threshold/cooldown, camera,
time-seeded stars, enemy spawning, homing, collision order, 1-pixel texture
creation and both draw batches remain close translations. The original's
`Texture2D.SetData` of a white pixel is genuinely present in the source and
is not a workaround. The earlier exact 270-player-pixel visual comparison and
native/browser two-finger results remain historical evidence; none came from
a current-head build.

A fresh run of the unchanged retained XNA diagnostic under Wine with
`WINEPREFIX=/home/robertvokac/.wine-cna-xna40` and
`WINEDLLOVERRIDES=d3d9=b` now stops before drawing: “No suitable graphics
card found. Unable to create the graphics device.” The 1280×1024 Xvfb run
captured that dialog at
`/rv/tmp/samples/SAMPLE-080-TouchThumbsticksSample_4_0/evidence/current-head-analysis-20260927/xna-large-screen/`.
Its screenshot is not a gameplay reference. Preserve the older successful
`evidence/xna-original/01-baseline.png` comparison; the source and original
diagnostic executable have not changed. The Win7 VM was previously blocked
by host `VERR_SVM_IN_USE`, so no new VM result is claimed here.

Gaps identified then (now closed):

- The old native and web products are from September 2026, not the active CNA
  and Sharp Runtime heads. Both build scripts still name retired
  `openeggbert/cna-samples` and omit the shared cache base path. Rebuild Release
  OPENGLES3 from `libcna/cna-samples` and exercise two simultaneous contacts,
  ship movement, right-stick aim/fire, release and clean exit.
- The retained WEBGL2 JS has `PThread`, and its historical Chrome run required
  `crossOriginIsolated=true` plus a COOP/COEP server. The sample itself uses no
  `System.Threading`; rebuild nonthreaded Release WEBGL2, serve it on ordinary
  HTTP and repeat the real-Chrome two-finger and 600-frame checks.
- The owner requested retention of desktop mouse-to-touch emulation for touch
  samples after SAMPLE-077. This port has no opt-in yet. Use CNA's existing
  general off-by-default `TouchPanel` bridge and record the owner-approved
  deviation in `diff.md`. A mouse provides one contact and therefore can
  operate one virtual stick at a time; simultaneous movement and aim still
  require two real contacts. Do not add a parallel sample-local input path.
- Restore the original `Microsoft Permissive License.rtf`, `Background.png`,
  `Game.ico` and `GameThumbnail.png` beside the port. The old artifact manifest
  also contains retired rebuild paths. No gallery card, detail, screenshot or
  exact published bundle exists for this sample yet.

The historical complete status below describes the earlier product and did
not establish a current-head pass. Analysis evidence and the retained artifact root are at
`/rv/tmp/samples/SAMPLE-080-TouchThumbsticksSample_4_0/`.

## Historical September 2026 audit

**Status: complete — no known behavior or content differences from the XNA 4.0 original.**

The historical port was not an acceptable endpoint. It changed all four content identifiers,
substituted loose PNG files for compiled content, added independent keyboard/mouse controls,
published TouchPanel dimensions manually, added Escape/F1 behavior and loaded the documentation
image at runtime. All of those workarounds are removed.

Artifact root:
`/rv/tmp/samples/SAMPLE-080-TouchThumbsticksSample_4_0/`

## Original surface audited

All 22 files in the Windows Phone/Reach XNA 4.0 product were retained and reviewed. The complete
game surface consists of `TouchThumbsticksGame`, `VirtualThumbsticks`, `Ship`, `PlayerShip`,
`EnemyShip` and `Bullet`, plus the platform-provided entry boundary and assembly metadata. The
solution/project, both phone manifests, four source textures, icons, thumbnail, HTML document and
license were also included in the audit.

The port preserves the original namespace and class/member names. `Ship` remains abstract;
reference ownership in the C# `List<Bullet>` is represented by `unique_ptr` elements so the
readonly bullet rotation and deferred-removal behavior remain intact.

The original target requests 800x480, fullscreen on Windows Phone and a 30 Hz target time. It reads
only GamePad Back and raw `TouchPanel` snapshots. There is no keyboard, mouse, gesture recognizer,
Escape exit, F1 overlay or desktop fallback. The first touch in each screen half establishes that
stick's center, the touch ID remains tracked across later snapshots, an available previous location
is used as the earliest center, and displacement is divided by exactly 60 pixels and normalized
only beyond unit length.

The left stick applies 0.75 acceleration per update followed by 0.98 drag. The right stick aims
above magnitude 0.3 and fires a 20-pixel-per-update bullet every 0.15 seconds; otherwise the left
stick controls rotation above magnitude 0.2. The 1000x1000 clamp, bullet cleanup, enemy radius,
four-pixel border, camera transform, 1000 time-seeded stars, immediate first spawn, subsequent
two-second spawn interval, one-or-two-enemy `Random.Next(1, 3)` result, homing, collision/removal
order and both draw batches are direct translations of the original formulas and ordering.

`TouchThumbSticks.htm` is byte-identical to the upstream document. The repository's legacy
documentation-only `help.png` is retained beside it and is neither packaged nor loaded.

## Authentic content

The unchanged official XNA Game Studio 4.0 content pipeline built both Windows/Reach fixtures for
the desktop reference and Windows Phone/Reach fixtures for the authentic target. `Content/`
contains only the four retained Phone outputs, and each is byte-identical to the artifact:

| File | Bytes | SHA-256 |
|---|---:|---|
| `alien.xnb` | 4,283 | `6d581238223fd436431fa45b20811b67424ced275bc5d90e3e5a110ef5cb3448` |
| `bullet.xnb` | 699 | `bab5819fc71182e14541ba267b551c83473d331e26bdce3ff7f3b0caeb1e281a` |
| `player1.xnb` | 4,283 | `0f65e5347d30ba35510336a67b05e9f92cb6992788343708819b59f9bfe467fa` |
| `thumbstick.xnb` | 14,587 | `2cfe77736745edb5ee3561f315b1620684e08590649206ec5e1759c224d08793` |

The original asset names `alien`, `bullet`, `player1` and `thumbstick` are restored. The copied
`Images/*.png` files and sample-local help content are gone.

## Qualification

All CNA builds used `CCACHE_DIR=/rv/cnaccache` and no more than eight parallel jobs.

- The unchanged seven C# units and assembly metadata compiled against the XNA 4.0 Windows
  assemblies with the authentic Phone conditional active. The resulting reference executable ran
  through WineD3D at exactly 800x480 using the official Windows/Reach XNBs. Ordinary X11 pointer
  input produced neither virtual-stick indicator nor player control, as expected for the original
  touch-only implementation.
- Debug and Release OPENGLES3 builds both loaded the four authentic Phone XNBs on a real Mesa
  OpenGL ES 3.2 context. An external artifact-only SDL adapter supplied two independent finger IDs
  below CNA; it is not linked into or shipped with the sample.
- Both native configurations tracked the contacts simultaneously. The centered left contact
  produced about 1,548 green-dominant indicator pixels; dragging it shifted the world/camera.
  While it remained active, the right contact produced about 1,518–1,548 blue-dominant pixels,
  rotation and multiple red bullets. Releasing both removed both indicators, and queued SDL quit
  completed teardown without a runtime error.
- The complete Release WEBGL2 bundle ran in system Google Chrome. CDP supplied two simultaneous
  browser touch IDs. Chrome obtained `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, measured 1,986 green
  pixels for the left stick and 1,994 blue pixels with both active, increased red-dominant pixels
  from 1,972 to 3,335 while firing, removed the indicators on release, completed 600 additional
  `requestAnimationFrame` callbacks and reported no exception, rejection, fatal console message or
  relevant HTTP error.
- The starfield and enemies intentionally use a time-seeded static `Random`, so whole frames are
  nondeterministic. The deterministic 282-pixel player-ship mask at the initial centered position
  nevertheless matched the XNA reference pixel-for-pixel in Debug, Release and browser output;
  all three exact source colors matched at every player pixel.
- No CNA or Sharp Runtime change was required for this sample.

## Retained evidence

- Exact source snapshot and hashes: `xna4-original/`, `original-manifest.txt`,
  `original-sha256.txt`
- Official pipeline outputs and unchanged reference executable: `xna4-build/`,
  `evidence/xna-content-sha256.txt`
- Original run: `evidence/xna-original/`
- Debug and Release native multi-touch runs: `evidence/cna-native-opengles3/`,
  `evidence/cna-native-opengles3-release/`
- Real-browser result and captures: `evidence/cna-web-webgl2-qualified/`
- Deterministic visual comparison: `evidence/visual-comparison.json`
- Reproducible original/native/web build and capture drivers: `scripts/`

---

## Re-audited 2026-09-09: the player-pixel claim verified independently

All eight original types have counterparts, the four XNBs are byte-identical to the official
pipeline output, and the source carries no mouse path, no `F1` and no help image.

**The player-ship claim holds.** The largest connected bright component of the XNA baseline was
located independently — 270 pixels at x 385–414, y 226–253, centred, which is the ship (this
document counts 282 with a looser threshold for its dim edge). Comparing exactly those coordinates:

| product | player pixels differing |
| --- | --- |
| native Debug | **0 of 270** |
| native Release | **0 of 270** |
| browser WEBGL2 | **0 of 270** |

**And the frame around it is nondeterministic, as documented.** The whole baseline differs from XNA
in 650 pixels of 384,000 — 0.2 % — scattered across the entire frame, `x` 1–794 and `y` 0–479. That
is the time-seeded starfield, and it is why a whole-frame RMSE against XNA reads 0.041–0.044 and
means nothing.

**The control leg is verified too, and more sharply than stated.** This document says ordinary X11
pointer input produced neither a stick indicator nor player control. Measured: between
`xna-original/01-baseline.png` and `02-after-pointer.png` the ship mask changes **0 of 270 pixels**
— the ship did not move at all — while 668 pixels elsewhere changed, which is the starfield
advancing. So the reference genuinely does not substitute pointer input for `TouchPanel`, and the
frames it cannot capture are absent for a platform reason rather than an untested one.

Nothing needed correcting.
