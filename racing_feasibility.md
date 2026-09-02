# Racing Game Kit → CNA feasibility audit

## Implementation rebaseline — 2026-09-02

The feasibility gate is now closed and the Racing plan is active. The exact live
baseline and executable evidence are in [`racing_baseline.md`](racing_baseline.md).
This supersedes two central assumptions in the historical audit below:

- CNA already has a production XNA/FNA compiled-Effect object model and EasyGL
  execution path. All ten original FNA `.efb` files create and every technique/pass
  applies on OPENGL33. No new portable Effect container or manual shader rewrite is
  planned. Milestone 0 found and fixed one real general defect as CNA `FX-128`
  (`51d61ef42`).
- CNA's glTF/CNB Model implementation is now substantially beyond the old
  `cnaintegration` evidence. Fifty-six of the 57 Racing GLBs pass the current direct
  converter. The exception is a concrete malformed accessor in `Cube.glb`, not a
  reason to design a model subsystem in advance.

The other measured gates remain: the uncompressed RGB888 normalization DDS cube,
the malformed-but-shader-unused Cube accessor, truthful `Rgba64` support/fallback,
full sidecar-to-effect validation, and pixel/audio fidelity. Asset-level
redistribution provenance remains a release decision even though repository-level
Ms-PL files are present.

## Executive summary

**Current verdict: ACTIVE IMPLEMENTATION; MILESTONE 0 COMPLETE.**
Confidence is **medium-high** for one `OPENGL33` implementation. The modern
`rds1983/RacingGame` repository should become the **primary implementation and
content reference**, while the older XNA 4 conversion remains the historical and
behavioral cross-check. This is a hybrid source hierarchy, not a claim that the
modern port is byte-for-byte authoritative for every original behavior.

The delta audit inspected the already-cloned repository at:

```text
/rv/tmp/RacingGame
d8092633e4e43e014ff168d8e913a9373538b851
author date:    2025-10-01T12:41:23+07:00
committer date: 2025-10-01T12:42:29+07:00
subject:        Moved back onto DigitalRiseModel
```

The checkout was clean when the baseline and final safety checks were made. The
user separately confirmed that its FNA build runs on Linux. That gives the future
CNA port a practical FNA/OpenGL reference oracle that the initial audit did not
have.

The modern source does **not** remove the C#-to-C++ port or the hard rendering
work. It does remove or materially reduce four unusually uncertain work packages:

- all 57 legacy `.x` models have checked-in `.glb` replacements;
- 57 checked-in JSON `.material` sidecars preserve effect, technique, parameter,
  texture, and ordered mesh-part bindings;
- real runtime-used `.xgs`, `.xsb`, and `.xwb` banks are present;
- runnable FNA and MonoGame implementations replace the missing visual/audio
  oracle.

Accordingly, the old custom `.x` converter, proposed monolithic `RacingPackage`,
manual material reconstruction, and XACT-bank acquisition tasks are
**superseded**. A realistic one-backend estimate falls from **930–1,250 hours
(central estimate about 1,100)** to **690–880 hours (central planning estimate
about 780)**. The revised central split is approximately **522 hours of Racing
port/tooling**, **230 hours of reusable CNA work**, and **25 hours of remaining
baseline/reference analysis**. These are supervised engineering hours, not
calendar time.

The largest architectural decision from this audit, custom effects, is closed by
live CNA's XNA/FNA Effect Framework bytecode path. MonoGame `MGFX` remains separate
and unsupported, but is not the Linux reference asset.

No gameplay C++ or asset conversion exists yet. Milestone 0 performed executable
reference/content qualification and one reusable CNA renderer fix.

## Evidence convention and source baselines

The document labels conclusions as follows:

- **FACT** — directly observed in tracked source, Git metadata, binary headers,
  or a user-confirmed local run.
- **INFERENCE** — a conclusion supported by observed evidence but not directly
  encoded or executed.
- **UNKNOWN** — evidence remains insufficient and requires a focused experiment
  or provenance decision.

### Current source hierarchy

| Role | Exact baseline | Use |
|---|---|---|
| Modern primary | `/rv/tmp/RacingGame`, `d8092633e4e43e014ff168d8e913a9373538b851` | Current gameplay implementation, GLB/material/raw assets, effects, generated XACT banks, FNA/MonoGame behavior |
| Older comparison | `/rv/tmp/XNAGameStudio/Samples/XNA-4-Racing-Game-Kit-master` | Original XNA 4 conversion, `.x` hierarchy/material processor, historical shader and platform behavior |
| CNA implementation baseline | `../cnanext`, `51d61ef42d1105d97387feeba11eae91a2f3e2e9` | Active modular runtime, OPENGL33/EasyGL compiled Effects, glTF/CNB, XACT and tests |

The pre-modularization CNA rows below are retained only as historical context.

## OLD XNA 4 SOURCE FINDINGS

The initial audit remains useful history. Its source and gameplay inventory,
render-pass map, physics/screen architecture, custom vertex layouts, ten-effect
scope, `Rgba64` use, DDS normalization-cube issue, and C# translation risks remain
valid. The older `.x` files and `RacingGameModelProcessor` are still valuable for
checking how hierarchy, pivots, tangents, material metadata and technique semantics
were originally produced.

The following initial conclusions are now historical rather than current blockers:

- the future port must build a 57-model `.x` converter;
- hierarchy/material/technique metadata must be reconstructed into a custom
  `RacingPackage`;
- generated XACT banks must be found or recreated;
- no runnable implementation can serve as an oracle;
- code licensing has no repository-level statement.

They were reasonable conclusions for the evidence then available. They are marked
superseded, not silently erased.

## MODERN RDS1983 SOURCE FINDINGS

