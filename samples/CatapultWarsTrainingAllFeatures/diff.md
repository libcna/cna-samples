# Differences from the XNA 4.0 original

There are no known active behavioral differences. The product is the complete combined exercise,
using exact official content and the original mode/feature composition.

Required representation adaptations are:

- Explicit AOT screen factories replace `Type.GetType`/`Activator.CreateInstance` while retaining
  the same fully-qualified type names and serialized screen behavior.
- Concrete objects expose CNA's `CNAEXT GetTypeName()` metadata.
- C++ shared ownership preserves C# projectile identity and lifetime in active projectile lists.
- Default values on two boolean screen-factory constructor arguments allow the AOT registry to
  instantiate the same default screen state that CLR reflection constructs; normal call sites keep
  the original explicit arguments.
- `Program.cpp` and `Properties/AssemblyInfo.cpp` provide the native entry point and window title.

The artifact-only native shim injects SDL touch below CNA for automation. No mouse/keyboard game
logic or other alternate sample behavior was added.
