# CustomModelAnimation — the one line the original does not have

Everything in `src/` is a direct translation of the three upstream projects except a single call
in the game's constructor:

```cpp
CNAEXT ModelContentReaderRegistrationEXT::RegisterEXT();
```

## Why it exists

`AnimatedModelProcessor` and `SkinnedModelProcessor` attach a `ModelData` to `Model.Tag`, and none
of `ModelData`, `ModelAnimationClip` or `ModelKeyframe` has a `ContentTypeWriter` of its own. XNA's
content pipeline therefore compiles them through its implicit `ReflectiveReader<T>`, which walks
each type's fields with .NET reflection at load time. The `.xnb` names those three types and the
two closed generics over them, and nothing else:

```text
[3] ReflectiveReader`1[[CustomModelAnimation.ModelData, CustomModelAnimationWindows, ...]]
[4] DictionaryReader`2[[System.String, ...],[CustomModelAnimation.ModelAnimationClip, ...]]
[5] ReflectiveReader`1[[CustomModelAnimation.ModelAnimationClip, ...]]
[7] ListReader`1[[CustomModelAnimation.ModelKeyframe, ...]]
[8] ReflectiveReader`1[[CustomModelAnimation.ModelKeyframe, ...]]
```

C++ has no reflection. `ReflectiveTypeReaderBuilder<T>` is CNA's answer: **the game supplies the one
thing reflection provided — its own field list — and CNA builds the reader.** That is what
`ModelDataContentReaders.cpp` does, once, in wire order. The two closed generics over the game's own
types are registered beside them, because only the game has the C++ types to instantiate them with;
`ListReader<Matrix>` and `ListReader<int>` are framework combinations and were added to CNA itself.

## Why it is not a workaround

The field lists are the same lists the C# declares, in the same order, and every value keeps the
type its own reader produced. `evidence/dump/compare.txt` is the check: with `CNA_DUMP=1` both
engines print all 5388 values they read out of the two `Tag`s — bone counts, clip names, durations
in ticks, every keyframe's bone index, time and transform, and the three skeleton arrays — and
**every one is bit-identical as `float32`, 0 ULP.**

Nothing else in the port deviates. `Model.Tag` reaches the game as a `System::Object*`, so
`model.Tag as ModelData` becomes a `dynamic_cast`, which is what `as` does.
