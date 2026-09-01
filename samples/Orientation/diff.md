# SAMPLE-102 intentional translation seams

There are no known observable behavior or content differences from the original Windows Phone
sample. The following are language/host representation choices, not sample workarounds.

## Object ownership and nullable content

The C#-owned `GraphicsDeviceManager` becomes a game-owned C++ value. `SpriteBatch` uses
`std::unique_ptr`, while the two content values use `std::optional` so their construction remains in
`LoadContent()`. These choices express C++ lifetime only; construction order, content identifiers,
loading order and use are unchanged.

The original private fields, lifecycle overrides and branch order remain private/protected in C++.
The CNAEXT `GetTypeName()` override supplies the fully qualified managed identity
`OrientationSample.OrientationSample`; it does not add a game feature.

## Executable host

The shipping Windows Phone project is launched by the phone application host and preprocessor-
excludes the dormant desktop `Program.Main`. CNA needs a normal native/Wasm entry point, so
`Program.cpp` constructs the same logical `OrientationSample::OrientationSample` class and calls
`Run()`. Stack lifetime provides the disposal boundary that the C# `using` statement supplies. No
input, orientation state or rendering behavior is added.

`AssemblyInfo.cs` metadata with runtime significance is represented by CNA's assembly-title
attribute so the product title remains `OrientationSample`; the remaining CLR-only package metadata
has no game behavior.

## Excluded source

`LayoutSample.cs` is deliberately not translated because the authoritative phone `.csproj` does not
compile it and no shipped entry point references it. Treating it as a second target would expand the
original product rather than port it.
