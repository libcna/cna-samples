# ShadowMapping — project metadata translation

Both original XNA project files declare `<XnaProfile>HiDef</XnaProfile>`. The
unchanged `ShadowMapping.cs` consequently has no assignment to
`graphics.GraphicsProfile`: XNA embeds the project setting in the executable.

The C++ port declares the same setting in `src/Properties/AssemblyInfo.cpp`
using CNA's `ProjectGraphicsProfileEXT`. That is a declarative counterpart to
the project metadata, not an alternative code path in the game or a renderer
workaround. If a C++ build acquires XNA's embedded-project-metadata mechanism,
this declaration can disappear; the game's translated methods need no change.
