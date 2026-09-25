# Particles3D — SAMPLE-043 requalification (2026-09-25)

**Status:** complete. The original XNA 4.0 Windows/Reach game, Release native
OPENGLES3 port and nonthreaded Release WEBGL2 port build and run. The five
particle systems, circular GPU queues, custom 36-byte vertex, partial dynamic
buffer uploads, three effects, camera and controls are present. There is no
remaining sample workaround, known behavioral gap or CNA/sharp-runtime change
required by this requalification.

## Source and content

The physical upstream directory is
`/rv/tmp/XNAGameStudio/Samples/Particles3DSample_4_0`. It matches the preserved
`/rv/tmp/samples/SAMPLE-043-Particles3DSample_4_0/xna4-original/` byte for
byte. All twelve runtime C# files, both game projects, the content project and
the port were rechecked. The Windows project uses Reach; the Xbox project uses
HiDef. The unchanged Windows source and its original XNA Content Pipeline build
and run under Wine/Xvfb. The original icon, screenshot and Microsoft Permissive
License were restored to `samples/Particles3D/`; `Particle3D.htm` was already
identical to upstream.

The content project lists six source assets. `grid.x` also draws in
`checker.bmp`, yielding seven XNBs. The original pipeline rebuilds six of the
seven pinned official XNBs byte for byte. Its newly compiled
`ParticleEffect.xnb` has the same length (7624 bytes) but eight different
bytes at offsets 462, 463, 557–559, 773, 833 and 1395. Both the original and
all CNA runs therefore use the **same seven checked-in official XNBs**. The
fresh effect is retained in `evidence/requal-20260925/` for inspection. No
hand-translated shader or content substitute is involved.

## Port corrections found by the current comparison

- `UpdateFire()` now uses double precision π for its angle, as upstream
  `Math.PI` does. The previous `MathHelper::Pi` was a float and gave fire ring
  positions slightly different trigonometry.
- `ParticleSystem::AddParticle()` evaluates the four random color channels
  into separate variables in source order before constructing `Color`.
  C# evaluates constructor arguments left to right; the previous C++ inline
  calls were evaluated right to left by the compiler. A vertex dump showed
  each particle's four packed random bytes reversed. This was a port error,
  not a CNA `Color` packing error.

The earlier port had already added CNA's windowed
`DynamicVertexBuffer::SetData` overload, unhidden inherited overloads and
`Vector3` compound operators. Its raw packed values in `ParticleVertex`
avoid putting CNA's polymorphic `Short2`/`Color` objects into a GPU vertex;
layout assertions hold the 36-byte XNA wire format. Those historical
framework fixes remain in CNA. The old claim that this sample requires a
handwritten GLSL effect was obsolete.

## Original, native and browser gates

- Fresh original XNA build: the game draws its grid, particle effects and
  overlay. On an isolated X display, Space cycles Explosions, SmokePlume and
  RingOfFire; Up/Left rotate, Z zooms, R resets and Escape closes with status
  zero. Captures: `evidence/requal-20260925/xna-original-full/`.
- Fresh native Release OPENGLES3 build: the same scene and controls pass.
  The retained executable was rebuilt with `CNA_SHARED_LIBRARY=OFF`, stripped,
  and packaged with adjacent SDL3 libraries. Its own render/input/Escape gate
  also exits zero; `ldd` resolves both SDL libraries beside the executable.
  Captures: `cna-native-full/` and `native-retained/` in the evidence directory.
- Fresh nonthreaded Release WEBGL2 build: real system Chrome reports WebGL 2,
  the original title and renderer, a visible grid and particles in all three
  effects, continuous animation, Space effect switching and camera movement.
  The four bundle assets return HTTP 200; no runtime exception, rejected
  promise, relevant HTTP error or fatal console message occurs. Both work and
  retained bundles pass; their four file hashes match. The gallery copy is
  tested separately with the same gate. Evidence: `web-work/`,
  `web-retained/`, `web-gallery/` and adjacent hash files.

The browser gate measures the grid by non-sky pixels in the lower half; the
camera-moved frame has its own lower threshold because tilting the camera
changes how much of the grid remains there. Its sky check uses the start,
SmokePlume, RingOfFire and moved frames. A random late explosion frame can
temporarily fill nearly all of the upper half with smoke, so it is checked
for animation and particle pixels rather than for a fixed amount of blue sky.

## Frozen comparison against XNA

The diagnostic builds only add equal seeds (`CNA_SEED=1234`), a fixed update
count (`CNA_FRAMES`) and a one-draw-per-update count for the queue retirement
rule (`CNA_ONE_DRAW_PER_UPDATE`) **to both engines**. These hooks are confined
to `scripts/diagnostic/`; neither shipped game contains them. Equal updates
alone do not fix draw count: the native and Wine render loops have different
speeds, and the original engine intentionally delays retired slots by three
draws. Normalizing that schedule lets the test compare equivalent queue
history while the live gates above check the unmodified programs.

| Fixed updates | Pixels within 8 RGB levels | Within 8 after 4 px blur | Mean absolute RGB error (0–255) |
|---:|---:|---:|---:|
| 60 | 99.6638% | 100% | 0.16830 |
| 180 | 99.6638% | 100% | 0.17534 |
| 360 | 99.6638% | 100% | 0.18768 |

The 360-update images were captured twice and had identical SHA-256 hashes
within each engine. At 360 updates the foreground counts are 334290 XNA and
334289 CNA pixels. The remaining difference is at rasterized edges. The
JSON, captures, comparison script and repeat hashes live in
`evidence/requal-20260925/`. The prior 60/180 table in this file measured an
older CNA clock and is superseded: CNA's XNA-first-update clock fix had already
landed before this requalification. An intermediate 360-update run without
normalizing draw count showed only 77.79% agreement; the mismatch followed
queue-slot retirement as the two processes drew different numbers of frames,
not a missing effect or an incorrect particle simulation.

## Reproduction and retained products

Artifact root: `/rv/tmp/samples/SAMPLE-043-Particles3DSample_4_0/`.
`scripts/build-original.sh` rebuilds the unchanged original; the diagnostic
scripts under `scripts/` rebuild and capture both sides. The retained products
are `xna4-build/bin/Particle3DSample.exe`,
`cna-native-opengles3/samples/Particles3D/Particles3D_cna_samples` and the
four-file `cna-web-webgl2/samples/Particles3D/` bundle. `MANIFEST.md` records
the local build commands and artifact inventory. The site card, detail page,
screenshot and matching web bundle are staged in `samples.libcna.com` for
owner review. They are local until the owner requests a push; publication to
GitHub Pages is not claimed. The artifact root is 656 MiB on disk while its
reusable build trees remain. A guarded prune **dry run only** identifies six
reproducible intermediate paths, estimates 565.9 MB freed and leaves no
unrecognized top-level source copy. Applying that prune awaits the owner's
separate instruction for SAMPLE-043.
