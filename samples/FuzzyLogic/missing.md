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

**Default window title** (`cnanext`). Spotted by the owner: every CNA window was titled
`Game`, where the original's reads `FuzzyLogic`.
`modules/graphics/src/Xna/GraphicsDevice.cpp` set `description.title = "Game"` as a
literal. XNA does not: `SDL3_FNAPlatform.CreateWindow` calls
`AssemblyHelper.GetDefaultWindowTitle()`, which reads the entry assembly's
`AssemblyTitleAttribute` and falls back to the assembly's simple name. The new
`CNA::Internal::GetDefaultWindowTitle()` (`modules/core`) is the same chain for a native
program -- the title a program declares through the new `CNA::AssemblyTitleAttributeEXT`,
then the running executable's own file name, then `"Game"` as a last resort so nothing
that declares neither regresses. Five tests cover all three steps.

This also fixes the browser tab: SDL's Emscripten video backend forwards the window title
into `document.title`, which is why the web build showed `Game` too and now shows
`FuzzyLogic`. The web gate asserts it (`titleMatchesOriginal`).

**`Properties/AssemblyInfo.cs`** is a source file of the original that no port in this
campaign had carried. This one does, as `src/Properties/AssemblyInfo.cpp`: a
namespace-scope `CNA::AssemblyTitleAttributeEXT` registers the title before `main()`, the
way the C# attribute is read before the game runs. The executable keeps its
`_cna_samples` suffix at the owner's instruction, so the title cannot come from the file
name -- which is correct anyway, since in XNA the file name is only the fallback.

Nothing else in `cnanext` was missing.

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
`rendererLogged` (`CNA: graphics renderer: WEBGL2`) and `titleMatchesOriginal`
(`document.title` is `FuzzyLogic`, not `Game`) all true, an 800×480 WebGL2 canvas, and no
rejections, runtime exceptions, HTTP errors or fatal console messages.
Full record in `evidence/cna-web-webgl2/browser-result.json`.

## 6. Content provenance

All **4** XNBs are byte-identical to this sample's own official XNA 4.0 Content Pipeline
output for the Windows target (`cmp`); hashes for both platforms in
`evidence/content-sha256.txt`. `tank.tga` and `mouse.tga` come from the `TextureImporter`,
`OnePixelWhite.png` likewise, and `hudFont.spritefont` from `FontDescriptionProcessor`.
The `help.png` the previous port kept in `Content/` is not a content-pipeline asset.
The historical image is retained at the sample root, outside `Content/`, and is not
loaded or displayed.

## 7. Deviations

None. Nothing in the original's behaviour is left unreproduced, and no upstream defect was
found in this sample to preserve or to report.

Both windows -- the native one and the browser tab -- are titled `FuzzyLogic`, the same as
the original's, verified from `xdotool getwindowname` and from `document.title`.

## 8. Requalification on 2026-09-20

The physical `/rv/tmp/XNAGameStudio/Samples/FuzzyLogicSample_4_0` and the retained
29-file `xna4-original/` snapshot still compare byte-for-byte. The Windows and Phone
project declarations, ten C# sources, four source assets and original HTML documentation
were checked against the existing C++ file decomposition and content manifest. Three
port omissions in the previous audit were corrected: the Phone constructor's
`TimeSpan.FromTicks(333333)` and fullscreen settings, the original
`WINDOWS || XBOX` guard around `Program`, and the direct `CNA/CNAHelper.hpp` include
for `CNAEXT`. The historical 502×192 `help.png` was restored from the old port
commit to the sample root, not `Content/`; its SHA-256 is
`7ca4d06c7ce9e0f90c69f1475c7cfb2cdc5652b3cdf55f4c69c1a97916788fb3`.

The unchanged XNA Windows Debug/x86 game was rebuilt with Wine and the official
Content Pipeline via `scripts/build-original.sh`. All four freshly generated Windows
XNBs are byte-identical to the four checked-in assets; the Phone pipeline also
rebuilt all four assets. Original and fresh Release OPENGLES3 captures made on
isolated virtual displays reproduce the same deterministic bar sequence:
`42/42/42`, `85/42/42`, `85/0/42`, `85/0/85`. The unselected Angle and Time
label glyph masks in the start frames match pixel-for-pixel at 178 and 142 white
pixels. Mouse positions and the selected bar's pulse vary randomly by run, so
whole-frame equality is not a valid gate. The stripped canonical native executable
starts with OPENGLES3 and exits with status zero after Escape is held across frames.
Both native and Emscripten compilers syntax-check the Phone-only branch.

The fresh non-threaded Release WEBGL2 work bundle, byte-identical canonical
bundle, and byte-identical local gallery copy each passed `scripts/capture-web.sh`
in system Chrome over HTTP. All three reproduced the four original keyboard
bar states and two actual touch drags: Angle changed from 0 to 85 and Distance
from 85 to 0. Each run reported an 800×480 WebGL2 canvas, the `FuzzyLogic`
title, renderer log, six touch events and no rejected promises, runtime
exceptions, HTTP errors or fatal console messages. WASM has no `debug_info`
marker and the JS has no pthread/shared-memory marker. The gallery serves the
detail, images and four bundle files with HTTP 200, and has 26 cards on
12/12/2 pages. The gallery change is local, not yet published.

Fresh commands, canonical product paths and retention policy are in
`/rv/tmp/samples/SAMPLE-027-FuzzyLogicSample_4_0/MANIFEST.md`.
Fresh captures/results are under `evidence/requal-20260920/` there: in
particular `hud-comparison.txt`, `artifact-sha256.txt`,
`native-canonical-smoke/smoke-result.txt`,
`web-{work,canonical,gallery}/browser-result.json` and
`gallery-http-result.txt`. The exact original `SetData` for the one-pixel bar
texture is retained, not a workaround. No new CNA or sharp-runtime fix or
stub was needed; no active behavior gap or intentional deviation remains.
All compilation used at most four jobs. Work trees and original-content
intermediates remain unpruned pending owner authorization.