The modern repository supplies direct replacement evidence: recognizable gameplay,
57 GLBs, 57 material sidecars, raw assets, modernized HLSL plus FNA/MonoGame effect
binaries, generated XACT banks, FNA and MonoGame project variants, a repository
license, and a user-confirmed working Linux/FNA run. Its actual runtime accesses
named nodes/parent bones, sidecar-bound Effects/techniques/parameters, and the
generated banks, so these are not merely unused conversion artifacts.

It also introduces new facts/risks: sibling dependencies are not revision-locked;
DigitalRiseModel/XNAssets behavior must be replaced or adapted; effect binaries are
backend-specific rather than CNA-portable; the material adapter has a scalar JSON
quirk; mobile/Web packaging, memory and controls were never part of this desktop
port; and asset-level provenance is still not itemized.

## CURRENT RECOMMENDATION

Use the modern repository as the primary implementation/content/runtime reference
and the older source as historical/original-behavior validation. Consume validated
GLB + `.material` + raw assets; do not revive `.x` conversion or the custom package.
After CNA modularization, implement a CNA-owned portable Effect runtime and the
bounded generic model/glTF semantics Racing needs. Complete Linux `OPENGL33` first,
then qualify Windows `OPENGL33`, Android `OPENGLES`, and Web `WEBGL2` as separate
platform milestones.

## Repository provenance and modernization

### Relationship to the old project

**FACT:** `README.md` identifies the project as a port of the XNA 4 Racing Game
Kit to FNA and MonoGame without the XNA Content Pipeline. Source comparison backs
that statement:

- the modern tree has 63 C# files and about 23,642 physical C# lines, versus 58
  files and about 23,428 lines in the older audited source;
- after whitespace normalization, 33 of the 58 old files are essentially
  unchanged, 24 are changed, six are new, and only generated `AssemblyInfo` is
  removed;
- the same race, car, track, screen, rendering, and sound class structure remains
  visible;
- 185 non-model/non-shader raw assets compared byte-for-byte are identical.

This is a direct continuation/port of the same game, not a different racing game
with similar assets.

### Git history

**FACT:** the repository has 22 commits between the first commit on 2024-09-12
and the inspected head on 2025-10-01. Important history includes:

- `55c2bae` — first commit, including modern and legacy model forms and generated
  audio banks;
- `a428f5c` — license files added;
- `27ddf18` — substantial HLSL refactoring;
- `e0a7008` / `45930d8` — MonoGame addition and working implementation;
- `76897a6`, `75b5365` — DigitalRiseModel adoption/update;
- `95299b3` — temporary NursiaModel move;
- `d809263` — return to DigitalRiseModel.

The short NursiaModel excursion changed only a small adapter surface. This is
evidence that the model library is replaceable and is not embedded throughout the
gameplay code.

### Projects and dependencies

| Variant | Target | Important references | Status |
|---|---|---|---|
| `RacingGame.FNA.Core.csproj` | `net8.0` | sibling FNA, XNAssets, DigitalRiseModel sources | Cross-platform design; Linux run confirmed by the user |
| `RacingGame.MonoGameDX.csproj` | `net8.0-windows` | MonoGame WindowsDX 3.8.4, DigitalRiseModel.MonoGame 0.6.0 | Practical Windows/DX11 reference |

There is no separate MonoGame OpenGL game project in the inspected head, although
OpenGL effect-build scripts exist. The FNA variant is the best oracle for CNA's
`OPENGL33` path because it retains XNA-like Effect and XACT behavior on a portable
OpenGL stack. MonoGame WindowsDX is a useful secondary backend comparison.

Important dependency evidence:

| Dependency | Inspected/reference version | Racing role |
|---|---|---|
| FNA | sibling source project, revision not pinned | FNA framework/runtime variant |
| MonoGame.Framework.WindowsDX | 3.8.4 | Windows MonoGame framework variant |
| DigitalRiseModel | 0.6.0 package/project reference | GLB model graph/buffers/parts/bounds; no renderer |
| XNAssets | date-adjacent 0.7.7 upstream | loose textures/cubes/effects and loader registration |
| AssetManagementBase | date-adjacent 0.7.1 upstream | relative load contexts, path resolution and managed cache |
| StbImageSharp | 2.30.15 through XNAssets | ordinary image decode |
| DdsKtxSharp | 1.1.0.1 through XNAssets | DDS 2D/cube decode |
| Cyotek.Drawing.BitmapFont | 2.0.4 through XNAssets | dependency present, but Racing does not use XNAssets SpriteFont loading |
| Newtonsoft.Json | 13.0.4 through DigitalRiseModel | library-internal GLB JSON handling |
| `System.Text.Json` | .NET 8 framework | Racing `.material` parsing |

The repository does not use submodules or a dependency lock for the sibling FNA,
XNAssets, DigitalRiseModel, and DdsKtxSharp sources. **UNKNOWN:** the exact
dependency SHAs used by the author's last successful build. Date-adjacent upstream
revisions inspected as supporting evidence were:

- AssetManagementBase `18fb0bc0434422022150fa14ca3a039546920505`;
- XNAssets `7ae71d0c8adf974c1ca59579d23bc099dcb1386c` (package 0.7.7);
- DigitalRiseModel `ff6b72f46110b09cb2b226a6ef314fa7dd534b02`
  (package 0.6.0).

The audited agent did not launch or repair the reference build because another
agent was already doing that work. **FACT (user-confirmed):** the FNA game runs on
Linux from the local setup. The clean Racing checkout and modern assets therefore
provide a usable oracle, while reproducible dependency pinning remains a future
baseline task.

### Key evidence paths in the modern checkout

All paths below are under `/rv/tmp/RacingGame`:

