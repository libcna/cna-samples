# SAMPLE-113 — `AvatarAnimPack_4_0_FBX` audit and owner decision

## Status

Fresh audit complete enough to require an owner representation decision under
`SAMPLES-DEC-004` and `SAMPLES-DEC-005`. The old “art data” label is now measured rather than
assumed: every source asset is valid input to the documented Microsoft custom-avatar pipeline,
but this physical directory contains no application or build project. No invented viewer,
generic-model replacement, source-FBX runtime loader or CNA substitute Avatar was added.

## Classification and complete inventory

This is a licensed source-animation delivery containing 22 files and 201,337,809 bytes:

- 21 distinct binary Kaydara FBX 6.1 files named `climb`, `crawl`, `faint`, `jump`, `kick`,
  `pickUp`, `pull`, `punch`, `push`, `run`, `sitIdle`, `steer`, `strafeLeft`, `strafeRight`,
  `swimFreestyle`, `swimUnderwater`, `swingBat`, `swingClub`, `swingHammer`, `throw` and `walk`;
- the Microsoft Permissive License in RTF form.

There is no `.sln`, project/content-project, C#/C++ source, entry point, build script, readme, HTML
documentation, screenshot or declared standalone UI. The FBX files range from 9,404,800 to
9,779,344 bytes. Each has the standard `Kaydara FBX Binary` signature and little-endian format
version 6100; embedded metadata identifies the FBX 2009.2 toolchain and export timestamps from
11–12 November 2010. The files include `Avatar_Mesh`, `BASE__Skeleton` and the detailed skinned
Avatar hierarchy. This identifies authored rig content, not a runnable XNA product.

All source hashes are retained. Representative SHA-256 identities are:

| Asset | SHA-256 |
|---|---|
| `climb.fbx` | `ca823e4c1ae53dcffbc34a37c37745192c2ce4e08725c3f44c35432e35450032` |
| `jump.fbx` | `4a9cd38b489a11bbca0773ee405807d7b2055fb2cd2772b4b4e02169cc28b0f8` |
| `run.fbx` | `628d2cf3675e60780db7911e0e27f80bb198a27a7b30b805c9bd3395288b46dc` |
| `walk.fbx` | `fbfee29cb7a01c86d5b94b2ca35fe49c2c258d3c44d8fd7966800baba65d5869` |

## Relationship to the other Avatar deliveries

The local Microsoft `CustomAvatarAnimation` documentation states that custom animation requires
an Avatar-specific rig, that animation packs were supplied for packages such as Maya and
Softimage, and that exported FBX should be loaded by the content processor in that sample. That
is the only authoritative consumer contract found locally.

The 21 FBX basenames pair exactly 1:1 with all 21 `SAMPLE-112` BIN basenames. However, neither
directory supplies a working converter: SAMPLE-112 only names a missing
`AvatarAssetConverter_e.exe`, and its script invokes the wrong variable. Without that converter or
a format specification, the audit does not claim the BIN files were produced from these exact FBX
bytes.

This pack is also not a duplicate of SAMPLE-094. Its five overlapping action names (`faint`,
`jump`, `kick`, `punch`, `walk`) all have different FBX hashes from the complete game sample's
inputs. SAMPLE-094 remains the executable Xbox game and custom-pipeline lesson; SAMPLE-113 is a
much larger 21-animation support delivery with no game.

## Authentic XNA 4.0 pipeline evidence

The documentation-supported consumer was tested without altering any source asset. A retained
diagnostic harness uses the official XNA 4.0 `FbxImporter`, the exact Ms-PL
`CustomAvatarAnimationProcessor` and runtime data classes from Microsoft's SAMPLE-094 source, and
the original Xbox360/HiDef `BuildContent` task. The harness is evidence, not a project silently
invented for this directory.

All **21/21** FBX files imported and processed successfully. The processor found the expected
Avatar skeleton and exactly one animation in every source, completed its 71-bone validation and
emitted 21 Xbox360, XNB version 5, LZX-compressed custom-animation objects. `BuildContent` returned
`True`. The importer reported the documented expected warning classes—unsupported `Controls`
attribute data, per-mesh decal UV layers and multiple bind poses—but no asset failed.

The outputs range from 67,028 to 266,936 bytes. Representative SHA-256 identities are:

