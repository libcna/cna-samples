# SAMPLE-018 — PerPixelCollisionSample_4_0 audit record

Audit date: 2026-08-25. Upstream directory:
`/rv/tmp/XNAGameStudio/Samples/PerPixelCollisionSample_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-018-PerPixelCollisionSample_4_0`.

## 1. What upstream actually contains

The physical directory holds 16 files and no hidden second product:

| Path | Role |
|---|---|
| `PerPixelCollision (Windows).sln`, `PerPixelCollision (Xbox).sln` | The two solutions. |
| `PerPixelCollision/PerPixelCollision/PerPixelCollisionWindows.csproj` | Windows Reach game project, `DEBUG;TRACE;WINDOWS`. |
| `PerPixelCollision/PerPixelCollision/PerPixelCollisionXbox.csproj` | Xbox 360 project; identical source list, only platform/`DefineConstants` differ. |
| `PerPixelCollision/PerPixelCollision/Game1.cs` | The whole game: `PerPixelCollisionGame`. |
| `PerPixelCollision/PerPixelCollision/Program.cs` | `Main`, `using (game) game.Run()`. |
| `PerPixelCollision/PerPixelCollision/Properties/AssemblyInfo.cs` | Assembly metadata only. |
| `PerPixelCollision/PerPixelCollisionContent/PerPixelCollisionContent.contentproj` | Two items, both `TextureImporter`/`TextureProcessor` at their defaults. |
| `PerPixelCollisionContent/Block.bmp`, `Person.bmp` | 32x32 24-bit BMPs with a magenta key colour. |
| `PerPixelCollision.htm` | The tutorial page (Collision series 2). Kept verbatim in this directory. |
| `Documentation/blueblock.png`, `Documentation/redblock.png`, `Game.ico`, `GameThumbnail.png` | Documentation and shell artwork; not runtime content. |
| `Microsoft Permissive License.rtf` | Licence. |

`Game1.cs` has no `#if` of any kind, so the Windows and Xbox projects compile the
same code and the Windows configuration is the reference. There is no separate
tool, library, importer or processor to audit: the content project uses the stock
texture importer and processor only.

The upstream snapshot used for this audit is retained at `xna4-original/` with
per-file SHA-256 in `evidence/xna4-original-sha256.txt`.

## 2. Original XNA 4.0 build and run

The unchanged original was built and run on this Linux host — no VM was needed.

- `scripts/build-original.sh` runs the official `BuildContent` MSBuild task through
  a small `XnaPipelineRunner.exe` under the XNA 4.0 Wine prefix, then compiles the
  three unmodified `.cs` files with the in-prefix `csc.exe` using the Windows
  project's own `/define:DEBUG;TRACE;WINDOWS`, `/platform:x86` and
  `Microsoft.Xna.Framework.RuntimeProfile` resource.
- Prefix `/home/robertvokac/.wine-cna-xna40`, run with `WINEDLLOVERRIDES=d3d9=b`
  (WineD3D). Output: `xna4-build/bin/PerPixelCollision.exe`, content in
  `xna4-build/Content/`.
- `scripts/capture-original.sh` runs it on an isolated Xvfb display and captures
  the start frame, the left clamp, the right clamp and the clean Escape exit.
- `scripts/capture-original-collision.sh` records the window with `ffmpeg` for 180 s
  while sweeping the person with the game's own arrow keys, then extracts a frame
  in which the game itself reported a hit.

Window title `Per Pixel Collision`, client area 800x480 (the XNA default back
buffer; the game sets no preferred size).

## 3. Content provenance

`Block.xnb` and `Person.xnb` in `Content/` are the **official XNA 4.0 Content
Pipeline output** for the unchanged `Block.bmp`/`Person.bmp`, produced by the
`BuildContent` run above:

| File | SHA-256 |
|---|---|
| `Content/Block.xnb` | `b509da3d04de79e10f074a2481f6c1858d1f6814c36e750c505db6b57def667f` |
| `Content/Person.xnb` | `7e9cff8ab0f5a5bc0e06282bed455d7783d7425589fea564400243c70d4c6a72` |

