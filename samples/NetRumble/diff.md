# SAMPLE-062 NetRumble intentional differences

## Owner-approved platform boundary

The original ships Windows and Xbox 360 XNA 4.0 projects. CNA ports the complete game to native
OPENGLES3 and qualifies its System-Link create/find/join, lobby and packet-gameplay paths using two
independent processes.

On 2026-09-05 the project owner explicitly decided that SAMPLE-062 will not receive a web port.
CNA's browser target cannot advertise or discover System-Link sessions over raw UDP and cannot host
an inbound peer; implementing the unchanged browser flow would require a reusable broker and relay.
No WEBGL2 target, menu-only build, fake lobby or manual-address sample workaround is substituted.
This exception applies only to SAMPLE-062 and does not relax the browser gate for other samples.

## C++ language mechanics

Ownership is expressed with RAII smart pointers and C# event handlers are mapped to CNA/Sharp
Runtime event subscriptions. Any reflection-only dispatch in the original is translated to an
explicit typed C++ interface while preserving the same observable control flow. These are language
mechanics, not gameplay changes.

Required C# object arguments that the sample immediately dereferences are C++ references. This
makes their non-null contract explicit and removes unreachable `null` checks; nullable objects and
the public pointer-based XNA networking surfaces remain pointers and retain the original guards and
exceptions.

The authentic pipeline-produced `One Step Beyond.xnb` and external WMA stream are retained. The
additional `One Step Beyond.oga` is Ogg FLAC, lossless relative to the 16-bit PCM decoded from that
exact WMA stream. CNA's general Song reader probes `.oga` and its mixer decodes Ogg FLAC; game code
still performs the unchanged `Content.Load<Song>("One Step Beyond")` request. The older lossy
Vorbis `.ogg` companion was removed. The 37,279,699-byte FLAC and the source WMA produce the same
16-bit stereo PCM SHA-256 `4d8d5be04a89c990f1bbe30a43ec082d493534245a540a7114c46268f40619aa`.
Reproduction: `ffmpeg -i 'One Step Beyond.wma' -map 0:a:0 -c:a flac -sample_fmt s16
-compression_level 5 -f ogg 'One Step Beyond.oga'`. The XNB's official duration is 366.085 s;
FFmpeg's full decoded stream reports 366.132 s, including its decoder padding.

The original `Program` class is nested in `NetRumbleGame.cs`; C++ places the process entry point in
`Program.cpp`. `ParticleEffectType.cs` is represented in the shared sample enum header. Managed
assembly title, company, GUID and version attributes have no native runtime equivalent and remain
identified in `Properties/AssemblyInfo.cpp` instead of inventing a game-facing API.

## Original content paths and XML metadata

The original `audio/wav` and `Textures/Clouds` identifiers are kept. CNA's shared `ContentManager`
already resolves content paths with different ASCII casing. SharpRuntime `DirectoryInfo` now
resolves the original `Content/audio/wav` directory only when its exact lookup fails; this is a
general, unambiguous existing-path rule, not a NetRumble path rewrite.

The particle XML uses CNA's shared `XmlSerializationEXT.hpp` mappings for `Vector2` and `Vector4`.
The duplicate sample-local XML adapter has been removed; all six original XML graphs stay verbatim.
