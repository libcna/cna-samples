# Differences from the XNA 4.0 original

There are no known active behavioral differences after the `SAMPLE-067` audit. The sample uses
the exact official XNA content products and follows the complete EX2 endpoint line by line.

Three representation adaptations are required by C++ and CNA's host model:

- C# `Type.GetType` plus `Activator.CreateInstance` restores screen types from isolated storage.
  C++ has no CLR reflection, so `ScreenManager` keeps an explicit registry for the same four
  concrete screen types. It stores the original fully-qualified names and creates the same types;
  serialized data and lifecycle behavior are unchanged.
- Concrete `System::Object`-derived classes expose CNA's `CNAEXT GetTypeName()` metadata. This is
  the established AOT counterpart used by the screen registry and does not add game behavior.
- `Program.cpp` supplies the native `main`, while `Properties/AssemblyInfo.cpp` retains the
  assembly title used for the game window. These replace CLR executable metadata only.

The desktop qualification harness is external evidence, not sample code. It feeds SDL touch
events so the native run exercises the original `TouchPanel` gesture path; the browser probe uses
real browser touch events. No mouse or keyboard gameplay branch was added to the game.
