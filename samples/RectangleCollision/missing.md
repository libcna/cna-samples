# SAMPLE-019 — RectangleCollisionSample_4_0 audit record

## Fresh sequential requalification — 2026-09-19

**Result: complete; no sample, CNA or sharp-runtime code change was needed.**
The historical 2026-08-25 investigation below records the original repair and
its then-current results. The fresh run uses the current `libcna/cna-samples`,
`libcna/cna` and `libcna/sharp-runtime` checkouts; its results supersede the
older build paths, recordings, browser timing and window-title conclusion.

- Recounted the entire 19-file physical upstream directory and verified its
  `xna4-original/` snapshot with `diff -qr`: no differences. The Windows and
  Xbox projects compile the same three C# files, with no conditional game
  branch or second product. The unchanged Windows Debug/x86/Reach game and its
  own stock `TextureImporter`/`TextureProcessor` content project rebuilt in
  `work-xna4-20260919/`. Fresh `Block.xnb` and `Person.xnb` exactly match the
  two checked-in official XNB hashes in section 4.
- The original ran under Wine/Xvfb with WineD3D at 800×480 and title
  `Rectangle Collision`. Start, both safe-area clamps, falling blocks, a
  rectangle-only hit and clean Escape exit are saved in
  `evidence/requal-20260919/xna-original/`. Its 180-second, 1,800-frame
  recording contains 34 red rectangle hits, zero unambiguous rectangle
  overlaps left blue, and 15 red frames where the official sprite masks have
  no estimated opaque-pixel contact.
- A fresh sample-only Release OPENGLES3 build used the current CNA and
  sharp-runtime with at most four compile jobs. Native smoke, keyboard clamps,
  a 180-second recording and Escape passed. The 1,800 frames contain 54 red
  hits, zero unambiguous overlaps left blue, and 26 red frames without
  estimated opaque-pixel contact. The direct screenshot
  `evidence/requal-20260919/native/rc-cna-rectangle-only-hit.png` shows the
  block and player visibly separated while the background is red. The
  canonical stripped executable passed a separate smoke after promotion.
- Original, native and browser captures locate the player at `(343,399)` on
  start, `(40,399)` at the left clamp and `(727,399)` at the right clamp.
  All 240 opaque player-sprite pixels match the XNA original at each state in
  both CNA builds. The browser's left-clamp background happened to be red from
  a random block, so transparent/background pixels were correctly excluded
  from that comparison. Details are in
  `evidence/requal-20260919/player-parity.txt`.
- The fresh non-threaded Release WEBGL2 four-file bundle was served over HTTP
  and played in isolated system Google Chrome. The 800×480 canvas, real WebGL2
  context, renderer banner, Left/Right motion and a real red collision passed,
  with no rejected promise, runtime exception, failed HTTP asset or fatal
  console message. The exact byte-identical gallery copy passed the same
  Chrome gate. Results/screenshots are in `evidence/requal-20260919/web/`
  and `gallery-web/`. The `.data` is exactly 8,566 bytes: this sample's two
  official XNBs concatenated, with no loose substitute.
- The current CNA `Rectangle::Intersects` still has FNA's four strict edge
  comparisons. The native window now has the original `Rectangle Collision`
  title: the port's `src/Properties/AssemblyInfo.cpp` registers the general
  assembly-title metadata used by CNA. The old title difference in section 9
  is resolved outside game logic. A scan found no renderer helper, `NOXNA`,
  handwritten loader, sidecar, F1 overlay or other sample-side workaround.

The fresh XNA/native/web canonical products were refreshed from these passing
builds. The owner subsequently requested push and pruning: audit commit
`705c95e` and gallery commit `c544390` reached origin. Both CMake work trees
were removed with the guarded prune script. Its policy leaves non-CMake work
trees alone, so the duplicate original XNA work tree was checked against the
retained executable/content and removed separately. Exact source, scripts,
evidence, canonical products and product hashes remain; the root is now
18.5 MB. Rebuild commands and the removal record are in `MANIFEST.md`.

Audit date: 2026-08-25. Upstream directory:
`/rv/tmp/XNAGameStudio/Samples/RectangleCollisionSample_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-019-RectangleCollisionSample_4_0`.

## 1. What upstream actually contains

19 files, no hidden second product:

