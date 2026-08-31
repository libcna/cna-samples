# Differences from the XNA 4.0 original

There are no known active behavioral differences. Shot velocity, angle calculation and ballistic
behavior follow the exercise source.

The unavoidable representation changes are:

- The CLR reflection used to restore screens is replaced by an AOT registry with the same type
  names and concrete constructors.
- Concrete object types supply CNA's `CNAEXT GetTypeName()` metadata.
- Native `Program.cpp`/`Properties/AssemblyInfo.cpp` represent the C# executable entry point and
  assembly title.

Native test automation injects touch below CNA from an artifact-only shim. The shipped game gains
no alternate input branch.