| Evidence | Path |
|---|---|
| provenance/build instructions | `README.md`, `LICENSE.txt`, `RacingGame.FNA.Core.sln`, `RacingGame.MonoGameDX.sln` |
| target/framework references | `RacingGame/RacingGame.FNA.Core.csproj`, `RacingGame/RacingGame.MonoGameDX.csproj` |
| asset-manager adapters | `RacingGame/AMBExtensions.cs`, `RacingGame/AMBExtensions.EffectInfo.cs`, `RacingGame/EffectInfo.cs`, `RacingGame/ModelInfo.cs` |
| model use/drawing | `RacingGame/Graphics/Model.cs`, `RacingGame/Utilities/ModelUtils.cs`, `RacingGame/Landscapes/Landscape.cs` |
| controls/physics | `RacingGame/GameLogic/Input.cs`, `RacingGame/GameLogic/CarPhysics.cs` |
| XACT runtime | `RacingGame/Sounds/Sound.cs`, `RacingGame/Assets/Audio/` |
| model/material assets | `RacingGame/Assets/Models/` |
| compiled effects | `RacingGame/Assets/Shaders/FNA/`, `RacingGame/Assets/Shaders/MonoGameDX/` |
| effect sources/build paths | `RacingGame/EffectsSource/*.fx`, `RacingGame/EffectsSource/{FNA,MonoGameOGL,MonoGameDX11}/` |

## Architecture delta

### Classification by subsystem

| Subsystem | Classification | Evidence and planning effect |
|---|---|---|
| `CarPhysics` | Essentially unchanged | Simulation formulas and tuning remain recognizable; old translation estimate still applies |
| `Player` / `BasePlayer` | Essentially unchanged | Control, state, lap and camera integration remain the same |
| `ChaseCamera` | Essentially unchanged | Spring-camera behavior carries forward directly |
| `Replay` | Mechanically modernized | Gameplay logic remains; paths and persistence conditionals changed; the old `replayFileFound` quirk remains |
| `TrackLine`, `TrackVertex`, guard rails, columns | Essentially unchanged | Custom geometry and collision/path assumptions remain valid |
| `Track`, `TrackData`, `TrackCombi` | Mechanically modernized | `Content` calls became raw-asset calls; raw track/combi data is unchanged |
| `Landscape` | Mechanically modernized | Asset/model access changed; terrain and draw architecture remain recognizable |
| screens/UI | Essentially unchanged | Screen lifecycle and menu/race flow remain valid; asset access is modernized |
| rendering wrappers | Significantly refactored at the asset boundary | `Model`, `Texture`, manager and shader wrappers now bridge raw assets/DigitalRiseModel, while pass ordering remains familiar |
| shader wrappers | Mechanically modernized plus new adapters | Named techniques and parameters remain; `EffectInfo`, `ModelInfo`, and adapter helpers are new |
| sound | Essentially unchanged | Still constructs XACT engine, wave bank and sound bank and uses named cues/categories/variables |
| persistence | Mechanically modernized with variant divergence | FNA retains file persistence; several MonoGame paths are conditionally disabled |
| input | Essentially unchanged | Keyboard/gamepad/mouse surface remains; existing one-sided mouse-control behavior is not corrected |
| Windows Forms/GamerServices integration | Removed | No longer part of the primary route |
| XNA Content Pipeline | Replaced by new dependencies and checked-in assets | XNAssets + DigitalRiseModel + GLB/material/raw files |

### What remains expensive

The modern source is still approximately a 23.6k-line C# application. Translation
must preserve C# initialization behavior, collections, nullable/reference
assumptions, content lifetime, render state, deterministic physics, replay data,
and screen transitions. The new repository lowers uncertainty; it does not turn
the game into a small demo.

MonoGame and FNA contain a few deliberate branches. For example, settings/replay
load/save and screenshot behavior are not identical between variants. The future
port should use FNA as the primary current implementation, compare disagreements
with the older source, and record intentional CNA platform choices rather than
blindly carrying every preprocessor branch.

## Content pipeline delta

### Inventory

**FACT:** the modern repository has 441 tracked files. Relevant counts are:

| Kind | Count | Notes |
|---|---:|---|
| `.glb` | 57 | exact basename match for all 57 old `.x` models |
| `.material` | 57 | one JSON sidecar per GLB |
| `.x` | 0 | legacy files are absent at inspected head |
| `.tga` | 129 | loose textures |
| `.wav` | 28 | authored XACT source waves |
| `.fx` | 10 | modernized effect source |
| `.efb` | 20 | ten FNA and ten MonoGameDX compiled effects |
| `.Track` / `.CombiModel` | 3 / 10 | raw game data |
| `.DDS` | 2 | sky DXT1 cube and 24-bit normalization cube |
| `.xgs` / `.xsb` / `.xwb` / `.xap` | 1 each | generated runtime banks plus source project |

### Models and hierarchy

All 57 old `.x` basenames have exactly one modern `.glb` equivalent. No source
`.x`, `.gltf`, or external `.bin` files remain at head. The GLBs report glTF 2.0
and Assimp 5.2.0 generation.

Aggregate GLB facts:

- 116 nodes, 100 meshes, and 106 primitives;
- 95 matrix-authored nodes and 59 non-root nodes;
- 32-bit indexed primitives throughout;
- positions and normals throughout, UVs on 105 primitives;
- custom `_TANGENT` and `_BINORMAL` VEC3 float attributes on every primitive;
- no skins or animation clips; game animation is node-transform driven;
- no embedded textures; sidecars and loose textures are the intended binding path;
- non-required `KHR_materials_pbrSpecularGlossiness` and `FB_ngon_encoding`
  declarations, with no required extensions.

The hierarchy and pivots needed by gameplay are present and actively consumed:

- the car contains named `Car`, `Glass`, and four wheel children; wheel matrices
  preserve translations/scales and are used through parent-bone transforms;
- the windmill contains `Windmill_Wings`; runtime code finds that mesh by name and
  rotates its parent bone around the preserved pivot;
- render code calls the model/bone equivalent of
  `CopyAbsoluteBoneTransformsTo`, uses each mesh's parent bone, and assumes car
  mesh-part ordering.

