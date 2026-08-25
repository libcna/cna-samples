# SAMPLE-020 — TransformedCollisionSample_4_0 audit record

Audit date: 2026-08-25. Upstream directory:
`/rv/tmp/XNAGameStudio/Samples/TransformedCollisionSample_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-020-TransformedCollisionSample_4_0`.

This upstream sample ships **two** runnable products. This record covers the whole
directory and the primary game; the second product has its own record in
[`../TransformedCollisionTest/missing.md`](../TransformedCollisionTest/missing.md),
and `plan.md`'s row cites both.

## 1. What upstream actually contains

36 files, in two independent Visual Studio solutions:

| Path | Role |
|---|---|
| `TransformedCollision (Windows).sln`, `TransformedCollision (Xbox).sln` | The primary game's two solutions. |
| `TransformedCollision/TransformedCollision/TransformedCollisionWindows.csproj` | Windows Reach game project, `DEBUG;TRACE;WINDOWS`. |
| `TransformedCollision/TransformedCollision/TransformedCollisionXbox.csproj` | Xbox 360 project, `DEBUG;TRACE;XBOX;XBOX360`. |
| `TransformedCollision/TransformedCollision/Game1.cs` | The game: `TransformedCollisionGame`, both `IntersectPixels` overloads and `CalculateBoundingRectangle`. |
| `TransformedCollision/TransformedCollision/Block.cs` | `Block`: a `Vector2 Position` and a `float Rotation`. |
| `TransformedCollision/TransformedCollision/Program.cs` | `Main`. |
| `TransformedCollision/TransformedCollision/Properties/AssemblyInfo.cs` | Assembly metadata only. |
| `TransformedCollision/TransformedCollisionContent/TransformedCollisionContent.contentproj` | Three items, all `TextureImporter`/`TextureProcessor` at their defaults. |
| `TransformedCollisionContent/Block.bmp`, `Person.bmp`, `SpinnerBlock.bmp` | The source art. |
| `TransformedCollisionTest/…` (12 files) | The second product — see its own record. |
| `TransformedCollision.htm` | The tutorial page (Collision series 3). Kept verbatim in this directory. |
| `Documentation/*.png`, `Game.ico`, `GameThumbnail.png` | Documentation and shell artwork; not runtime content. |
| `Microsoft Permissive License.rtf` | Licence. |

`Game1.cs`, `Block.cs` and `Program.cs` contain no `#if` of any kind, so the Windows
and Xbox projects compile the same code and the Windows configuration is the
reference. The content project uses the stock texture importer and processor only —
there is no tool, library or custom processor to audit.

`Block.bmp` is declared by the content project but never loaded by the game. It is
kept as `Content/Block.xnb` because it is part of the sample's content set; removing
it would be an edit to the original, not a cleanup.

The upstream snapshot used for this audit is retained at `xna4-original/` with
per-file SHA-256 in `evidence/xna4-original-sha256.txt`.

## 2. Relationship to SAMPLE-018 and SAMPLE-019, established rather than assumed

This is tutorial **3** of the Collision series. Tutorial 1 (SAMPLE-019) tests
axis-aligned rectangles, tutorial 2 (SAMPLE-018) tests the pixels inside them, and
this one tests the pixels of sprites that are **rotated**, so neither the drawn shape
nor the tested shape is axis-aligned any more.

The three differ in content as well as code: this sample's falling sprite is
`SpinnerBlock.bmp`, a **128x128** spinner whose texture is opaque in only 3940 of its
16384 texels, where tutorials 1 and 2 drop a solid 32x32 `Block.bmp`. `Person.bmp` is
byte-identical across all three (`5cae4d554d64c4a6…46db0bb4`, the same file SAMPLE-018
and SAMPLE-019 use), and this sample's own content project was still built through
the official pipeline rather than copying either predecessor's output. Its
`Person.xnb` is `7e9cff8ab0f5a5bc…0d4c6a72`; all six hashes are in
`evidence/content-sha256.txt`.

That size difference is the whole point of the tutorial: the axis-aligned bounding
rectangle of a rotated 128x128 square reaches 181x181, so the rectangle test says
"maybe" far more often than the rotated pixel test says "yes".

## 3. Original XNA 4.0 build and run

The unchanged original was built and run on this Linux host.

- `scripts/build-original.sh` runs the official `BuildContent` MSBuild task through
  `XnaPipelineRunner.exe` under the XNA 4.0 Wine prefix
  `/home/robertvokac/.wine-cna-xna40`, on **this** sample's own two content projects,
  and compiles both games with the in-prefix `csc.exe`.
- Output: `xna4-build/bin/TransformedCollision.exe` with `Content/{Block,Person,SpinnerBlock}.xnb`,
  and `xna4-build/test-bin/TransformedCollisionTest.exe` with `Content/{F,Point,R}.xnb`.
- `scripts/capture-original.sh` and `scripts/capture-original-collision.sh` run it
  under an isolated Xvfb display with `WINEDLLOVERRIDES=d3d9=b`.

