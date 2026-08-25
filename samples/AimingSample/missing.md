# SAMPLE-026 — AimingSample_4_0 audit record

Audit date: 2026-08-25. Upstream directory:
`/rv/tmp/XNAGameStudio/Samples/AimingSample_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-026-AimingSample_4_0`.

## 1. What upstream actually contains

16 files, two solutions, one project, one game source:

| Path | Role |
|---|---|
| `Aiming (Windows).sln` / `(Phone).sln` | The two solutions. Windows is the audited configuration. |
| `Aiming/AimingWindows.csproj` / `AimingWindowsPhone.csproj` | The two projects. |
| `Aiming/AimingGame.cs` | The whole sample, plus a nested `Program` inside `#if WINDOWS \|\| XBOX`. |
| `AimingContent/cat.tga`, `spotlight.png` | The content. |
| `Game.ico`, `GameThumbnail.png`, `Aiming.htm`, licence | Shell artwork and documentation. |

Three `#if` regions: the phone back buffer and fixed timestep, the phone's empty
`KeyboardState`, and the `Program` class. All are preserved.

The upstream snapshot is retained at `xna4-original/` with per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## 2. Relationship to SAMPLE-025, established rather than assumed

SAMPLE-025's own source calls this "the previous sample in this series, the aiming sample"
and reuses its texture-centring technique, so `TurnToFace` and `WrapAngle` were diffed
before anything was written, as the handoff required.

The result: the two versions' **code is identical**; only the comment diagram differs —
this sample labels the triangle `S` (spot light) and `C` (cat), SAMPLE-025 labels it `A`
(object) and `B` (target), because by then the helper had been generalised in prose but not
in code. The port reproduces this sample's own comment, not the other's.

## 3. What the previous port claimed, and what is actually true

Four documented deviations, and the port was 174 lines against the original's 357.

| Old claim | Finding |
|---|---|
| "Spotlight additive blending omitted … CNA's Vulkan backend has a bug where only the last `Begin()/End()` pair per frame is rendered … the single-block workaround was kept here **for Vulkan-backend safety**" | The record itself says a faithful two-pass port "would render correctly on EasyGL". EasyGL is this campaign's only renderer. Restored: the cat in its own pass, then the spotlight in `Begin(SpriteSortMode::FrontToBack, BlendState::Additive)`. Section 6 measures that it works. |
| "Windows Phone portrait/fullscreen branch removed … out of scope" | **Not how this campaign works**; `rules.md` requires inactive platform branches to be preserved. All three regions restored, including the unconditional `SupportedOrientations = Portrait`. |
| "Viewport X/Y offset ignored … `GraphicsDevice::UpdateViewportFromWindow()` always hardcodes `viewport_.x = 0`" | The original reads `vp.X`/`vp.Y` in five places; the port now does too. Whether CNA currently ever produces a non-zero origin is beside the point — the translation is of the original's code, and `Viewport::getXProperty()`/`getYProperty()` exist and work. |
| "Texture converted from TGA to PNG … CNA's `ContentManager`/asset pipeline does not support `.xnb`/TGA source assets" | **False, and stale by a wide margin.** This campaign has shipped official `.tga`-sourced XNBs since SAMPLE-022. Both assets are now byte-identical official pipeline output. |

## 4. Content provenance

Both XNBs are byte-identical to this sample's own official pipeline output for the Windows
target (`cmp`); hashes for both platforms in `evidence/content-sha256.txt`. `cat.xnb` comes
from the original 128x128 RLE Targa; `spotlight.xnb` from the original PNG.

The previous port shipped a hand-converted `cat.png` and a copied `spotlight.png`.
`help.png` moved to the sample root.

## 5. Translation

`src/AimingGame.{hpp,cpp}` mirrors the single upstream source, with `Program.cpp` for the
entry point the original nests at the bottom of the same file. Namespace `Aiming`;
`GetTypeName()` returns `"Aiming.AimingGame"`.

Both constants, the spotlight origin trick (`spotlightOrigin.X = 0`, Y at half the
texture's height, so the cone rotates about the light source rather than its centre), the
viewport-relative placement, `TurnToFace`, `WrapAngle`, both draw passes and the whole of
`HandleInput` — including the mouse-pointer control with its `smoothStop` easing — are
present.

### Deviations, all mechanical

| Original | Port | Why |
|---|---|---|
| `spotlightAngle.ToString()`-free; no enums | — | nothing to map. |
| nested `Program` class | `src/Program.cpp` | The campaign's entry-point convention. |
| `MathHelper.Clamp(x, 0, CatSpeed)` | `Clamp(x, 0.0f, CatSpeed)` | C++ overload resolution needs the literal typed. |

No `std::optional` was needed: this sample loads no `SpriteFont`.

## 6. Framework work this sample required

**None.** `git status` is clean in both `../cnanext` and `../sharp-runtimenext`. Third
sample in a row with no framework change.

## 7. What was measured

Nothing in this sample is random. The cat moves only on input and the spotlight turns
towards it at a fixed rate, so once it has finished aiming the frame is fully
deterministic — the first sample in this run of six where a whole frame can match.

| Frame | Identical pixels (of 409440) | Max channel difference |
|---|---|---|
| `start` | **409440 / 409440** | 0 |
| `settled` | **409440 / 409440** | 0 |
| `up` | 409438 | 1 |
| `right` | 409432 | 2 |
| `down` | 409437 | 2 |

Every pixel of all five frames is **within 2** of the original. The handful that differ are
where the cat landed a fraction of a pixel apart, because how far it moved depends on how
many frames the key was held for.

### The additive pass, measured rather than assumed

The old port avoided the original's second `SpriteBatch` pass. The game clears to **black**
and draws the cone with `BlendState::Additive`, so every pixel the cone covers is strictly
brighter than the black beneath it — which makes "did the second pass happen, and did it
blend additively" directly countable.

| Frame | Lit pixels, XNA | Lit pixels, native CNA |
|---|---|---|
| `settled` | 63373 | **63373** |
| `up` | 40050 | **40050** |
| `right` | 61084 | **61084** |
| `down` | 61148 | **61148** |

Exactly equal in all four. The `up` frame lights fewer pixels than the others because the
steeper aim puts part of the cone off-screen.

### The browser

`scripts/capture-web.sh` drives Google Chrome 151.0.7922.71 on its own Xvfb display through
the same key sequence. Canvas 853x480. Lit pixels per frame: 65750, 42391, 63371, 63441 —
the same shape as the native run, differing because the cat's travel depends on frame
timing. The gate asserts the cone lights more than a sixteenth of the screen in every frame
(a threshold set from the original's measured 40050 low-water mark, not guessed), that most
of the screen is still black, and that every frame differs. `moduleReady`, `webgl2: true`,
the banner `CNA: graphics renderer: WEBGL2`, no unhandled rejection, no runtime exception,
no HTTP error, no fatal console message.
`evidence/cna-web-webgl2/browser-result.json`.

## 8. Scans

No `NOXNA`, no CNAEXT usage, no renderer/backend include, no `SetData`, no loose non-XNB
content, no invented control, no help overlay. `help.png` sits at the sample root and is
never loaded.

## 9. Known differences

None active.

## 10. Regression

No CNA or sharp-runtime file was changed, so both suites stand where SAMPLE-024 left them:
`CnaTests` 8529/8615 with the same 14 failures present on unmodified `next`, sharp-runtime
17853/17853.
