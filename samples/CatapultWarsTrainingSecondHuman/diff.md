# Differences from the XNA 4.0 original

There are no known active behavioral differences. Both sides use the original `Human` logic and
the port retains the exact two-player turn flow.

Only these language/host representations differ:

- CLR `Type.GetType`/`Activator.CreateInstance` screen restoration is represented by explicit AOT
  factories registered under the same fully-qualified names.
- Concrete object types provide CNA's `CNAEXT GetTypeName()` metadata.
- `Program.cpp` and `Properties/AssemblyInfo.cpp` replace the CLR entry point and assembly title.

The external native qualification shim feeds SDL touch events and is not sample code. No alternate
mouse or keyboard gameplay path was introduced.
