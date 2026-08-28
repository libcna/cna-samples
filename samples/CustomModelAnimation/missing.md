# CustomModelAnimation — port notes

Upstream: `CustomModelAnimation_4_0` (SAMPLE-051). **Ported from nothing** — this directory held a
write-up and the sample's `.htm` and no code at all. Ported whole: both content-pipeline
processors' output, all three runtime data types, all four animation players, the rigid and the
skinned model, the HUD and every control.

Artifact root: `/rv/tmp/samples/SAMPLE-051-CustomModelAnimation_4_0/`.

## What the 2026-07-11 note said, and what is left of it

That note declared the sample **blocked twice over** by DEFERRED item #13: no
`AnimationClip`/`AnimationPlayer`/`Keyframe`/`SkinningData` equivalent anywhere in CNA, no
per-vertex bone weights in `.model.json`, and no per-mesh parent bone for the rigid half. All three
were true of the `.model.json` era and none of them is a gap now:

| The old note said | Measured now |
|---|---|
| No `AnimationClip`/`AnimationPlayer` equivalent exists in CNA | Correct, and none is needed. Those classes are **the sample's own**, not framework types — `ModelAnimationClip`, `ModelKeyframe` and the four players are 400 lines of game code, ported here like any other game code. |
| `.model.json` carries no per-vertex bone weights | Irrelevant: the real `.xnb` does, and `SkinnedEffect` consumes them. |
| No per-mesh parent bone for the rigid half | Fixed long ago and re-measured by SAMPLE-050: the pipeline's model carries the real nested bone hierarchy. |
| The sample's processors "would still need re-authoring" | No. They are **content-build** code and they run, unmodified, in the official pipeline under Wine. |

Three listed assets produce **8 `.xnb`**: `font.spritefont`, `AnimatedCube.fbx` through the sample's
`AnimatedModelProcessor` and `DudeWalk.fbx` through its `SkinnedModelProcessor`, plus five textures
the two FBX materials pull in without being content-project rows. Every project declares
`<XnaProfile>HiDef</XnaProfile>`, and all three profile legs build.

The `.spritefont` asks for **Kootenay**, an XNA Game Studio font that was not in the Wine prefix.
It ships with the same Game Studio install this campaign already takes its reference assemblies
from; `scripts/build-original.sh` installs and *registers* it, because Wine's GDI does not
enumerate a font that is only dropped into the Fonts directory — the same two-step SAMPLE-029
established for Segoe UI.

## Two framework gaps, both found by decoding the real file

The `.xnb`'s type-reader table was read out of the built `AnimatedCube.xnb` and `DudeWalk.xnb`
(`scripts/dump-xnb-readers.py`) before a line of the port was written.

**1. A reflectively-serialized class could not be read from inside a collection**
(`cnanext 2ce51f673`). `ReflectiveTypeReaderBuilder` existed — SAMPLE-044 put it there — but it
could only register the **value** shape, which is right for an `.xnb`'s root asset and wrong
everywhere else. XNA writes a reference type with its own 1-based reader index in front, and
`ListReader<T>`, `DictionaryReader<TKey,TValue>` and `ModelReader::ReadTag` all decide between the
inline and the dispatched form by whether `T` is `shared_ptr`-shaped; the value-shaped reader reads
the payload one index short and desynchronises everything after it. `RegisterShared()` and
`RegisterShared<TStored>()` are the fix. Two closed generics went with it, neither ever registered
for any element type: **`ListReader<Matrix>`** and **`ListReader<int>`** — a skinned model's bind
pose, inverse bind pose and skeleton hierarchy — the same gap SAMPLE-048 found for
`ArrayReader<Vector3>`.

**2. EasyGL refused a `Vector4` BLENDINDICES** (`cnanext ee093a53b`, `plans/plan_fx.md` FX-127):

> EasyGL: this VertexDeclaration cannot be bound to the stock 'skinned3d_vertexlit' program --
> element 1 declares BlendIndices0@12 Vector4 but shader input 'aBoneIndices' expects Byte4.

XNA's `VertexElementFormat` describes the **bytes**; the shader register a BLENDINDICES semantic
arrives in is a float4 either way, so a processor may write either — and this sample's own
`SkinnedModelProcessor` deliberately writes `Vector4`
(`ConvertChannelContent<Vector4>("BlendIndices0")`). Real XNA renders it. One shader attribute
cannot be both an integer and a float, so the fix is one read mode on every profile: the skinned
programs declare `in vec4 aBoneIndices` and cast to `int()`, `Byte4` binds as unsigned bytes read
as floats, `Vector4` binds as floats. The CNAEXT layer keeps its own copies of that shader in
`ShadowMap` and `DepthNormalPrepass`; `ShadowVisibilityTest.ASkinnedMeshShadowsItself` is what
caught them, passing on the pristine tree, failing with the renderer change alone, and passing
again once they were updated.

**The refusal is what the browser gate is calibrated against.** With FX-127 reverted the sample
does not draw a wrong picture, it aborts — `evidence/sabotage/no-fx127/run.log` is that run.

## Agreement with real XNA 4.0 — the data first

