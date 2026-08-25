# SAMPLE-023 — WaypointSample_4_0 audit record

Audit date: 2026-08-25. Upstream directory:
`/rv/tmp/XNAGameStudio/Samples/WaypointSample_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-023-WaypointSample_4_0`.

## 1. What upstream actually contains

25 files, two solutions, one project:

| Path | Role |
|---|---|
| `Waypoints (Windows).sln` / `Waypoints (Phone).sln` | The two solutions. Windows is the audited configuration. |
| `Waypoints/Waypoints/Waypoints (Windows).csproj` | `DEBUG;TRACE;WINDOWS`. |
| `Waypoints/Waypoints/Waypoints (Phone).csproj` | `DEBUG;TRACE;WINDOWS_PHONE`. |
| `…/WaypointSample.cs` | The game: cursor, HUD, input, and the tank as a game component. |
| `…/Tank.cs` | `Tank : DrawableGameComponent`, plus the `BehaviorType` enum. |
| `…/WaypointList.cs` | `Queue<Vector2>` that draws itself as a red→blue gradient. |
| `…/Behaviors/Behavior.cs` | Abstract base; its constructor gives the tank the maximum move speed. |
| `…/Behaviors/LinearBehavior.cs` | Turns instantly, drives straight at the waypoint. |
| `…/Behaviors/SteeringBehavior.cs` | Turns and accelerates gradually, with a turning-radius speed limit. |
| `…/Program.cs` | `Main`, inside `#if WINDOWS || XBOX`. |
| `WaypointsContent/{blank,cursor,dot,tank}.png`, `HUDFont.spritefont` | The content. |
| `Background.png`, `Game.ico`, `GameThumbnail.png`, `Waypoints.htm`, licence | Shell artwork and documentation. |

`Program.cs` is the only file with a `#if`; the game has `#if WINDOWS_PHONE` regions for a
480x800 fullscreen backbuffer, a render-target rotation, an on-screen menu bar and a touch
path. All are preserved in the translation.

The upstream snapshot is retained at `xna4-original/` with per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## 2. Relationship to SAMPLE-021 and SAMPLE-022, measured rather than assumed

