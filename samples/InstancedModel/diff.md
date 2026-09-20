# InstancedModel — project metadata translation

The original Windows and Xbox `.csproj` files both select
`<XnaProfile>HiDef</XnaProfile>`. XNA embeds that build setting in the game;
there is no corresponding line in `InstancedModelSampleGame.cs`. CNA's C++
project declares the same setting in `src/Properties/AssemblyInfo.cpp` through
the general `CNA::ProjectGraphicsProfileEXT` metadata mechanism. This is a
project-file translation, not a game-logic override or a rendering workaround.

The original `InstancedModel.fx` needs HiDef for its `vs_3_0` and `ps_3_0`
hardware-instancing technique. The three original technique branches and
their input mappings remain unchanged in the port.
