# Differences from the XNA 4.0 original

There are no known active behavioral differences. World coordinates, gesture handling, camera
clamps, inertia/zoom and projectile following retain the advanced sample's algorithms.

The representation-only differences are:

- CLR screen restoration is replaced by an explicit AOT registry under the same fully-qualified
  type names.
- Concrete object classes provide CNA's `CNAEXT GetTypeName()` metadata.
- The catapult's C# reference to its owning `GameplayScreen` is a non-owning C++ pointer with the
  same lifetime and call relationship.
- `Program.cpp` and `Properties/AssemblyInfo.cpp` supply the native entry point and title.

The native qualification harness is outside the repository product. It generates SDL touch events
for repeatability and does not add an alternate input path to the sample.
