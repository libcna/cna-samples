# Differences from the XNA 4.0 original

There are no known active behavioral differences. Crate state, collision, ammo award and split
projectile behavior are translated from the exercise rather than recreated locally.

Necessary C++/host adaptations are:

- CLR screen-type reflection is represented by explicit AOT factories using the same serialized
  type names.
- Concrete object types expose CNA's `CNAEXT GetTypeName()` metadata.
- C++ `shared_ptr` instances preserve the reference identity and lifetime of C# projectile objects
  while they move through the active-projectile collection.
- `Program.cpp` and `Properties/AssemblyInfo.cpp` replace the CLR executable entry point and title.

The external native test shim feeds SDL touch and is not linked into or copied with the sample.
