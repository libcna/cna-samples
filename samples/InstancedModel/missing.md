# InstancedModel — port notes

## Sequential requalification — 2026-09-20

The exact snapshot at
`/rv/tmp/samples/SAMPLE-040-InstancedModelSample_4_0/xna4-original/`
still matches the physical `InstancedModelSample_4_0` upstream directory.
The unchanged Windows/HiDef game and its `InstancedModelProcessor` rebuilt
with the official XNA 4.0 tools; Windows/HiDef and Xbox/HiDef content builds
passed. The original executable ran on an isolated Xvfb display through
WineD3D (`WINEDLLOVERRIDES=d3d9=b`), rendered the animated 1000-model spiral,
responded to A/X/Y, and closed on Escape. The prior completed status missed
project metadata: both `.csproj` files select HiDef, while CNA's newer general
profile mechanism defaulted this port to Reach. The port now declares HiDef
through `ProjectGraphicsProfileEXT` in `src/Properties/AssemblyInfo.cpp`;
[`diff.md`](diff.md) records why this belongs beside assembly metadata, not in
game logic. No renderer, shader, content or game-logic workaround was added.

The three non-effect XNBs (`Cats`, `Font`, `cat_0`) rebuilt byte-for-byte.
The official `InstancedModel.xnb` rebuilt to the same 8612-byte length but
differs from the checked-in official XNB in 18 bytes before offset 977, inside
effect-parameter metadata. Every byte from offset 977 onward, including the
compiled shader payload, is identical. Thus the earlier statement that all
four files are always byte-identical was too strong. The original executable,
diagnostic original, native work product and retained native product were all
given the same checked-in four XNBs before comparison; the freshly produced
effect remains in `xna4-build/Content-hidef/` as evidence. The original build
helper was made hardlink-safe after the previous prune had made its output and
executable copies share inodes. The diagnostic comparison uses separate source
copies under this artifact root; no `CNA_SEED`/`CNA_TIME`/`CNA_FPS`/
`CNA_TECHNIQUE` hook is present in the shipped port.

Fresh Release OPENGLES3 and non-threaded Release WEBGL2 builds used the active
`cna` and `sharp-runtime` sibling checkouts, at most four compilation jobs and
the shared ccache (`CCACHE_DIR=/home/robertvokac/.cache/ccache`,
`CCACHE_BASEDIR=/rv`). The live native work and independently tested stripped
retained product render the spiral, show the overlay, accept A/X/Y and exit on
Escape with no fatal log. Five new frozen full-frame, 800×480 comparisons
using the same seed, animation time, FPS text, technique and four XNBs in
both engines give:

| Technique and time | Pixels within eight RGB levels | RGB MAE / 255 |
|---|---:|---:|
| Hardware instancing, 2 s | 383970/384000 (99.9922%) | 0.005625 |
| Hardware instancing, 8 s | 383878/384000 (99.9682%) | 0.037021 |
| Hardware instancing, 20 s | 383857/384000 (99.9628%) | 0.050417 |
| No instancing, 2 s | 383970/384000 (99.9922%) | 0.005625 |
| No state batching, 2 s | 383970/384000 (99.9922%) | 0.005624 |

The fresh, retained and exact local-gallery WEBGL2 bundles all passed the
real system-Chrome gate: 800×480 WebGL2 canvas, visible moving models, all
three drawing techniques and wraparound, A/X interaction, four HTTP 200
assets, and no runtime rejection, exception, HTTP error or fatal console
message. The `.wasm` has no `debug_info` section and the `.js` has no pthread
markers, so the bundle is suitable for static hosting. The gallery now has
39 cards across 12/12/12/3 pages, with this sample's detail page, real-canvas
screenshot and thumbnail. Original `Documentation/` figures, `Game.ico`,
`InstancedModelSample.png`, Microsoft Permissive License and four
`SourceContent/` spreadsheets were restored byte-for-byte outside runtime
`Content/`; the existing `InstancedModel.htm` was retained.

