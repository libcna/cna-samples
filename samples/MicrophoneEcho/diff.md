# MicrophoneEcho — project metadata and C++ mechanics

The port preserves the original game behavior without a sample workaround.

## Project graphics profile

The Windows and Xbox 360 projects declare `<XnaProfile>HiDef</XnaProfile>`; the Windows Phone
project declares `Reach`. XNA's build writes this into the assembly's
`Microsoft.Xna.Framework.RuntimeProfile` resource. The C++ assembly metadata uses CNA's existing
`ProjectGraphicsProfileEXT` declaration for the same setting, including the inactive
`WINDOWS_PHONE` branch. `AssemblyTitleAttributeEXT` similarly supplies the original window title.

The original `Game.ico` and `GameThumbnail.png` are retained unchanged at sample root.

## Language and API mechanics

The stored event token is removed during destruction or failed microphone initialization so that
CNA's cached microphone cannot call a destroyed C++ game. Negative sample indices throw the
original bounds exception explicitly; C# array access provides that check automatically.
`GetTypeName()` supplies the original managed type identity.

The current `EffectPassCollection` returns a pointer, so its `Apply()` call uses `->`.
The original 100 ms capture, 150 ms delay, 0.5 feedback, input and waveform code remain intact.

Current original/native/browser comparison and real audio results are recorded in
[`missing.md`](missing.md), under the 2026-09-28 completion section.
