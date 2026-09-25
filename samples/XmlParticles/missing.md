# XmlParticles — SAMPLE-045 requalification

Upstream: `XmlParticles_4_0`. The port preserves the five particle systems,
projectiles, all three effects, camera and every original key/gamepad binding.
Each system loads its settings through `Content.Load<ParticleSettings>` from an
XML asset compiled by the original XNA Content Pipeline. There are no effect
specific subclasses, as in the original.

## Original and content provenance

The exact physical upstream directory under `/rv/tmp/XNAGameStudio/Samples/`
contains 31 files. `xna4-original/` in the artifact root now has exactly those
31 files, byte for byte. The older snapshot had 27 extra duplicate files at its
top level; they matched the real files but caused the reproduction scripts to
build the wrong physical path. The duplicates were removed and both XNA build
scripts now use `xna4-original/XmlParticles/Particle3DSample/` and its sibling
`ParticleSettings/` assembly. File hashes are in
`evidence/requal-20260925/upstream-snapshot-sha256.json`.

The unchanged Windows game project declares **Reach**; the Xbox project declares
HiDef. The sample's own `ParticleSettings` assembly is compiled first and passed
to the content project. Its five XML files use `XmlImporter` and
`PassThroughProcessor`, followed by the stock effect, texture, model and font
processors. The rebuilt Windows/Reach game and all three content legs
(Windows/Reach, Windows/HiDef and Xbox360/HiDef) completed. The Windows/Reach
build produces 12 XNBs, including `checker_0.xnb` imported with `grid.x`.
Eleven checked in XNBs match the fresh Windows/Reach output byte for byte. The
exception is `ParticleEffect.xnb`: rebuilding from the corrected nested source
path makes it 24 bytes longer because the compiled effect records the source
path twice. The checked in file matches the earlier Microsoft pipeline output
retained in `xna4-build/bin-diag/Content/` byte for byte, and both diagnostic
engines use that pinned file. See
`evidence/requal-20260925/windows-reach-xnb-comparison.json`.

The port's `Content/` contains only these twelve XNBs, with their original load
names. The upstream license, game icon, screenshot and documentation page are
restored outside `Content/`. No loose shader, image, font or model sidecar is
loaded by the port.

## Translation audit

This is the SAMPLE-043 particle engine with a concrete `ParticleSystem` driven
by an asset name. `ParticleEmitter.cs`, `ParticleVertex.cs` and `Projectile.cs`
are byte identical to that sibling's originals. The C++ port retains its five
component instances, original draw order, projectile lifetime, shared random
generators, camera matrices, effect parameters and input handling.

The prior port still had two C++ evaluation differences already fixed in
SAMPLE-043. The four `Random.Next(255)` color draws were constructor arguments,
whose order is unspecified in C++; they are now evaluated in C# order before
constructing `Color`. The RingOfFire angle now uses double precision π, as
C# `Math.PI` does, rather than `MathHelper::Pi`'s float value. Neither change
adds behavior. The port uses CNA's default Reach graphics profile, matching the
Windows project. There are no active sample workarounds.

The five settings XNBs name a reflective reader. XNA gets the C# type's members
through reflection; C++ declares them once through CNA's
`ReflectiveTypeReaderBuilder<ParticleSettings>` in wire order. The private
serialized `BlendStateSerializationHelper` property comes first, then the public
fields. `FireSettings.xnb` decodes to its final byte (574 of 574) with XML values
including `fire`, 2400 particles, a two second duration, gravity `(0,15,0)` and
alpha limits 10 and 40. The builder reads the inline `TimeSpan` tick count and
maps the private blend-state string with the original setter's choices and
exception. The one `CNAEXT` registration call is documented in [diff.md](diff.md).

## Live execution

The unchanged Windows/Reach XNA executable ran under Wine with
`WINEPREFIX=/home/robertvokac/.wine-cna-xna40` and
`WINEDLLOVERRIDES=d3d9=b` on an isolated Xvfb display. It drew the grid and
particle effects; Space switched Explosions, SmokePlume and RingOfFire; Up/Left
orbited, Z zoomed, R reset and Escape exited zero. Captures are in
`evidence/requal-20260925/xna-original-full/`.

Release OPENGLES3 built against sibling CNA `9bb6dc0a7` (`next` head) and
sharp-runtime `41b918c9`, with no dependency source change. It passed the
same scene, input and clean exit gate in
`evidence/requal-20260925/native-full/`. The retained executable is stripped,
uses `$ORIGIN` first in its RUNPATH and resolves both SDL3 libraries beside
it, making its product directory runnable without the CNA checkout's SDL path.
The Xbox project shares the same game source, but Xbox execution was not
available on this Linux host.

A fresh nonthreaded Release WEBGL2 bundle ran in system Google Chrome over
local HTTP. The gate measured a visible grid, particles in all three effects,
continuous animation, Space switching and camera motion. All four bundle
files returned HTTP 200, with no rejected promise, runtime exception, relevant
HTTP error or fatal console message. The exact four file copy at
`samples.libcna.com/XmlParticles/` passed the same Chrome gate; its hashes
match the retained web product. Evidence:
`evidence/requal-20260925/web-current/` and `web-gallery/`. The local gallery
now has the 44th card, detail page, screenshot and thumbnail. It is committed
as gallery commit `f52d629` and pushed to `origin/main`. The public detail page
and all four bundle files return HTTP 200; downloaded bundle hashes match that
commit byte for byte. The matching samples commit `d29a22f` reached
`origin/develop`.

## Seeded XNA/CNA comparison

The diagnostic builds are separate from both shipped games. Both engines use
seed `1234`, freeze at the same update count and count one draw per update for
the queue retirement rule. That last normalization matters: Wine and native
window loops draw at different rates independently of the simulation. The
original and CNA diagnostic use the same pinned official effect XNB.

| Updates | RGB pixels within 8 levels | Within 8 after 4px blur | Mean absolute error / 255 |
|---:|---:|---:|---:|
| 60 | 99.6638% | 100.0% | 0.16831 |
| 180 | 99.6638% | 100.0% | 0.17648 |
| 360 | 99.6638% | 100.0% | 0.18507 |

The original/native foreground counts are exactly equal at 60 and 180
updates, and differ by two pixels at 360. The old 2026-08-27 comparison
reported only 80.08% within eight levels at 180 updates and attributed its
residue to accumulated translucent blending. It predated the color RNG order
fix and did not normalize draw counts. Its queue/clock probe did **not** inspect
per-particle random attributes. The fresh synchronized images supersede that
conclusion. Exact PNG hashes, counts and scores are in
`evidence/requal-20260925/frozen-comparison.json`.

## Reproduction and artifact root

All generated material is below
`/rv/tmp/samples/SAMPLE-045-XmlParticles_4_0/`. The scripts there rebuild the
original (`build-original.sh`), production CNA (`build-current.sh native|web`),
separate diagnostics (`build-original-diag.sh`, `build-native-diag.sh`), live
captures and the frozen score (`compare-frozen.sh 60 180 360`,
`compare-frozen-images.py`). Native and browser products are under
`cna-native-opengles3/samples/XmlParticles/` and
`cna-web-webgl2/samples/XmlParticles/`. The current reusable CMake trees remain
in the artifact root; they have not been pruned after this requalification.
`MANIFEST.md` records the retained products and cleanup candidates.
