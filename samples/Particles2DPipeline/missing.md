# Particles2DPipeline — port notes

Upstream: `Particles2DPipeline_4_0` (SAMPLE-044). Re-ported from scratch. Four particle systems,
the emitter, the projectile-free 2D particle engine, and — the point of the row — **every setting
loaded from the content pipeline** rather than hand-written in C++.

## What the previous port got wrong

A header-only port whose own `missing.md` opened with two admissions, both of which this re-port
removes:

| Old claim | Now |
|---|---|
| "XML settings hand-translated to C++ construction code … CNA has no general content-pipeline deserializer for custom types" | The four settings assets are built by the real pipeline (`XmlImporter` → `PassThroughProcessor`) and **loaded from their `.xnb`**, through CNA's documented custom-reader extension point. No value is written twice. |
| "`DrawableGameComponent`/`Game.Components` not used … design choice" | The systems are `DrawableGameComponent`s registered in `Components`, with the original's `DrawOrder` values, exactly as upstream. |

## Content

Nine listed assets: two textures, one `.bmp`, a spritefont, two more textures and the four
settings XML files. The XML ones are the row's subject:

```xml
<Compile Include="ExplosionSettings.xml">
  <Importer>XmlImporter</Importer>
  <Processor>PassThroughProcessor</Processor>
</Compile>
```

`XmlImporter` deserializes the file into a `ParticlesSettings.ParticleSystemSettings` — a type
that lives in the sample's **own** `ParticleSettings` assembly, which the content project
references — and `PassThroughProcessor` hands it on untouched, so `IntermediateSerializer` writes
that object into the `.xnb`. The assembly is compiled first and declared in `PipelineAssemblies`.

## Reading a reflectively-written `.xnb` in CNA

XNA compiles a type with no explicit `ContentTypeWriter` through an implicit
`ReflectiveReader<T>`, which walks the type's fields with .NET reflection at load time. **CNA has
no such reflection and says so** — `docs/xnb-content-pipeline-support.md` (XNB-42A) declares the
reflective path unsupported by design and offers `ContentTypeReaderManager::AddTypeCreator()`
instead. So the port supplies the one thing reflection would have provided and that the game has
anyway: the layout of its own type.

The reflective payload is not an opaque format, and this port decodes it rather than guessing:

- value-type fields are written **inline, in declaration order**;
- a reference-type field is preceded by the **1-based index of its own type reader**;
- the decode of `ExplosionSettings.xnb` ends on the file's last byte (847 of 847) with every value
  matching the XML — `MinNumParticles` 10, `MaxNumParticles` 12, texture `explosion`,
  `AccelerationMode` `EndVelocity`, `SourceBlend` `SourceAlpha`, and so on.

The port originally hand-wrote a `ContentTypeReader` doing exactly that. **On the owner's decision
that CNA should grow the layer itself, it no longer does:** `cnanext` gained
`ReflectiveTypeReaderBuilder<T>`, and the sample declares its field list once —

```cpp
ReflectiveTypeReaderBuilder<S>("ParticlesSettings.ParticleSystemSettings")
    .Field(&S::MinNumParticles)
    .Field(&S::TextureFilename)
    .EnumField(&S::AccelerationMode, "ParticlesSettings.AccelerationMode")
    …
    .Register();
```

— and CNA builds the reader from it. The member's C++ type decides how each field is read:
arithmetic types and the XNA math structs inline, anything else through
`ContentReader::ReadObject`, which consumes the reference type's own reader index first. The
registration key is derived rather than spelled out: CNA normalizes the `.xnb`'s
assembly-qualified name down to
`Microsoft.Xna.Framework.Content.ReflectiveReader\`1[[ParticlesSettings.ParticleSystemSettings]]`,
and `Register()` produces exactly that.

`EnumField` also registers the enum's own `EnumReader`, which is the part easiest to miss: a
`.xnb`'s type-reader **table must resolve in full before any object is read**, so the two
`EnumReader`s this file names must exist even though the reflective payload writes each enum
inline as an `Int32` and never dispatches to them.

