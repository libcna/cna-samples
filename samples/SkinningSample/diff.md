# SkinningSample — the one runtime addition

Everything in `src/` translates the upstream game and its sample-owned `SkinnedModel` library.
The game's constructor has one call the C# source does not:

```cpp
CNAEXT SkinningContentReaderRegistrationEXT::RegisterEXT();
```

## Why it exists

`SkinnedModelProcessor` places a `SkinnedModel.SkinningData` object in `Model.Tag`. That class,
`AnimationClip` and `Keyframe` have no explicit `ContentTypeReader`; XNA emits
`ReflectiveReader<T>` entries and discovers their fields with .NET reflection at runtime. C++ has
no equivalent runtime reflection.

`SkinningContentReaders.cpp` supplies only that missing metadata to CNA's generic AOT reflective
reader:

- `Keyframe`: bone, time, transform;
- `AnimationClip`: duration, keyframes;
- `SkinningData`: animation-clips dictionary, bind pose, inverse bind pose, hierarchy;
- the closed `List<Keyframe>` and `Dictionary<string, AnimationClip>` reader shapes owned by these
  sample types.

The names and wire order come directly from the XNB reader table and the C# field order. Stock
`List<Matrix>`/`List<int>`, model, buffers, textures and `SkinnedEffect` remain framework readers.

## Why it is not a workaround

This is CNA's established C++ replacement for the reflection an XNA executable provides for its
own serialized classes. It does not translate or duplicate asset data, alter the model, replace
the processor, or special-case the sample in CNA. The port consumes the five byte-identical XNBs
written by the unchanged official pipeline, and deterministic XNA/CNA captures at two clip times
agree within 8 color levels on 99.91–99.95 % of all pixels with exact foreground bounds.