This is the sample the other two borrowed from: SAMPLE-021's `Tank.cs` says so in its own
comment ("largely borrowed from the Waypoint Sample with the exception that behaviors have
been removed"), and SAMPLE-022 carries a further-diverged copy.

Diffing before writing anything, as the handoff required:

| File | 023 vs 021 | 023 vs 022 |
|---|---|---|
| `Tank.cs` | 192 differing lines | 262 differing lines |
| `WaypointList.cs` | 82 | 94 |

Three genuinely different files, three different SHA-256s. The structural differences are
not cosmetic: only **this** version's `Tank` is a `DrawableGameComponent` added to
`Game.Components`, only this one has the `BehaviorType` enum and the `Behaviors/`
hierarchy, and only this one's `WaypointList` special-cases a single waypoint to draw it
pure red. Nothing was copied from either existing port.

## 3. Original XNA 4.0 build and run

`scripts/build-original.sh` builds the content through the official `BuildContent` task
for **both** target platforms and links `Waypoints.exe` from the game's eight unmodified
sources with the Windows defines. `scripts/capture-original.sh` runs it under an isolated
Xvfb display with `WINEDLLOVERRIDES=d3d9=b`, driving the sample's own keys: arrows move the
cursor, **A** places a waypoint, **B** cycles the behavior, **X** resets.

## 4. Content provenance

All **5** XNBs are byte-identical to this sample's own official pipeline output for the
Windows target (`cmp`); hashes for both platforms in `evidence/content-sha256.txt`.
`dot.xnb` and `tank.xnb` came out identical to SAMPLE-022's, independently rebuilt from
the same source art — a useful cross-check on the pipeline being deterministic.

The previous port shipped loose `cursor.png`, `dot.png` and `tank.png`, and had **no
`blank` asset and no font at all**. Both are restored. `help.png` moved to the sample root.

## 5. What the previous port was missing

Its `missing.md` did not exist as a deviation catalogue; the gaps were structural:

- `WaypointSampleGame.hpp` and a `TankBehaviorImpl.hpp` that has no counterpart upstream —
  both removed, replaced by the original's own file decomposition.
- No `HUDFont`, so no `Behavior Type:` line and no help text.
- No `blank` texture, so the phone menu-bar branch could not have been translated at all.
- Loose PNGs instead of the official XNBs.

## 6. Translation

`samples/WaypointSample/src/` mirrors the original file decomposition one-for-one, as
`.hpp`/`.cpp` pairs — the arrangement SAMPLE-013 and SAMPLE-014 established for a sample
whose classes reference each other. It is what makes `Behavior` ↔ `Tank` translate
directly: `Behavior.hpp` forward-declares `Tank`, and `Behavior.cpp` includes it.

Namespace `Waypoint` (singular, as upstream). `GetTypeName()` returns
`"Waypoint.WaypointSample"` and `"Waypoint.Tank"`.

`Tank` is a real `Microsoft::Xna::Framework::DrawableGameComponent` added to
`Game::getComponentsProperty()`, so the game's `base.Update`/`base.Draw` drive it exactly
as the original's do — not a hand-called object.

### Deviations, all mechanical

| Original | Port | Why |
|---|---|---|
| `BehaviorType.ToString()` | `BehaviorTypeToString()` | C++ enums have no member names at runtime; the HUD prints the C# names. |
| `Behavior currentBehavior` | `std::unique_ptr<Behavior>` | C# reference field with ownership. |
| `SpriteFont hudFont;` | `std::optional<SpriteFont>` | XNA's SpriteFont is a reference type; SAMPLE-017's established pattern. |
| one `.cs` per class | one `.hpp`/`.cpp` pair per class | C++ needs the split for mutually referencing classes; SAMPLE-013/014 precedent. |

## 7. Framework work this sample required

**None.** No change was needed in `../cnanext` or `../sharp-runtimenext`; `git status` is
clean in both. `DrawableGameComponent`, `GameComponentCollection`, `SpriteFont`,
`Queue<T>`, `MathHelper::Clamp` and the rest already existed and already behaved
correctly, which section 8 demonstrates rather than asserts.

## 8. What was measured

Both builds were driven through the same key sequence on an isolated Xvfb display.

One harness defect had to be fixed first, and it is worth recording because it looked
exactly like a CNA bug: `import -window <id>` returned an **850x480** image for a window
`xwininfo` reported as **853x480** at every instant of the run. The port was not resizing
anything — `import` trims columns for that window shape. Both captures now grab the root
window at the window's absolute geometry, so the two images are the same region by
construction, and the Xvfb screen was widened so the crop is not clipped by the screen
edge either.

With that fixed:

| Frame | Identical pixels (of 409440) | Note |
|---|---|---|
| `start` | **409440 / 409440** | cursor, HUD line, four-line help text, tank |
| `driving` | 409434 | 6 pixels, in one 42x42 box |
| `steering` | 409434 | 6 pixels, same box |
| `linear-again` | 409434 | 6 pixels, same box |
| `reset` | 409437 | 3 pixels |
| `waypoints` | 407399 | 2041 pixels in one 46x54 box |

Every one of those differences is inside a single bounding box, with **0 differing pixels
outside it** in all six frames, and the box is the cursor sprite. The cursor's position is
a time integral (`elapsedTime * cursorMoveSpeed`), so it cannot match to the pixel between
two runs; nothing else in the frame moves.

### The timing-independent measurement

The HUD band (rows 40–90) carries `Behavior Type: <name>` and does not depend on how long
any key was held. Across all six frames it is **byte-identical** between the original and
the port. Better, the discrete transition matches exactly:

| | XNA original | CNA native | CNA WEBGL2 |
|---|---|---|---|
| HUD band, start vs after **B** | 188 pixels differ | 188 | 188 (564 subpixels) |
| HUD band, start vs after a second **B** | 0 differ | 0 | identical |

Three builds, the same 188 pixels changing when the behavior cycles Linear → Steering and
the same exact restoration when it cycles back.

### The browser

`scripts/capture-web.sh` drives Google Chrome 151.0.7922.71 on its own Xvfb display
through the same sequence. Canvas 853x480 — the backbuffer the sample asks for. Waypoint
dots appear on **A** (pure red 0 → 22 pixels, pure blue 0 → 325, the gradient's two ends)
and are cleared by **X** (back to 0 and 0). `moduleReady`, `webgl2: true`, the banner
`CNA: graphics renderer: WEBGL2`, no unhandled rejection, no runtime exception, no HTTP
error, no fatal console message, all four assets served `200`.
`evidence/cna-web-webgl2/browser-result.json`.

## 9. Scans

No `NOXNA`, no CNAEXT usage, no renderer/backend include, no `SetData`, no loose non-XNB
content, no invented control, no help overlay, no runtime file parsing. `help.png` sits at
the sample root and is never loaded.

## 10. Known differences

None active. Nothing was omitted, simplified or substituted.

## 11. Regression

No CNA or sharp-runtime file was changed, so both suites stand where SAMPLE-022 left them:
sharp-runtime 17853/17853, `CnaTests` 8524/8609 with the same 14 failures present on
unmodified `next`.
