# Differences from the XNA 4.0 original

There are no known active behavioral differences. The guide uses the original trajectory
prediction and official guide-dot asset.

The only differences are language/host mechanics:

- Screen persistence uses explicit AOT factories in place of CLR type reflection, with unchanged
  fully-qualified names and concrete screen set.
- Concrete object classes publish CNA's `CNAEXT GetTypeName()` metadata.
- Native `main` and assembly-title translation live in `Program.cpp` and
  `Properties/AssemblyInfo.cpp`.

The native automation shim is external to the product and emits SDL touch events; it does not add
sample input behavior.
