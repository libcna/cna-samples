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
additional OGG is a decode-only native runtime companion derived from that exact stream because the
cross-platform audio backend does not decode WMA; game code still performs the unchanged
`Content.Load<Song>("One Step Beyond")` request.

The original `Program` class is nested in `NetRumbleGame.cs`; C++ places the process entry point in
`Program.cpp`. `ParticleEffectType.cs` is represented in the shared sample enum header. Managed
assembly title, company, GUID and version attributes have no native runtime equivalent and remain
identified in `Properties/AssemblyInfo.cpp` instead of inventing a game-facing API.

## Native Linux filesystem casing

- The original Windows path `Content/audio/wav` is spelled `Content/Audio/wav`
  in the C++ port because the shipped content directory is named `Audio` and
  Linux filesystems are case-sensitive.
- Likewise, the original logical name `Textures/Clouds` is spelled
  `Textures/clouds` because the shipped texture file is named `clouds.xnb`.
