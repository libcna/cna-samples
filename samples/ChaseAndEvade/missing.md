# SAMPLE-025 — ChaseAndEvadeSample_4_0 audit record

Audit date: 2026-08-25. Upstream directory:
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

What can: the **static half of the two HUD lines** — the columns holding `Tank State: ` and
`Mouse State: `, x 45–178, y 45–95. That text is drawn at a fixed position every frame.

| Build | Frames | Dominant label region |
|---|---|---|
| XNA 4.0 original | 11 | `2ccb55c0`, in 9 |
| CNA native OPENGLES3 | 11 | `2ccb55c0`, in 7 |
| CNA WEBGL2 in Chrome | 8 | `2ccb55c0`, in 6 |

**The same hash in all three.** The frames that differ are the ones the wandering tank
happened to cross — it moves freely and passes behind the text.

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

No `NOXNA`, no CNAEXT usage, no renderer/backend include, no `SetData`, no loose non-XNB
content, no invented control, no help overlay, no runtime file parsing. `help.png` sits at
the sample root and is never loaded.

## 9. Known differences

None active.

## 10. Regression

No CNA or sharp-runtime file was changed, so both suites stand where SAMPLE-024 left them:
`CnaTests` 8529/8615 with the same 14 failures present on unmodified `next`, sharp-runtime
17853/17853.
