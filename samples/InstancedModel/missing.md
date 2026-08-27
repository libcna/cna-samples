# InstancedModel — port notes

Upstream: `InstancedModelSample_4_0` (SAMPLE-040). Ported whole — all three instancing techniques,
the custom two-technique compiled effect, the spiralling instance motion, the overlay and every key
binding. Nothing is missing, stubbed or simplified.

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

Unlike SAMPLE-039, this sample's processor **is** deterministic: the content is byte-identical
across pipeline runs, so the port and the original can be rebuilt independently without diverging.

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