DigitalRiseModel recomputes a bounding box for every mesh part from indexed
positions. Racing then derives a radius with its own formula. Bounds are not stored
as an authoritative old XNA `BoundingSphere`; this is a small behavior-difference
area to compare visually/collision-wise.

### Current CNA glTF gap

CNA can parse GLB and ordinary indexed vertex data. That is not yet equivalent to
loading these models correctly. In the inspected integration code, unskinned
meshes are grouped without retaining the complete scene-node identity/transform
graph; primitives become synthetic meshes/bones; only standard `TANGENT` is
recognized; material binding and original part grouping are lost; bounds are not
equivalent. The sky cube's VEC3 texture coordinate is also outside the currently
assumed VEC2 path.

Therefore:

- **FACT:** no `.x` importer or batch model conversion is needed for Racing;
- **FACT:** CNA needs a node-preserving model/glTF path or a narrow adapter;
- **INFERENCE:** this is a small-to-medium general model-module enhancement, much
  smaller and safer than the old conversion/package plan;
- **UNKNOWN:** exact rendering/collision parity until the car, windmill, alpha
  model, and sky cube are proven in a CNA harness.

The required generic model behavior is bounded: preserve unskinned node names,
parents, matrices and instances; group a mesh's primitives as ordered parts; expose
material name/index hooks; compute bounds; accept/repack `_TANGENT`; and handle the
cube's deliberate VEC3 coordinate. Porting all DigitalRiseModel features is not
justified.

### Material sidecars

Each `.material` file is JSON with two top-level maps:

- `Materials`: material name → `Effect`, `Technique`, `Parameters`;
- `MeshesMaterials`: GLB node/mesh name → ordered material-name array.

Across the 57 files there are 92 material records (82 active and 10 null
placeholders), 101 mesh mappings, and 94 active ordered part bindings. Every GLB
mesh node has a sidecar mapping key, and non-empty mapping lengths agree with the
primitive counts. The one unused cube material is irrelevant because that sky
geometry follows a direct path.

The sidecars already externalize nearly everything the old audit proposed to
reconstruct in `RacingPackage`:

- effect name and technique name;
- parameter values;
- diffuse, normal, normalization-cube, and reflection-cube references;
- ordered mesh-to-material/mesh-part mapping;
- alpha factor where used;
- car hue and shadow color parameters;
- light, ambient, diffuse, specular, and shininess values.

Observed active techniques are `Diffuse20`, `Specular20`,
`DiffuseSpecular20`, `SpecularWithReflection20`, and `ReflectionSpecular20`.
Active materials use `NormalMapping` or `ReflectionSimpleGlass`.

Not everything is explicit. Alpha-model handling is still inferred from a model
filename prefix; blend/double-sided flags, bounds, schema version, conversion
provenance, and checksums are absent. Also, all numeric `lightDir` entries are JSON
numbers, while the inspected adapter handles string and array cases but no numeric
case, so those values are silently skipped. Dynamic light updates/defaults may mask
that quirk, but CNA should use a typed validated schema rather than reproduce it.

**Current recommendation:** use checked-in **GLB + `.material` + raw assets** as
the future content contract. Add a small validated schema/manifest if needed; do
not repack geometry into a Racing-only binary and do not preserve the modern
adapter's technique-index digit hack.

## XNAssets analysis

Racing uses only a narrow part of XNAssets/AssetManagementBase:

| Used behavior | Actual use | CNA disposition |
|---|---|---|
| path/context resolution | relative references in material/effect/model loads | `PORT-SIDE WRAPPER`; a general canonical raw-asset stream API would be useful after modularization |
| cache | shared loaded objects keyed by asset/settings | `NEW GENERAL CNA FEATURE WOULD BE USEFUL`; use explicit RAII/lifetime rather than copying the managed dictionary design |
| `ReadAsString` | JSON material files | `CNA ALREADY HAS EQUIVALENT`/ordinary file stream |
| `LoadTexture2D` | loose TGA/PNG, premultiplication options | `CNA ALREADY HAS EQUIVALENT`, subject to focused format tests |
| `LoadTextureCube` / polymorphic texture | two DDS cube maps from material parameters | `SMALL CNA GAP` for the supplied 24-bit RGB normalization cube |
| `LoadEffect` | reads `.efb`, constructs framework `Effect` | `THIRD-PARTY FORMAT STILL UNSUITABLE`; use the CNA effect strategy below |
| `LoadGltf` | delegated to DigitalRiseModel | `SMALL/MEDIUM CNA MODEL GAP`, not a reason to port XNAssets |
| SpriteFont / SoundEffect loaders | not used by Racing | no Racing requirement |

XNAssets uses StbImageSharp for ordinary images and DdsKtxSharp for DDS/KTX;
AssetManagementBase provides automatic load context and managed-object caching.
Its `Unload()` clears the cache but does not define the explicit GPU destruction
model a C++ framework needs.

**Recommendation:** do not port XNAssets wholesale and do not build a second CNA
content framework solely for Racing. After modularization, provide a small general
raw-asset module (canonical path, stream/provider, typed cache key, explicit
lifetime) and implement a narrow Racing material binder on top. This also avoids
hard-coding the modern repository's mixed-case path conventions into framework
policy. Although those conventions did not prevent the user-confirmed Linux/FNA
run, CNA should canonicalize its own contract.

## DigitalRiseModel analysis

DigitalRiseModel is a loader/model representation, not Racing's renderer. Racing
uses only XNA-like model types and capabilities:

- `DrModel`: bones, roots, meshes, absolute-transform copying;
- `DrModelBone`: name, parent/children, default/animated transform;
- `DrMesh`: name, parent bone, ordered parts;
- `DrMeshPart`: vertex/index buffers, material name, bounds, draw;
- material-name lookup for sidecar binding.

Its glTF loader preserves nodes/names/matrices/parents, recognizes both `TANGENT`
and `_TANGENT` plus `_BINORMAL`, supports 32-bit indices and VEC3 texture
coordinates, unwinds the expected winding, and recomputes bounds. It also contains
skins/animations and other features unused by this game. Racing owns the render
loop: it applies the selected Effect passes and draws each part.

