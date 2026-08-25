# SAMPLE-020 — TransformedCollisionTest audit record

Audit date: 2026-08-25. Second product of upstream
`/rv/tmp/XNAGameStudio/Samples/TransformedCollisionSample_4_0`.
Artifact root: `/rv/tmp/samples/SAMPLE-020-TransformedCollisionSample_4_0`
(shared with the primary game).

The directory-wide audit, the content pipeline run, the framework work and the
regression results are in
[`../TransformedCollision/missing.md`](../TransformedCollision/missing.md). This
record covers what is specific to this product.

## 1. Why this is a separate sample directory

Upstream ships it as its own solution, `TransformedCollisionTestWindows.sln`, with its
own project, its own `Program.cs`, its own content project and its own three assets.
It is a second interactive game, not a test suite: it draws two sprites the player
transforms with the mouse, gamepad and arrow keys, and colours the background by the
result of the rotated per-pixel test.

SAMPLE-017's second product was a unit-test source file with no content and no entry
point, so it became a second target inside the same sample directory. This one cannot:
it has a `Program.cpp` of its own and a content set of its own, and merging the two
would mean one `Content/` holding both games' assets. It is therefore
`samples/TransformedCollisionTest/`, and `plan.md`'s SAMPLE-020 row cites both
`missing.md` files so the artifact-prune tool keeps both products.

## 2. What upstream contains

| Path | Role |
|---|---|
| `TransformedCollisionTestWindows.sln` | The only solution — there is **no** Xbox project for this product. |
| `…/TransformedCollisionTestWindows.csproj` | Windows Reach game project, `DEBUG;TRACE;WINDOWS`. |
| `…/TransformedCollisionTestGame.cs` | The game. Carries three `#if !XBOX360` regions even though no Xbox project exists. |
| `…/TransformedSprite.cs` | A sprite with origin, rotation and scale; owns its colour data, its world transform, and both `IntersectPixels` implementations. |
| `…/Program.cs` | `Main`. |
| `…/Properties/AssemblyInfo.cs` | Assembly metadata only. |
| `…/TransformedCollisionTestContent/TransformedCollisionTestContent.contentproj` | Three items, stock `TextureImporter`/`TextureProcessor`. |
| `…/TransformedCollisionTestContent/{F,Point,R}.bmp` | The source art: two 128x128 glyphs and a 32x32 dot. |
| `Game.ico`, `GameThumbnail.png` | Shell artwork; not runtime content. |

## 3. Translation

`src/TransformedSprite.hpp` ← `TransformedSprite.cs`,
`src/TransformedCollisionTestGame.hpp` ← `TransformedCollisionTestGame.cs`,
`src/Program.cpp` ← `Program.cs`. Namespace `TransformedCollisionTest`;
`GetTypeName()` returns
`"TransformedCollisionTest.TransformedCollisionTestGame"`.

Everything the original has is present:

- both collision implementations — `IntersectPixelsSlow`, which transforms every pixel
  of A individually, and `IntersectPixels`, which steps incrementally. As upstream,
  only the second is called;
- the instance `IntersectPixels(TransformedSprite b)` forwarder;
- the whole gamepad path — trigger to transform, shoulder to move the pivot, both
  thumbsticks — even though this host has no pad attached;
- the mouse path: drag to move, left control to move the origin, wheel to rotate, alt
  to scale, arrow keys to rotate and scale;
- the three `#if !XBOX360` regions, preserved as `#if !defined(XBOX360)` per the
  campaign rule on inactive platform branches;
- the full `Draw`: the grey visualisation of R in F's local space, the green and brown
  origin points there, both sprites in world space, and the two yellow world origins.

Integer division is preserved where C# has it — `pointTexture.Width / 2` and
`viewport.Height / 2` are `int` divisions in the original and are cast after dividing
here, not before.

The only deviation is the `Vector2::operator+=` C++ mechanic described in the primary
record; `TransformedSprite` is a reference type in C# and is passed by reference here,
so the input handlers mutate the same object the original does.

## 4. Content provenance

`Content/{F,Point,R}.xnb` are byte-identical to this product's own official pipeline
output in `xna4-build/TestContent/`, verified with `cmp`; hashes in
`evidence/content-sha256.txt`. No loose source art, no `SetData`.

## 5. What was measured

This product takes deterministic input, so all three builds can be given exactly the
same drag and compared directly — which is why it carries the campaign's sharpest
comparison rather than being a secondary check.

`scripts/capture-original-test.sh`, `scripts/capture-cna-native-test.sh` and
`scripts/chrome-smoke-test.mjs` each hold the left mouse button and move the cursor to
canvas-local `(128, 128 - step)` for `step` in 0, 8, 16 … 128, so the F sprite walks up
onto R, which rests at (128, 0) with the same origin and scale.

| Build | Frames cleared to red |
|---|---|
| XNA 4.0 original, Wine + WineD3D, Xvfb | steps 64, 72, 80, 88, 96, 104, 112, 120, 128 |
| CNA native OPENGLES3, Xvfb | steps 64, 72, 80, 88, 96, 104, 112, 120, 128 |
| CNA WEBGL2, Google Chrome 151.0.7922.71 | steps 64, 72, 80, 88, 96, 104, 112, 120, 128 |

Nine of seventeen positions, the same nine, flipping at the same step. The drawn
geometry agrees as well — the bounding box of the black world-space sprites is
`(42,28)-(222,96)` with 2662 black pixels at the start and `(164,28)-(222,96)` with
1949 at the end, identical in all three builds.

Other results:

- `scripts/smoke-cna-native.sh TransformedCollisionTest`: alive after 6 s, renderer
  banner `CNA: graphics renderer: OPENGLES3`, no fatal log line.
- Escape exits cleanly in the original and in the native port; in the browser it
  collapses the canvas, this build's exit signature.
- Browser gate: `moduleReady`, `webgl2: true`, banner `CNA: graphics renderer: WEBGL2`,
  `collisionSeen: true`, `spritesMoved: true`, no unhandled rejection, no runtime
  exception, no HTTP error, no fatal console message, all four assets served `200`.
  `evidence/cna-web-webgl2/TransformedCollisionTest/browser-result.json`.

Captures: `evidence/{xna-original,cna-native-opengles3}/TransformedCollisionTest/`
(start, `drag-000` … `drag-128`, final, hashes) and
`evidence/cna-web-webgl2/TransformedCollisionTest/`.

## 6. Scans

No `NOXNA`, no CNAEXT graphics helper, no renderer/backend include, no `SetData`, no
loose non-XNB content, no invented control, no help overlay. This product has no
`help.png` upstream and none was added.

## 7. Known differences

None active.