| Path | Role |
|---|---|
| `RectangleCollision (Windows).sln`, `RectangleCollision (Xbox).sln` | The two solutions. |
| `RectangleCollision/RectangleCollision/RectangleCollisionWindows.csproj` | Windows Reach game project, `DEBUG;TRACE;WINDOWS`. |
| `RectangleCollision/RectangleCollision/RectangleCollisionXbox.csproj` | Xbox 360 project; same three source files, only platform and `DefineConstants` differ. |
| `RectangleCollision/RectangleCollision/Game1.cs` | The whole game: `RectangleCollisionGame`. |
| `RectangleCollision/RectangleCollision/Program.cs` | `Main`, `using (game) game.Run()`. |
| `RectangleCollision/RectangleCollision/Properties/AssemblyInfo.cs` | Assembly metadata only. |
| `RectangleCollision/RectangleCollisionContent/RectangleCollisionContent.contentproj` | Two items, both `TextureImporter`/`TextureProcessor` at their defaults. |
| `RectangleCollisionContent/Block.bmp`, `Person.bmp` | 32x32 24-bit BMPs with a magenta key colour. |
| `RectangleCollision.htm` | The tutorial page (Collision series 1). Kept verbatim in this directory. |
| `Documentation/*.png`, `Game.ico`, `GameThumbnail.png` | Documentation and shell artwork; not runtime content. |
| `Microsoft Permissive License.rtf` | Licence. |

`Game1.cs` contains no `#if` of any kind, so the Windows and Xbox projects compile
the same code and the Windows configuration is the reference. The content project
uses the stock texture importer and processor only — there is no tool, library or
custom processor to audit.

The upstream snapshot used for this audit is retained at `xna4-original/` with
per-file SHA-256 in `evidence/xna4-original-sha256.txt`.

## 2. Relationship to SAMPLE-018, established rather than assumed

This is tutorial **1** of the Collision series; SAMPLE-018 is tutorial 2, written by
adding per-pixel testing to this code. Diffing the two originals shows tutorial 2
adds exactly the two `Color[]` fields, the two `GetData` calls and `IntersectPixels`,
and replaces

```csharp
if (personRectangle.Intersects(blockRectangle))
    personHit = true;
```

with the per-pixel call. Everything else — fields, constants, safe-area arithmetic,
input, spawn and removal, draw order — is identical.

The two source BMPs are byte-identical across the two samples
(`Block.bmp b46095bd…c473`, `Person.bmp 5cae4d55…0bb4`) and both content projects
declare them with the same stock importer/processor. That was **not** taken as
licence to copy SAMPLE-018's XNBs: this sample's own content project was built
through the official pipeline (section 3), and the output turned out byte-identical,
which is now a measured fact rather than an inference.

## 3. Original XNA 4.0 build and run

The unchanged original was built and run on this Linux host.

- `scripts/build-original.sh` runs the official `BuildContent` MSBuild task through
  `XnaPipelineRunner.exe` under the XNA 4.0 Wine prefix — using **this** sample's
  `ProjectGuid` `{D3B9991E-01CA-4481-8B68-CC862B5CD6C1}` and its own
  `RectangleCollisionContent` directory — then compiles the three unmodified `.cs`
  files with the in-prefix `csc.exe` under the Windows project's own
  `/define:DEBUG;TRACE;WINDOWS` and `/platform:x86`.
- Prefix `/home/robertvokac/.wine-cna-xna40`, run with `WINEDLLOVERRIDES=d3d9=b`
  (WineD3D). Output: `xna4-build/bin/RectangleCollision.exe`.
- `scripts/capture-original.sh` runs it on an isolated Xvfb display and captures the
  start frame, both safe-area clamps and the clean Escape exit.
- `scripts/capture-original-collision.sh` records the window for 180 s while sweeping
  the person with the game's own arrow keys.

Window title `Rectangle Collision`, client area 800x480 (the XNA default back
buffer; the game sets no preferred size).

## 4. Content provenance

`Block.xnb` and `Person.xnb` in `Content/` are the official XNA 4.0 Content Pipeline
output for this sample's own content project:

| File | SHA-256 |
|---|---|
| `Content/Block.xnb` | `b509da3d04de79e10f074a2481f6c1858d1f6814c36e750c505db6b57def667f` |
| `Content/Person.xnb` | `7e9cff8ab0f5a5bc0e06282bed455d7783d7425589fea564400243c70d4c6a72` |

The same hashes appear in the pipeline output `xna4-build/Content/` and in the native
build's deployed `Content/`. Both are `Texture2DReader`, `SurfaceFormat` `Color`,
32x32, one mip level. `Content.Load<Texture2D>("Block")` and `("Person")` are
unchanged.

