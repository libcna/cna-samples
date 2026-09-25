# Inverse Kinematics — port notes

## Pre-work current-head analysis — 2026-09-25

This pass inspected the physical upstream directory, port sources, retained
products and historical evidence only. It did not rebuild or run SAMPLE-057
on current CNA `cefe6c83b` and SharpRuntime `41b918c9`; the previous `✅`
is historical until those gates are repeated.

- The retained `xna4-original/` matches all **18** physical files under
  `/rv/tmp/XNAGameStudio/Samples/InverseKinematics_4_0/` (`diff -qr` is empty).
  Windows and Xbox projects share the game and content project. The Windows
  project selects HiDef, and its unchanged source registers
  `GamerServicesComponent`, loads the stock cylinder `Model`, cat texture and
  font, and keeps the Xbox-specific HUD behind `#if XBOX`.
- All three checked-in XNBs still match the retained Windows HiDef XNA build
  byte for byte (`cat`, `cylinder`, `font`; hashes below). This verifies the
  retained content, not a fresh XNA pipeline run on current heads. The port
  still calls `Content.Load<Model>("cylinder")`, and a targeted scan found no
  raw-model, loose-asset or renderer bypass in its game sources. A full
  source/branch re-audit remains part of requalification.
- The original Windows/Xbox projects include `Game.ico` and
  `GameThumbnail.png`; neither is present beside the C++ port. Restore these
  upstream packaging files. The original HTML, three JPEGs and license are
  already retained. No Inverse Kinematics gallery card, detail page or
  published WEBGL2 copy exists yet.
- The pruned artifact root retains original, diagnostic, OPENGLES3 and
  WEBGL2 products, but the native binary's RUNPATH points into the old
  `openeggbert/cnanext` checkout, and `MANIFEST.md` rebuild commands point
  into old `openeggbert/cna-samples`. Rebuild against the active siblings and
  replace the stale reproduction commands before interpreting runtime results.
  The old web capture helper also uses fixed ports and a broad profile-based
  `pkill`; use scoped cleanup and isolated evidence in this pass.
- Historical evidence says the unchanged XNA executable cannot initialize
  GamerServices under Wine without the retired `XnaLiveProxy.exe`. The retained
  diagnostic patch removes exactly that one component registration; it is an
  **audit-only** copy, not a change to the port. Reproduce the unchanged
  failure, then compare original diagnostic, native and real-Chrome behavior
  for convergence, pause, single step, movement, reset, HUD and clean exit.
  Verify the avatar `Ready` path's source mapping and record that Windows
  avatar rendering cannot be observed without the service.

Next: restore icon and thumbnail, update safe active-checkout build/capture
scripts, rebuild unchanged XNA content/game and current Release OPENGLES3 and
nonthreaded WEBGL2 products, compare the three exact XNBs, exercise the
above behavior in isolated XNA/native and real Chrome, add and check the
gallery entry, then replace this pre-work status with current evidence.

## Historical completion record — 2026-08-30

Upstream: `InverseKinematics_4_0` (SAMPLE-057). The complete Windows sample is ported,
including the cylinder and avatar IK paths, both platform-specific HUD branches, all input,
documentation and exact pipeline content.

Artifact root: `/rv/tmp/samples/SAMPLE-057-InverseKinematics_4_0/`.

## Fresh source and project audit

Every C# file, Windows/Xbox project and solution, content declaration, source asset and HTML topic
was reviewed against the C++ port. The previous port was not faithful: it replaced
`Content.Load<Model>("cylinder")` with a sample-local raw vertex/index loader, used converted
JSON/PNG/font sidecars, omitted the Xbox HUD branch, changed the logical game type from `IKSample`
to `InverseKinematicsGame`, and added an F1 help overlay. All of those differences are removed.

The port now retains the original behavior:

- the 20-link cylinder hierarchy uses the original bind pose, parent list, reverse CCD chain,
  per-bone transforms and current-bone/end-effector coloring;
- `UpdateBone` preserves the goal/end-effector local-space transforms, normalized cross-product
  axis, clamped dot product, wrapped angle and post-multiplied rotation;
- the stock cylinder `Model` is loaded by `Content.Load<Model>("cylinder")`, every `BasicEffect`
  receives default lighting and per-pixel lighting, and every link is drawn through `ModelMesh`;
- the cat uses the original six `VertexPositionTexture` vertices and constrained billboard;
- the avatar path retains `AvatarDescription.CreateRandom`, the four named left-arm bones, all 71
  transforms, right-shoulder rotation, look-at transform, Ready guards and renderer draw call;
- `P`, Enter and Space control running/single-step behavior; WASD/QE move the cat; arrows/ZX move
  the camera; `R` resets; Escape and Back exit; the matching game-pad paths remain;
- the inactive `XBOX` HUD branch remains alongside the active Windows branch;
- the original 853x480 backbuffer, multisampling request, camera, projection, draw state, colors,
  text, update/draw order and title are unchanged.

The HTML topic says Shift pauses the simulation, while `IKSample.cs` and the on-screen HUD both use
`P`. The port follows the executable source and HUD (`P`), preserving this upstream documentation
inconsistency rather than inventing a third behavior.

## Exact official content

The original XNA 4.0 content project builds unchanged. Windows HiDef is the selected project
configuration; audit builds of Windows Reach and Xbox HiDef also succeed. The three checked-in
Windows HiDef products are byte-identical to the retained official build:

