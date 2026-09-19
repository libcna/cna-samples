# SAMPLE-025 — ChaseAndEvadeSample_4_0 audit record

Original audit: 2026-08-25. Fresh sequential requalification: 2026-09-19.
Upstream directory:
`/rv/tmp/XNAGameStudio/Samples/ChaseAndEvadeSample_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-025-ChaseAndEvadeSample_4_0`.

## 1. What upstream actually contains

18 files, two solutions, one project, **one** game source:

| Path | Role |
|---|---|
| `ChaseAndEvade (Windows).sln` / `(Phone).sln` | The two solutions. Windows is the audited configuration. |
| `ChaseAndEvade/ChaseAndEvadeWindows.csproj` / `…WindowsPhone.csproj` | The two projects. |
| `ChaseAndEvade/ChaseAndEvadeGame.cs` | The whole sample: both AI state enums, the tank and mouse AI, the wander/turn helpers, input, drawing — and a nested `Program` class at the bottom inside `#if WINDOWS \|\| XBOX`. |
| `ChaseAndEvade/Properties/{AssemblyInfo,AppManifest,WindowsPhoneManifest}` | Metadata and phone deployment. |
| `ChaseAndEvadeContent/{cat,mouse,tank}.tga`, `hudFont.spritefont` | The content — all three textures are TGA. |
| `Game.ico`, `GameThumbnail.png`, `ChaseAndEvade.htm`, licence | Shell artwork and documentation. |

`ChaseAndEvadeGame.cs` is the only file with a `#if`, and it has three regions: the phone
back buffer and fixed timestep, the phone's empty `KeyboardState`, and the `Program` class.
All are preserved.

The upstream snapshot is retained at `xna4-original/` with per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## 2. What the previous port claimed, and what is actually true

Two documented deviations, and the port was 258 lines against the original's 717 — a
paraphrase, not a translation.

| Old claim | Finding |
|---|---|
| "SpriteFont / state text omitted … **Not** a CNA limitation" | Correct diagnosis, never acted on. Both HUD lines are restored, each drawn twice — black at a one-pixel offset, then white — exactly as the original does. |
| "Windows Phone portrait/full-screen branch removed … Desktop-only port target; phone-specific XNA platform code is out of scope" | **Not how this campaign works.** `rules.md` requires inactive platform branches to be preserved. All three `#if` regions are back, including `graphics.SupportedOrientations = DisplayOrientation.Portrait`, which the original sets unconditionally. |

Neither the old record nor the old port mentioned that **the mouse pointer controls the cat
in the original** — `Mouse.GetState()`, left button held, with a `smoothStop` term that
eases the cat to a halt as it reaches the pointer. That is upstream behaviour, not an
invented control, and it is translated.

One upstream quirk is preserved in place: inside `HandleInput`, a local named
`mousePosition` shadows the field of the same name that holds the mouse *sprite's*
position. The local is the pointer's position and the field is unused in that scope, so it
is harmless; it is kept as written rather than renamed, with a comment saying so.

## 3. Original XNA 4.0 build and run

`scripts/build-original.sh` builds the content through the official `BuildContent` task for
both target platforms and links `ChaseAndEvade.exe` from the unmodified sources with the
Windows defines. `scripts/capture-original.sh` runs it under an isolated Xvfb display with
`WINEDLLOVERRIDES=d3d9=b`, driving the arrow keys that move the cat.

## 4. Content provenance

All **4** XNBs are byte-identical to this sample's own official pipeline output for the
Windows target (`cmp`); hashes for both platforms in `evidence/content-sha256.txt`. Three
come from `.tga` sources.

The previous port shipped loose `Cat.png`, `Mouse.png` and `Tank.png` and **no font at
all**. `help.png` moved to the sample root.

`Content.Load<Texture2D>("Tank")` is the original's spelling, against an asset the pipeline
named `tank`; CNA's case-insensitive content resolution handles it, as the running port
shows.

## 5. Translation

`src/ChaseAndEvadeGame.{hpp,cpp}` mirrors the single upstream source, with `Program.cpp`
for the entry point the original nests at the bottom of the same file. Namespace
`ChaseAndEvade`; `GetTypeName()` returns `"ChaseAndEvade.ChaseAndEvadeGame"`.

Every constant, both enums, the hysteresis logic in `UpdateTank`, the evade-target
reflection in `UpdateMouse`, `Wander`'s random walk and turn-to-centre correction,
`TurnToFace`, `WrapAngle`, `ClampToViewport` and the whole of `HandleInput` are present.

### Deviations, all mechanical

| Original | Port | Why |
|---|---|---|
| `ref Vector2 wanderDirection, ref float orientation` | reference parameters | C++ has no `ref`. |
| `tankState.ToString()` | `ToString(TankAiState)` | C++ enums have no member names at runtime; the HUD prints the C# names. |
| `SpriteFont spriteFont;` | `std::optional<SpriteFont>` | SAMPLE-017's established pattern. |
| nested `Program` class | `src/Program.cpp` | The campaign's entry-point convention. |
| `MathHelper.Clamp(x, 0, MaxCatSpeed)` | `Clamp(x, 0.0f, MaxCatSpeed)` | C++ overload resolution needs the literal typed. |

## 6. Framework work this sample required

**None.** `git status` is clean in both `../cnanext` and `../sharp-runtimenext`. This is the
second sample in a row that needed no framework change, after four that each found one.

## 7. What was measured

The tank and the mouse both wander using a time-seeded `Random`, and their AI states depend
on distances that change every frame, so **no whole frame can match between two runs**.

