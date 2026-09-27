# SAMPLE-076 — Translation metadata

The selected original `SplitScreenSample (Windows).csproj` declares
`<XnaProfile>HiDef</XnaProfile>`. XNA embeds this project setting in the
executable; the C# game does not assign it in its constructor.

The C++ port declares the same metadata once through CNA's general
`ProjectGraphicsProfileEXT` in `src/Properties/AssemblyInfo.cpp` before the
game creates its `GraphicsDeviceManager`. This is a translation of the
original project setting, not a change to the game's logic or a workaround
for rendering. The original Phone project separately declares Reach; the
selected Windows product and Xbox project declare HiDef. See `missing.md`
for build and run evidence.
