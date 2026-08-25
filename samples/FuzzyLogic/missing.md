# FuzzyLogic — SAMPLE-027 audit record

Upstream: `FuzzyLogicSample_4_0`, ported against the unchanged XNA 4.0 sources snapshotted
at `/rv/tmp/samples/SAMPLE-027-FuzzyLogicSample_4_0/xna4-original`, per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## 1. What was ported

The whole sample, 1298 lines of C# across 10 files, as `.hpp`/`.cpp` pairs mirroring the
original's own directory layout:

| Original | Port |
|---|---|
| `Behaviors/Behavior.cs` | `src/Behaviors/Behavior.{hpp,cpp}` |
| `Behaviors/ChaseBehavior.cs` | `src/Behaviors/ChaseBehavior.{hpp,cpp}` |
| `Behaviors/EvadeBehavior.cs` | `src/Behaviors/EvadeBehavior.{hpp,cpp}` |
| `Behaviors/WanderBehavior.cs` | `src/Behaviors/WanderBehavior.{hpp,cpp}` |
| `Entities/Entity.cs` | `src/Entities/Entity.{hpp,cpp}` |
| `Entities/Mouse.cs` | `src/Entities/Mouse.{hpp,cpp}` |
| `Entities/Tank.cs` | `src/Entities/Tank.{hpp,cpp}` |
| `FuzzyLogicGame.cs` | `src/FuzzyLogicGame.{hpp,cpp}` |
| `Program.cs` | `src/Program.cpp` |

`Tank.cs`, `WanderBehavior.cs` and `Behavior.cs` share filenames with three earlier ports
in this campaign. They were diffed before anything was written — this sample's `Tank.cs`
differs from SAMPLE-021's by **427** lines, from SAMPLE-022's by **391** and from
SAMPLE-023's by **421**; `Behavior.cs` differs from SAMPLE-023's by **70** and from
SAMPLE-024's by **82**. Nothing was reused: this `Tank` derives from `Entity` and carries
the fuzzy-logic target selection, which none of the others have.

## 2. What the previous port claimed, and what is actually true

The old port was 518 lines in a single header, with the entire `Behaviors/` and
`Entities/` hierarchies collapsed into it, and it shipped loose PNGs.

| Old claim | Finding |
|---|---|
| "SpriteFont bar labels omitted" | Restored. `hudFont.xnb` is the official pipeline output and the three labels are drawn with `SpriteFont::MeasureString` positioning, exactly as the original does. Their glyph rasters are **pixel-identical** to the original's (see §5). |
| "Touch input omitted — phone-specific" | **False.** CNA has the whole `Input::Touch` surface. The `TouchPanel::GetState()` block — press/move/release, the 20×20 touch rectangle, `Rectangle::Intersects` against each bar, and the drag delta — is translated line for line. |
| "Gamepad D-Pad / thumbstick weight selection omitted" | **False.** `IsPressed(Buttons)`, `DPadUp`/`DPadDown`/`LeftThumbstickUp`/`LeftThumbstickDown` selection and `ThumbSticks.Left.X` / `DPadLeft` / `DPadRight` adjustment are all present. |
| "`Mouse` renamed to `MouseEntity` — C++ name collision with `Input::Mouse`" | **False, and it was the one claim that sounded like a real constraint.** C++ unqualified lookup searches the enclosing namespace first: `FuzzyLogic::Mouse` is found before any name a `using namespace Microsoft::Xna::Framework::Input;` directive injects, because that directive's names behave as members of the nearest namespace enclosing both — the global one. The port keeps the original's name, uses that exact using-directive in `FuzzyLogicGame.hpp`, and compiles clean for both targets. |
| "TGA → PNG conversion; CNA does not support .xnb/TGA" | Stale. Both `.tga` sources go through the official `TextureImporter`/`TextureProcessor` and ship as `.xnb`. |
| "`TitleSafeArea` note" | Was correct and already self-corrected; `Viewport::getTitleSafeAreaProperty()` exists and is used. |

## 3. Framework work this sample required

**`System::TimeSpan::operator+=` / `operator-=`** (`sharp-runtimenext`). `Tank.ChooseBehavior`
does `timeChasingThisMouse += gameTime.ElapsedGameTime;`. C# synthesizes compound
assignment from `operator +`; C++ does not, so the operator has to exist. Added to
`modules/core/include/System/TimeSpan.hpp` and `src/System/TimeSpan.cpp`, forwarding to
`Add`/`Subtract` so they inherit the overflow behaviour, alongside the existing
`Decimal::operator+=` precedent. Six new tests in `modules/core/tests/System/TimeSpanTests.cpp`
cover in-place value, the returned reference identity, and the overflow path; all pass.

