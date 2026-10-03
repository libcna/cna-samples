# Owner-approved differences and C++ translation mechanics

The owner resumed the series 085 → 086 → 101 → 094 on 2026-10-03, accepting CNA's standard
original-art avatars and preset motions instead of Xbox assets. The game retains Celebrate,
Wave, its complete 71-bone list, the right-arm subtree overwrite, all three playback modes,
Celebrate's expression even in Wave-only mode, camera, text and original GamePad input paths.
Art and preset timings differ as documented in `../cna/docs/avatars.md`; Xbox pixel parity is
not claimed. The old cancellation is superseded by that explicit instruction.

A single `CNAEXT GamePad::setKeyboardEmulationEnabledEXT(true)` enables the shared off-by-default
keyboard source with the owner's approved layout. Q is LB (mode cycle), E is RB (new avatar),
arrows are the right stick, Z/C triggers, Right Shift the stick click and Escape Back. Physical
input still uses the standard API. No per-game keyboard branch or rendering workaround is added.

The unchanged Font asset/importer/processor/identifier is rebuilt for Windows/HiDef through the
official XNA 4.0 pipeline; the Xbox original remains retained. C++ uses smart-pointer ownership,
optional non-default-constructible resources and System List objects. EnsureCapacity expresses
the original List's capacity-only initialization; ToVector is the lossless standard Draw adapter.
ReadOnlyCollection's explicit getItem expresses the original read-only index getter in C++.
The original game/source-unit/type names and entry point are retained. Required runtime type name,
project HiDef and observable assembly title are declared through the existing CNA metadata idiom.
