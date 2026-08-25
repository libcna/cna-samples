# SAMPLE-024 — FlockingSample_4_0 audit record

Audit date: 2026-08-25. Upstream directory:
`/rv/tmp/XNAGameStudio/Samples/FlockingSample_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-024-FlockingSample_4_0`.

## 1. What upstream actually contains

32 files, 14 C# sources — the largest source count since SAMPLE-014:

| Path | Role |
|---|---|
| `Flocking/Flocking/FlockingSample.cs` | The game, and the `AIParameters` struct declared above it. |
| `…/Flock.cs` | 40 birds; every bird reacts to every other bird and to the cat. |
| `…/InputState.cs` | Keyboard/gamepad edge detection and the analog fallbacks. |
| `…/Animals/{Animal,Bird,Cat}.cs` | The animal base, the flock member, the player-controlled cat. |
| `…/Behaviors/{Behavior,Behaviors}.cs` | The reaction base class and `List<Behavior>`. |
| `…/Behaviors/{Align,Cohesion,Flee,Separation}Behavior.cs` | The four reactions. |
| `…/Program.cs` | `Main`, inside `#if WINDOWS || XBOX`. |
| `FlockingContent/{cat,mouse}.tga`, `xboxControllerButton{B,X,Y}.tga`, `HUDFont.spritefont` | The content — all four textures are TGA. |
| `Background.png`, `GameThumbnail.png`, `Flocking.htm`, `Documentation/`, licence | Shell artwork and documentation. |

`Program.cs` and `FlockingSample.cs` carry `#if`; both are preserved. The Windows
configuration is the audited one.

The upstream snapshot is retained at `xna4-original/` with per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## 2. The plan row's flag, and what it turned out to be

`plan.md`'s row for this sample was worded unlike its neighbours — *"Reconcile any
deliberately unreproduced original behavior with the zero-deviation gate"* — and the old
`missing.md` says what that meant:

> **Bird vertical movement is not doubled (original XNA bug not reproduced)** … *Deliberate
> non-reproduction of an apparent upstream bug; the port implements the evidently-intended
> single-application movement instead.*

The upstream code is exactly as described. `Bird.Update`:

```csharp
Vector2 moveAmount = direction * moveSpeed * elapsedTime;
location = location + moveAmount;          // adds X and Y
// … wrap X …
location.Y += direction.Y * moveSpeed * elapsedTime;   // adds Y a second time
```

so birds drift about twice as fast vertically as horizontally.

**The rule is not ambiguous.** `rules.md` and the campaign handoff both say: *"Preserve
original observable bugs/quirks unless evidence proves XNA itself behaves differently. Do
not 'improve' the sample while porting it."* There is no such evidence here — this is what
XNA 4.0 does. The owner confirmed the reading directly on 2026-08-25: if the XNA 4.0
original has bugs, CNA must have them too.