Nothing in `cnanext` was missing.

## 4. C++ mapping notes — not deviations

- **`MathHelper::Clamp(value, 0, 1)`** is ambiguous in C++ where C# picks the `float`
  overload: `int`→`float` and `float`→`int` are both standard conversions here, so both
  candidates are viable. The port passes `0.0f, 1.0f`. Same values, same overload.
- **`List<T>`'s indexer** returns a mutation-tracking `ElementReference` proxy whose
  `operator->` yields `const T*` and therefore stops at the element rather than chaining
  into a `shared_ptr`. Each site binds `const std::shared_ptr<Mouse>&` first. This is the
  proxy's documented shape, not a missing feature.
- **`List<Mouse>` → `List<std::shared_ptr<Mouse>>`.** The original's list holds a
  reference type, and both the game and the tank hold the *same* list object; the tank
  also keeps `currentlyChasingMouse` across the frame in which the game removes that
  mouse from the list, and then dereferences it to clear its highlight. A `unique_ptr`
  list would make that a use-after-free; `shared_ptr` reproduces C# lifetime exactly.
- **`CurrentBehavior`** is a `std::unique_ptr<Behavior>` — the C# property owns the
  behavior object and replaces it — and the `is WanderBehavior` / `is EvadeBehavior` type
  tests become `dynamic_cast`.

## 5. Verification

Both builds are EasyGL: native `OPENGLES3` and web `WEBGL2` under Emscripten.

Every mouse is placed at a random position and wanders randomly, so **no two runs of
either build produce the same frame** — not even two runs of the original. The comparison
therefore targets what is deterministic, and the sample offers a strong target: the three
weight bars are drawn *after* every mouse and after the tank, so nothing occludes them,
and each weight **saturates** at 0 or 1 after enough held frames — a state that does not
depend on how many frames actually elapsed.

The original and the native port were driven through the same key sequence
(`scripts/capture-original.sh`, `scripts/capture-cna-native.sh`) and measured
(`evidence/hud-comparison.txt`):

| Frame | Expected widths | XNA 4.0 | CNA native |
|---|---|---|---|
| start (all weights .5) | 42, 42, 42 | 42, 42, 42 | 42, 42, 42 |
| Right held → Distance = 1 | 85, 42, 42 | 85, 42, 42 | 85, 42, 42 |
| Down, Left held → Angle = 0 | 85, 0, 42 | 85, 0, 42 | 85, 0, 42 |
| Down, Right held → Time = 1 | 85, 0, 85 | 85, 0, 85 | 85, 0, 85 |

`(int)(85 * .5f)` is 42, and the selection cycles on the key-**release** edge in both, as
`IsPressed` specifies. The non-pulsing labels' opaque glyph cores are **byte-identical**
between the two builds — 178 px for "Angle", 142 px for "Time", 229 px for "Distance", at
identical coordinates. The pulsing bar's exact tint is a function of
`Math.Sin(10 * TotalGameTime)` and so differs between runs; what is checked is that it
lies on the Red→White lerp line (R = 255, G = B), which both satisfy in every frame.

The random field corroborates rather than gates: at the start frame the original shows 14
sprite blobs totalling 9148 px outside the HUD column and the port 13 totalling 8815 px,
with mouse blobs at 504–510 px in both and the tank at 1939 px against 1938 px.

The web build was driven through the same sequence in real Google Chrome over local HTTP
(`scripts/capture-web.sh`, `scripts/chrome-smoke.mjs`), measuring the canvas pixels:
`measuredWidths` came back `[[42,42,42],[85,42,42],[85,0,42],[85,0,85]]` — identical to
the original's — with `widthsMatch`, `tintsOnLerpLine`, `mostlyBackground`, `framesDiffer`
and `rendererLogged` (`CNA: graphics renderer: WEBGL2`) all true, an 800×480 WebGL2
canvas, and no rejections, runtime exceptions, HTTP errors or fatal console messages.
Full record in `evidence/cna-web-webgl2/browser-result.json`.

## 6. Content provenance

All **4** XNBs are byte-identical to this sample's own official XNA 4.0 Content Pipeline
output for the Windows target (`cmp`); hashes for both platforms in
`evidence/content-sha256.txt`. `tank.tga` and `mouse.tga` come from the `TextureImporter`,
`OnePixelWhite.png` likewise, and `hudFont.spritefont` from `FontDescriptionProcessor`.
The `help.png` the previous port kept in `Content/` was not a content-pipeline asset and
is gone with it.

## 7. Deviations

None. Nothing in the original's behaviour is left unreproduced, and no upstream defect was
found in this sample to preserve or to report.
