# SkinningSample — port notes

**Status: complete on the active heads (2026-09-25).**

The owner authorized the 2026-09-25 prune after publication. The 054 root went
from 319.2 MB to 54.3 MB, freeing 265.0 MB across 28 intermediate paths,
stripping and deduplication. Pre/post hashes of 16 retained products are in
`evidence/requal-20260925/post-prune/`; only the native executable changed
because it was stripped. Its RUNPATH remains on active libcna, and the stripped
program rendered two differing frames and exited on Escape. A repeat dry run
reports zero deletions. `MANIFEST.md` contains current rebuild commands.

## Current-head requalification — 2026-09-25

The retained `xna4-original/` is byte-identical to all 35 physical upstream files. The
unchanged Windows/Reach XNA game, its `SkinnedModel` library and its sample-owned
`SkinnedModelProcessor` rebuilt; the Windows/HiDef and Xbox/Reach content checks passed too.
All five newly generated Windows/Reach XNBs match checked-in, native and retained originals
byte for byte (hashes below). The original Microsoft license, icon, screenshot, two Phone
tiles and both Phone manifest pairs are now retained beside the translated source. The
upstream `#if WINDOWS_PHONE` constructor branch sets 333333 ticks and fullscreen in the port;
the branch passes a `-DWINDOWS_PHONE` syntax check.

Release OPENGLES3 and nonthreaded WEBGL2 were rebuilt against sibling CNA `5229c992e`
and sharp-runtime `41b918c9`. The native executable's RUNPATH points to the active
`libcna/cna` checkout. Live original XNA/WineD3D and native captures show the textured
animated character, differ across frames, and close on Escape. Frozen 0.5 and 0.9 second
diagnostics are built from an isolated copy of the current sample source. Both pairs have
**99.99% of pixels within eight RGB levels** and **100% after a four-pixel blur**, with
identical foreground bounds. Each leg's two frozen frames are byte-identical.

Holding Up, Right and X changes the camera in both the original and native builds; R
returns each to its exact neutral frozen-frame hash, and Escape closes the window. The
exact four-file gallery bundle passes real system-Chrome rendering, animation, W/D/X/R
camera input, title, Escape, required HTTP asset and runtime-error gates. Nine new gallery
routes returned HTTP 200. The copied bundle hashes match the build product, and the WASM
has no `debug_info` or pthread/shared-memory marker. No CNA or sharp-runtime source change
was required. The existing `CNAEXT` reflective-reader registration is documented in
`diff.md`; the Phone branch is a direct translation. No sample-side workaround remains.

Current evidence: `evidence/requal-20260925/` in the artifact root. Rebuild with
`scripts/build-original.sh`, `scripts/build-current.sh native|web` and
`scripts/build-diag.sh`; run `scripts/compare-frozen.sh` with `CNA_TIME=0.5` or `0.9`.
The former source-swapping diagnostic and obsolete build paths were replaced.

## Previous analysis — 2026-09-25

The gaps recorded in this section were resolved in the current-head requalification above.

No 054 source, content, artifact product or gallery file was changed, built or run in this
inspection. The historical XNA/native pixel and Chrome results below remain useful evidence,
but they are not a test of the current libcna heads.

- The retained `xna4-original/` matches all **35** physical upstream files byte for byte.
  Phone, Windows and Xbox solutions share one game class, the sample-owned `SkinnedModel`
  runtime library and `SkinnedModelPipeline` processor; they are platform variants of one
  runnable product. The game projects select Reach, while the Windows/Xbox library projects
  declare HiDef and the Phone library declares Reach. One listed `dude.fbx` passes through
  the original `SkinnedModelProcessor`; its four material textures are implicit dependencies.
- The five checked-in XNBs match the retained official Windows/Reach
  `xna4-build/bin/Content/` and native Content byte for byte. The hashes are in the table
  below. The C++ source retains the `SkinnedModel` clip/keyframe/data classes, the original
  no-interpolation player, hierarchy and inverse bind pose, `Model.Tag` load, stock
  `SkinnedEffect`, camera/input controls and game title. The sample-owned AOT field
  registration is documented in `diff.md` and uses CNA's existing generic reflective reader;
  current CNA `5229c992e` still has that API and its focused tests. Sharp-runtime is
  `41b918c9`. Source inspection found no model/shader or renderer workaround.
