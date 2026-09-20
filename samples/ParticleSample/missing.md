# ParticleSample — SAMPLE-029 audit record

## Requalification — 2026-09-20

The complete physical `ParticleSample_4_0` was rechecked against the retained
`xna4-original/` snapshot: `evidence/requal-20260920/upstream-snapshot-diff.txt`
is empty. Its unchanged Windows game and official Windows/Phone content rebuilt
successfully. All three Windows XNBs are byte-identical to the port's files
(`evidence/requal-20260920/content-gate.txt` and `content-pair-sha256.txt`).
The historical `help.png` is restored at the sample root, outside `Content`.

The source audit found three small translation omissions, now corrected: the
`CNAEXT`-using header directly includes `CNA/CNAHelper.hpp`; the desktop entry
point has the original `WINDOWS || XBOX` guard; and CMake defines `WINDOWS` for
the desktop target. The Phone-only branches were syntax-checked with both the
native and Emscripten compilers (`phone-branch-check.log`). No sample-local
workaround, framework change, runtime change, stub or active deviation was needed.

Fresh Release `OPENGLES3` and non-threaded Release `WEBGL2` were built against
`/rv/data/development/github.com/libcna/cna` and
`/rv/data/development/github.com/libcna/sharp-runtime`, with compiled effects
enabled and at most four compile jobs. The unchanged original and CNA native
captures were made on isolated displays. In the SmokePlume state, the first two
overlay rows (4200 + 5400 pixels) have identical raw RGB bytes between original
and native. These rows also have identical white-glyph masks. The rest of the
image is intrinsically random and often overdraws the HUD, so no whole-frame
pixel equivalence is claimed. The canonical stripped native executable starts,
reports `OPENGLES3`, and exits cleanly on Escape.

The real-Chrome gate uses five screenshots after keyboard and real touch input:
Explosions → SmokePlume → Explosions → SmokePlume → Explosions. OCR of the
effect-name row positively reads each state even when additive particles wash
out all pure-white pixels; the top SmokePlume row's white mask matches the
fresh XNA capture exactly. The second row is measured but not asserted in the
browser because the random plume sometimes reaches it. The gate also verifies
particles in all five states, 800×480
WebGL2, title, renderer log, four touch DOM events, distinct frames, requested
bundle files and no runtime/HTTP/fatal errors. Evidence is under
`/rv/tmp/samples/SAMPLE-029-ParticleSample_4_0/evidence/requal-20260920/`.
The complete fresh bundle and native product replace the stale retained copies;
the gallery gets that same web bundle and a fresh native screenshot. No public
deployment or pruning was requested.

The sections below preserve the original porting audit and its historical
measurements; the requalification above is the current verification record.

Upstream: `ParticleSample_4_0`, ported against the unchanged XNA 4.0 sources snapshotted at
`/rv/tmp/samples/SAMPLE-029-ParticleSample_4_0/xna4-original`, per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## 1. What was ported

The whole sample, 1130 lines of C# across 8 files, as `.hpp`/`.cpp` pairs mirroring the
original's own layout. The previous port was a single header plus `Program.cpp`.

| Original | Port |
|---|---|
| `Particle.cs` | `src/Particle.{hpp,cpp}` |
| `ParticleSystem.cs` | `src/ParticleSystem.{hpp,cpp}` |
| `ExplosionParticleSystem.cs` | `src/ExplosionParticleSystem.{hpp,cpp}` |
| `ExplosionSmokeParticleSystem.cs` | `src/ExplosionSmokeParticleSystem.{hpp,cpp}` |
| `SmokePlumeParticleSystem.cs` | `src/SmokePlumeParticleSystem.{hpp,cpp}` |
| `ParticleSampleGame.cs` | `src/ParticleSampleGame.{hpp,cpp}` |
| `Program.cs` | `src/Program.cpp` |
| `Properties/AssemblyInfo.cs` | `src/Properties/AssemblyInfo.cpp` |

## 2. What the previous port claimed, and what is actually true

Six deviations were recorded. **Five were false**; every framework feature they said was
unavailable is present and is now used.

| Old claim | Finding |
|---|---|
| "Additive blending not used … all three particle systems drawn in one shared `SpriteBatch::Begin()/End()` … the single-block workaround was kept here for Vulkan-backend safety" | The record argues itself out of its own workaround — it says outright that "a faithful three-Begin/End port with `BlendState.Additive` would render correctly on EasyGL — only Vulkan would break", and EasyGL is this campaign's only renderer. This is the same shape SAMPLE-026 was corrected for. The port has the original's three separate `Begin(SpriteSortMode::Deferred, blendState)` pairs, and the explosion is additive again. |
| "`DrawableGameComponent` not used — CNA component system not confirmed to support this pattern" | **False.** `DrawableGameComponent`, `Game::getComponentsProperty()` and `DrawOrder` all exist. The three systems derive from `DrawableGameComponent`, are added to `Components`, and set `AdditiveDrawOrder`/`AlphaBlendDrawOrder` exactly as upstream does. |
| "Touch tap gesture not implemented" | **False.** `TouchPanel::setEnabledGesturesProperty`, `getIsGestureAvailableProperty` and `ReadGesture` are all present. The constructor enables `GestureType::Tap` and `HandleInput` drains the gesture queue, as upstream does. |
| "SpriteFont substituted — DejaVu Sans via `tools/make_font.py`, because CNA has no `.spritefont` support and Segoe UI is unavailable" | The CNA half is stale — the font is built by the official `FontDescriptionProcessor`. The Segoe UI half was **correct at the time**: the machine had only the redistributable *Segoe UI Mono* that ships with XNA Game Studio. The owner supplied `segoeui.ttf`; see §3. |
| "Status text reformatted … three separate DrawString calls with condensed wording" | Restored to the original's single multi-line `DrawString`, verbatim, including the mention of tapping the screen. |
| "Windows Phone full-screen/frame-rate branch removed" | Restored under `#if defined(WINDOWS_PHONE)`, as are the three reduced particle counts the phone build uses. |

