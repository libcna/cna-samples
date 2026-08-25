# SAMPLE-022 — Pathfinding_4_0 audit record

Audit date: 2026-08-25. Upstream directory:
`/rv/tmp/XNAGameStudio/Samples/Pathfinding_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-022-Pathfinding_4_0`.

## 1. What upstream actually contains

42 files, **two** solutions and **three** projects:

| Path | Role |
|---|---|
| `Pathfinding (Windows).sln` | Game + content + `PathfindingData`. The audited configuration. |
| `Pathfinding (Phone).sln` | The same three plus a phone build of the data library. |
| `Pathfinding/Pathfinding/Pathfinding (Windows).csproj` | `DEBUG;TRACE;WINDOWS`. |
| `Pathfinding/Pathfinding/Pathfinding (Phone).csproj` | `DEBUG;TRACE;WINDOWS_PHONE`. |
| `…/PathfindingSample.cs` | The game: class `Pathfinding` in namespace `Pathfinding`. |
| `…/PathFinder.cs` | Breadth-first, best-first and A* over the map, one step per time step. |
| `…/Map.cs` | The tile grid, its four maps, and map↔world translation. |
| `…/Tank.cs` | Drives the found path. |
| `…/WaypointList.cs` | `Queue<Vector2>` that draws itself as a red→blue gradient. |
| `…/Program.cs` | `Main`, inside `#if WINDOWS || XBOX`. |
| `MapData/` (10 files) | The `PathfindingData` class library and its four project files. |
| `PathfindingContent/*.png`, `*.tga`, `HUDFont.spritefont` | Textures, four Xbox button glyphs and the HUD font. |
| `PathfindingContent/Map1-4.xml` | The four maps, `XmlImporter` + `PassThroughProcessor`. |
| `Pathfinding.htm`, `Documentation`, `Game.ico`, licence | Documentation and shell artwork. |

Conditional compilation is real here, unlike SAMPLE-021: `#if WINDOWS || XBOX` around
`Program`, and two `#if WINDOWS_PHONE` regions in the game — one setting the 30 Hz
frame rate and fullscreen, one swapping the desktop's Xbox-glyph HUD for on-screen
buttons. All three are preserved in the translation.

The upstream snapshot is retained at `xna4-original/` with per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## 2. The `PathfindingData` library: status decided, not assumed

`MapData/` is a **runtime class library**, not a Content Pipeline extension. It
references only `Microsoft.Xna.Framework`, and it exists to declare one type — `MapData`,
five public fields — that the game consumes and that the content project references so
`XmlImporter` can resolve `PathfindingData.MapData` when deserializing the four maps.

That makes it a different case from both existing precedents. SAMPLE-004's StockEffects
was a pipeline *compiler* and became an evidence-backed non-port; SAMPLE-020's
TransformedCollisionTest was a second *runnable game* and became its own sample
directory. This is neither: it is a type the game cannot run without.

**Decision: the type is ported, the project file is not.**
`samples/Pathfinding/src/MapData.hpp` carries `PathfindingData::MapData` with its
namespace, its five fields in declaration order and both of its constructors. No CMake
target corresponds to `PathfindingData.csproj`, because a C++ header needs no separate
library to be linked into the one consumer that exists. Nothing about the type's
serialized shape or its runtime behaviour changes.

The library is still built during the audit — `scripts/build-original.sh` compiles it
with `csc` and hands it to the pipeline as a `PipelineAssembly`, which is exactly what
the content project's `ProjectReference` does in Visual Studio.

## 3. Original XNA 4.0 build and run

The unchanged original was built and run on this Linux host from its Windows
configuration:

1. `PathfindingData.dll` from the library's unmodified sources;
2. the content, through the official `BuildContent` task with that DLL loaded, for
   **both** target platforms so the difference could be measured;
3. `Pathfinding.exe` from the game's seven unmodified sources with the Windows defines.

`scripts/capture-original.sh` then runs it under an isolated Xvfb display with
`WINEDLLOVERRIDES=d3d9=b`, driving the sample's own keys: **A** starts the search, **X**
cycles the search method, **Y** loads the next map.

## 4. Content provenance, including a type this sample declares

All **13** XNBs in `Content/` are byte-identical to this sample's own official pipeline
output for the Windows target (`cmp`, all 13); hashes for both platforms in
`evidence/content-sha256.txt`. That includes the four `.tga` Xbox button glyphs — the
first TGA importer path in this campaign — and `HUDFont.xnb`.

The previous port's loose `barrier.png`, `dot.png`, `tank.png`, `whiteTile.png`, four
`xboxControllerButton*.png` and the four raw `Map*.xml` are gone.