### The reference build's own safe-area arithmetic

`Initialize` computes

```csharp
safeBounds = new Rectangle(
    (int)(viewport.Width * SafeAreaPortion),
    (int)(viewport.Height * SafeAreaPortion),
    (int)(viewport.Width * (1 - 2 * SafeAreaPortion)),
    (int)(viewport.Height * (1 - 2 * SafeAreaPortion)));
```

Evaluated entirely in `float`, `800 * 0.9f` rounds **up** to exactly `720.0f` and
truncates to 720. The 32-bit reference build keeps the product in an extended-precision
register instead, where it is `719.99998…` and truncates to **719**. The measured
original settles the question: the person starts at x=343 and clamps at x=727, which
requires `safeBounds.Width == 719` and `Right == 759`, not 720/760. Height behaves the
same way: 431, not 432, giving the person's y=399.

The port reproduces the reference build's arithmetic with widening casts, and the
deviation is commented in the source. Measured person positions:

| Build | Start | Left clamp | Right clamp |
|---|---|---|---|
| XNA 4.0 original | (343, 399) | (40, 399) | (727, 399) |
| CNA native OPENGLES3 | (343, 399) | (40, 399) | (727, 399) |
| CNA WEBGL2 in Chrome | (343, 399) | (40, 399) | (727, 399) |

## 4. Content provenance

All six XNBs in the two ports are **byte-identical** to this sample's own official
pipeline output:

```
Block.xnb Person.xnb SpinnerBlock.xnb   vs xna4-build/Content/
F.xnb Point.xnb R.xnb                   vs xna4-build/TestContent/
```

verified with `cmp` after the pipeline run; hashes in `evidence/content-sha256.txt`.
No loose PNG/BMP substitute remains in either port, and neither game calls `SetData`.

## 5. Translation

`samples/TransformedCollision/src/` mirrors the original file decomposition:
`Block.hpp` ← `Block.cs`, `TransformedCollisionGame.hpp` ← `Game1.cs`,
`Program.cpp` ← `Program.cs`. Namespace `TransformedCollision`; `GetTypeName()`
returns `"TransformedCollision.TransformedCollisionGame"`.

Both `IntersectPixels` overloads are present — the `Rectangle` one the original
inherits unchanged from tutorial 2, and the `Matrix` one this tutorial adds — plus
`CalculateBoundingRectangle`. Only the `Matrix` overload is called by `Update`, in the
original and here alike.

### Deviations, all mechanical and all commented in the source

| Original | Port | Why |
|---|---|---|
| `Block` is a C# class; `blocks[i].Position += …` mutates in place | `List<Block>` holds elements by value: read with `getItem`, mutate, write back with `setItem` | C++ value semantics. Nothing in the loop aliases the element, so the two are equivalent. |
| `Vector2.Transform(ref v, ref m, out v)` | `Vector2::Transform(v, m, v)` | CNA's out-ref overload computes into locals before assigning, so the aliased call in `CalculateBoundingRectangle` is safe. Verified in `modules/math/src/Vector2.cpp`. |
| `(int)(viewport.Width * (1 - 2 * SafeAreaPortion))` | widening cast before the multiply | Reproduces the reference build's extended precision; section 3. |

## 6. Framework work this sample required

One change, in `sharp-runtimenext`.

**`List<T>` must be instantiable for a `T` that declares no equality.** `Block` has no
`operator==`, and `List<T>::Contains/IndexOf/Remove` are `override`s, so they are
instantiated with the class and the template `System::detail::findValue` failed to
compile — `List<Block>` could not exist at all. C#'s `List<T>` has no such
restriction: the equality it needs comes from `EqualityComparer<T>.Default`, which
exists for every `T`.

`findValue` now refuses at the call rather than at instantiation, throwing
`NotSupportedException` for an element type with no equality and keeping the ordinary
`std::find` path for every type that has one. This game never calls those members —
it removes blocks by index — so the port exercises only the part that now compiles.

Tests: `modules/collections/tests/System/Collections/Generic/ListNonComparableElementTests.cpp`,
seven cases covering storage, index-based mutation, enumeration, the three refusals
and the unchanged behaviour when equality does exist. Full sharp-runtime suite after
the change: **17847/17847 passed**.

`cnanext` needed `Vector2::operator+=` / `operator-=` (`CNAEXT`, matching the existing
`Vector3` pair) because C# synthesises compound assignment from `operator+` and C++
does not. Three tests added in `modules/math/tests/…/Vector2Tests.cpp`.

No CNA behavioural fix was needed: `Matrix::Invert`, `Vector2::Transform`,
`Vector2::TransformNormal`, `Rectangle::Intersects` and `SpriteBatch`'s rotated draw
path all already match, which the measurements in section 7 confirm.

## 7. What was measured

### 7.1 Recorded play, original against native port

Both were driven for 180 s by the same script — the game's own Left/Right keys, no
state written — and every one of the 1800 recorded frames was classified by
`scripts/analyze-frames.py`. The person is located by an exact subset match of its own
two opaque colours; block ink inside the person's 32x32 rectangle that is not the
person's own is what makes a rectangle overlap visible.

