# SAMPLE-102 intentional translation seams

The owner requested one desktop input addition. The following other seams are language/host
representation choices, not sample workarounds.

## Owner-requested mouse input — 2026-09-27

The shipped scenario #1 displays the direction guide and has no active touch action. The
preserved scenario #4 branch uses a Tap gesture to lock or unlock orientation if a reader enables
it in source. At the owner's request the constructor opts into CNA's off-by-default
`TouchPanel::setMouseTouchEmulationEnabledEXT(true)` extension, so a left mouse click can follow
that same Tap path. It does not enable scenario #4, change orientation rules, add a separate mouse
handler or replace actual touch input. The one source addition is marked `CNAEXT`.

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