- **One real translation gap is visible before running:** the upstream constructor's
  `#if WINDOWS_PHONE` branch sets `TargetElapsedTime` to 333333 ticks (30 fps) and
  `graphics.IsFullScreen = true`; the C++ constructor omits it. Restore this branch with
  the existing XNA-shaped CNA setters, and syntax-check the Phone build configuration.
  The remaining game logic is common to the three targets.
- The port does not retain the physical original's Microsoft license, `Game.ico`,
  `SkinningSample.png` or either Phone tile `Background.png` beside its translated source.
  The two Phone manifest pairs and both background images remain in the exact original
  snapshot; review which ancillary files should accompany the port. `Skinning.htm` is
  already byte-identical. There is no upstream `help.png`.
- The retained native executable has a `RUNPATH` into the obsolete
  `openeggbert/cnanext` checkout; `MANIFEST.md` names the old sources. The old
  `scripts/compare-frozen.sh` swaps a checked-in source file and points at the former
  `openeggbert/cna-samples` checkout. Replace it with an isolated diagnostic source tree.
  The original/native capture scripts need a clean Escape gate and window positioning;
  the original helper also has a broad `pkill`. The gallery has no 054 card, detail page
  or browser bundle.

For requalification, restore the omitted Phone branch and original ancillary assets,
rebuild the unchanged XNA Windows/Reach game and processor, and verify all five generated
XNBs. Rebuild Release OPENGLES3 and nonthreaded WEBGL2 against the current sibling
checkouts. Test animation, camera rotation/zoom/reset and Escape against live XNA/native
behavior; use isolated frozen 0.5/0.9 s captures for raster and pose comparison. Exercise
the exact browser product in system Chrome, then add and test its byte-identical gallery
copy. Confirm no workaround or new framework gap remains before marking the row `✅`.
Artifact root: `/rv/tmp/samples/SAMPLE-054-SkinningSample_4_0/` (historically pruned).

## Historical port record

Upstream: `SkinningSample_4_0` (SAMPLE-054). The canonical XNA skinned-model sample is now
ported whole: the sample-owned runtime library, its processed model, the animation player, camera,
input, lighting and all target-independent game behavior.

Artifact root: `/rv/tmp/samples/SAMPLE-054-SkinningSample_4_0/`.

## The old blocker was a category error

The previous note treated `AnimationClip`, `Keyframe`, `AnimationPlayer` and `SkinningData` as
missing CNA/XNA framework APIs. They are not framework APIs. They are the sample's own
`SkinnedModel` library, built beside the game and copied into other samples that use this
pipeline. They therefore belong in this port just as the game's `SkinningSampleGame` does.

The same note was also bound to the obsolete `.model.json` route. The real game does not load that
format: its unchanged `SkinnedModelProcessor` writes a normal XNA `Model` XNB whose vertices carry
`BLENDINDICES`/`BLENDWEIGHT`, whose meshes use `SkinnedEffect`, and whose `Model.Tag` carries the
sample-owned `SkinningData`. CNA's current XNB path supports all of those pieces.

No new CNA or sharp-runtime change was needed for SAMPLE-054. The generic reflective-reader and
closed-collection support repaired by SAMPLE-051 is the correct reusable seam.

## Original build and content

The artifact scripts compile the unchanged `SkinnedModel` library and unchanged
`SkinnedModelPipeline` processor with the official XNA 4.0 assemblies, run the real processor, and
link the unchanged Windows game. The Windows project declares Reach; Windows HiDef and Xbox Reach
were also built as additional pipeline checks.

One listed `dude.fbx` produces five runtime XNBs. The four textures are implicit FBX material
dependencies rather than content-project rows. The five XNB files checked into this port are
byte-identical to the official Windows Reach output:

| asset | SHA-256 |
|---|---|
| `dude.xnb` | `3a8b6533afc21069206f1f9b14c1a3641234d33e8315b217ab146bb8abcc991e` |
| `head_0.xnb` | `baeaf3ad2fa90fe5c721e2fa1e563248bd446fe8e6456df8a6f93b78b83d2d74` |
| `jacket_0.xnb` | `bdbcf3f4e3b45ed137f5214496dc4c23864f14fc01853f586858a45925363d20` |
| `pants_0.xnb` | `847b874455caee44e5b5d761755a176591b7c67f875482f7bed9b5223d2b1e92` |
| `upBodyC_0.xnb` | `d11e1c66db4d1b716547bb18169c021bed375e6f3556ee02dcef39fc3e9a5b52` |

