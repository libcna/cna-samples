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

The original content project lists nine compiled assets: four textures (one sourced from a
`.bmp`), a spritefont and four settings XML files. The port ships the nine exact Windows HiDef
`.xnb` outputs. Rebuilding the unchanged upstream project on 2026-09-25 reproduced every XNB
byte for byte. Loose decoded PNG/font files from the older port were removed from `Content`;
the original documentation, license, icon, thumbnail and background are retained at the sample
root, alongside the repository's historical `help.png`. The XML assets are the row's subject:

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

Verified end to end: the seeded, frozen comparison remains **100.00 % within eight color levels**
at 60 updates in Explosions and at 60 and 180 updates in SmokePlume. The latter state exercises
the `AccelerationMode.Vector` settings decoded by this reader.

## Comparison against the original

`CNA_SEED` seeds the shared generator and `CNA_FRAMES` freezes the simulation after a fixed number
of updates (`scripts/compare-frozen.sh`, `cna-diag/README.md`).

| Effect | Updates | Exact RGB pixels | Within 8 levels |
|---|---:|---:|---:|
| Explosions | 60 | 98.79 % | **100.00 %** |
| SmokePlume | 60 | 99.93 % | **100.00 %** |
| SmokePlume | 180 | 98.22 % | **100.00 %** |

This 2026-09-25 comparison uses fresh diagnostic builds of both programs, seed 1234, and the
same update count. The diagnostic copies alone add `CNA_SEED`, `CNA_FRAMES` and `CNA_STATE`;
these hooks are absent from the shipped port and unchanged original. The agreement tests the
pipeline settings, the shared random sequence and particle emission/ageing in both the default
and vector-acceleration effects. The previous claim of pixel-identical frames was too strong:
small RGB differences remain, although none exceeds eight levels in these captures. Evidence is
under `evidence/requal-20260925/frozen/` in the sample artifact root.

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

## Native and `WEBGL2`

The fresh Release `OPENGLES3` executable opens at 800×480, loads the nine original XNBs and
renders Explosions, SmokePlume and Emitter. The unchanged XNA Windows executable was rebuilt
and run separately. Both executables cycle through all three effects, wrap around and close
cleanly on Escape. Live captures prove the effects and input work, but their unsynchronised
random seeds and clocks do not make those frames suitable for pixel scores. The seeded/frozen
captures above provide that comparison.

The fresh Release `WEBGL2` bundle was built and driven in real Google Chrome
(`scripts/capture-web.sh`). The gate reads the rendered HUD by OCR for Explosions, SmokePlume,
Emitter, wraparound and a further SmokePlume transition caused by a CDP touch gesture. It also
checks that the instructions and free-particle readout render over the black clear, that
**particles are drawn at all** — which proves the settings `.xnb` loaded, since a failed load
leaves the systems with no texture and nothing to draw — and that explosions fire without input.

The harness polls for the `#canvas`
element instead: this sample's bundle takes longer to instantiate, and the fixed sleep turned that
into a crash in the first `evaluate()` rather than a slower run.

The exact four-file retained bundle is also installed in the local
`samples.libcna.com/Particles2DPipeline/` gallery directory, with an 800×480 screenshot,
560×336 thumbnail, detail page and page-4 card (the gallery's 43rd sample). Its four files are
byte-identical to the retained build. Real Chrome ran the gallery copy through the same render,
Space, tap, HTTP and runtime-error gates; the detail page and full gallery page were previewed
in Chrome. Gallery commit `461876c` is local, not published.

## Reproduction and artifact root

Everything generated for this requalification is under
`/rv/tmp/samples/SAMPLE-044-Particles2DPipeline_4_0/`. The exact upstream directory is
`xna4-original/`; the unchanged Windows/HiDef executable and all official XNBs are in
`xna4-build/bin-requal/`. `scripts/build-original.sh` rebuilt them with the XNA 4.0 Windows
references and `ParticleSettings` assembly. `scripts/capture-original.sh` ran the executable
through Wine with `WINEPREFIX=/home/robertvokac/.wine-cna-xna40`,
`WINEDLLOVERRIDES=d3d9=b` and a private Xvfb display. The release products were built with
`scripts/build-current.sh native|web` against sibling `../cna` commit `9bb6dc0a7` (the `next`
head) and `../sharp-runtime` commit `41b918c9`. The verified retained products are under
`cna-native-opengles3/samples/Particles2DPipeline/` and
`cna-web-webgl2/samples/Particles2DPipeline/`; the reusable build trees are
`work-native-20260925/` and `work-web-20260925/`. All new captures and logs are under
`evidence/requal-20260925/`.
`scripts/score-frozen.py` reproduces the three numeric pixel scores above from the saved PNGs.
No CNA or sharp-runtime source change was needed for this requalification.

## Deviations

- **The settings field list has no counterpart in the original** — see above. It is one
  `CNAEXT RegisterParticleSystemSettingsReader()` call and a chain of `.Field(...)` in declaration
  order. [diff.md](diff.md) records why this C++ reflection substitute is required.
- `List<Particle>` + `Queue<Particle>` become `std::deque<Particle>` plus a `std::deque<Particle*>`
  of borrowed pointers; `deque` is what keeps those pointers valid as the pool grows.
- `Components.Add` takes borrowed pointers, so the game owns the four systems and the emitter
  through `std::unique_ptr`.
- The `ParticleSystemSettings.AccelerationMode` field keeps XNA's name even though it matches its
  own type's name; C++ allows it only when the type is written fully qualified at that point.
- The original enables and drains tap gestures on all platforms; the port now does the same.
  The phone fullscreen/30 fps settings and Xbox left-stick movement are present under their
  original platform guards. Both branches passed syntax compilation with the native target's
  include/define set; actual execution was verified on Linux and in Chrome, not on Xbox or
  Windows Phone.
- The original Windows/Xbox project selects HiDef; the port now declares that profile on its
  desktop/browser build. Its C++ `main` applies to CNA desktop/browser hosts, which do not define
  XNA's `WINDOWS` or `XBOX` macros.
- C# evaluates the two `RandomBetween` arguments for vector acceleration from X to Y. The port
  now draws each value before constructing `Vector2`, preserving the shared RNG sequence on C++
  compilers with a different argument evaluation order.
