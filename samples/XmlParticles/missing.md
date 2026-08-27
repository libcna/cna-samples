# XmlParticles — port notes

Upstream: `XmlParticles_4_0` (SAMPLE-045). Ported whole — the five particle systems, the emitter,
the projectiles, all three effects and every key binding, with **every setting loaded from XML the
content pipeline compiled**.

## What this sample is

It is `Particles3DSample` (SAMPLE-043) with one change, and that change is the row's subject.
Three of its files are **byte-identical** to that sample's — `ParticleEmitter.cs`,
`ParticleVertex.cs`, `Projectile.cs` — and the port reuses them unchanged. The difference:

| | Particles3D (SAMPLE-043) | XmlParticles (this) |
|---|---|---|
| `ParticleSystem` | abstract, one subclass per effect | **concrete**, driven by a settings asset name |
| Settings | five `InitializeSettings` overrides in C# | five XML files through `XmlImporter` → `PassThroughProcessor` |
| Effect classes | `ExplosionParticleSystem`, `FireParticleSystem`, … | none — five instances of the same class |
| `ParticleSettings` | a class inside the game | its own assembly, referenced by the content project |

So the port has **no per-effect subclasses at all**, exactly as upstream.

## Reading the settings

`cnanext` grew `ReflectiveTypeReaderBuilder<T>` for SAMPLE-044; this is its second user and the
first outside the sample it was written for. The game declares `ParticleSettings`' members once and
CNA builds the reader.

Two things this sample taught the layer, both measured on real pipeline output rather than assumed:

**1. The wire order is properties first, then fields.** SAMPLE-044's type had no serialized
properties, so "declaration order" was enough there. This type marks its `BlendState`
`[ContentSerializerIgnore]` and serializes a **private** `[ContentSerializer]` string property in
its place — and that property comes out **ahead of every field**. Decoding `FireSettings.xnb` with
the fields-first assumption produced nonsense (`MaxParticles` = 1768293378); with the property
first it lands on the file's last byte, 574 of 574, with every value matching the XML — `fire`,
2400 particles, `PT2S` → 2 s, gravity `0 15 0`, `MinColor` alpha 10, `MaxColor` alpha 40. The
builder's documentation now says so, and the rule is: **decode one real file before writing the
member list.**

**2. `TimeSpan` and a caller-supplied conversion.** `Duration` is a .NET `TimeSpan`, written inline
as its Int64 tick count; the builder now reads it. And the `BlendState` helper needs a string
mapped onto an object, so the builder grew `.Custom(...)`, which the port uses to reproduce the
original's `BlendStateSerializationHelper` setter verbatim — including its
`ArgumentException("Unknown blend state ...")`.

## Comparison against the original

`CNA_SEED` seeds the three generators and `CNA_FRAMES` freezes the simulation after a fixed number
of updates (`scripts/compare-frozen.sh`, `cna-diag/README.md`).

**The simulation is exact.** At update 180 the probe shows all five systems holding identical
queues and identical clocks, to the digit:

```
XNA  ProjectileTrailSettings: draws=180 active=0 new=800 free=800 retired=0 t=2.9833
CNA  ProjectileTrailSettings: draws=180 active=0 new=800 free=800 retired=0 t=2.9833
```

That is the game-clock change SAMPLE-043 found and SAMPLE-044 landed doing its work: before it,
the trail differed by 6 particles in 806 at this same point.

The **frames** then differ where the translucent volume is:

| Updates | Within 8 levels | Within 16 | Within 32 | Mean abs. | Median |
|---|---|---|---|---|---|
| 60 | 99.43 % | 99.9 % | 99.9 % | 0.68/255 | 0 |
| 180 | 80.08 % | 91.6 % | **98.5 %** | 4.74/255 | 1 |

The 180-update figure is not boundary noise and this port does not pretend it is: a 4 px blur
barely moves it (80.08 → 80.78 %) and the differing pixels are **less** likely than average to sit
on an edge (15 % against 20 % of the frame). The difference map shows it spread through the
interior of the smoke and explosion, and the numbers say what it is — dozens of overlapping
translucent sprites, each contributing a fraction of a level, accumulating: the median difference
is **one level** and 98.5 % of pixels are within 32. The frames are visually indistinguishable.

At 60 updates, with far less overlap, the same scene agrees to 99.43 % and behaves like ordinary
boundary noise (73 % of differing pixels on an edge, blur recovers it to 99.83 %).

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the grid floor
renders, that **every one of the three effects puts particles in the sky** — which is what proves
the settings assets loaded, since a load that silently produced defaults would leave the systems
with no texture and nothing to draw — that the scene animates with no input, and that Space cycles
the effects and the camera keys move the view.

## Deviations

None in behavior. Beyond the ones the sibling sample already lists (borrowed component pointers,
`Load<std::shared_ptr<Effect>>`, packed vertex members, `IndexElementSize`):

- **The settings member list has no counterpart in the original** — .NET reflection supplies it
  there. It is one `RegisterParticleSettingsReader()` call and a chain of `.Field(...)` in wire
  order.
- `ParticleSettings.BlendState` keeps XNA's name even though it matches its own type's name; C++
  allows that only when the type is written fully qualified at that point.
- The original has no `Initialize` override here: the pool size comes from the settings, so the
  particle array is allocated in `LoadContent` after they are read. The port follows.
