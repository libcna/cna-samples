# Owner-approved differences and C++ translation mechanics

The owner authorized reopening SAMPLE-085 on 2026-10-03, confirmed by the request to resume
`handoff.md`. The former Xbox-only cancellation is superseded for this product.

The game uses CNA's standard `AvatarDescription`, `AvatarAnimation`, `AvatarRenderer` and
`IAvatarAnimation` API. CNA supplies its own original avatar bodies, appearances and preset clips,
as documented in `../cna/docs/avatars.md`; their artwork and clip timings differ from Microsoft's
Xbox avatars. This is the explicitly approved visual/animation boundary, not a claim of Xbox
pixel parity. The game's four preset choices, 250 ms blending algorithm, facial-expression
selection, camera, text and input transitions are unchanged.

The owner also requested keyboard-to-GamePad emulation and approved its key layout. A single
`CNAEXT GamePad::setKeyboardEmulationEnabledEXT(true)` call enables CNA's shared, off-by-default
software input for PlayerIndex.One. No per-game keyboard branch exists. Physical input continues
through the standard API; GamePad Back (Escape in the emulation) exits.

The Xbox-only Font content is rebuilt for Windows/HiDef by the official XNA 4.0 pipeline using the
unchanged `Font.spritefont`, importer, processor and asset identifier. No loose font atlas or
replacement asset is loaded. The original Xbox executable and content remain in the artifact root.

C++ owns reference objects with smart pointers and represents the 71-element bone array with a
shared vector so `ReadOnlyCollection` retains the original live view. Animation references remain
non-owning pointers to the game's four owned presets, preserving identity comparisons. CNA's
required type name and project HiDef/title metadata have their original values. The entry point
stays in `AvatarAnimationBlendingSample.cpp`, matching the original source unit.