Replacing this dependency with a CNA model adapter is straightforward in concept
but not zero work because CNA's current generic glTF representation drops exactly
the node/part semantics Racing consumes. The clean solution is to improve CNA's
model/glTF module to the bounded requirements above, then write a thin game-side
adapter. Reimplementing or vendoring all DigitalRiseModel would add licensing and
maintenance surface without a Racing benefit.

The checked-in GLBs and the proven DigitalRise behavior **substantially reduce the
old content risk** even though CNA integration still requires work.

## Effects and shaders

### Modern source and artifacts

All ten original effect names remain:

```text
LandscapeNormalMapping, LightingShader, LineRendering, NormalMapping,
PostScreenGlow, PostScreenMenu, PostScreenShadowBlur,
PreScreenSkyCubeMapping, ReflectionSimpleGlass, ShadowMap
```

The repository commits 20 binaries: ten under FNA and ten under MonoGameDX. FNA
files have the DX9 Effect binary signature and are built by scripts invoking
`fxc /T:fx_2_0`; MonoGameDX files have the `MGFX` signature and are built with
`mgfxc /Profile:DirectX_11`. MonoGame OpenGL scripts invoke
`mgfxc /Profile:OpenGL`, but their output is not committed and no matching game
project is present. Build scripts contain author-machine absolute paths, so they
are evidence of the toolchain, not reproducible build automation.

Commit `27ddf18` significantly refactored HLSL: old assembly/low-profile fallbacks
were removed, shared macros were introduced, and modern shader profiles were used.
The game-facing technique/pass/parameter model and important multi-pass behavior
remain: glow, menu composition, two-pass blur, shadow generation/application,
normal/specular/reflection variants, sky, glass, lines, landscape, and lighting.

Runtime selects the FNA or MonoGameDX binary directory through compile-time
conditionals, loads a normal framework `Effect`, chooses named techniques, sets
named parameters, and calls every pass's `Apply()` before drawing. The modern port
therefore confirms that custom Effect semantics are central rather than obsolete.

### CNA comparison

CNA already has `EffectParameter`, `EffectTechnique`, `EffectPass`, and collection
types used by stock effects. It does not yet have a complete portable path that
loads/populates an arbitrary custom Effect with Racing's named parameters,
techniques, pass shaders, render states, and backend resources. Existing compiled
custom-effect XNB support is intentionally incomplete, and `ShaderEffect` alone
does not supply the XNA object model.

### Strategy comparison

| Strategy | Estimated additional scope | Benefit beyond Racing | Backend/architecture assessment | Order |
|---|---:|---|---|---|
| **A. Racing-specific explicit shaders** | Racing 110–190 h; CNA 10–30 h | Low | Fastest first image, but duplicates technique/parameter dispatch in game code and scales poorly to more XNA samples/backends | Accept only as a throwaway proof or very thin binding layer |
| **B. CNA-owned portable Effect runtime** | CNA 120–220 h; Racing shader assets/bindings 60–110 h | High | Clean named Effect API with backend shader modules; preserves XNA usage without tying CNA to one compiled format | **Recommended after modularization and before full Racing rendering** |
| **C. Consume/translate FNA or MGFX binaries** | 20–40 h research, then roughly 180–400+ h if pursued | Medium but format-specific | FNA DX9 bytecode/MojoShader and MGFX are different, OGL artifacts are absent, licensing/toolchain/backend coupling is high | Research spike only; do not make it the plan |
| **D. Compile legacy/direct `.fx`** | Roughly 400–1,000+ h | Potentially high long-term | Requires a real FX/HLSL compiler, profiles, reflection, states, and cross-backend translation; disproportionate for Racing | Separate long-term capability, not a Racing prerequisite |

Strategy B should define a CNA-owned portable effect description containing named
parameters, techniques, passes, state requirements, and references to
backend-specific shader modules. `CurrentTechnique`, collections, parameter
updates, and `Apply()` should live above graphics backends; compilation and binding
belong in backend adapters. Racing's ten effects still need a semantic shader port,
but game code can remain close to FNA/XNA and future samples can reuse the runtime.

This work should follow CNA modularization so the public Effect model is not
entangled with one backend. A full FX compiler or direct FNA/MGFX loader should not
be implemented merely for this game.

## Audio and XACT

### Generated banks are present and used

The old “obtain or generate authentic Racing banks” task is superseded. The modern
tree contains:

| File under `RacingGame/Assets/Audio` | Size | SHA-256 |
|---|---:|---|
| `RacingGameManager.xgs` | 510 | `f45fc8a923fd10746eb5dc512c7989aa60dc3194fe27576b74868983f5298087` |
| `Sound Bank.xsb` | 1,535 | `bce693991f9347a619c89623028f3746884c9dc93fde8c529149102b376a38af` |
| `Wave Bank.xwb` | 18,779,556 | `62d95e9dfbc22af5838c1f9d0ae13137a1312083f2682e781071e6350a78e5a3` |
| `RacingGame.xap` | 69,573 | `f49b42dc2546c62b4b0464dbdef1b9f234a907811d29316d8060d2cd2a8a4902` |

Binary headers identify version-46 XACT data (`XGSF`, `SDBK`, `WBND`). The XAP and
all 28 WAVs are byte-identical to the older audited source. `Sounds/Sound.cs`
constructs `AudioEngine`, `WaveBank`, and `SoundBank` from the exact three generated
filenames and uses the expected cue/category/variable names. The banks were present
from the modern repository's first commit.

This makes the banks immediately useful as **local compatibility inputs and
reference fixtures**. CNA source reports version-46 parsing and substantial XACT
behavior, but this audit did not run these banks through CNA. The first post-
modularization XACT gate must load all three, enumerate/resolve authored objects,
and play representative music, engine/gear, skid/brake, crash, and UI cues while
comparing variables/categories with FNA.