## 3. The one real constraint, and how it was resolved

`font.spritefont` asks for **"Segoe UI"**. The Wine prefix had only *Segoe UI Mono* — a
different family, redistributed by Microsoft with XNA Game Studio itself, which is why
SAMPLE-027 and SAMPLE-028 (both of which ask for the Mono face) built without trouble. The
proportional Segoe UI is a Windows system font and was nowhere on the machine; both
platform targets refused:

```text
The font family "Segoe UI" could not be found.
```

Both textures built for both targets regardless, so the block was the font alone. The owner
supplied `segoeui.ttf`. Dropping it into the prefix's `Fonts` directory was not enough —
Wine's GDI did not enumerate it — so it was registered:

```text
HKLM\Software\Microsoft\Windows NT\CurrentVersion\Fonts
  "Segoe UI (TrueType)" = "segoeui.ttf"
```

After that both targets build. The resulting `font.xnb` is the same size as SAMPLE-027's and
SAMPLE-028's, because all three pack the same glyph range into the same atlas — but its
bytes differ from both, so the pipeline really did use Segoe UI and did not fall back.

## 4. Content

All **3** XNBs are byte-identical to this sample's own official pipeline output for the
Windows target (`cmp`), hashes for both platforms in `evidence/content-sha256.txt`. The
previous port's `font.font.json` + `font.png` pair and its loose PNGs are gone.

## 5. C++ mapping notes — not deviations

- `string.Format("{0}", currentState)` prints a C# enum member's own name. C++ has no such
  reflection, so `ParticleSampleGame::ToString(State)` spells the two names out, with a
  comment saying why.
- C#'s `Particle[]` holds references and `Queue<Particle>` holds the same references. The
  port uses `std::vector<Particle>` sized once in `Initialize` and `Queue<Particle*>`
  pointing into it, which reproduces that aliasing exactly.
- The three systems are `std::unique_ptr` members of the game and are added to `Components`
  as raw pointers, because `GameComponentCollection::Add` takes `IGameComponent*` and the
  game outlives them.

## 6. Verification

Both builds are EasyGL: native `OPENGLES3` and web `WEBGL2` under Emscripten. Both windows
are titled `ParticleSample`, from the ported `AssemblyInfo.cs`.

**This sample resists pixel comparison more than any so far, and the gate is correspondingly
narrow.** Every particle is spawned with a random speed, direction, lifetime, scale and
rotation; the game draws its overlay *before* the components, so particles overdraw the
text; and the plume spawns at `Viewport.Width/2` and rises straight through it. Three
approaches were tried and measured before one held:

- **Comparing whole text bands** — fails: smoke reaches most of them.
- **Per-pixel minimum over an 8-frame burst**, to subtract the moving particles — fails,
  and the measurement said why: the plume is *alpha-blended* smoke, so it darkens white
  text rather than only brightening it. The minimum removed the text as well.
- **Excluding smoke-covered pixels from the numeric columns** — measures nothing: all 5500
  pixels of that box are under smoke in both engines, so it "passed" vacuously. Recorded
  here because a vacuous pass is worse than a failure.

What does hold, from `evidence/comparison.txt`, in the SmokePlume state three seconds after
the switch (both explosion systems then idle past their longest particle lifetime):

| Overlay line (x 40–339) | Pixels | Result |
|---|---|---|
| effect name | 4200 | **identical** |
| instructions | 5400 | **identical** |
| "Free particles:" | 5400 | 112 differ, worst channel delta 3 |
| Explosion count line | 5400 | 404 differ, worst delta 10 |
| ExplosionSmoke count line | 5400 | 582 differ, worst delta 22 |
| SmokePlume count line | 5400 | 1125 differ (its own count is random) |

Two full lines of Segoe UI text byte-identical across 9600 pixels; the lines below differ
only by the plume's faint outer haze, tens of levels out of 255, not by their glyphs. The
two saturated counts themselves sit directly above the plume's spawn point and cannot be
compared automatically at all; they were read from the retained side-by-side crop
(`evidence/overlay-side-by-side.png`), where both engines show **25** and **40**.

The initial 2026-09-09 browser gate asserted the effect-name mask, canvas,
renderer, title, particles and error-free startup. Its record is retained in
`evidence/cna-web-webgl2/browser-result.json`. The 2026-09-20 requalification
above adds positive state recognition and two real tap gestures, and is the
current browser verification record.

## 7. Deviations

None.
