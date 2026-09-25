# CPU Skinning — the one runtime addition

## Desktop and Phone entry-point mapping

The original game has a `Main` method only under `WINDOWS || XBOX`; the
Windows Phone project starts the `Game` through its manifest. The C++ port
places `main` in `Program.cpp` and excludes it under `WINDOWS_PHONE`.
The upstream `#if WINDOWS` mouse path is compiled for the port's two desktop
targets (native and browser) by `!WINDOWS_PHONE`; the original Phone
fullscreen/touch branch is preserved. This is the platform mapping needed
to keep the same controls on CNA's Linux and WebGL2 desktop targets, not an
extra control or a workaround.

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
