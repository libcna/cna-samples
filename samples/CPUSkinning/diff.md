# CPU Skinning — the one runtime addition

Everything under `src/` translates the upstream game and its sample-owned runtime data types.
The game constructor has one call the C# source does not:

```cpp
CNAEXT CpuSkinningContentReaderRegistrationEXT::RegisterEXT();
```

## Why it exists

The unchanged XNA pipeline records two explicit game-owned readers and four reflectively serialized
game-owned types in `dude_cpu.xnb`:

- `CpuSkinnedModelReader` reads the part list and `SkinningData`;
- `CpuSkinnedModelPartReader` reads triangle count, `CpuVertex[]`, the index buffer and a shared
  `BasicEffect`;
- `CpuVertex` stores position, normal, texture coordinate, four weights and four indices;
- `Keyframe`, `AnimationClip` and `SkinningData` retain the original serialized field order.

C++ has no .NET assembly discovery or general reflection. `CpuSkinningContentReaders.cpp` therefore
registers those exact canonical reader names, fields and closed collections through CNA's generic
AOT content-reader API. All stock scalar, vector, matrix, index-buffer, effect and texture readers
remain framework built-ins.

## Why it is not a workaround

The registration declares type metadata; it does not parse or substitute asset data, specialize
CNA for this sample, bypass `Content.Load<T>()`, change the processor output or replace CPU
skinning with a GPU path. The game consumes the seven byte-identical official Windows Reach XNBs.

The CPU mode still evaluates the upstream four-influence 4x3 matrix blend for every position and
normal, uploads the resulting `VertexPositionNormalTexture` array through a
`DynamicVertexBuffer` with `SetDataOptions::Discard`, and draws it using the shared `BasicEffect`.
At pinned animation times 0.5 and 0.9 seconds, its model pixels agree with the unchanged XNA CPU
path at 99.99% within eight color levels; CPU and GPU output inside CNA agree at 99.97–99.98%
within eight levels. Native OPENGLES3 and real-Chrome WEBGL2 runs both exercise the right-click
mode switch and left-button camera drag.