**UNKNOWN:** exact XACT tool version and generation log; **INFERENCE:** identical
XAP/WAV sources plus runtime cue names strongly indicate the banks correspond to
the supplied project. Before copying banks into CNA's own test repository, confirm
redistribution/provenance policy. Keep full banks as opt-in/local fixtures if that
policy is not settled.

## Other graphics and platform findings

- `Rgba64` remains relevant: unchanged Racing render-target code still requests
  it, while CNA capability reporting and construction were contradictory in the
  inspected baseline. Fix/test this after modularization.
- `SkyCubeMap.DDS` is a 512×512 DXT1 cube and fits CNA's current loose path.
  `NormalizeCubeMap.DDS` is an uncompressed 24-bit RGB 128×128 cube and remains a
  genuine loose-DDS cube gap.
- Track, combinational-model, height, font, and other raw files need only narrow
  readers; they are not arguments for an offline package.
- `System.Text.Json` is used only for material sidecars. CNA can use its chosen JSON
  library behind the content/material module; framework API emulation is not needed.
- Input remains ordinary keyboard/gamepad/mouse. UI uses the game's bitmap texture
  font, not XNA SpriteFont or XNAssets SpriteFont.
- Storage should preserve settings and replay semantics in a platform storage
  adapter. Windows Forms and GamerServices are no longer required.

## Target-platform feasibility

The **690–880 hour realistic estimate covers one Linux `OPENGL33` reference
implementation only**. C++ and CNA are portable foundations, but a compiled game
does not automatically become a supported Windows, Android, and Web product. Each
target needs its own graphics profile, packaging, audio/storage, input, lifecycle,
performance, and acceptance gates.

The newer `../cnaintegration` evidence is especially relevant here. Its GL family
exposes `OPENGL33`, `OPENGLES`, `WEBGL2`, and `WEBGL1` over one internal EasyGL
implementation. `OPENGL33` and `OPENGLES` are real-driver tested on Linux;
`WEBGL2` configures/builds/links through Emscripten but still lacks a real-browser
Racing-class rendering proof. `OPENGL33` has not been qualified on Windows, and
Android graphics/sensors have not been qualified with this game on physical
hardware. These are evidence levels, not reasons to duplicate game logic.

| Target | Recommended backend | Feasibility | What prevents an unconditional “yes” today |
|---|---|---|---|
| Linux desktop | `OPENGL33` | **Primary target; high** | Complete Racing Effect/model/XACT integration and parity work still has to be implemented |
| Windows desktop | `OPENGL33` first; D3D11 only as a later native option | **High after Linux** | Windows GL-driver qualification, packaging, paths/storage/audio; D3D11 would require another shader-module set |
| Android | `OPENGLES` (GLES 3.0), landscape | **Feasible with substantial platform work** | APK/Gradle lifecycle, real-device GPU testing, GLES shader/format fallbacks, asset memory/package size, XACT playback proof, touch UI |
| Web | `WEBGL2` | **Feasible but highest risk** | Real-browser shader/render-target proof, 289 MiB source asset delivery, Wasm memory/startup, browser audio gesture/XACT proof, persistent storage and browser matrix |

Confidence by target is **medium-high for Linux**, **high-feasibility but not yet
qualified for Windows**, **medium for Android**, and **medium-low for Web**. These
ratings assume CNA modularization lands and the recommended GL-family/Effect design
is adopted.

`SDL_RENDERER` and `CANVAS` are 2D-oriented paths and cannot render Racing's custom
3D/multi-pass effects. `WEBGL1` is also not a sensible minimum for this game. Android
should explicitly select `OPENGLES`; Web should require WebGL 2.

### Shared graphics strategy across platforms

Racing's shaders use vertex/fragment stages and features representable in the
GLSL ES 3.00 / GLSL 3.30 common subset. The recommended portable Effect asset
should therefore keep one semantic technique/pass graph and, where practical, one
shared GLSL ES 3 body adapted to:

- GLSL 330 core for Linux/Windows `OPENGL33`;
- GLSL ES 300 for Android `OPENGLES` and Web `WEBGL2`.

Do not assume identical render-target formats. `Rgba64`, shadow depth/stencil,
floating/half-float filtering, cube sampling, and readback must be capability-gated.
Mobile/Web quality presets should be able to use `Color` render targets, smaller
shadow/post-process targets, disabled post effects, and reduced texture residency
without changing simulation.

### Asset and performance implications

The checked-in assets occupy about 289 MiB. The 140 TGA/PNG textures contain
56,967,168 pixels: approximately 217.3 MiB as base-level RGBA8 or 289.8 MiB with
a complete mip chain, before render targets, model buffers, decoded audio, and
driver duplication. Maximum texture dimensions are 2048×2048, which is reasonable;
aggregate residency and delivery size are the issue.

- Desktop can initially load the canonical loose assets.
- Android needs measured residency, loading screens, lifecycle/context-loss
  restoration, and likely platform packaging plus optional downscaled/compressed
  quality sets before release.
- Web should not make a monolithic 289 MiB preload the production contract. It needs
  compression, cacheable/lazy delivery or content groups, progress/error UX, and a
  strict memory budget. These are derived platform assets; the canonical audited
  GLB/material/raw sources remain unchanged.

### Android controls

The modern game has no touch or sensor input; it reads keyboard, mouse, and
gamepad. Car physics consumes a conceptually simple control set: analog/digital
steering, throttle, deceleration/reverse, stronger brake/handbrake, pause/back, and
optional camera-distance controls. CNA already exposes multi-touch through
`TouchPanel`, maps Android Back to `Keys::Escape`, and has an Android accelerometer
implementation with landscape-axis remapping. Therefore the required work is
mostly a **game-side logical-input and overlay layer**, not a new CNA input API.

Recommended landscape control modes:

