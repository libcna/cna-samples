# Difference from the XNA 4.0 original

## Reflection registration

The original content pipeline writes a `ReflectiveReader<ParticleSettings>` name
into each of the five settings XNBs. XNA finds the C# type and its serialized
members through reflection. C++ has no equivalent runtime reflection, so the
port calls `CNAEXT RegisterParticleSettingsReader()` once in the game
constructor. Its `ReflectiveTypeReaderBuilder<ParticleSettings>` registration
names the same logical type and reads the original serialized order: the
private `BlendStateSerializationHelper` property first, then the public fields.
The settings still come from `Content.Load<ParticleSettings>(settingsName)`;
the registration changes neither content nor behavior.

The other representation differences are the ordinary C++ translation of
ownership and value types. No owner-approved behavior change is present.
