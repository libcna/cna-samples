# SpriteEffects — C++ language adaptation

## Runtime type identity is explicit

This is not a behavioral deviation or an owner-added feature. The .NET runtime can obtain
`SpriteEffects.SpriteEffectsGame` through reflection. C++ has no assembly reflection, so the
concrete `Game` class provides CNA's required, explicitly marked override:

```cpp
CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;
```

The returned name is the original fully qualified .NET type name. It does not alter content,
rendering, input or lifecycle behavior.

The original source's nested `Program.Main` is emitted as the CMake executable's `Program.cpp`.
Its stack object gives the same deterministic cleanup as C#'s `using` block. There are no other
intentional differences from the original sample.