Current evidence is under
`/rv/tmp/samples/SAMPLE-040-InstancedModelSample_4_0/evidence/requal-20260920/`:
`comparison.txt`, frozen XNA/CNA captures, native input captures and three
Chrome runs. The reproducible build trees were
`work-native-opengles3-20260920/`, `work-native-diag-20260920/` and
`work-web-webgl2-20260920/`; retained products remain under
`cna-native-opengles3/samples/InstancedModel/` and
`cna-web-webgl2/samples/InstancedModel/`. The diagnostic source copy is
`cna-diag-source-20260920/`; its only game-code differences are the four
comparison hooks. `scripts/build-original.sh`, `build-original-diag.sh`,
`compare-frozen.sh` and `compare-pixels.py` reproduce the original and
comparison gates. No new CNA or sharp-runtime source change was needed.
The owner-requested push sent sample commit `84e1dd5` to `origin/develop` and
gallery commit `7836103` to `origin/main`. The authorized scoped prune removed
those three reproducible work trees plus XNA `obj/` and `pipeline-runner/`;
stripping and deduplication reduced the artifact root from 270.4 MB to
27.4 MB (243.0 MB freed). The exact source snapshot, original and diagnostic
executables, native and web products, scripts and comparison evidence remain.
All four pinned XNBs still match across the original and retained native
content; all four gallery web bundle files still match the retained product.
A second dry run found no removable path. Public Pages deployment was not
independently verified.

## Earlier port assessment — historical evidence

Upstream: `InstancedModelSample_4_0` (SAMPLE-040). Ported whole — all three instancing techniques,
the custom two-technique compiled effect, the spiralling instance motion, the overlay and every key
binding. Runtime code was complete; the supplementary upstream files were restored during the
2026-09-20 requalification above.

The placeholder that stood here claimed the sample was blocked on its custom `InstancedModel.fx`.
That is stale: compiled custom effects have worked since SAMPLE-032, and this sample renders
correctly on the first run. One genuine framework gap was found, and it was not the shader.

## Content

Three assets, one of them through the sample's own pipeline extension:

| Asset | Importer / Processor |
|---|---|
| `Cats.fbx` | `FbxImporter` → `InstancedModelProcessor` (repoints the material at `InstancedModel.fx`) |
| `Font.spritefont` | `FontDescriptionImporter` → `FontDescriptionProcessor` |
| `InstancedModel.fx` | `EffectImporter` → `EffectProcessor` |

**HiDef, and measured rather than assumed.** The `HardwareInstancing` technique compiles
`vs_3_0`/`ps_3_0`, so the Reach leg of the content build refuses it outright:

```
XNA Framework Reach profile does not support vertex shader model 3.0.
```

As in SAMPLE-039, the profile is enforced a second time at load, so the executable's embedded
`Microsoft.Xna.Framework.RuntimeProfile` resource says `Windows.v4.0.HiDef`.

Unlike SAMPLE-039's random vegetation layout, this processor does not change the model,
font or texture output across rebuilds. The effect XNB's 18 variable metadata bytes,
found in the later requalification above, prevent a four-file byte-identity claim.

## Framework gap found and fixed in `cnanext`: `DynamicVertexBuffer.SetData<T>` with options

XNA's `DynamicVertexBuffer` has a generic
`SetData<T>(T[] data, int startIndex, int elementCount, SetDataOptions options)`. CNA carried it
for four built-in vertex types only — `VertexPositionColor`, `VertexPositionColorTexture`,
`VertexPositionNormalTexture`, `VertexPositionTexture` — each of which packs the C++ object into a
compact GPU stream first. A game supplying **its own** element type had no way to upload one with
streaming semantics at all, and this sample's per-instance stream is exactly that: an array of
plain `Matrix` values.

The addition is the generic overload plus the raw path it needs
(`VertexBuffer::SetDataRawWithOptions`). An application-defined type has nothing to pack, so the
bytes go up as they sit in memory and the buffer's `VertexDeclaration` must describe exactly
`sizeof(TVertex)` — which the existing raw-upload validation already enforces.

`modules/graphics/tests/.../DynamicVertexBufferGenericSetDataTests.cpp` pins three things: the
round trip, the stride refusal, and that **`startIndex` selects where reading from the source
begins** while the destination write starts at the buffer's beginning. The last one is the
behavioural half — an implementation that forgets to advance the source pointer still compiles and
still uploads the right number of bytes, just the wrong ones — and it was confirmed to fail with
that pointer advance removed.