Both engines were compared on the **data the processors baked**, not only on the picture. `CNA_DUMP`
prints every value read out of the two `Tag`s in both engines: bone and mesh counts, both clip
dictionaries, each clip's duration in ticks, every keyframe's bone index, time and transform, and
the three skeleton arrays — 5388 lines a side.

**Every value is bit-identical as `float32`, 0 ULP.** 1178 lines match as text and 4210 differ only
in how the two runtimes spell the same float (C#'s `"R"` against C's `%.9g`), which is why the
comparison parses both back to `float32` rather than diffing text — SAMPLE-049's lesson, and here
it would have reported 3060 differences that do not exist.

Structurally: `AnimatedCube` has 6 bones and 5 meshes, one root clip and one model clip of
2.833 s (171 and 684 keyframes), and an empty bind pose — a rigid model needs none; `DudeWalk` has
60 bones in one mesh, one model clip of 1.267 s with 4400 keyframes, **no** root clip, and a
58-entry bind pose, inverse bind pose and skeleton hierarchy. Both sides agree on all of it.

## Agreement with real XNA 4.0 — then the pixels

Both clips play from the moment a key is pressed, so `../../../cna-diag/` and `../../../xna4-diag/`
add one hook: `CNA_ANIM_TIME=<seconds>` starts both clips and advances every player by exactly one
synthetic step of that length — the identical arithmetic on both sides — then leaves them alone.

| pinned instant | within 0 | within 8 | within 32 | after 4 px blur |
|---|---|---|---|---|
| 0.35 s | 98.20 % | **99.94 %** | 99.98 % | **100.00 %** |
| 0.70 s | 97.60 % | **99.96 %** | 99.99 % | **100.00 %** |
| 1.10 s | 97.94 % | **99.94 %** | 99.98 % | **100.00 %** |

Coverage matches to the third decimal in every leg and the centroids to two decimals, while the
centroid moves 354 → 385 → 408 across the legs, so the hook is doing something. Within each leg both
engines' captures are byte-identical two seconds apart, so the clock really is pinned.

## Web

`WEBGL2` built under a real Emscripten toolchain and driven in real Google Chrome. Three counts,
each calibrated against the native frames and each separating a thing that can fail alone: the HUD
text (white), the rigid cube (achromatic checker) and the skinned Dude (warm-toned):

| frame | native cube / dude | Chrome cube / dude |
|---|---|---|
| start | 0 / 0 | 0 / 0 |
| after **A** | 3721 / 0 | 3834 / 0 |
| after **B** | 13454 / 3262 | 11786 / **2897** |
| after both clips end | 0 / 0 | 0 / 0 |

- **`dude > 800` is the gate this sample is pinned by.** The Dude cannot be drawn at all unless the
  reflectively-serialized `ModelData` came off `Model.Tag` *and* the renderer accepted its `Vector4`
  BLENDINDICES, so both framework fixes are load-bearing for that one number;
- A draws the rigid model and **only** the rigid model, which is what `dude === 0` in that frame
  says; B then draws the skinned one;
- both clips stop themselves and the models disappear, exactly as they do natively;
- Escape reaches the game and `Exit()` stops it;
- no runtime exceptions, no HTTP errors, no fatal console messages, `WEBGL2` renderer logged,
  document title `CustomModelAnimationSample`.

A key press had to be **held**: `IsNewKeyPress` needs the key down in one polled frame and up in the
previous one, and a 12 ms `xdotool key` can fall entirely between two polls. The first capture of
this sample recorded A doing nothing while B worked, which is exactly that race.

## Deviations from the original source

- the three `Tag` types derive `System::Object` and are held by `std::shared_ptr`, because
  `Model.Tag` is a `System::Object*` and the `.xnb` dispatches to the other two as reference types;
- each has a **public** default constructor where the C# marks it private "for use by the XNB
  deserializer": CNA's reflective reader constructs the object with `std::make_shared`, from
  outside the class;
- `model.Tag as ModelData` becomes a `dynamic_cast`, a checked cast that yields null on a mismatch;
- `foreach (SkinnedEffect effect in mesh.Effects)` becomes a `dynamic_cast` per element that throws
  `System::InvalidCastException`, which is what the C# cast-per-element loop does;
- the players derive `System::Object` so `Completed(this, EventArgs.Empty)` can pass the sender the
  C# passes; the event itself is `System::EventHandler<System::EventArgs>`, this project's mapping
  of the non-generic `EventHandler` delegate;
- `SkinnedAnimationPlayer` copies the three skeleton lists instead of holding references to the
  `ModelData`'s;
- `static void Main(string[])` becomes `int main()` in the same place;
- one added line, the reader registration — see [`diff.md`](diff.md).

## Evidence

- `evidence/build-original.log` — the official pipeline: the game library, the two processors, the
  content, all three profile legs.
- `evidence/dump/{xna,cna}/run.log` and `evidence/dump/compare.txt` — the 5388-value comparison.
- `evidence/frozen/<t>/{xna,cna}/` — one directory per pinned instant.
- `evidence/sabotage/no-fx127/run.log` — the refusal the web gate is calibrated against.
- `evidence/cna-web-webgl2/browser-result.json` — the browser gate's own numbers.