The same two hashes appear in the pipeline output `xna4-build/Content/` and in the
native build's deployed `Content/`, so the checked-in files are byte-identical to
what the original pipeline produced. Both are `Texture2DReader`, `SurfaceFormat`
`Color`, 32x32, one mip level, 4096 bytes of pixels. The processor's default colour
key turned the magenta background into `(0,0,0,0)` — 480 transparent texels in
`Block`, 784 in `Person` — which is exactly the data the sample's per-pixel test
depends on.

`Content.Load<Texture2D>("Block")` and `("Person")` are unchanged.

### Removed substitutes

- `Content/Block.png` and `Content/Person.png` — ImageMagick colour-key conversions
  of the BMPs. Deleted. They were not equivalent: they kept `alpha = 0` but left
  `RGB = magenta`, whereas the official pipeline premultiplies and writes
  `(0,0,0,0)`. 480 of 1024 texels differed in `Block` and 784 of 1024 in `Person`.
- `Content/help.png` and the whole F1 overlay (texture, timer, key edge detection
  and the overlay draw) — invented, no XNA equivalent. The historical `help.png` is
  preserved at the sample root beside `CMakeLists.txt` and is neither packaged nor
  loaded.

## 4. Translation

`src/PerPixelCollisionGame.hpp` + `src/Program.cpp` are a line-by-line translation
of `Game1.cs` + `Program.cs`, keeping the `PerPixelCollision` namespace, the
`PerPixelCollisionGame` type, the field order and names, the constants
(`PersonMoveSpeed`, `BlockFallSpeed`, `SafeAreaPortion`, `BlockSpawnProbability`),
the update/draw order, the input mapping and the original comments.
`GetTypeName()` reports `PerPixelCollision.PerPixelCollisionGame`.

Only lossless C#-to-C++ mechanics were used:

- `List<Vector2> blockPositions` → `System::Collections::Generic::List<Vector2>`;
  reads go through `getItem(i)` because the non-const indexer returns a mutation
  proxy, writes through `setItem(i, …)`.
- `Color[]` → `std::vector<Color>`; `Math.Max/Min` → `System::Math::Max/Min`.
- `SpriteBatch` is a `std::unique_ptr` created in `LoadContent`, as in the original.
- `graphics.GraphicsDevice` → `graphics.getGraphicsDeviceProperty()`.
- `Color.Red` / `Color.CornflowerBlue` / `Color.White` replace the previous RGBA
  literals.

### The one deliberate arithmetic deviation

`Initialize()` multiplies the viewport size by `SafeAreaPortion` at **double**
precision before truncating. C# permits a floating-point expression to be evaluated
with more precision than its type, and the 32-bit reference build does exactly
that. Measured directly in the reference prefix
(`xna4-build/fp-probe/FpProbe.cs`, run with the in-prefix `csc.exe`):

```
portion bits = 66-66-66-3F        (0.89999997615814208984375f)
(int)(800 * portion)   = 719      (the expression, extended precision)
(int)(480 * portion)   = 431
(int) via float local  = 720 / 432
```

So the original's safe area is `(40, 24, 719, 431)`, its person rest position is
`(343, 399)` and its reachable X range is `40 … 727`. A plain float translation
produces 720/432 and would put the person one pixel lower and let it reach 728. The
widening cast reproduces the reference build's own arithmetic; the observed
positions now match the original exactly (section 6).

## 5. Framework fixes made in this session

Both are general XNA/FNA reimplementation fixes in `../cnanext`, with no
sample-name special case.

### 5.1 `Color` had no default constructor

`Color` is a C# value type, so `new Color[n]` fills the array with `default(Color)`
= transparent black, and `LoadContent`'s `new Color[w*h]` depends on it. CNA's
`Color` declared no default constructor at all, so `std::vector<Color>(n)` did not
compile and the previous port worked around it with an explicit
`resize(n, Color(0,0,0,0))` fill value. Added `Color()` initialising the packed
value to 0, matching what `Vector2`, `Point` and `Rectangle` already do for the
other XNA value types. Tests: `ColorTest.DefaultConstructorIsTransparentBlack`,
`ColorTest.DefaultConstructedArrayElementsAreTransparentBlack`.

### 5.2 SpriteBatch quantised every sprite destination to whole pixels

