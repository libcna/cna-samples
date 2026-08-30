# SkinnedModelExtensions — the one runtime addition

Everything in `src/` translates the upstream game and its sample-owned `SkinnedModel` and
`Primitives3D` sources. The game constructor has one call the C# source does not:

```cpp
CNAEXT SkinningContentReaderRegistrationEXT::RegisterEXT();
```

## Why it exists

The unchanged XNA pipeline serializes four sample-owned reference classes using .NET reflection:

- `Keyframe`: bone, time, transform;
- `AnimationClip`: duration, keyframes;
- `SkinningData`: animation clips, bind pose, inverse bind pose, hierarchy, bone indices;
- `SkinnedSphere`: bone name, radius, optional offset.

C++ has no equivalent run-time reflection. `SkinningContentReaders.cpp` declares that exact field
order once through CNA's generic AOT reflective reader and registers only the closed collection
shapes containing sample-owned types: `List<Keyframe>`, `Dictionary<string, AnimationClip>` and
`SkinnedSphere[]`.

The standard primitive-only `Dictionary<string, int>` is deliberately not registered here. Its
missing framework registration was repaired generally in `cnanext` commit `72262a33e`. Stock model,
buffer, texture, string, scalar, matrix, `BasicEffect` and `SkinnedEffect` readers remain CNA
built-ins.

## Why it is not a workaround

This is CNA's established C++ replacement for reflection over game-owned serialized classes. It
does not translate or duplicate asset data, special-case SAMPLE-055 in the framework, replace
`Content.Load<T>()`, alter the processor or change behavior. The port consumes all seven
byte-identical official XNA products. Deterministic native comparisons reach 99.92–99.95 % within
8 color levels with exact foreground bounds, while ordinary XNA, OPENGLES3 and WEBGL2 runs all
show the same animated collision spheres, bone controls and attached bat.
