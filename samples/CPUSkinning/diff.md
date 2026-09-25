# CPU Skinning — differences from the original XNA sample

## Open FPS text difference — owner-directed deferral (2026-09-25)

The original `FrameRateCounter.cs` computes a `float` as `1000f / frameRate`
and passes it to `string.Format("fps: {0} ({1} ms)", ...)`. The C++ port
preserves that expression and format string. At 30 FPS, the original XNA 4.0
application prints `fps: 30 (33.33333 ms)`, while CNA currently prints
`fps: 30 (33.333332 ms)` in the native and browser builds. This is an active,
visible difference, not a sample-side change or a fixed-five-decimal rule.

An isolated .NET Framework 4 probe using the exact `float` bit pattern
`0x42055555` confirms that the original's default composite format is
`33.33333` (`G7`, seven significant digits). SharpRuntime's default
`Single::ToString` uses a shortest-round-trip conversion and yields
`33.333332`; `System::String::Format` calls that conversion for a `float`
with no explicit specifier. The probe and its output are retained under
`evidence/requal-20260925/format-probe/` in the artifact root.

The earlier frozen image comparison deliberately excluded the FPS text region,
so its 99.99% model/background result does not establish text parity. The owner
requested that this difference be recorded, SAMPLE-056 be pruned and pushed,
and SharpRuntime remain unchanged for now. No `G7` specifier or other
sample-side workaround has been added. A future correction belongs in a
general, opt-in .NET Framework 4 formatting mode, with separate regression
coverage for existing SharpRuntime clients.

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