XNA 4.0 and FNA carry a sprite's destination through the batch as floats — FNA's
`SpriteBatch.PushSprite` takes `float destinationX/Y/W/H` — so a sprite drawn at a
fractional position lands between pixels and the default `LinearClamp` sampler
filters its edges. CNA truncated the destination inside `SpriteBatch` because the
internal `ISpriteBatchRenderer` seam accepted only an integer `Rectangle`.

This sample makes it visible: every falling block is drawn at
`(float)random.NextDouble() * (ClientBounds.Width - 32)`, i.e. essentially always a
fractional X. Counting the pixels in each recording's first 60 frames that are
neither block black, background blue/red, nor person white — that is, the filtered
sprite edges:

| Build | Blended pixels | Block sightings | Per block |
|---|---:|---:|---:|
| Original XNA | 9384 | 110 | 85.3 |
| CNA native OPENGLES3, before the fix | **0** | — | **0** |
| CNA native OPENGLES3, after the fix | 6452 | 53 | 121.7 |

The per-block figures are not expected to match exactly: each block's sub-pixel
phase is a fresh random number, and a block only partly on screen contributes fewer
edge pixels. What matters is that the original filters its sprite edges and CNA
produced not one such pixel until this fix.

Fix: `ISpriteBatchRenderer` gained a sub-pixel `Draw` overload whose **default
implementation truncates and forwards to the existing integer overload**, so no
other renderer changes behaviour; `SpriteBatch` stops quantising (it still
validates the Int32 destination window and still rejects NaN/infinite/out-of-range
input); the EasyGL renderer overrides the new overload and builds its quad from the
unrounded values. Tests: four new `SpriteBatchSubPixelDestinationTest` cases,
including one proving a renderer that has *not* adopted the overload still receives
the same integer rectangles as before.

`SpriteBatch::DrawString` still **rounds** glyph destinations
(`RoundDestinationComponent`). That is a separate question with its own recorded
pixel-identical baselines in SAMPLE-009/010/011, this sample draws no text, and no
evidence here bears on it — so it was deliberately left untouched rather than
changed without evidence.

### Regression runs

Full `CnaTests`, repo-root CWD, `Xvfb :119`, `SDL_VIDEODRIVER=x11`,
`LIBGL_ALWAYS_SOFTWARE=1`:

- With the `Color` fix, before the SpriteBatch one: 8586 ran, 8501 passed, 71
  skipped, **14 failed**.
- Those 14 were then confirmed pre-existing rather than assumed: the `Color` change
  was reverted on disk, `CnaTests` rebuilt, and exactly those 14 re-run against
  unmodified `next` HEAD — the identical 14 failed. The change was restored and
  rebuilt afterwards.
- With the SpriteBatch fix as well: 8586 ran, 8500 passed, 71 skipped, 15 failed —
  the same 14 plus `GltfConformanceL6.OnlyAMaskMaterialWritesAnAlphaTestReference`,
  which threw `AcquireSubsystem(Video) failed: x11 not available` (a transient X
  failure during a 500 s run) and passes on re-run.
- With every change and the six new tests in place: 8590 ran, 8505 passed, 71
  skipped, 14 failed — exactly the pre-existing 14, and this time with no flake.
- Focused re-runs: `GltfConformanceL6.*:SpriteBatch*:SpriteFont*:SpriteEffect*`
  129/129 passed; `SpriteBatch*:ColorTest.*` 114/114 passed.

The pre-existing 14 are `VertexDeclarationLayoutTest` (6), `DeclarationGuardTest`
(4), `GltfLimitationsDoc.CnaextSection32DoesNotClaimMoreThanTheRegistry` (cannot
open `CNAEXT.md` from this CWD), `GltfRendererPbrFallbackPolicy.…`,
`GpuTimerTest.MoreWorkTakesMoreGpuTime` (a timing ratio under software GL) and
`ShadowVisibilityTest.…`. None involve `Color` or `SpriteBatch`.

No `sharp-runtimenext` change was needed.

## 6. Verification

### Person position — original vs native vs browser

The person is drawn at an integral position, so it can be located exactly by
matching the official texture's opaque-black and opaque-white masks against each
capture:

