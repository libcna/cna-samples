# SimpleAnimation — port notes

Upstream: `SimpleAnimation_4_0` (SAMPLE-050). **Re-ported from scratch.** Ported whole — the tank,
its nine animated bones, the orbiting world rotation, the fixed camera and Escape.

Artifact root: `/rv/tmp/samples/SAMPLE-050-SimpleAnimation_4_0/`.

## Current-head requalification — 2026-09-25

The retained `xna4-original/` is byte-identical to all **20 files** in the physical
`/rv/tmp/XNAGameStudio/Samples/SimpleAnimation_4_0/` directory. The original
Windows project specifies Reach; its unchanged game and stock `FbxImporter` /
`ModelProcessor` content were freshly rebuilt with XNA 4.0. One listed FBX
produced `tank.xnb` and two material-texture XNBs. All **three Windows/Reach
outputs** are byte-identical to the checked-in `Content/` and the current
native product. The separately rebuilt HiDef and Xbox outputs are retained for
provenance; their target-specific bytes differ from Reach. The authoring FBX,
two TGAs and `.contentproj` remain in the exact artifact snapshot, so they
need not be duplicated in the port alongside the shipped XNBs.

The port now retains the original Microsoft license, `Game.ico`,
`SimpleAnimationSample.png` and the phone project's `Background.png`; the
existing `SimpleAnimation.htm` remains byte-identical. That HTML references
`Model-ModelMesh.png`, which is missing from the **physical upstream directory**
as well. No replacement image was invented. The original source's conditional
`WINDOWS_PHONE` constructor branch is now preserved in C++: 333333-tick frame
time and full screen. The tested Windows build leaves that branch inactive, as
the original Windows project does. A separate `-DWINDOWS_PHONE -fsyntax-only`
translation of `SimpleAnimationGame.cpp` succeeds; no phone hardware run is
claimed.

The current builds use `libcna/cna` at `96b5038de` and
`libcna/sharp-runtime` at `41b918c9`, through `scripts/build-current.sh
native|web`; the diagnostic pair comes from `scripts/build-diag.sh native|web`
in a separate source tree. Neither dependency repository needed a source fix.
The shipped game code has no sample workaround or diagnostic hook. The native
binary now has the current `libcna/cna` SDL RUNPATH, replacing the historical
`openeggbert/cnanext` path.

The unchanged XNA build and current native OPENGLES3 game both render the
800×480 tank, animate without input and close their window on Escape. An
isolated `CNA_TIME` diagnostic pair fixes both engines to the same instant;
the two captures within every leg are byte-identical, while the four instants
change the tank position and silhouette:

| time | XNA vs native within 8 | after 4 px blur |
|---|---:|---:|
| 0 s | 99.98% | 100.00% |
| 3 s | 99.97% | 100.00% |
| 7.5 s | 99.97% | 100.00% |
| 12.25 s | 99.96% | 100.00% |

The current Release, no-threads WEBGL2 bundle runs in system Google Chrome
over local HTTP. Its gate reports textured tank, changing frames, Escape
exit, original title, WEBGL2 renderer, and no application exceptions or
missing bundle requests. The browser requests an optional `favicon.ico`
that is not in the Emscripten bundle; the four required files all return 200.
At pinned 3 s, Chrome vs real XNA is **99.74% within 8 levels** and **100.00%
after blur**; Chrome vs native is 99.77% within 8. The exact four bundle
files were copied into the gallery's 49th entry and that copy independently
passed the same real-Chrome gate. The gallery has a card on page 5, a detail
page, screenshot, thumbnail and previous/next links.

Fresh logs, SHA-256 values, captures and comparisons are under
`evidence/requal-20260925/` in the artifact root. `scripts/build-original.sh`
rebuilds the official XNA game/content; `scripts/capture-original.sh`,
`scripts/capture-cna-native.sh` and `scripts/capture-web.sh` reproduce the
runtime legs. The tables and sabotage experiment below are historical
evidence from the earlier port pass; the current-head numbers above supersede
their build and runtime status.

## What the 2026-07-11 pass built, and what is left of it

That pass had no content pipeline, so it hand-converted `tank.fbx` into a `.model.json` plus 24
`_verts.bin`/`_idx.bin` files and two PNGs, and then spent most of its write-up repairing that
conversion. **All of it is gone**, replaced by the three `.xnb` the official pipeline wrote. The
findings go with it:

| The old note said | Measured now |
|---|---|
| `.model.json` has no per-mesh rest transform, so `Tank::Load` needs an `ApplyRestTransforms()` table of hand-computed absolute offsets, or every part piles up at the tank's origin | Gone, and the hierarchy is dumped below rather than assumed. `Load` is a straight translation of `Tank.cs`: nine `Bones["name"]` lookups and nine saved `Transform`s, nothing else. |
| The parts render flat white because `.model.json` had no `"texture"` field | Gone. The two textures are **not rows in the content project** — the `FbxImporter` resolves them from the FBX's own materials — and the build emits three `.xnb` from one listed asset. |
| All 12 index buffers had reversed winding and were rewritten by hand | Moot: no index buffer is shipped any more. Nothing about winding needed changing on the pipeline's own data. |
| The port sets `1280x720` explicitly because the implicit default misbehaved | Gone. The port sets no back-buffer size, exactly as the original does not, and the window comes out **800x480** — the same size XNA's own window is captured at. |
| An F1 help overlay and its `help.png` | Not in the original; removed. |

The one thing that survived is the shape of the sample: `SimpleAnimation.cs` is a thin wrapper and
`Tank.cs` is the sample.

## The bone hierarchy, read out of the model rather than assumed

The old pass's central claim was that the reader gave every mesh its own bone directly under a
synthetic root, with an identity transform. A one-off probe in `Load` (run, recorded, removed —
`evidence/probe/cna-bones.txt`) shows what the pipeline's `.xnb` actually produces: **12 bones,
four levels deep, every one with a real translation, and the root is the tank body itself.**

```text
tank_geo                                    (root, 3 children)
├── r_engine_geo        (-139.501, 163.813, -10.957)
│   ├── r_back_wheel_geo   (-134.906, -58.927, -234.273)
│   └── r_steer_geo         (-31.751,  22.161,  251.485)
│       └── r_front_wheel_geo  (85.214, -112.979, -1.132)
├── l_engine_geo         (139.501, 163.813, -10.957)   … mirrored
└── turret_geo             (0.000, 231.754, -35.595)
    ├── canon_geo          (0.000, 104.642, 102.744)
    └── hatch_geo         (62.984, 125.905, -43.586)
```

That is why `Tank.cs`'s own technique works unchanged: `l_front_wheel_geo`'s world transform is
composed by `CopyAbsoluteBoneTransformsTo` through `l_steer_geo` and `l_engine_geo`, so steering
the wheel and rolling it are two separate bones and neither has to know about the other.

## Content and profile

**One listed asset, three `.xnb`.** `SimpleAnimationContent.contentproj` lists exactly one row,
`tank.fbx` through the stock `FbxImporter`/`ModelProcessor`; `engine_diff_tex_0.xnb` and
`turret_alt_diff_tex_0.xnb` arrive because the FBX's two materials name them. No pipeline extension
of any kind — the first of the recent samples that needs none.

The Windows project declares `<XnaProfile>Reach</XnaProfile>`, so the executable's embedded
`RuntimeProfile` resource says Reach too.

## No CNA or sharp-runtime change was needed

Nothing was fixed in either repository for this sample. The nine bone lookups, the saved rest
transforms, `CopyAbsoluteBoneTransformsTo` and the per-mesh `ParentBone.Index` lookup all work on
the pipeline's own model, and the numbers below are what says so.

## Agreement with real XNA 4.0

Everything in this sample moves on its own — the wheels at 5 rad/s, the steering, the turret, the
cannon, the hatch and the world rotation are all functions of `gameTime.TotalGameTime` — and
Escape is the only input. Two engines free-running can therefore never be compared frame for
frame. `../../../cna-diag/` and `../../../xna4-diag/` add a single hook, `CNA_TIME=<seconds>`,
which pins the instant both engines animate from; `scripts/compare-frozen.sh` drives it.

| pinned instant | within 0 | within 8 | within 32 | after 4 px blur |
|---|---|---|---|---|
| t = 0 s | 89.51 % | **99.98 %** | **99.99 %** | **100.00 %** |
| t = 3 s | 90.17 % | **99.97 %** | **99.98 %** | **100.00 %** |
| t = 7.5 s | 90.07 % | **99.97 %** | **99.98 %** | **100.00 %** |
| t = 12.25 s | 88.09 % | **99.96 %** | **99.98 %** | **100.00 %** |