**The quirk is therefore reproduced**, line for line, with a source comment saying it is
deliberate and why (`src/Animals/Bird.cpp:86`, against the original's `Bird.cs:104`).

### A second upstream defect, which the old record did not mention

`Flock.FlockParams`'s getter is `get { return FlockParams; }` — it returns the property
rather than the field, so reading it recurses until the stack is exhausted. Nothing in the
sample reads it (the game only assigns), so the original never faults. It is reproduced
too, with a `@warning` on the C++ getter explaining what it does and why it is kept.

A third, smaller one is preserved in place: `SliderInputHelper` measures the *separation*
slider's value from `barDetectionDistance.X`, not from its own bar's X. Both bars share an
X so it makes no difference, and it is left as written rather than "corrected".

## 3. Original XNA 4.0 build and run

`scripts/build-original.sh` builds the content through the official `BuildContent` task
for both target platforms and links `Flocking.exe` from the game's 14 unmodified sources
with the Windows defines. `scripts/capture-original.sh` runs it under an isolated Xvfb
display with `WINEDLLOVERRIDES=d3d9=b`, driving the sample's own keys.

## 4. Content provenance

All **6** XNBs are byte-identical to this sample's own official pipeline output for the
Windows target (`cmp`); hashes for both platforms in `evidence/content-sha256.txt`. Four of
them come from `.tga` sources, the second sample in this campaign to use that importer.

The previous port shipped loose `cat.png` and `mouse.png` and **had no font and no button
glyphs at all**, so the entire HUD was absent. `help.png` moved to the sample root.

## 5. What the previous port claimed, and what is actually true

| Old claim | Finding |
|---|---|
| "SpriteFont / HUD labels omitted … Porting simplification — **not** a current CNA framework limitation" | Correct diagnosis, never acted on. Both slider bars, both labels, the pulsing red tint on the selected label, and the three button glyphs with their text are all restored. |
| "Xbox controller button textures omitted" | Restored, from the official `.tga` XNBs. |
| "Touch input omitted … Phone-specific input not applicable to desktop" | **False.** CNA has the whole `Input::Touch` surface. The raw-touch slider drag and the `Tap` gesture handling for the three buttons are translated exactly. |
| "Gamepad trigger slider control omitted … CNA's `GamePadState` does implement `getTriggersProperty()`" | Restored; `InputState::getSliderMoveProperty()` falls back to `-Triggers.Left + Triggers.Right` as upstream does. |
| "Bird vertical movement is not doubled" | **Reversed.** See section 2. |
| "Flock/cat movement bounds use full viewport instead of TitleSafeArea" | Restored; `SpawnFlock` and `ToggleCat` use `Viewport.TitleSafeArea` as upstream does. |

## 6. Translation

`samples/FlockingSample/src/` mirrors the original file decomposition as `.hpp`/`.cpp`
pairs — the SAMPLE-013/014/023 arrangement, needed here because `Behavior` and `Animal`
reference each other. Namespace `Flocking`; `GetTypeName()` returns
`"Flocking.FlockingSample"`.

`AIParameters` is upstream declared at the top of `FlockingSample.cs`; it is its own header
here only because `Bird` and every behavior take it by reference and compile before the
game does. Nothing about the type changes. The previous port's `AIParameters.hpp` is
therefore kept, but its contents are now the original's eight fields rather than a
reinvention.

### Deviations, all mechanical

| Original | Port | Why |
|---|---|---|
| `Behaviors : List<Behavior>` | `List<std::shared_ptr<Behavior>>` | C# holds references to polymorphic behaviors; C++ must state ownership. |
| `Dictionary<AnimalType, Behaviors> behaviors` | same, read through `TryGetValue` | The port's read site needs a non-const lookup. |
| `flock.Capacity = flockSize` | `flock.reserve(flockSize)` | Same call — and load-bearing here, because each bird's behaviors hold a pointer back to it and the storage must not reallocate. |
| `Cat cat; Flock flock;` (nullable) | `std::unique_ptr` | C# reference fields set to null; `ToggleCat` really does destroy the cat. |
| `SpriteFont hudFont;` | `std::optional<SpriteFont>` | SAMPLE-017's established pattern. |
| `#if WINDOWS \|\| XBOX` | `WINDOWS` defined for the sample target | The desktop and web builds are the Windows configuration; the original's Windows project defines it. |

`onePixelWhite.SetData(...)` is the **original's own** call and stays.

## 7. Framework work this sample required

One change, in `../cnanext`.

**`Vector2` needed `operator*=` and `operator/=`.** The flocking behaviors write
`pullDirection *= weight` and `pushDirection *= weight`; C# synthesises those from
`op_Multiply` and `op_Division`, and C++ does not. This is the same gap SAMPLE-020 found
for `+=`/`-=`, and it is fixed the same way — a `CNAEXT` pair each for the vector and the
scalar form, matching the `Vector3` precedent.

Six tests in `modules/math/tests/…/Vector2Tests.cpp`, including that each compound form
agrees with its binary form and that all four return a reference to `*this`.

No sharp-runtime change was needed.

## 8. What was measured

This sample is a continuously simulated flock seeded from a time-based `Random`, and the
selected slider's label pulses on `sin(10 * TotalGameTime)`. **No whole frame can be
byte-identical between two runs**, so the comparison targets what is deterministic.

The two slider bars are exactly that: they are drawn *after* the flock, so nothing occludes
them, and each bar's orange button sits at a position that is a pure function of the AI
parameter it displays.

| Frame | Detection bar (205,35)-(291,85) | Separation bar (205,115)-(291,165) |
|---|---|---|
| `start` | **4300 / 4300** | **4300 / 4300** |
| `settled` | **4300 / 4300** | **4300 / 4300** |
| `cat` | **4300 / 4300** | **4300 / 4300** |
| `slider2` | **4300 / 4300** | **4300 / 4300** |
| `slider3` | 4290 / 4300 | 4122 / 4300 |

Byte-identical in four of five frames; in the fifth a bird happened to fly across the bar
between the two runs' captures.

Frame-wide, the pixels that are **exactly** orange (255,165,0) — the two 10x40 slider
buttons — number **800 in the original, 800 in the native port and 800 in the browser, in
every frame captured**. That single number pins both bars' geometry and both AI parameter
values at once.

Whole-frame agreement is 372160–372611 of 384000 across the five frames; the remainder is
the flock and the pulse phase, neither of which can agree between runs.

### The browser

`scripts/capture-web.sh` drives Google Chrome 151.0.7922.71 on its own Xvfb display.
Beyond the 800-orange-pixel match, the gate asserts the discrete behaviour:

- adding the cat with **Y** leaves both bar regions byte-identical;
- **Down** then holding **Right** moves the *separation* bar (1120 subpixels changed) and
  leaves the detection bar **byte-identical** — so slider selection works and affects only
  the selected slider;
- **B** restores the separation bar to its start bytes exactly.

`moduleReady`, `webgl2: true`, the banner `CNA: graphics renderer: WEBGL2`, no unhandled
rejection, no runtime exception, no HTTP error, no fatal console message.
`evidence/cna-web-webgl2/browser-result.json`.

### What is not measured

The doubled vertical movement is established by **line-for-line translation**, quoted in
section 2 and verifiable by diffing `src/Animals/Bird.cpp` against `Bird.cs`. It is not
separately measured at runtime: the flock is randomly seeded, so a statistical
speed-anisotropy measurement would need a harness this audit did not build. A future
session wanting stronger evidence could add one.

## 9. Scans

No `NOXNA`, no CNAEXT usage in the sample, no renderer/backend include, no loose non-XNB
content, no invented control, no help overlay, no runtime file parsing. The single
`SetData` is the original's own one-pixel white texture. `help.png` sits at the sample root
and is never loaded.

## 10. Known differences

None active. The three upstream defects in section 2 are reproduced, not deviations.

## 11. Regression

- `CnaTests` full suite after the `Vector2` change: the same 14 failures present on
  unmodified `next`, no new one. Log: `evidence/cnatests-full.log`.
- sharp-runtime was not touched; its suite stands at 17853/17853.