| State | XNA original | CNA native OPENGLES3 | Chrome WEBGL2 |
|---|---|---|---|
| Start (no input) | `(343, 399)` | `(343, 399)` | `(343, 399)` |
| Left held to the clamp | `(40, 399)` | `(40, 399)` | `(40, 399)` |
| Right held to the clamp | `(727, 399)` | `(727, 399)` | `(727, 399)` |

The 32x32 person sprite region is **byte-identical** between the XNA original and
both CNA builds at all three positions, which also verifies the colour key,
premultiplied alpha and `SpriteBatch` tint end to end.

### Per-pixel collision, not rectangle collision

`scripts/analyze-frames.py` locates the person and every block in each recorded
frame and classifies it. `rectangleOverlapWithoutPixelHit` counts frames where the
two 32x32 rectangles overlap while the game left the background blue — the exact
behaviour this tutorial exists to demonstrate:

| Recording | Frames | Red (pixel hit) | Rectangle overlap, no pixel hit | Pixel hit without rectangle overlap |
|---|---:|---:|---:|---:|
| XNA original, 180 s | 1800 | 21 | 16 | 0 |
| CNA native, 180 s | 1800 | 38 | 25 | 0 |

Both directions are therefore proven on the real `Texture2D.GetData` readback of
the official XNB: transparent texels are reported transparent (otherwise every
rectangle overlap would turn the screen red) and opaque texels are reported opaque
(otherwise nothing would ever turn red). `personY` is `399` in every located frame
of both recordings and `personXMax` is `727` in both. The XNB→`GetData` path itself
is separately covered by CNA's own
`modules/content/tests/Microsoft/Xna/Framework/Content/ContentManagerTexture2DXnbTests.cpp`.

### Native OPENGLES3

`scripts/smoke-cna-native.sh` (renderer banner `CNA: graphics renderer: OPENGLES3`,
alive after 6 s, no fatal log line) and `scripts/capture-cna-native.sh`
(start/left/right captures, 180 s recording, collision and near-miss frames,
`exited_on_escape=1`).

### Browser WEBGL2

`scripts/capture-web.sh` serves the complete bundle
(`.html`, `.js`, `.wasm`, `.data`) over local HTTP and drives the system
`/usr/bin/google-chrome`. Three properties of that harness are worth recording,
because each one was learned by a run that went wrong:

- **Chrome runs on its own X display.** An earlier run on the shared `:0` desktop
  recorded stray external `ArrowUp`/`ArrowLeft`/`Shift` DOM key events that moved the
  person before the script pressed anything. The script now logs every DOM key event
  the page receives, so contamination is visible in `browser-result.json` instead of
  being silently folded into the result.
- **Every CDP request carries a 60 s deadline.** On the Xvfb display Chrome's GPU
  raster path intermittently fails (`Creation of StagingBuffer's SharedImage failed`)
  and the page loses its WebGL context, after which `Page.captureScreenshot` and
  `Runtime.evaluate` simply never answer. The deadline turns that into a fast,
  obvious failure. The run of record therefore uses ANGLE/SwiftShader
  (`--use-gl=angle --use-angle=swiftshader`), which is stable on this host.
- **The harness plays the game rather than waiting for luck.** Blocks spawn at 1 %
  per frame at random X, so a pixel-perfect hit is a rare event, and it is rarer
  still on a software rasteriser. Each step therefore reads the canvas back, locates
  the person by its white head pixels and, in the lowest drawn row above it, the
  contiguous run of block pixels nearest the person, then presses the arrow key that
  closes the gap — using nothing but the sample's own Left/Right controls. No game
  state is written and no timing is altered. Two details decide whether it works:
  taking a contiguous run rather than the row's mean (with two blocks in one row the
  mean sits between them and the person chases a point where nothing is falling), and
  stopping entirely once it is lined up under a block that is nearly on top of it —
  otherwise that block drops out of the search window on its last frames, the
  next-lowest block becomes the target, and the person walks out from under the one
  about to land on it.