The reader table was decoded before the port was written. `dude.xnb` names the stock model,
vertex/index/declaration and `SkinnedEffect` readers plus exactly the expected reflective graph:
`SkinningData`, `Dictionary<string, AnimationClip>`, `AnimationClip`, `List<Keyframe>`, `Keyframe`,
`List<Matrix>` and `List<int>`. The four other files use `Texture2DReader`.

## Runtime fidelity

The port retains the original algorithm rather than using CNA's unrelated CNAEXT animation
helper. `AnimationPlayer` consumes the single chronological keyframe list without interpolation,
restores the bind pose when the clock moves backward or loops, constructs world transforms through
the serialized parent hierarchy, then multiplies by inverse bind pose for the final skin matrices.
The game starts `Take 001`, advances it from `ElapsedGameTime`, feeds every `SkinnedEffect`, enables
default lighting, and preserves the original keyboard/gamepad camera and reset controls.

The ordinary native run renders the fully textured Dude and two captures two seconds apart have
different hashes and visibly different poses. For a stricter comparison, the audit-only
`CNA_TIME` hook sets both engines to the same absolute clip position. Within each leg, captures two
seconds apart are byte-identical, proving the clocks are pinned; the two legs themselves differ,
proving different keyframe ranges were exercised.

| pinned clip time | exact pixels | within 8 | within 16 | after 4 px blur | foreground coverage XNA / CNA |
|---|---:|---:|---:|---:|---:|
| 0.5 s | 94.43 % | **99.95 %** | 99.98 % | **100.00 %** | 10.449 % / 10.447 % |
| 0.9 s | 93.35 % | **99.91 %** | 99.98 % | **100.00 %** | 10.301 % / 10.301 % |

At 0.5 s the foreground bounds are exactly `329–497 x 48–479` in both engines and their centroids
differ by 0.02 px. At 0.9 s the bounds are exactly `327–481 x 56–479` and their centroids again
differ by only 0.02 px. This jointly exercises reflective `Model.Tag` loading, keyframe selection,
hierarchy multiplication, inverse bind pose, skinned vertex attributes, stock effect parameters,
textures and drawing.

## Web

The complete `WEBGL2` bundle builds under Emscripten and runs in the system Google Chrome. The
browser gate verifies an 800x480 WebGL 2 canvas, the `WEBGL2` renderer log, the original
`Skinning Sample` title, differing animated-frame hashes, Escape shutdown, successful HTTP loads of
all four bundle files, and no promise rejection, runtime exception, HTTP failure or fatal console
message. Its screenshots show the same textured animated character.

## Intentional C++ mappings

- The three reflectively serialized reference types derive `System::Object` and use
  `std::shared_ptr`; `Model.Tag as SkinningData` becomes the equivalent checked `dynamic_cast`.
- Their private parameterless C# constructors are public in C++ so the AOT reader can construct
  them. Their regular constructors, fields, ordering and behavior remain the same.
- C# array/list/dictionary storage maps to `std::vector`/`std::unordered_map`; `CopyTo` becomes a
  vector assignment and index arithmetic uses `std::size_t` where required by C++ containers.
- Null guards on non-null C++ references are omitted. Runtime-state and range failures retain
  `InvalidOperationException` and `ArgumentOutOfRangeException`.
- `foreach (SkinnedEffect effect in mesh.Effects)` uses a checked `dynamic_cast` and throws
  `InvalidCastException` on a mismatched effect, preserving the C# loop's cast behavior.
- `static void Main()` becomes `int main()`, and C# properties use CNA's getter/setter convention.
- The one AOT reader-registration call is documented in [`diff.md`](diff.md).

## Evidence

- `scripts/build-original.sh` and `evidence/xna-original/` — unchanged original build and live run.
- `scripts/dump-xnb-readers.py` — decoded runtime reader contract.
- `evidence/cna-native-opengles3/` — ordinary native animation run.
- `evidence/frozen/{t0.5,t0.9}/{xna,cna}/` — deterministic image pairs and logs.
- `evidence/cna-web-webgl2/browser-result.json` — real-Chrome behavior gate.

There is no remaining SAMPLE-054 blocker or sample-side workaround.
