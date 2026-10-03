# Owner-approved differences and C++ translation mechanics

The owner authorized series 085 → 086 → 101 → 094 with standard CNA original-art avatars and
preset timings. The port keeps the bat FBX, four presets, 71-bone animation × bind × parent
composition, SpecialRight attachment and original offset, BasicEffect lighting, draw order,
camera and GamePad branches. CNA bodies/motions differ from Xbox ones; pixel parity is not claimed.
The original fingers keep animating and do not grip the bat, as Microsoft's documentation notes.

One shared off-by-default CNAEXT GamePad keyboard opt-in uses the approved layout: K/L/J/I for
A/B/X/Y (Celebrate/Clap/Stand5/Stand0), E new avatar, arrows orbit, Z/C zoom, Right Shift reset,
Escape Back. No sample-local input or renderer workaround. The general GS-009g avatar delta and
coordinate contract fixes are in CNA, preserving the original calculation and offset here.

The unchanged bat/importer/processor/identifier is rebuilt by official XNA Windows/HiDef pipeline.
C++ uses owned resources, optional Model/description, a borrowed current animation pointer,
System List EnsureCapacity for original capacity semantics and an iterator-range vector copy for lossless Draw interop.
The cast throws when an effect is not BasicEffect, matching the original foreach cast. Source-unit,
class/type and entry-point names are retained; original HiDef and title use existing CNA metadata.