Silhouette coverage matches to the third decimal in every leg (23.522 / 23.521, 22.843 / 22.842,
21.735 / 21.734, 21.464 / 21.464 %) and the centroids to two decimals. Mean absolute difference is
0.060 to 0.080 of 255.

**The hook is verified from both sides.** Within each leg the two captures taken two seconds apart
are byte-identical on both engines — so the clock really is pinned — while across legs all eight
hashes differ and the coverage and centroid move (centroid x 433.9 → 433.6 → 415.1 → 395.1). A
metric that could not tell the legs apart would prove nothing.

## Web

`WEBGL2` built under a real Emscripten toolchain and driven in real Google Chrome. This sample gets
**two** browser legs, because the same `CNA_TIME` hook works there: a browser tab has no
environment, so the frozen build reads the instant out of the page's query string instead.

**The frozen leg is the strong one.** Loaded as `…SimpleAnimation_cna_samples.html?t=3`, the canvas
goes still (two captures two seconds apart are byte-identical) and its pixels are then directly
comparable with the two native captures at `CNA_TIME=3`. Cropping the 1 px canvas border and focus
ring Chrome paints inside its own screenshot clip (`compare.py --inset 3`):

| | within 8 | within 32 | after 4 px blur | coverage |
|---|---|---|---|---|
| Chrome/WEBGL2 vs **real XNA** | **99.74 %** | 99.87 % | **100.00 %** | 23.306 / 23.304 % |
| Chrome/WEBGL2 vs native OPENGLES3 | **99.77 %** | 99.88 % | **100.00 %** | 23.306 / 23.304 % |

Centroids agree to 0.05 px. That is the browser drawing the same picture as real XNA, not merely a
picture.

The shipped bundle is gated separately, on the numbers a free-running tab can produce:

- `tankFraction` — everything that is not the `Color.DarkGray` clear — must stay inside
  0.15–0.32; natively the whole animation cycle spans 21.46 %–23.52 %, and Chrome measured
  23.14 %, 23.32 % and 22.45 %;
- **`greenFraction` is the gate this sample is pinned by**, and it was calibrated by breaking the
  thing it watches. It is the share of the silhouette that is actually green, which is only true if
  the two FBX-implicit textures loaded and lit. Natively it spans 23.20 %–39.96 %; with
  `TextureEnabled` forced off in a sabotage build it falls to **5.77 %**, while `tankFraction`
  barely moves (22.84 % → 22.56 %). The gate demands > 15 %, so it fails for exactly that defect and
  the silhouette check alone would not have caught it. Chrome measured 19.95 %–29.17 %;
- consecutive frames must differ, because nothing here waits for input;
- Escape reaches the game and `Exit()` stops it — the canvas is identical across the two captures
  taken after it;
- no runtime exceptions, no HTTP errors, no fatal console messages, `WEBGL2` renderer logged,
  document title `Simple Animation`.

## Deviations from the original source

All of them are this campaign's established C++ mappings, not decisions taken here:

- `Model tankModel` is `std::optional<Model>`, because XNA's `Model` is a reference type that is
  null until `Load` assigns it;
- C# properties become `getWheelRotationProperty()`/`setWheelRotationProperty()` and so on;
- `foreach (BasicEffect effect in mesh.Effects)` becomes a `dynamic_cast` per element that throws
  `System::InvalidCastException` on a mismatch, which is what the C# cast-per-element loop does;
- `Math.Sin` becomes `std::sin`, and `MathHelper.Clamp(x, -1, 0)` becomes `Clamp(x, -1.0f, 0.0f)`
  because the integer literals would make the C++ overload ambiguous;
- `static class Program` at the bottom of the file becomes `int main()` in the same place;
- `[assembly: AssemblyTitle("Simple Animation")]` becomes `CNA::AssemblyTitleAttributeEXT`, which
  is where the window title comes from;
- the `#if WINDOWS_PHONE` block (30 fps and full screen) is retained under the
  corresponding C++ conditional, using CNA's XNA-shaped setters.

## Evidence

- `evidence/build-original.log` — the official pipeline, one listed asset and three `.xnb`.
- `evidence/frozen/<t>/{xna,cna}/` — one directory per pinned instant, plus `frozen/t3/web/` and
  its `compare.txt` for the browser leg.
- `evidence/sabotage/no-texture/` — the frame the `greenFraction` gate is calibrated against.
- `evidence/cna-web-webgl2/browser-result.json` — the shipped bundle's own numbers.
