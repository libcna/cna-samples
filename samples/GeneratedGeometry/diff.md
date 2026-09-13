# GeneratedGeometry — C++ language adaptation

## The reflective Sky reader is registered explicitly

This is not a behavioral deviation or an owner-added feature. The original XNA runtime finds the
closed `ReflectiveReader<GeneratedGeometry.Sky>` through .NET assembly reflection. C++ has no
assembly reflection, so the port performs one explicitly marked registration in the game
constructor:

```cpp
CNAEXT ContentReaders::Register();
```

The registered reader answers to the exact runtime identity stored in `sky.xnb`, then reads the
processor-generated `Model` and `Texture` fields in their original serialized order. It does not
generate geometry at runtime, change the asset, or bypass `Content.Load<Sky>("sky")`.

The checked-in `sky.xnb` is byte-identical to the output of the unchanged Microsoft XNA 4.0
`SkyProcessor`; `missing.md` records the fresh build and hash evidence.

## Everything else

There is no other behavioral difference from the original. C++ value ownership, `std::optional`,
RAII process lifetime, property-call syntax and `std::cos`/`std::sin` are lossless language-level
representations of the same source behavior.