The old audit attempted to compare the fixed part of the two HUD lines in
columns x 45–177, y 45–94. That crop is **not truly static**: it includes the
first glyph of a changing AI state word. The table below is historical
measurement, corrected in section 11.

| Build | Frames | Dominant label region |
|---|---|---|
| XNA 4.0 original | 11 | `2ccb55c0`, in 9 |
| CNA native OPENGLES3 | 11 | `2ccb55c0`, in 7 |
| CNA WEBGL2 in Chrome | 8 | `2ccb55c0`, in 6 |

The historical `2ccb55c0` hash appears in all three, but its variation to
`42658185` is caused by a different state-word initial. The corrected static
crop is verified across every fresh frame in section 11.

On the two captured frames where both the original and the native port were in the same AI
state, the **state words** are byte-identical too: the tank's word hashes `f8e4a634` in
both and the mouse's `d88f3051` in both. Where the words differ, the difference is the AI
state itself, which a time-seeded wander decides.

Whole-frame agreement runs 391444–405045 of 409440; the remainder is three moving sprites
and two state words.

### The browser

`scripts/capture-web.sh` drives Google Chrome 151.0.7922.71 on its own Xvfb display through
the same arrow-key sequence. Canvas 853x480 — the back buffer the sample asks for.
`moduleReady`, `webgl2: true`, the banner `CNA: graphics renderer: WEBGL2`, every frame
distinct (the cat moves), no unhandled rejection, no runtime exception, no HTTP error, no
fatal console message. `evidence/cna-web-webgl2/browser-result.json`.

The gate's label assertion is a *dominant* hash rather than an invariant one, for the
reason above: the tank can pass behind the text. It requires the dominant appearance in at
least half the frames, and got 6 of 8.

## 8. Scans

No `NOXNA`, no renderer/backend include, no `SetData`, no loose non-XNB
content, no invented control, no help overlay, no runtime file parsing. `help.png` sits at
the sample root and is never loaded. The fresh requalification marks only the
CNA-required `GetTypeName()` override with `CNAEXT`.

## 9. Known differences

None active.

## 10. Regression

No CNA or sharp-runtime file was changed, so both suites stand where SAMPLE-024 left them:
`CnaTests` 8529/8615 with the same 14 failures present on unmodified `next`, sharp-runtime
17853/17853.

## 11. Fresh sequential requalification — 2026-09-19

The retained 18-file original snapshot is byte-identical to the physical
upstream directory (`diff -qr`, 18/18 SHA-256 checks). The unchanged Windows
Debug/x86/Reach XNA game and both official Windows/Phone content sets rebuilt
successfully. All four checked-in Windows XNBs are byte-identical to the
fresh official pipeline outputs. The original ran under the established
XNA 4.0 Wine prefix with `WINEDLLOVERRIDES=d3d9=b` on isolated Xvfb `:141`.

The source review confirmed the complete one-file game logic, including
three AI states for the tank, two for the mouse, random wander, hysteresis,
four shadowed HUD draws, keyboard/gamepad controls, original held-mouse
`smoothStop`, both Phone branches and the desktop-only entry point. The port
now marks `GetTypeName()` as `CNAEXT`, guards `Program.cpp` with the original
`WINDOWS || XBOX` condition and defines `WINDOWS` on the audited desktop
CMake target. Native and Emscripten compilers both accepted the two C++
translation units with `WINDOWS_PHONE` instead; this verifies branch syntax,
not a Phone application run. These are structural/source-fidelity changes,
not workarounds. No CNA or sharp-runtime source was changed or stub added.

Fresh Release OPENGLES3 and non-threaded Release WEBGL2 were built against
the active `../cna` and `../sharp-runtime` checkouts with the shared ccache,
`CCACHE_BASEDIR=/rv` and at most four simultaneous compile jobs. Native
Xvfb `:142` and the stripped canonical native product on `:146` both showed
853×480, responded to held Left/Right, moved the cat toward a held mouse
pointer and exited cleanly on held Escape. The canonical binary has the
active `libcna/cna` SDL RUNPATH. Both the WEBGL2 work bundle and its
byte-identical local gallery copy and its exact site-root URL passed real
system Chrome over HTTP:
853×480 WebGL2 canvas, 12 key events, three pointer events, visible cat
movement, no runtime exception, rejection, relevant HTTP error or fatal
console message. Their four files have no debug or pthread requirement.

The previous audit's 133-column HUD crop included the initial of a mutable
state word, so the two reported hashes were not an invariant rendering
measure. The corrected 120×50 RGB region, x 45–164 and y 45–94, holds
only the fixed label text and has SHA-256
`fa71ef10047d6a81e64e5010ce99cddf13230bb4c245c62a9a808a7071b7aa04`
in **all 69** fresh original/native/web/gallery captures
(13+13+13+10+10+10).
Whole frames differ as expected because `Random` seeds the tank and mouse
independently. The before/after mouse captures show the cat move from the
far right toward the lower-left target in each engine.

The local gallery has 24 cards on two 12-card pages. Its detail page,
neighbour navigation, screenshot, thumbnail and four game files all returned
HTTP 200; the source, canonical and gallery bundles are SHA-256-identical.
No sample workaround, active visual/behavioral difference or missing
dependency is known. Evidence and exact products are under
`evidence/requal-20260919/` and the canonical native/web directories named
in `MANIFEST.md`; that manifest also gives the active checkout and
`--parallel 4` restoration commands. The newly created work trees are
retained; no prune was authorized. No push or public deployment was
requested. The old `../cnanext` / `../sharp-runtimenext` names and test totals
in section 10 describe the historical 2026-08-25 run only.
