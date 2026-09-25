# Difference from the XNA 4.0 original

## Registering the sample's reflective content type

The original `ParticleSystemSettings` type is serialized into four XNBs by XNA's
`XmlImporter` and `PassThroughProcessor`. XNA's `ReflectiveReader<T>` finds and
reads its fields through .NET reflection, so the C# game has no reader list or
registration call.

C++ has no equivalent runtime reflection. The port makes that one language
mechanism explicit in the game's constructor:

```cpp
CNAEXT RegisterParticleSystemSettingsReader();
```

The function lists `ParticleSystemSettings` members in their serialized order
and passes them to CNA's shared `ReflectiveTypeReaderBuilder<T>`. It also
registers the enum readers named by the XNB's type table. It does not replace
the original XNB or set any game value by hand. A complete decode of the
847-byte `ExplosionSettings.xnb` matches its XML source, and fresh seeded XNA
versus CNA captures agree within eight RGB levels for every pixel in both
Explosions and SmokePlume. See [missing.md](missing.md) for the evidence and
remaining C++ representation differences.