### The four maps, and the closed AOT reader they need

Each map XNB holds a `ReflectiveReader<PathfindingData.MapData>` over four type readers.
Decoding `Map1.xnb` by hand establishes the exact layout the reader must implement:

```
readers: ReflectiveReader<PathfindingData.MapData>, Int32Reader, PointReader,
         ListReader<Microsoft.Xna.Framework.Point>
object : NumberRows=4  NumberColumns=8  Start=(0,0)  End=(7,3)
         Barriers -> reader index 4, count 5, [(2,0) (2,1) (2,2) (4,2) (4,3)]
```

which matches `Map1.xml` field for field. `src/ContentReaders.hpp` registers a closed
reader for that name and a `ListReader<Point>` for the barrier list, and reads the object
in that order.

One detail had to be measured rather than guessed: the reflective writer **inlines a
value-type field with no reader index**, so `NumberRows`, `NumberColumns`, `Start` and
`End` are read raw — a `Point` being exactly the two `Int32`s `PointReader` writes — while
only `Barriers`, a reference type, carries an index for `ReadObject` to consume. Reading
the Points through `ReadObject` instead loaded `map1` with silently wrong values and then
failed on `map2` with "incorrect type reader index"; that is how the layout was pinned.

`Content.Load<MapData>("map1")` is the original call, lowercase, against an asset the
pipeline named `Map1` — CNA's case-insensitive content resolution handles it, as the
running port demonstrates.

## 5. What the previous port claimed, and what is actually true

| Old claim | Finding |
|---|---|
| "HUD text labels omitted… CNA had no SpriteFont support" | **Removed.** Every `DrawString` is restored from the official `HUDFont.xnb`, including `MeasureString` for the slider label. |
| "Touch input omitted… CNA targets desktop; no touch panel abstraction" | **False.** CNA has the whole `Input::Touch` surface. The `Tap` gesture handling for the four buttons and the raw-touch slider drag are translated exactly. |
| "Custom Content Pipeline type replaced by direct XML parsing" | **Removed.** The port hand-parsed the XML with `std::ifstream` and string search. It now loads the official XNBs through a closed AOT reader (section 4). |
| "All Draw calls merged into a single SpriteBatch Begin/End… CNA Vulkan discards all but the last pair" | **Not true on EasyGL.** The original issues six independent `Begin()/End()` pairs per frame — `Map`, `PathFinder`, `WaypointList`, `Tank`, `DrawHUD`, `DrawPathStatus` — and all six are restored and render correctly. |
| "Color named constants replaced with RGBA literals" | **Removed.** `Color::Navy`, `LightBlue`, `Green`, `Red`, `Orange`, `Black`, `White` are used directly. |

## 6. Translation

`samples/Pathfinding/src/` mirrors the original file decomposition, one header per C#
file, plus `ContentReaders.hpp` for the closed reader and `Program.cpp` for the entry
point. Namespace `Pathfinding`; `GetTypeName()` returns `"Pathfinding.Pathfinding"`.

All three search methods, the `SearchStatus`/`SearchMethod` enums and their exact cycling,
the A* tie-break that prefers the longer equal-heuristic path, `FinalPath()`'s backward
walk, the waypoint gradient, the tank's steering, and the whole HUD are present.

### Deviations, all mechanical

| Original | Port | Why |
|---|---|---|
| `IEnumerable<Point> OpenMapTiles` with `yield return` | returns `std::vector<Point>` | C++ has no `yield return`; the same four candidates in the same order. |
| `MapTileType[,]` | one flat `std::vector` with `[column, row]` index maths | C++ has no built-in rectangular array. |
| `struct SearchNode` (implicit value equality) | an explicit `operator==` | C# synthesises struct equality; `openList.Remove` needs it. |
| `out SearchNode result` | a reference parameter | C++ has no `out`. |
| `SearchMethod.ToString()` | `SearchMethodToString()` | C++ enums have no member names at runtime; the HUD prints the C# names. |
| `SpriteFont HUDFont;` | `std::optional<SpriteFont>` | XNA's SpriteFont is a reference type; SAMPLE-017's established pattern. |
| `PathfindingData` as a linked library | the type as a header in the same target | Section 2. |

`onePixelWhite.SetData(...)` is the **original's own** call and stays.

## 7. Framework work this sample required

One change, in `sharp-runtimenext`.

