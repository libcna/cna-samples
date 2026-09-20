# BillboardSample — project metadata translation

Both original XNA project files declare `<XnaProfile>HiDef</XnaProfile>`.
The unchanged C# game therefore never sets `graphics.GraphicsProfile` itself:
XNA embeds the project setting in the executable.

The C++ port declares that same setting in `src/Properties/AssemblyInfo.cpp`
through CNA's `ProjectGraphicsProfileEXT`. This translates project metadata,
not game logic, and does not bypass Reach's primitive-count limit. The original
processor emits a billboard mesh part larger than that limit, so the original
project and the port must both run as HiDef.
