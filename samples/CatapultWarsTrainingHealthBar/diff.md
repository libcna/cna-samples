# Differences from the XNA 4.0 original

There are no known active behavioral differences. The exact exercise-specific source structure,
content and gameplay rules are preserved.

The following host/language adaptations do not change game behavior:

- C# restores screens with `Type.GetType` and `Activator.CreateInstance`; C++ registers the same
  fully-qualified screen names with explicit AOT factories.
- Concrete `System::Object` classes expose CNA's `CNAEXT GetTypeName()` metadata for those factories.
- `Program.cpp` supplies native `main`, and `Properties/AssemblyInfo.cpp` retains the assembly title.

The native qualification shim is external evidence only. It converts host mouse events to SDL
touch events below CNA; no mouse or keyboard gameplay branch was added to the sample.