Everything else the sample needs was already there and correct: `VertexBufferBinding` with an
instance frequency, `GraphicsDevice.SetVertexBuffers`, `DrawInstancedPrimitives`, and — the part
that matters most here — EasyGL binding a compiled effect's `BLENDWEIGHT0..3` attributes to the
second stream with a divisor of 1 (`plans/plan_fx.md` FX-082).

## Comparison against the original

Nothing this sample draws is reproducible on its own: the instances come from an **unseeded**
`System.Random`, their motion is driven by `TotalGameTime` with `IsFixedTimeStep = false`, and the
overlay prints a measured frame rate. `CNA_SEED`, `CNA_TIME`, `CNA_FPS` and `CNA_TECHNIQUE` pin all
four in both engines (`scripts/compare-frozen.sh`, `cna-diag/README.md`).

That `CNA_SEED` works at all is a result in itself: sharp-runtime's `System::Random` is a
byte-for-byte port of .NET's Knuth subtractive generator, so one seed places all 1000 instances
identically in both engines. A different generator would scatter them elsewhere and no comparison
would be possible.

**Across three animation phases** (`evidence/frozen/`):

| Animation time | Within 8 levels | Model coverage XNA / CNA | Centroid offset | Differing pixels on an edge |
|---|---|---|---|---|
| 2 s | **99.26 %** | 13 003 / 13 012 | 0.30 px, 0.37 px | 99.6 % |
| 8 s | **95.24 %** | 73 326 / 73 445 | 0.50 px, 0.31 px | 99.6 % |
| 20 s | **93.46 %** | 99 543 / 99 623 | 0.41 px, 0.19 px | 99.6 % |

The falling percentage is **not** drift. Coverage tracks to within 0.16 % at every phase, the
centroid of everything drawn stays under half a pixel apart, and the share of differing pixels
lying on an edge is pinned at 99.6 % throughout — what changes is how much of the frame *is* an
edge, rising from 6.0 % to 28.8 % as the spiral spreads the models across the screen. Mean absolute
difference at 2 s is 0.32/255 with a median of 0 and no signed bias in any channel.

**All three techniques agree, in both engines, to the same figure.** Captured with
`CNA_TECHNIQUE`, XNA against CNA is 99.26 % / 99.26 % / 99.27 % for HardwareInstancing,
NoInstancing and NoInstancingOrStateBatching. Within each engine, technique 0 against 1 is 99.80 %
and 1 against 2 is 99.85 % — **identical on both sides to two decimal places**. CNA reproduces even
the small difference the hardware path has against the others, which comes from the shader's
`mul(World, transpose(instanceTransform))` versus the CPU-side `World * instance` of the other two.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the model
renders against the clear, the instances **move with no input at all**, `A` changes the frame, `X`
draws more, and — the sample's own subject — **every one of the three techniques draws the model**:
walking all three and wrapping back to the first keeps model coverage inside 0.6×–1.7× of the
first frame's. A technique that silently drew nothing, which is how a capability-shaped renderer
fails, would collapse that to the overlay alone. Coverage across the six captured frames is
83 226–103 941 pixels; both engines' native frames score ~21 % coverage on the same measurement.

## Deviations

None in behavior. Four C++ shapes worth naming:

- `static Random random = new Random()` becomes a function-local static inside a helper, which
  keeps the C# field's single shared instance without a static-initialization-order dependency.
- `Array.Resize(ref instanceTransforms, instances.Count)` becomes `std::vector::resize`, and the
  `List<SpinningInstance>` becomes `std::vector<SpinningInstance>` with `emplace_back`/`pop_back`.
- `instancingTechnique++` on a C# enum becomes an explicit cast through `int`, since C++ scoped
  enums have no increment.
- `GraphicsDevice.SetVertexBuffers(a, b)` takes a `std::vector<VertexBufferBinding>` in CNA rather
  than a `params` array, so the two bindings are brace-initialized at the call.