Verified in `evidence/cna-web-webgl2/browser-result.json`: `moduleReady`, a real
`webgl2` context, canvas 800x480, the `CNA: graphics renderer: WEBGL2` banner, all
four assets served `200`, no rejected promise, no runtime exception, no HTTP error,
no fatal console message, and every DOM key event accounted for by the harness. The
person reaches `(343, 399)`, `(40, 399)` and `(727, 399)` — the same three positions
as XNA and the native build, with a byte-identical sprite. The guided player reached
a real per-pixel hit in 10 steps, and the red frame written out is the very frame
that was tested, so it cannot be a later, already-cleared one.

Getting there took several runs, and the failures are recorded because they are
about the harness, not the port: a stale HTTP server left over from an interrupted
run served one attempt and left the evidence log holding only its own bind error
(the script now refuses to start when the port is taken), and a version of the
guided player without the commit rule chased the wrong target for 900 steps.

### Escape

Escape exits cleanly in the original and in the native build (both scripts assert
the process is gone). In the browser the module stops and the canvas is torn down,
which is why the post-Escape capture is recorded as a note in
`evidence/cna-web-webgl2/after-escape.txt` rather than an image.

## 7. Mechanical no-workaround scan

`grep -rnE "RawMesh|RawModel|NOXNA|SetData|\.model\.json|\.font\.json|help\.png|helpTimer|Keys::F1|CNAEXT" src/ CMakeLists.txt`
returns no hits at all. `GetTypeName()` is overridden without the `CNAEXT` marker,
matching every other sample port in this repository — the marker belongs on CNA's own
declaration, not on a game's override. `Content/` holds only the two official XNBs.
No loose image, sidecar, hand-written shader, renderer helper, invented control,
diagnostic overlay or omitted branch remains.

## 8. Known differences

None in the sample's own translation, content, rendering, input or behaviour.

One framework-level difference is worth recording because it is visible and is not
sample-owned: the **window caption**. XNA/FNA take the initial `Window.Title` from
the entry assembly's `AssemblyTitle` (here `Per Pixel Collision`, from
`Properties/AssemblyInfo.cs`), while CNA opens its window with the fixed title
`"Game"` (`modules/graphics/src/Xna/GraphicsDevice.cpp`). A C++ build has no
assembly metadata to read, the original never calls `Window.Title` itself, and
every port in this repository is affected identically — so it was left alone rather
than papered over with a `setTitleProperty` call the original does not contain. It
is a question about a CNA default, not about this sample.

The Xbox 360 project is not built. It compiles the same three source files with
`XBOX;XBOX360` instead of `WINDOWS`, and `Game1.cs` contains no conditional
compilation, so there is no Xbox-only behaviour to translate; the reference target
for this campaign is the Windows configuration.

## 9. Reproduction

```bash
root=/rv/tmp/samples/SAMPLE-018-PerPixelCollisionSample_4_0
$root/scripts/build-original.sh              # official pipeline + original .exe
$root/scripts/capture-original.sh            # XNA start / clamps / Escape
$root/scripts/capture-original-collision.sh  # XNA 180 s recording + collision frame
cmake -S /rv/data/development/github.com/openeggbert/cna-samples \
      -B $root/cna-native-opengles3 -DCMAKE_BUILD_TYPE=Release
cmake --build $root/cna-native-opengles3 --target PerPixelCollision_cna_samples -j6
$root/scripts/smoke-cna-native.sh
$root/scripts/capture-cna-native.sh
/home/robertvokac/emsdk/upstream/emscripten/emcmake cmake \
      -S /rv/data/development/github.com/openeggbert/cna-samples \
      -B $root/cna-web-webgl2 -DCMAKE_BUILD_TYPE=Release
cmake --build $root/cna-web-webgl2 --target PerPixelCollision_cna_samples -j6
$root/scripts/capture-web.sh
python3 $root/scripts/analyze-frames.py <frames-dir> \
        $root/evidence/content/Person-xnb.png $root/evidence/content/Block-xnb.png \
        <report.json>
```

To play the original interactively:

```bash
cd /rv/tmp/samples/SAMPLE-018-PerPixelCollisionSample_4_0/xna4-build/bin
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine PerPixelCollision.exe
```

Controls (identical in all three builds): Left/Right arrow or gamepad D-pad move
the person; Escape or gamepad Back exits. There are no other controls.
