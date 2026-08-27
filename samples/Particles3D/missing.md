# Particles3D — port notes

Upstream: `Particles3DSample_4_0` (SAMPLE-043). Ported whole — the five particle systems, the
circular-queue lifecycle, the custom vertex type, the emitter, the projectiles, all three effects
and every key binding. Nothing is missing, stubbed or simplified.

The placeholder that stood here claimed the sample was blocked on `ParticleEffect.fx` needing a
hand translation to GLSL plus a `.shader.json` descriptor. Stale: the official pipeline compiles
the effect and CNA runs it. That is the fourth consecutive sample whose recorded blocker was
obsolete.

## Content

Six listed assets, all through stock importers and processors, Reach profile. `grid.x` is a
DirectX `.x` model, so `XImporter` joins the stock importers — the same importer SAMPLE-039 needed.
The build emits **seven** XNBs: the grid's material pulls in `checker.bmp`, which the content
project does not list.

## What this sample exercises in CNA

This is the largest sample of the campaign so far (~1 900 lines of C# across twelve files) and the
row's subject is the GPU particle engine:

- a **custom vertex type** — corner (`Short2`), start position, start velocity, four packed random
  bytes and a creation time, 36 bytes, declared through five `VertexElement`s;
- a **circular queue in four regions** (active / new / free / retired) with the retired region held
  back for three draws so the CPU never overwrites what the GPU may still be reading;
- **windowed uploads** of only the newly created particles into a `DynamicVertexBuffer`;
- five systems sharing one compiled effect through `Effect.Clone()`, each with its own parameters;
- `DrawableGameComponent` ordering: `DrawOrder` decides that fire draws over smoke.

## Framework gaps found and fixed in `cnanext`

Three, all in the same family — API shapes XNA has that CNA had not needed yet:

1. **`DynamicVertexBuffer.SetData<T>(int offsetInBytes, T[], int, int, int, SetDataOptions)`** —
   the windowed overload. `ParticleSystem.AddNewParticlesToVertexBuffer` writes only the newly
   created particles at the queue's write position; without it the sample could not upload at all.
   The streaming hint is accepted and deliberately **not** forwarded: CNA composes a windowed write
   in a CPU shadow and uploads the buffer whole, which cannot keep a `NoOverwrite` promise. The
   result is correct and merely costlier than XNA's.
2. **The inherited `SetData` overloads were hidden.** `DynamicVertexBuffer` declares its own, and
   C++ name lookup stops at the first scope that declares the name — so
   `vertexBuffer.SetData(particles)`, which this sample calls to restore a lost buffer, did not
   compile. `using VertexBuffer::SetData;` restores what XNA's class inherits.
3. **`Vector3::operator*=` and `operator/=`** (scalar and componentwise). C# synthesises them from
   the declared operators, so `velocity *= settings.EmitterVelocitySensitivity` needs no
   declaration there; `Vector2` already had the full set in CNA and `Vector3` had only `+=`/`-=`.
   The scalar `/=` multiplies by a reciprocal so that `v /= s` and `Vector3::Divide(v, s)` agree
   bit for bit.

All three are covered by tests, and the windowed one was confirmed to fail with the destination
offset removed.

**A CNA-side layout rule this sample makes concrete:** `Short2` and `Color` both inherit CNA's
polymorphic `IPackedVectorT`, so an object of either carries a vtable pointer and must never sit in
a vertex struct. `ParticleVertex` holds both as their raw 32-bit packed values and uses the packed
types only to compute them; `static_assert`s in `ParticleVertex.cpp` pin the 36-byte layout and
every member offset, so a future change to either type breaks the build rather than the picture.

## Comparison against the original

`CNA_SEED` seeds all three generators and `CNA_FRAMES` freezes the simulation after a fixed number
of updates, so both engines are photographed after the same number of fixed-timestep steps
(`scripts/compare-frozen.sh`, `cna-diag/README.md`).

| Updates | Within 8 levels | After a 4 px blur | Differing pixels on an edge |
|---|---|---|---|
| 60 | **99.46 %** | 99.84 % | 72 % (edges are 20 % of the frame) |
| 180 | 87.73 % | 88.65 % | 20 % (edges are 21 % of the frame) |

The 60-update figure is boundary noise, as in every earlier sample. The 180-update one is not —
the blur does not help and there is no edge enrichment at all, which says the particles really are
in different places rather than merely rasterized differently. `CNA_PROBE` found why, and it is
not a defect in the port:

**XNA and FNA advance the game clock differently, and CNA follows FNA.** At the same update index:

```
XNA  update 1:   elapsed=0.000000000 total=0.000000
CNA  update 1:   elapsed=0.016666700 total=0.016667
XNA  update 180: elapsed=0.016666700 total=2.966673
CNA  update 180: elapsed=0.016666700 total=3.000006
```

XNA's first update runs with **zero** elapsed time and its `TotalGameTime` lags its update count by
one further step, so after N updates it has accumulated (N−2) steps where CNA has accumulated N.
FNA sets `ElapsedGameTime = TargetElapsedTime` for every update including the first
(`FNA/src/Game.cs:475`) and advances `TotalGameTime` **before** calling `Update`; CNA's `Game.cpp`
is a faithful port of that loop. `CLAUDE.md` names the FNA tree as the authoritative behavioural
reference, so this is a recorded XNA-vs-FNA difference rather than a CNA bug — but it is the reason
this sample's frames drift apart, and **it applies to every CNA game whose state accumulates over
frames**, not only this one.

The queue probe confirms the size of it exactly: at update 180 four of the five systems hold
identical queues (0/0/0, 100 explosion-smoke, 60 explosion particles), and the projectile trail
differs by **6 particles in 806** — precisely two frames of its 200-per-second emission rate.
Shifting the comparison by one or two updates does not recover the agreement, because running the
original longer also emits more particles; the two effects are coupled.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the grid floor
really renders (counting its **non-sky** pixels, so a grid that failed to load would fail rather
than pass on the lower half's size), that the sky is still visible above the particles, that
**every one of the three effects puts particles in the sky** — the check a custom vertex
declaration that failed to bind would fall to — that the scene animates with no input at all, that
Space cycles the effects and that the camera keys move the view.

Two thresholds had to be corrected after they failed on a working sample, both by measuring the
wrong thing: `skyFraction` was taken over the whole frame while the sky is only counted in the
upper half, and the floor check was applied to the camera-pitched frame, where sky in the lower
half is the camera working rather than the grid failing.

## Deviations

None in behavior. Five C++ shapes worth naming:

- `Components.Add` takes borrowed pointers, so the game owns the five systems through
  `std::unique_ptr` and hands over `.get()`.
- `content.Load<Effect>` is `Load<std::shared_ptr<Effect>>` in CNA, whose `Effect` is not copyable.
- `ParticleVertex.Corner`/`.Random` hold packed 32-bit values rather than `Short2`/`Color` objects;
  see the layout rule above.
- `IndexBuffer(device, typeof(ushort), …)` becomes `IndexElementSize::SixteenBits`.
- `Projectile`'s constructor randomises `velocity` before handing `position` to the emitter, which
  a C++ member-initialiser list cannot do in place, so the randomisation moved to a helper.