### Removed substitutes

- `Content/Block.png` and `Content/Person.png` — direct BMP-to-PNG conversions with
  **no colour key applied at all**: 480 of `Block`'s texels and 784 of `Person`'s
  were fully opaque magenta `(255,0,255,255)`. The sprites therefore rendered as
  solid magenta squares with the shape inside, instead of a transparent-background
  shape over the play field. The previous `missing.md` recorded this as a known
  defect; it is now gone, replaced by the pipeline output where the key colour is
  `(0,0,0,0)`.
- `Content/help.png` and the F1 overlay — invented, no XNA equivalent. The historical
  `help.png` is preserved at the sample root beside `CMakeLists.txt` and is neither
  packaged nor loaded.

## 5. Translation

`src/RectangleCollisionGame.hpp` + `src/Program.cpp` are a line-by-line translation
of `Game1.cs` + `Program.cs`, keeping the `RectangleCollision` namespace, the
`RectangleCollisionGame` type, the field order and names, the constants
(`PersonMoveSpeed`, `BlockFallSpeed`, `SafeAreaPortion`, `BlockSpawnProbability`),
the update/draw order, the input mapping and the original comments.
`GetTypeName()` reports `RectangleCollision.RectangleCollisionGame`.

Only lossless C#-to-C++ mechanics were used:

- `List<Vector2> blockPositions` → `System::Collections::Generic::List<Vector2>`;
  reads through `getItem(i)` because the non-const indexer returns a mutation proxy,
  writes through `setItem(i, …)`.
- `SpriteBatch` is a `std::unique_ptr` created in `LoadContent`, as in the original.
- `graphics.GraphicsDevice` → `graphics.getGraphicsDeviceProperty()`.
- `Color.Red` / `Color.CornflowerBlue` / `Color.White` replace the previous RGBA
  literals.

`personRectangle.Intersects(blockRectangle)` maps straight onto CNA's
`Rectangle::Intersects`, whose implementation is character-for-character FNA's
(`value.Left < Right && Left < value.Right && value.Top < Bottom && Top < value.Bottom`),
so no framework work was needed for the sample's defining call.

### The one deliberate arithmetic deviation

As in SAMPLE-018, `Initialize()` multiplies the viewport size by `SafeAreaPortion` at
**double** precision before truncating, because the 32-bit reference build evaluates
that expression at extended precision: `(int)(800 * (1 - 2 * 0.05f))` is 719 there and
720 in a plain float translation. This was re-derived for this sample rather than
carried over — the captured original puts the person at `(343, 399)` and clamps it to
`40 … 727`, which is the 719x431 safe area, and the port now reproduces all three
(section 7).

## 6. Framework fixes

**None were needed.** The two general CNA fixes SAMPLE-018 landed — the XNA
value-type default for `Color`, and sub-pixel sprite destinations in `SpriteBatch` —
were already on `next` before this sample was ported, and this sample exercises the
second one the same way (its blocks are drawn at
`(float)random.NextDouble() * (ClientBounds.Width - 32)`). No `cnanext` or
`sharp-runtimenext` change accompanies this task.

## 7. Verification

### Person position — original vs native vs browser

The person is drawn at an integral position, so it can be located exactly by matching
the official texture's opaque-black and opaque-white masks against each capture:

| State | XNA original | CNA native OPENGLES3 | Chrome WEBGL2 |
|---|---|---|---|
| Start (no input) | `(343, 399)` | `(343, 399)` | `(343, 399)` |
| Left held to the clamp | `(40, 399)` | `(40, 399)` | `(40, 399)` |
| Right held to the clamp | `(727, 399)` | `(727, 399)` | `(727, 399)` |

The 32x32 person sprite region is byte-identical between the XNA original and the
native build at all three positions.

### Rectangle collision, not per-pixel collision

`scripts/analyze-frames.py` locates the person and every block in each recorded frame
and classifies it. For this tutorial the expected result is the **opposite** of
SAMPLE-018's: every rectangle overlap must turn the background red, including the
ones where the drawn pixels are nowhere near each other.

| Recording | Frames | Red | Rectangle overlap, no hit | of those, unambiguous | Hit without rectangle overlap |
|---|---:|---:|---:|---:|---:|
| XNA original, 180 s | 1800 | 68 | 0 | 0 | 0 |
| CNA native, 180 s | 1800 | 72 | 1 | **0** | 0 |