**A deviation from the original's file list remains** — XNA needs no field list here, because
.NET reflection supplies it. What changed is where the knowledge lives: in the type's own
declaration order, declared once, instead of duplicated in a hand-written reader that could drift
from it. Real reflection would need a code generator over the headers; that is not what this is.

Verified end to end: after moving to the builder the frozen comparison is **still 100.00 %** at
both 60 and 180 updates, so the layer reads exactly what the hand-written reader did.

## Comparison against the original

`CNA_SEED` seeds the shared generator and `CNA_FRAMES` freezes the simulation after a fixed number
of updates (`scripts/compare-frozen.sh`, `cna-diag/README.md`).

| Updates | Within 8 levels |
|---|---|
| 60 | **100.00 %** |
| 180 | **100.00 %** |

Pixel-identical to the original, twice. That is the strongest result in the campaign so far, and
it is worth naming what it proves at once: the settings really came from the pipeline `.xnb` (a
single wrong field would move a particle), `System::Random` reproduces .NET's sequence exactly
(same seed, same explosion positions), and the whole 2D particle engine — emission, ageing,
recycling, blending — is bit-faithful.

It also took a **change to CNA's game loop** to get there, which SAMPLE-043 had found and this
sample forced. See below.

### The game clock, changed on the owner's decision

SAMPLE-043 measured XNA's clock against FNA's and recorded the difference without changing CNA.
The owner then ruled that the **XNA 4.0 original is authoritative over FNA**, so `cnanext` now
follows XNA. Measured on the real runtime through this sample's `CNA_PROBE` hook, in both timing
modes:

```
fixed=True  update 1: elapsed=0.000000000 total=0.000000000
fixed=True  update 2: elapsed=0.016666700 total=0.000000000
fixed=True  update 3: elapsed=0.016666700 total=0.016666700
fixed=False update 3: elapsed=0.021096900 total=0.000000000
fixed=False update 4: elapsed=0.032696600 total=0.021096900
```

Two rules, both now in `Game::Tick()`:

1. the game's **first** update runs with `ElapsedGameTime = TimeSpan.Zero`;
2. `TotalGameTime` is the time **before** the step, so it advances once `Update` returns — in both
   the fixed and the variable path.

FNA does neither (`FNA/src/Game.cs:475`), which left a CNA game two fixed steps ahead of XNA's at
the same update index. Of the two rules, the `TotalGameTime` lag is the one that matters in
practice: reverting only the first-update rule still scores 99.99 % here, while the pair together
score 100.00 %.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the
instructions and the free-particle readout render over the black clear, that **particles are drawn
at all** — which is what proves the settings `.xnb` loaded, since a failed load leaves the systems
with no texture and nothing to draw — that the explosions fire with no input, and that Space
cycles all three effects and wraps back.

The harness also stopped waiting a fixed 15 seconds for the page and now polls for the `#canvas`
element instead: this sample's bundle takes longer to instantiate, and the fixed sleep turned that
into a crash in the first `evaluate()` rather than a slower run.

## Deviations

- **The settings field list has no counterpart in the original** — see above. It is one
  `RegisterParticleSystemSettingsReader()` call and a chain of `.Field(...)` in declaration order.
- `List<Particle>` + `Queue<Particle>` become `std::deque<Particle>` plus a `std::deque<Particle*>`
  of borrowed pointers; `deque` is what keeps those pointers valid as the pool grows.
- `Components.Add` takes borrowed pointers, so the game owns the four systems and the emitter
  through `std::unique_ptr`.
- The `ParticleSystemSettings.AccelerationMode` field keeps XNA's name even though it matches its
  own type's name; C++ allows it only when the type is written fully qualified at that point.
- Touch input (`TouchPanel.EnabledGestures = Tap`) is omitted: this port targets the Windows
  configuration, where the original reads the gesture only on the phone build.