**`Dictionary<TKey,TValue>` must accept a key that carries the .NET contract.**
`PathFinder` keys its `paths` dictionary by `Point`. .NET reaches
`EqualityComparer<TKey>.Default`, which exists for every type; sharp-runtime's
`DefaultKeyHash<T>` fell through to `std::hash<T>`, which has no `Point` specialization,
so `Dictionary<Point,Point>` would not compile — the same shape of gap SAMPLE-020 found
in `List<T>`.

`DefaultKeyHash` now selects a hasher that calls `T::GetHashCode()` when — and only
when — `std::hash<T>` is absent and `GetHashCode()` is present. The substitution is
deliberately narrow: a type with `std::hash` keeps using it, and a type with neither
keeps selecting `std::hash<T>` exactly as before. That last case matters: an existing
contract test asserts `DefaultKeyHash<std::tuple<double>>` **is** `std::hash<...>`, and a
first, wider version of this change broke it. The narrowed form leaves it standing.

Tests: `modules/collections/tests/System/Collections/Generic/DictionaryGetHashCodeKeyTests.cpp`,
six cases covering usability, equal-key collapse, remove/lookup, a deliberate hash
collision between unequal keys, the unchanged `std::hash` selection, and ordinary keys.
Full sharp-runtime suite after the change: **17853/17853**.

No CNA change was needed.

## 8. What was measured

Both builds were driven through the same key sequence on an isolated Xvfb display, and
every frame compared pixel for pixel.

| Frame | Identical pixels | Note |
|---|---|---|
| `start` | **384000 / 384000** | map, HUD, glyphs, font |
| `searching` (6 s in) | **384000 / 384000** | 7388 open-node and 11259 closed-node pixels, identical |
| `map2` (after **Y**) | **384000 / 384000** | a different map, loaded through the same reader |
| `late` (16 s in) | 375420 / 384000 | differences confined to one region |
| `method2` (after **X**) | 375423 / 384000 | likewise |
| `method3` | 376391 / 384000 | likewise |

The three that differ do so only because the tank is **moving**: masking the single
bounding box that contains the differing pixels leaves **0** differing pixels outside it
in all three, and the box contains the tank plus the waypoint dots it has already
consumed. The search itself is deterministic and identical — `searching` matches to the
byte, step counter included, and both builds report `Search Steps: 15` and `Path Found!`
on the `late` frame.

### The browser

`scripts/capture-web.sh` serves the bundle and drives Google Chrome 151.0.7922.71 on its
own Xvfb display through the same A/X/Y sequence. Colour-population counts across the
three builds:

| Frame | | navy | lightBlue | green | red | orange |
|---|---|---|---|---|---|---|
| start | XNA / native | 141952 | 126110 | 19 | 1431 | 400 |
| | WEBGL2 | 141150 | 125320 | 24 | 1433 | 400 |
| searching | XNA / native | 128092 | 115715 | 7388 | 11259 | 400 |
| | WEBGL2 | 127308 | 114925 | 7472 | 11297 | 400 |
| map2 | XNA / native | 61355 | 66855 | 1 | 283 | 400 |
| | WEBGL2 | 60871 | 66414 | 3 | 284 | 400 |

XNA and the native port agree exactly; the browser is within ~0.6% on the tile counts and
a handful of pixels on the node counts, which is the moving tank and tile-edge filtering
at a slightly different frame. Structure, map switching and the HUD match.

Gate results: `moduleReady`, `webgl2: true`, the banner `CNA: graphics renderer: WEBGL2`,
`mapDrawn`, `searchRan`, `mapChanged`, `hudDrawn`, no unhandled rejection, no runtime
exception, no HTTP error, no fatal console message, all four assets served `200`.
`evidence/cna-web-webgl2/browser-result.json`.

## 9. Scans

No `NOXNA`, no CNAEXT graphics helper, no renderer/backend include, no loose non-XNB
content, no invented control, no help overlay, no runtime XML parsing. The single
`SetData` is the original's own one-pixel white texture, which the rules explicitly
permit. `help.png` sits at the sample root and is never loaded.

## 10. Known differences

None active. The touch path is translated in full and is reachable, but this sample's
desktop configuration draws the Xbox-glyph HUD rather than the phone's on-screen buttons,
so on a host without a touch screen it is played with the keyboard or a gamepad, exactly
as the original's Windows build is. SAMPLE-021's `TouchPanel` mouse opt-in is available
should the owner want the phone button layout driven with a pointer here too; it is not
enabled, because this sample is fully playable without it.

## 11. Regression

- sharp-runtime full suite: **17853/17853** passed.
- `CnaTests` full suite: the same 14 failures present on unmodified `next`, no new one.
  Log: `evidence/cnatests-full.log`.
