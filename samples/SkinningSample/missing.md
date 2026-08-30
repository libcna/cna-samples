# SkinningSample — port notes

Upstream: `SkinningSample_4_0` (SAMPLE-054). The canonical XNA skinned-model sample is now
ported whole: the sample-owned runtime library, its processed model, the animation player, camera,
input, lighting and all target-independent game behavior.

Artifact root: `/rv/tmp/samples/SAMPLE-054-SkinningSample_4_0/`.

## The old blocker was a category error

The previous note treated `AnimationClip`, `Keyframe`, `AnimationPlayer` and `SkinningData` as
missing CNA/XNA framework APIs. They are not framework APIs. They are the sample's own
`SkinnedModel` library, built beside the game and copied into other samples that use this
pipeline. They therefore belong in this port just as the game's `SkinningSampleGame` does.

The same note was also bound to the obsolete `.model.json` route. The real game does not load that
format: its unchanged `SkinnedModelProcessor` writes a normal XNA `Model` XNB whose vertices carry
`BLENDINDICES`/`BLENDWEIGHT`, whose meshes use `SkinnedEffect`, and whose `Model.Tag` carries the
sample-owned `SkinningData`. CNA's current XNB path supports all of those pieces.

No new CNA or sharp-runtime change was needed for SAMPLE-054. The generic reflective-reader and
closed-collection support repaired by SAMPLE-051 is the correct reusable seam.

## Original build and content

The artifact scripts compile the unchanged `SkinnedModel` library and unchanged
`SkinnedModelPipeline` processor with the official XNA 4.0 assemblies, run the real processor, and
link the unchanged Windows game. The Windows project declares Reach; Windows HiDef and Xbox Reach
were also built as additional pipeline checks.

One listed `dude.fbx` produces five runtime XNBs. The four textures are implicit FBX material
dependencies rather than content-project rows. The five XNB files checked into this port are
byte-identical to the official Windows Reach output:

| asset | SHA-256 |
|---|---|
| `dude.xnb` | `3a8b6533afc21069206f1f9b14c1a3641234d33e8315b217ab146bb8abcc991e` |
| `head_0.xnb` | `baeaf3ad2fa90fe5c721e2fa1e563248bd446fe8e6456df8a6f93b78b83d2d74` |
| `jacket_0.xnb` | `bdbcf3f4e3b45ed137f5214496dc4c23864f14fc01853f586858a45925363d20` |
| `pants_0.xnb` | `847b874455caee44e5b5d761755a176591b7c67f875482f7bed9b5223d2b1e92` |
| `upBodyC_0.xnb` | `d11e1c66db4d1b716547bb18169c021bed375e6f3556ee02dcef39fc3e9a5b52` |

The reader table was decoded before the port was written. `dude.xnb` names the stock model,
vertex/index/declaration and `SkinnedEffect` readers plus exactly the expected reflective graph:
`SkinningData`, `Dictionary<string, AnimationClip>`, `AnimationClip`, `List<Keyframe>`, `Keyframe`,
`List<Matrix>` and `List<int>`. The four other files use `Texture2DReader`.

## Runtime fidelity

The port retains the original algorithm rather than using CNA's unrelated CNAEXT animation
helper. `AnimationPlayer` consumes the single chronological keyframe list without interpolation,
restores the bind pose when the clock moves backward or loops, constructs world transforms through
the serialized parent hierarchy, then multiplies by inverse bind pose for the final skin matrices.
The game starts `Take 001`, advances it from `ElapsedGameTime`, feeds every `SkinnedEffect`, enables
default lighting, and preserves the original keyboard/gamepad camera and reset controls.

The ordinary native run renders the fully textured Dude and two captures two seconds apart have
different hashes and visibly different poses. For a stricter comparison, the audit-only
`CNA_TIME` hook sets both engines to the same absolute clip position. Within each leg, captures two
seconds apart are byte-identical, proving the clocks are pinned; the two legs themselves differ,
proving different keyframe ranges were exercised.

| pinned clip time | exact pixels | within 8 | within 16 | after 4 px blur | foreground coverage XNA / CNA |
|---|---:|---:|---:|---:|---:|
| 0.5 s | 94.43 % | **99.95 %** | 99.98 % | **100.00 %** | 10.449 % / 10.447 % |
| 0.9 s | 93.35 % | **99.91 %** | 99.98 % | **100.00 %** | 10.301 % / 10.301 % |

At 0.5 s the foreground bounds are exactly `329–497 x 48–479` in both engines and their centroids
differ by 0.02 px. At 0.9 s the bounds are exactly `327–481 x 56–479` and their centroids again
differ by only 0.02 px. This jointly exercises reflective `Model.Tag` loading, keyframe selection,
hierarchy multiplication, inverse bind pose, skinned vertex attributes, stock effect parameters,
textures and drawing.

## Web

The complete `WEBGL2` bundle builds under Emscripten and runs in the system Google Chrome. The
browser gate verifies an 800x480 WebGL 2 canvas, the `WEBGL2` renderer log, the original
`Skinning Sample` title, differing animated-frame hashes, Escape shutdown, successful HTTP loads of
all four bundle files, and no promise rejection, runtime exception, HTTP failure or fatal console
message. Its screenshots show the same textured animated character.

## Intentional C++ mappings

- The three reflectively serialized reference types derive `System::Object` and use
  `std::shared_ptr`; `Model.Tag as SkinningData` becomes the equivalent checked `dynamic_cast`.
- Their private parameterless C# constructors are public in C++ so the AOT reader can construct
  them. Their regular constructors, fields, ordering and behavior remain the same.
- C# array/list/dictionary storage maps to `std::vector`/`std::unordered_map`; `CopyTo` becomes a
  vector assignment and index arithmetic uses `std::size_t` where required by C++ containers.
- Null guards on non-null C++ references are omitted. Runtime-state and range failures retain
  `InvalidOperationException` and `ArgumentOutOfRangeException`.
- `foreach (SkinnedEffect effect in mesh.Effects)` uses a checked `dynamic_cast` and throws
  `InvalidCastException` on a mismatched effect, preserving the C# loop's cast behavior.
- `static void Main()` becomes `int main()`, and C# properties use CNA's getter/setter convention.
- The one AOT reader-registration call is documented in [`diff.md`](diff.md).

## Evidence

- `scripts/build-original.sh` and `evidence/xna-original/` — unchanged original build and live run.
- `scripts/dump-xnb-readers.py` — decoded runtime reader contract.
- `evidence/cna-native-opengles3/` — ordinary native animation run.
- `evidence/frozen/{t0.5,t0.9}/{xna,cna}/` — deterministic image pairs and logs.
- `evidence/cna-web-webgl2/browser-result.json` — real-Chrome behavior gate.

There is no remaining SAMPLE-054 blocker or sample-side workaround.