| asset | SHA-256 |
|---|---|
| `cat.xnb` | `007fc1322fde3ff1b700f0cf83551bfbf5a792404b2f9cd8a8e5ff6e6ed17402` |
| `cylinder.xnb` | `4a2d7723c19880e19e0392b86e42fefbdaa966d1f785221e7ce826fd89896011` |
| `font.xnb` | `8e37de58d0236eab715067b1165d01bccdfa8d7bf26cd98d0f6508a2512744d2` |

The decoded `cylinder.xnb` table contains the stock `ModelReader`, vertex buffer/declaration,
index buffer and `BasicEffectReader`, with three shared resources. The loose model JSON, raw
vertex/index buffers and converted cat/font PNG/JSON files are gone. Historical `help.png` is at
the sample root and is neither packaged nor loaded.

## Original execution

The unchanged Windows HiDef game executable compiles successfully. Under the campaign Wine prefix
it stops during `GamerServicesComponent.Initialize` because Microsoft's discontinued
`XnaLiveProxy.exe` is not installed; the exception and full unchanged run are retained in
`evidence/xna-original/run.log`.

For visual/input evidence, `xna4-diag/IKSample.cs` removes only the single
`Components.Add(new GamerServicesComponent(this));` line. Its retained unified diff proves the
scope. No game, IK, content, rendering, camera, HUD or input logic changes. This audit build runs
under WineD3D, shows the expected Windows state with `AvatarRendererState.Unavailable`, and verifies
convergence, pause, single-step, movement, reset and clean Escape exit.

## Native XNA/CNA fidelity

The Release OPENGLES3 target builds and runs with the exact XNB files. The same interactive paths
pass, including clean Escape exit. Ordinary converged captures at 853x480 compare as follows:

| comparison | exact pixels | within 8 | within 16 | after 4 px blur |
|---|---:|---:|---:|---:|
| XNA vs CNA OPENGLES3 | 98.93% | **99.90%** | 99.92% | **100.00%** |
| XNA frame stability | 99.78% | 99.97% | 99.98% | 100.00% |
| CNA frame stability | 99.68% | 99.92% | 99.95% | 100.00% |

The frames visibly agree on the cat, all 20 shaded cylinder links, end-effector location, HUD,
background and absence of a Windows avatar. The minor ordinary-frame changes are the continuously
iterated CCD chain converging further between captures, not omitted content.

## Web

The complete Release Emscripten `WEBGL2` bundle runs in the system Google Chrome over local HTTP.
The gate verifies the 853x480 WebGL 2 canvas, `WEBGL2` renderer log, original title, pause,
single-step, movement and reset. All `.html`, `.js`, `.wasm` and `.data` requests succeed, with no
promise rejection, runtime exception, relevant HTTP failure or fatal console message.

After excluding only the browser's focus outline from the audit screenshot, XNA vs WEBGL2 reaches
**99.86% within 8 levels** and 100% after 4 px blur; OPENGLES3 vs WEBGL2 reaches **99.93% within
8** and 100% after blur.

## Framework and runtime result

No CNA, SharpRuntime, EasyGL or MetaGL repair was necessary. CNA's repaired stock XNB
`ModelTypeReader` path already handles this exact cylinder model. The old `CylinderModel.hpp`
workaround and its sidecars were removed rather than preserved. The sample build helper now exposes
a `GAMER_SERVICES` dependency so a sample that needs avatars does not also link the unrelated
networking module.

CNA's Windows-reference `AvatarRenderer` remains unavailable without Xbox LIVE avatar data. That
matches the observed Windows XNA state and the original sample's own Ready guards; no substitute
avatar, extension renderer or fake data is used.

## Intentional C++ mappings

- C# lists map to `std::vector`; CNA read-only avatar collections are copied into vectors only to
  call the same hierarchy transform algorithm.
- C# reference ownership maps to `std::unique_ptr`/`std::optional`; the cat texture property is a
  non-owning pointer whose owning game keeps the texture alive.
- C# `as BasicEffect` maps to `dynamic_cast` followed by the same null check.
- `static void Main()` maps to `int main()`, and sample properties use CNA getter/setter naming.

These are lossless language mappings. There is no owner-approved behavioral addition and no
`diff.md` is needed.

## Documentation and evidence

`InverseKinematics.htm`, its three referenced JPEGs and `Microsoft Permissive License.rtf` are
retained from upstream. Important artifact paths are:

- `xna4-original/`: complete untouched upstream snapshot;
- `xna4-build/`: unchanged executable and Windows HiDef/Reach plus Xbox HiDef official content;
- `xna4-diag/`: one-line GamerServices environment adaptation and its diff;
- `cna-native-opengles3/` and `cna-web-webgl2/`: reusable native and browser build trees;
- `scripts/`: original/content build, native/web capture, XNB inspection and comparison helpers;
- `evidence/xna-original/`, `evidence/cna-native-opengles3/` and
  `evidence/cna-web-webgl2/`: captures, logs, browser result and input evidence;
- `evidence/{xnb-reader-tables,xnb-sha256,pixel-comparison,web-pixel-comparison}.txt`: content and
  image comparisons.

There is no remaining SAMPLE-057 blocker, omission, substitute or sample-side workaround.