| Output | SHA-256 |
|---|---|
| `climb.xnb` | `6e09dff6aa5c456e13d48f945e438332448ebc2a3d3c4da56522ef1e435f5eae` |
| `jump.xnb` | `10742596def492177a3558afad56fbc332c9e4f1fd62169148ca66043ce82fcc` |
| `run.xnb` | `05af3ebdf17cf323f2bd497fb6d049a1b043dd004b09a197a99fd531f15117fb` |
| `walk.xnb` | `7c2866b6965cbcd498f57b3c386d72b9480092bdae6b2249d7a3bbc445604764` |

The first retained hash manifest and the final self-contained rebuild manifest are byte-identical
(manifest SHA-256 `da727b87c18db2f50997ccc9d2d76b9b40b851950f8bb1387edd62b3c3565d58`),
so the 21 outputs are reproducible. This disproves an FBX validity or original content-pipeline
blocker. It does not create an upstream game or prove a runtime rendering product that was never
shipped here.

## Live CNA boundary

The live dependency state remains CNA `7712534d3d22c7e284714e0e87afebba3f3cb472` and Sharp
Runtime `9cc96cd57cde394940cc24d58743edf9bf63d3fb`. CNA deliberately consumes compiled content at
runtime and has no XNA design-time FBX authoring pipeline. Adding a raw-FBX runtime route would
violate the campaign's content policy and would not solve the absent-product issue.

If a runtime consumer is later authorized, the exact 21 XNBs can follow the established
sample-owned AOT reader-registration pattern for the `CustomAvatarAnimationData` graph. The
defining visual still needs the normal XNA Avatar backend: current `AvatarAnimation` presets are
zero-length/zero-matrix, `AvatarRenderer` stays `Unavailable`, bind pose is unavailable and normal
`Draw` is a no-op. The immediately preceding current-HEAD gate passed **76/76** focused
AvatarAnimation/Description/Expression/Renderer tests with `SDL_VIDEODRIVER=offscreen`. CNA's
working real-render path is a non-authentic `CNAEXT` substitute and cannot be selected silently.

No CNA or Sharp Runtime source change was made. The authentic pipeline result narrows the owner
choice to product representation and Avatar rendering; it removes content conversion from the
blocker list.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-113-AvatarAnimPack_4_0_FBX/`.

- `xna4-original/` is the complete byte-for-byte upstream snapshot;
- `evidence/file-inventory.txt`, `sha256sum.txt`, `fbx-versions.txt` and
  `embedded-creation-times.txt` record every source identity and format fact;
- `evidence/fbx-bin-basename-pairing.txt` records all 21 exact SAMPLE-112 name matches;
- `evidence/assimp-info-climb.log` preserves the supplemental modern-Assimp rejection of old FBX
  6.1; this is not used as the product-scope reason;
- `scripts/reference-processor-source/` retains the exact seven Ms-PL source units plus licence
  used by the diagnostic, with inventory and hashes under `evidence/`;
- `scripts/build-original-content.sh` and `XnaPipelineRunner.cs` reproduce the original pipeline;
- `evidence/xna-pipeline-build.log`, `xna-output-file-types.txt`,
  `xna-output-sha256*.txt` and the empty `xna-output-rebuild-diff.txt` preserve the successful,
  deterministic 21-output result;
- `xna4-build/Content-xbox/` contains the exact 21 generated XNBs;
- `evidence/snapshot-diff.txt` is empty.

Native OPENGLES3 and WEBGL2 runtime gates are not applicable because upstream ships no executable
or standalone runtime consumer. They become mandatory if the owner authorizes a new product.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port/archive boundary for this valid source-animation delivery;
2. classify it as retained support content for SAMPLE-094 or a future faithful Avatar backend,
   keeping the reproducible XNB evidence but creating no standalone sample target; or
3. authorize a new 21-animation gallery/preview product, including the exact custom-data AOT
   reader and an explicit decision between an authentic Xbox Avatar dataset/backend and the
   deliberately non-authentic CNA substitute, with OPENGLES3 and WEBGL2 qualification.

Until that choice, a generic skinned-model viewer, raw-FBX loader or automatic CNAEXT body would
invent behavior absent from the source and violate the campaign rules.