| Control | Default touch mode | Optional tilt mode |
|---|---|---|
| steering | left-thumb analog steering pad/wheel, `[-1,+1]` | calibrated accelerometer roll, filtered/deadzoned/clamped |
| throttle | large hold button/pedal at lower right | same touch pedal |
| brake/reverse | large hold button beside/above throttle | same touch pedal |
| handbrake | smaller reachable button | same button |
| pause/back | top-corner button plus Android system Back | same |
| camera | optional small toggle cycling presets | same |
| menus | direct touch on existing visible controls | direct touch |

Touch must support simultaneous steering and pedals, safe areas, DPI/aspect scaling,
left-handed layout, adjustable size/opacity, and Bluetooth gamepads. Existing menu
mouse hit rectangles can share a pointer-action adapter, but gameplay controls need
true multi-touch and must not synthesize a single mouse.

Accelerometer steering should be **optional, never the only Android scheme**. It
needs a neutral calibration at race start/on demand, low-pass filtering, dead zone,
sensitivity and inversion settings, orientation-change handling, and a touch
fallback when the sensor is unavailable. Tilt supplies steering only; throttle,
brake, handbrake, pause, and menu actions still require touch/gamepad. CNA currently
excludes its Accelerometer API on Web, and browser sensor permissions vary, so tilt
should not be a Web launch requirement.

The clean implementation boundary is a game-owned `RacingControls` snapshot
(`steering`, `throttle`, `brakeReverse`, `handbrake`, edge-triggered UI actions)
fed by desktop, gamepad, touch, and optional tilt providers. Physics consumes that
snapshot and remains platform-independent.

### Incremental platform estimates

These are additional hours **after** the Linux `OPENGL33` implementation, and do
not include broad CNA modularization already counted above:

| Additional target | Optimistic | Realistic | Pessimistic | Scope |
|---|---:|---:|---:|---|
| Windows `OPENGL33` | 25–50 | 50–90 | 120–200 | toolchain/package, real drivers, paths/storage, audio/input, captures |
| Android `OPENGLES` | 100–160 | 180–300 | 360–550 | APK/lifecycle, GLES effects/formats, touch+optional tilt, assets, XACT, device/perf matrix |
| Web `WEBGL2` | 140–220 | 250–420 | 500–800 | Emscripten/browser, content delivery/memory, WebGL effects, audio gesture/XACT, storage/test matrix |
| **All three, de-duplicated** | **250–380** | **430–700** | **850–1,300** | shares GL shader bodies, logical input, quality tiers, and platform asset work |

Thus a realistic all-four-platform program is approximately **1,120–1,580 hours**
in total, versus 690–880 hours for the single Linux reference target. A native
D3D11 Windows renderer, broad browser compatibility, and store/release operations
would add scope beyond these bands.

## Licensing and provenance

This is an engineering/provenance assessment, not legal advice.

**FACT:** the modern repository contains `LICENSE.txt` and
`Microsoft Permissive License.rtf`; 56 C# files retain Microsoft XNA Community Game
Platform notices. The license was explicitly added in repository history. This
substantially improves the code-license evidence over the older loose archive.

Current classification: **substantially reduced; resolved for code only, assets
still require provenance confirmation**.

Reasons:

- repository code and modifications are distributed with a stated Microsoft
  Permissive License;
- models, textures, sounds, music, and generated banks do not have a per-asset
  manifest explaining original authorship or redistribution;
- GLB conversion and `.material` creation are additional repository contributions,
  but the conversion tool/version is only visible in GLB metadata and no generation
  script/license manifest is committed;
- generated XACT banks add tool-generated artifacts without a generation record;
- XNAssets/AssetManagementBase are MIT in inspected upstreams; DdsKtxSharp is
  BSD-2-Clause; the DigitalRiseModel repository license is BSD-3-Clause while its
  package metadata at the inspected revision says MIT, a contradiction to clarify
  if any of that code is copied. The recommended adapter approach avoids shipping
  DigitalRiseModel itself.

Before redistribution, preserve notices, map every shipped asset to a source and
license basis, and clarify whether generated GLB/material/bank artifacts may be
redistributed. This no longer blocks private feasibility work but remains a release
gate.

## CNA modularization implications

Racing supplies concrete evidence for the following boundaries; it does not justify
a broad redesign beyond them:

| Future boundary | Racing-driven requirement |
|---|---|
| core/game and math | No special change; keep simulation independent of rendering/content |
| graphics API | Resource/state/draw abstractions and render targets, including truthful `Rgba64` capability reporting |
| graphics backends | Backend shader modules, vertex layout realization, state binding; no game techniques here |
| effects/shaders | Portable Effect object model and description above backend programs |
| content/raw assets | Canonical path/stream/provider, typed cache, explicit lifetime; no Racing monolith |
| XNB | Separate optional compatibility module; not the primary Racing content route |
| model/glTF | Scene nodes, transforms, ordered parts, bounds, material hooks, custom tangent repack |
| audio/XACT | XACT compatibility module with authentic opt-in fixtures |
| input | Existing platform-neutral input plus game-side mapping |
| storage/platform | Save/replay/screenshot adapters; no GamerServices dependency |

All Racing implementation starts **after CNA modularization/stabilization** and
against one pinned commit.

## Feasibility verdict, estimates, and confidence

### Revised work-package estimate

The following are scenario point estimates in supervised engineering hours. Rows
that include both sides show the central ownership split in parentheses.

