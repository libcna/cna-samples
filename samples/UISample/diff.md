# SAMPLE-082 intentional translation seams

There are no known observable behavior or content differences from the selected Windows Phone XNA
4.0 product. The following are necessary C#-to-C++ representation changes rather than sample
workarounds.

## Reflection-free screen construction

The original persists each serializable screen's assembly-qualified type name, then restores it
with `Type.GetType(...)` and `Activator.CreateInstance(...)`. Sharp Runtime deliberately does not
provide general reflection. The port writes and reads the same names, including assembly, version,
culture and public-key fields, but resolves them through an explicit compile-time registry of the
five concrete screen types. Unknown names fail the restore, delete invalid state and fall back to
the normal initial screen stack, matching the original failure path.

This registry is marked `CNAEXT` and changes only the construction mechanism. It does not change
the serialized wire values, screen ownership or lifecycle.

## C++ representation

C# object references are represented by `shared_ptr`, `unique_ptr`, raw non-owning pointers or
`optional` according to their actual ownership and nullability. C# properties use the existing
sample-port conventions where a direct spelling is impractical. The build selects the original
`WINDOWS_PHONE` conditional surface on native and web targets so it retains touch and GamePad-Back
behavior rather than inventing desktop controls.

The external Xvfb SDL adapter retained in the evidence artifact is test infrastructure only. It
injects genuine SDL finger events below CNA and is absent from the repository target and shipped
sample.

## Original reference boundary

The upstream project is a Windows Phone game, not a Windows XNA executable. Its unchanged content
project and all 25 C# units compile in the offline Win7/XNA 4.0 environment; the bounded Windows
diagnostic cannot run past the original Phone-only application-isolated-storage call. Therefore
qualification proves source/content fidelity and native/browser behavior without claiming a
desktop screenshot comparison against a nonexistent equivalent XNA host.
