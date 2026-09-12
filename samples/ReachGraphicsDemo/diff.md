# ReachGraphicsDemo — C++ language/build adaptations

## Sample-owned content readers are registered explicitly

This is not a behavioral deviation and not an owner-added feature. The original XNA runtime finds
the sample's `SkinnedModel.*` and `GeneratedGeometry.Sky` readers through .NET reflection. C++ has
no assembly reflection, so the port performs one explicitly marked registration in `DemoGame`'s
constructor:

```cpp
CNAEXT ContentReaders::Register();
```

`ContentReaders.hpp` registers the exact runtime identities recorded by the official XNBs and
reads the same fields in their serialized order. It neither converts nor substitutes the assets.

## The executable entry point is unconditional

The original `Program.cs` compiles its entry point only for `WINDOWS || XBOX`; the Windows Phone
project supplies its entry point through the XNA project template. CNA's native and browser
products are ordinary CMake executables and therefore both compile the same `main`. This changes
only how the process starts. The original `WINDOWS_PHONE` constructor branch remains intact and is
covered by a compile-only build.

There are no other intentional differences from the original sample.