| Work package | Optimistic | Realistic | Pessimistic | Principal evidence/change |
|---|---:|---:|---:|---|
| C# → C++ translation | 170 | 230 | 320 | ~23.6k physical lines; much gameplay is unchanged |
| Gameplay correctness | 25 | 45 | 80 | FNA oracle reduces inference but physics/replay parity still needs tests |
| GLB/model integration | 40 | 75 | 135 | Realistic: Racing 25 + CNA 50; converter/package removed |
| Material loading | 10 | 18 | 32 | Checked-in sidecars replace metadata reconstruction |
| Raw asset loading | 16 | 30 | 55 | Realistic: Racing 15 + CNA 15; mostly existing/simple readers plus RGB cube |
| Shaders/effects | 105 | 180 | 320 | Realistic: Racing 60 + CNA 120; Strategy B |
| Audio/XACT | 8 | 27 | 75 | Realistic: Racing 12 + CNA 15; banks are present but parity is unproven |
| UI/input | 20 | 35 | 60 | Source unchanged and oracle available |
| Persistence/platform | 12 | 22 | 40 | Narrow adapters; resolve FNA/MG divergence |
| Other CNA fixes/tests | 15 | 30 | 60 | `Rgba64`, render/readback/state probes |
| Testing/reference comparison | 35 | 60 | 100 | Automated captures, deterministic simulation and race/audio comparisons |
| Remaining baseline/legal analysis | 15 | 25 | 40 | Dependency pins, capture corpus, asset manifest |
| **Total** | **471** | **777** | **1,317** | One `OPENGL33` backend; additional backends excluded |

Planning bands should remain wider than point estimates:

| Ownership | Old realistic | Revised realistic | Main reason |
|---|---:|---:|---|
| Racing port/content/tooling | 750–950 | **480–600** | `.x` conversion, package/material reconstruction, bank acquisition and oracle recovery removed |
| CNA framework/tests | 140–240 | **190–240** | content uncertainty shrank, but the recommendation shifts effort into a reusable general Effect/model path |
| baseline/reference analysis | 40–60 | **20–30** | modern runnable source and checked-in artifacts answer most open questions |
| **Total** | **930–1,250** | **690–880** | one `OPENGL33` target |

| Scenario | Racing port/tooling | CNA framework | Baseline/reference | Total |
|---|---:|---:|---:|---:|
| Optimistic | 320–370 | 110–130 | 15–20 | **450–550** |
| Realistic | **480–600** | **190–240** | **20–30** | **690–880** |
| Pessimistic | 760–850 | 400–470 | 35–45 | **1,050–1,350** |

The pessimistic case overlaps/exceeds the old estimate because effect-runtime scope,
CNA integration churn, or fidelity bugs can still dominate. The realistic reduction
comes specifically from removing model conversion, material archaeology, bank
acquisition, and oracle reconstruction—not from assuming faster line translation.

### Cumulative observable checkpoints

These start only after CNA modularization and include the relevant prerequisite
CNA work:

| Checkpoint | Optimistic | Realistic | Pessimistic |
|---|---:|---:|---:|
| First rendered representative scene | 80–120 h | 130–190 h | 220–320 h |
| First drivable car | 160–220 h | 250–340 h | 420–560 h |
| First complete race | 280–380 h | 420–560 h | 680–880 h |
| Feature-complete `OPENGL33` | 450–550 h | 690–880 h | 1,050–1,350 h |

### Confidence

**Medium-high** for feasibility and the chosen content route; **medium** for the
schedule. Confidence rose because the model/material/XACT artifacts exist and a
Linux/FNA reference runs. It is not high because CNA's general Effect/model
extensions are not yet implemented, full-bank XACT parity is untested, licensing
is incomplete for assets, and CNA will be modularized before implementation.

## Current source authority by subject

| Subject | Authority rule |
|---|---|
| gameplay | Modern source primary; old XNA source breaks ties where modern backend branches diverge |
| assets | Modern raw files and GLBs primary; old files establish provenance and conversion comparison |
| model hierarchy/pivots | Modern GLB + DigitalRise runtime behavior primary; old `.x` validates conversion fidelity |
| material metadata | Modern `.material` sidecars primary; old processor/material data validates intent |
| shader behavior | Modern `.fx` and running FNA primary; old effects explain removed profiles and original formulas |
| audio | Modern generated banks/runtime primary; identical old XAP/WAVs are authored-source provenance |
| UI/platform | Modern FNA primary; old source for original behavior removed by modernization |
| original XNA-era behavior | Older source/docs and captured comparisons remain authoritative, with modern FNA as executable evidence |

This is **Option 3 (hybrid)** with `rds1983/RacingGame` unambiguously the primary
implementation reference.

## Five largest remaining risks

1. The CNA-owned Effect runtime and faithful translation of ten multi-pass effects.
2. Node/part/tangent/material semantics in CNA's GLB/model module.
3. End-to-end XACT behavior against the supplied version-46 banks, not merely parse
   success.
4. Asset redistribution/provenance and generated-artifact records.
5. CNA modularization/integration churn plus fidelity/performance bugs across a
   complete race and render-target chain.

## Five risks/work packages materially reduced or removed

1. Legacy `.x` import and custom `RacingGameModelProcessor` replacement: removed.
2. Manual hierarchy/pivot/tangent reconstruction: substantially reduced by GLB.
3. Custom `RacingPackage` and material archaeology: removed by validated sidecars.
4. Acquisition/generation of authentic XACT banks: removed; validation remains.
5. Absence of a runnable reference: removed by the user-confirmed Linux/FNA build.

## Remaining decisive questions

1. Which exact post-modularization CNA SHA will be the implementation baseline?
2. Can the minimal CNA model enhancement render the car, windmill, alpha model,
   and sky cube with node/part/bounds parity?
3. What exact portable Effect asset schema and backend shader-module ABI should be
   frozen after modularization?
4. Which supplied XACT cues/features pass CNA unchanged, and which expose true CNA
   gaps?
5. Can a repeatable FNA capture/run recipe pin all sibling dependency SHAs?
6. What asset-level provenance/redistribution record is sufficient for release?

The next action is not a port. Complete CNA modularization, pin the resulting CNA
commit, preserve a repeatable FNA reference recipe/capture corpus, and run the
minimal model/effect/XACT/render-target gates described in `plan_racing.md`.
