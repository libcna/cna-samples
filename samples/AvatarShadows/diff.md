# Owner-approved differences and C++ translation mechanics

The owner reopened SAMPLE-087 for completion on 2026-10-03 after accepting CNA's standard
original-art avatars and shared keyboard GamePad emulation for the avatar sample family.

CNA supplies its own avatar bodies, appearances and preset animations through the standard
`AvatarDescription`, `AvatarAnimation` and `AvatarRenderer` API. Their artwork and motion timing
differ from Microsoft's proprietary Xbox Avatar data. The sample retains all 16 independently
animated characters, random appearances and presets, world transforms, planar-shadow pass, camera,
light, text and original GamePad input behavior. Xbox pixel parity is not claimed.

A single `CNAEXT GamePad::setKeyboardEmulationEnabledEXT(true)` call enables CNA's shared,
off-by-default keyboard source for PlayerIndex.One. WASD maps to the left stick, arrows to the
right stick and Escape to Back, so the original camera, light and exit paths remain unchanged.
Physical controllers continue through the standard API; no per-game keyboard branch exists.

The original requests a full-screen `SurfaceFormat::Alpha8` render target. EasyGL does not expose
one-byte Alpha8 render-target storage, so CNA's general XNA-style preferred-format selection uses
`Color`. The original compiled effect reads only the target's alpha channel, preserving the visible
shadow-mask result while using four bytes per pixel. The sample contains no format override.

C# reference ownership is represented by smart pointers and nullable content by `optional`.
The original `List<Avatar>` becomes an owning vector, and the four ground vertices use a fixed
array. These are mechanical lifetime and collection choices without observable behavior changes.
