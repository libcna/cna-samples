# ShatterEffect — port notes

Upstream: `ShatterEffectSample_4_0` (SAMPLE-042). Ported whole — the model, the compiled shatter
effect, the shatter clock and both key bindings. Nothing is missing, stubbed or simplified, and
**no framework change was needed**: the sample rendered correctly on its first run.

The placeholder that stood here claimed the sample was blocked on `ShatterEffect.fx`, which would
have to be "hand-translated to GLSL plus a `.shader.json` descriptor". That is stale twice over:
compiled custom effects have worked since SAMPLE-032, and nothing is hand-translated — the
official pipeline compiles the `.fx` and CNA runs it.

## Content

Three listed assets. `tank.fbx` goes through the sample's own pipeline extension:

| Asset | Importer / Processor |
|---|---|
| `ShatterEffect.fx` | `EffectImporter` → `EffectProcessor` |
| `tank.fbx` | `FbxImporter` → **`ShatterProcessor`** (this sample's own assembly) |
| `font.spritefont` | `FontDescriptionImporter` → `FontDescriptionProcessor` |

The build produces four XNBs — the FBX material pulls in `turret_alt_diff_tex` — and the sample
declares **Reach**, unlike the two before it; the shader compiles `vs_2_0`/`ps_2_0`, so all three
profiles build and the Reach output is what ships.

## What this sample exercises in CNA

`ShatterProcessor` is the point of the row, and it is where a port could plausibly have failed:

- it **splits every triangle into a disconnected copy**, so the tank arrives as thousands of
  independent triangles rather than a connected mesh;
- it adds **two processor-generated per-triangle vertex channels** — `TriangleCenter`
  (`TEXCOORD1`) and `RotationalVelocity` (`TEXCOORD2`), both `Vector3` — on top of the usual
  position/normal/texcoord;
- it repoints the material at `ShatterEffect.fx`, whose vertex shader builds a yaw/pitch/roll
  matrix per triangle from those channels and rotates each triangle about its own centre,
  translates it along its normal and drops it on a `time*time` arc.

So the whole effect is a five-channel vertex declaration feeding a compiled custom effect, with
the per-triangle data coming from the content rather than from the game. CNA read the declaration,
bound all five channels by semantic and ran the effect with no changes.

## Comparison against the original

`time` is the sample's only state, and everything visible is a pure function of it. `CNA_TIME`
pins it in both engines (`scripts/compare-frozen.sh`, `cna-diag/README.md`).

| Shatter clock | Within 8 levels | After a 4 px blur | Coverage XNA / CNA | Differing pixels on an edge |
|---|---|---|---|---|
| 0.0 s (intact) | 96.65 % | **98.39 %** | 75 622 / 75 180 | 95 % |
| 0.5 s | 89.82 % | **98.33 %** | 90 290 / 89 954 | 99 % |
| 1.0 s | 89.19 % | **98.59 %** | 97 465 / 97 220 | 99 % |
| 2.0 s (collapsed) | 96.90 % | **99.67 %** | 27 947 / 27 831 | 99 % |

The unblurred figure is the lowest in this campaign so far, and the blur column is why that is not
alarming. **A 4 px blur removes sub-pixel boundary noise but preserves any real displacement**, and
under it the two engines agree to 98.3–99.7 % at every instant. Coverage tracks to within 0.6 %,
the model occupies exactly rows 75–418 in both at `time = 0`, the median per-pixel difference is
**0** everywhere, and 95–99 % of the differing pixels lie on an edge.

That is what a mesh of thousands of *disconnected* triangles looks like when two rasterizers
disagree about a boundary pixel. The ratio of differing to covered pixels tells the same story: it
is 17 % on the intact model, jumps to 30.6 % after only `time = 0.02` — a rotation of about 11
degrees, enough to open a hairline between every pair of neighbouring triangles — and then
saturates at ~43 % rather than growing with time. A drift in the shatter maths would keep growing
and would move the blurred images apart; neither happens.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the model
renders against the clear, **the model shatters and falls**, that the shatter is a *continuum*
rather than a switch, that Down reverses it, and that holding Down long enough restores the start
frame — the last one by **hash equality**, which the sample earns by clamping `time` at 0.

The falling checks measure the **share** of the model's pixels below the vertical middle
(0.33 → 0.58 → 0.96 → 0.41 → 0.33), not a count. The first version of the gate counted pixels and
failed on a working sample: a fully collapsed pile covers a third of the pixels the standing tank
does, so the absolute count moves the wrong way at the end of the animation.

The capture scripts also use a `hold` helper rather than `tap`, because this sample advances only
while a key is held; a tap moves `time` by about one frame.

## Deviations

None in behavior. Three C++ shapes worth naming:

- `Matrix[] transforms = new Matrix[model.Bones.Count]` becomes a `std::vector<Matrix>` sized from
  `getBonesProperty().getCountProperty()`, passed to `CopyAbsoluteBoneTransformsTo` by reference.
- `effect.Parameters["name"].SetValue(v)` returns a pointer in CNA, so the calls chain through `->`.
- The instruction string's C# verbatim literal (`@"..."` plus `"\n..."`) becomes two adjacent C++
  string literals; the text and the `\n` are unchanged.