The single native case is a **one-pixel** overlap, which is below what this
measurement can resolve: a sprite's position is a float, only its rendered pixels are
visible to the analyser, and every position inside a one-pixel band rasterises
identically — so the game's own `(int)` of the true position can be one less than the
pixels imply. `frame-00058.png` was checked by hand: the block occupies exactly
columns 566–597 with no partial edge, the person starts at 597, so the true position
lies in `[565.5, 566)` and the game's `(int)` was 565, giving `Block.Right == 597`,
which `Intersects` correctly reports as no overlap. The analyser now reports
`unambiguousOverlapWithoutPixelHit` separately for exactly this reason, and it is 0 in
both recordings.

For the same 180 s in SAMPLE-018, per-pixel collision produced 16 (original) and 25
(native) genuine rectangle-overlap-without-hit frames. That contrast is the whole
point of the two tutorials, and both ports reproduce their own side of it.

`evidence/xna-original/rc-xna-rectangle-only-hit.png` and
`evidence/cna-native-opengles3/rc-cna-rectangle-only-hit.png` are the tutorial's own
opening illustration, captured from each build: the block is visibly clear of the
person and the background is red anyway.

### Native OPENGLES3

`scripts/smoke-cna-native.sh` (renderer banner `CNA: graphics renderer: OPENGLES3`,
alive after 6 s, no fatal log line) and `scripts/capture-cna-native.sh` (start, both
clamps, a 180 s recording, `exited_on_escape=1`).

### Browser WEBGL2

`scripts/capture-web.sh` serves the complete bundle (`.html`, `.js`, `.wasm`,
`.data`) over local HTTP and drives the system `/usr/bin/google-chrome` on its own X
display with ANGLE/SwiftShader, using the harness SAMPLE-018 developed: every CDP
request carries a deadline, every DOM key event the page receives is logged so
outside interference is visible, and the collision is reached by playing the game
with its own arrow keys rather than waiting for a chance encounter.

Verified in `evidence/cna-web-webgl2/browser-result.json`: `moduleReady`, a real
`webgl2` context, canvas 800x480, the `CNA: graphics renderer: WEBGL2` banner, all
four assets served `200`, no rejected promise, no runtime exception, no HTTP error,
no fatal console message, and all 264 DOM key events accounted for by the harness.
The person reaches the same `(343, 399)`, `(40, 399)` and `(727, 399)` as XNA and the
native build, and the guided player reached a real hit.
`evidence/cna-web-webgl2/rc-webgl2-collision.png` shows it, and shows the tutorial's
point again: the block is beside the person rather than on it, and the background is
red anyway.

## 8. Mechanical no-workaround scan

`grep -rnE "RawMesh|RawModel|NOXNA|SetData|\.model\.json|\.font\.json|help\.png|helpTimer|Keys::F1|CNAEXT" src/ CMakeLists.txt`
returns no hits. `Content/` holds only the two official XNBs. No loose image,
sidecar, hand-written shader, renderer helper, invented control, diagnostic overlay
or omitted branch remains.

## 9. Known differences at the time of the 2026-08-25 audit

None in the sample's own translation, content, rendering, input or behaviour.

The window caption once differed: XNA/FNA take the initial `Window.Title` from
the entry assembly's `AssemblyTitle`, while CNA then used the fixed title
`"Game"`. This **is no longer an active difference**. Current CNA supports
assembly-title metadata generally; the port registers the original
`Rectangle Collision` title in `src/Properties/AssemblyInfo.cpp`. The fresh
2026-09-19 native capture found that exact title without a game-logic call to
`setTitleProperty`.

The Xbox 360 project is not built. It compiles the same three source files with
`XBOX;XBOX360` instead of `WINDOWS`, and `Game1.cs` has no conditional compilation,
so there is no Xbox-only behaviour to translate.

## 10. Reproduction

Use the current `MANIFEST.md` in the artifact root for the exact `libcna`
checkouts, named work trees, shared cache and four-job XNA/native/WEBGL2
rebuild commands. The retained capture scripts accept build/evidence directory
overrides so a fresh run need not overwrite canonical products or historical
evidence. `scripts/analyze-frames.py` and
`scripts/analyze-rectangle-only-hits.py` reproduce the two frame reports using
the retained official sprite masks under `evidence/content/`.

To play the original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-019-RectangleCollisionSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine RectangleCollision.exe
```

Controls (identical in all three builds): Left/Right arrow or gamepad D-pad move the
person; Escape or gamepad Back exits. There are no other controls.