| | XNA 4.0 original | CNA native OPENGLES3 |
|---|---|---|
| frames | 1800 | 1800 |
| frames the game cleared to red (a pixel hit) | 166 | 158 |
| … person located | 86 | 72 |
| … person occluded by the block that hit it | 80 | 86 |
| **rectangle overlap, no pixel hit, unambiguous** | **52** | **48** |
| rectangle overlap within the resampling blur | 9 | 4 |
| no block ink in the person's rectangle | 1573 | 1590 |
| red frames with no block ink in the rectangle | 0 | 0 |
| person Y | always 399 | always 399 |
| person X range | 297–727 | 302–727 |

The two runs cannot match frame for frame — each game seeds its own `Random` and
spawns its own blocks — so the agreement is in rate and in the invariants: an
occluded person always means a red frame, a red frame always has block ink in the
rectangle, and roughly one frame in 35 shows the spinner's arm well inside the
person's rectangle with the background still blue. That last row is the tutorial's
subject, and it is the exact opposite of SAMPLE-019, where every unambiguous
rectangle overlap turns the background red.

Reports: `evidence/analysis/xna-original-frames.json`,
`evidence/analysis/cna-native-frames.json` (per-frame, not just totals).

### 7.2 Deterministic comparison, through the second product

The second product is mouse-driven, so it can be given **identical** input in all
three builds. `scripts/capture-cna-native-test.sh`, `scripts/capture-original-test.sh`
and `scripts/chrome-smoke-test.mjs` hold the left button and walk the F sprite up onto
R through the same 17 positions. The frame turns red at exactly the same step in all
three:

| Build | Red at drag steps |
|---|---|
| XNA 4.0 original | 64, 72, 80, 88, 96, 104, 112, 120, 128 |
| CNA native OPENGLES3 | 64, 72, 80, 88, 96, 104, 112, 120, 128 |
| CNA WEBGL2 in Chrome | 64, 72, 80, 88, 96, 104, 112, 120, 128 |

and the drawn geometry matches too — black-ink bounding box `(42,28)-(222,96)` with
2662 pixels at the start and `(164,28)-(222,96)` with 1949 at the end, identical in
all three. This is a per-pixel, per-transform agreement on the rotated collision code,
not a statistical resemblance. Details in the second product's record.

### 7.3 Native run

`scripts/smoke-cna-native.sh <product>` runs each product for 6 s on an isolated Xvfb
display: both stay alive, both log `CNA: graphics renderer: OPENGLES3`, neither logs a
fatal error. Both exit cleanly on Escape in the capture runs.

### 7.4 WEBGL2 in real system Chrome

`scripts/capture-web.sh <product>` serves the complete bundle over local HTTP and
drives Google Chrome 151.0.7922.71 on its own Xvfb display through the DevTools
protocol. Both products: `moduleReady`, `webgl2: true`, the renderer banner
`CNA: graphics renderer: WEBGL2`, no unhandled rejection, no runtime exception, no
HTTP error, no fatal console message, and all four assets served `200`.

- `TransformedCollision`: person at (343,399)/(40,399)/(727,399) as above, and guided
  play with the game's own arrow keys reaches a real per-pixel hit
  (`collisionSeen: true`). Escape collapses the canvas, which is this build's exit.
- `TransformedCollisionTest`: the 17-position drag above, `collisionSeen: true`.

Results: `evidence/cna-web-webgl2/<product>/browser-result.json`.

#### One gate defect found and fixed here

The first run of the second product's gate reported **no** collision while its own
final screenshot was plainly red. The cause was in the harness, not the port:
`Page.captureScreenshot` can answer from a partly composited frame — measured with as
few as 2212 of 384000 pixels carrying a clear colour — and the single probe pixel the
gate inherited from SAMPLE-019 landed in the stale region. Both gates now classify a
frame by counting the pixels that are exactly one of the game's two clear colours and
taking the larger; the two never appear together in one frame. `analyze-frames.py` and
the two capture scripts had the same single-pixel flaw and were corrected the same
way — a 128x128 spinner reaches the window corner often enough to matter.

## 8. Scans

`NOXNA`, CNAEXT graphics helpers, renderer/backend includes, `SetData`, loose
non-XNB content, invented controls and the historical F1 help overlay: no hits in
either port. `help.png` is retained at the sample root beside `CMakeLists.txt`, not in
`Content`, and is never loaded.

## 9. Regression

- sharp-runtime full suite: 17847/17847 passed.
- `CnaTests` full suite: 14 failures, the same 14 present on unmodified `next` HEAD
  (`VertexDeclarationLayoutTest` x6, `DeclarationGuardTest` x4, `GltfLimitationsDoc`,
  `GltfRendererPbrFallbackPolicy`, `GpuTimerTest`, `ShadowVisibilityTest`). No new
  failure. Log: `evidence/cnatests-full.log`.

## 10. Known differences

None active. Nothing was omitted, simplified or substituted.
