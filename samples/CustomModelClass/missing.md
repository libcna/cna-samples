# CustomModelClass — SAMPLE-052 audit

Upstream: `CustomModelClassSample_4_0`. The 2026-07-06 port was a workaround, not a port of
this sample: it removed the sample's own `CustomModel`/`ModelPart` runtime types, loaded CNA's stock
`Model` instead, copied a hand-converted `.model.json` and 24 buffer sidecars from CameraShake, and
added an F1 overlay. All of those substitutions are gone.

Artifact root: `/rv/tmp/samples/SAMPLE-052-CustomModelClassSample_4_0/`.
`xna4-original/` is an exact 19-file snapshot of the physical upstream directory; `diff -qr`
against `/rv/tmp/XNAGameStudio/Samples/CustomModelClassSample_4_0` is empty.

## What was ported

The runtime is again the sample's design:

- `Content.Load<CustomModel>("tank")`, represented by `std::shared_ptr<CustomModel>` because the
  original is a C# reference type;
- the private nested `CustomModel.ModelPart`, with the original two counts, vertex buffer, index
  buffer and shared `Effect` fields in source order;
- `CustomModel.Draw`, including the checked `BasicEffect` cast, per-draw
  `EnableDefaultLighting`, matrices, buffer binding, pass loop and original
  `DrawIndexedPrimitives` arguments;
- the original camera, 10/10000 near/far planes, time-based rotation, Escape/gamepad-Back input,
  clear colour, class/type names and update/draw order.

The original directory has two platform solutions but one runnable product. The Windows project
declares Reach and the Xbox project uses the same game and content. The whole Windows/Reach runtime
and both sample-owned pipeline files were audited. `CustomModelProcessor` flattens each node
transform into its geometry, resets the node to identity, cache-optimizes every mesh and chains
materials through `MaterialProcessor`; consequently this sample intentionally stores no model bone
hierarchy.

## Exact content and the framework gap it exposed

`scripts/build-original.sh` compiles the unchanged `CustomModelPipeline.dll`, runs the official XNA
4.0 pipeline, and compiles the unchanged Windows game. One listed `tank.fbx` produces three files:
the custom model plus two textures resolved from its materials.

| file | SHA-256 |
|---|---|
| `engine_diff_tex_0.xnb` | `a8a8df6194bfae78380c761a2eca3868e7038fb6e28afc00596579ae9b40268c` |
| `tank.xnb` | `af208946302f4175990297ae0e4a0b5dc5d2276fa9bfb0b70b03349cf5c2c492` |
| `turret_alt_diff_tex_0.xnb` | `e83058016f86ea61f575804c2a1e0cac00321eb386800dee6e7de13a0b8bdce5` |

Those hashes are byte-identical between the checked-in `Content/` and retained pipeline output.
The 839675-byte Reach `tank.xnb` is uncompressed and names nine readers: reflective readers for
`CustomModel` and `ModelPart`, the closed `ListReader<ModelPart>`, `Int32Reader`, vertex/index
buffer readers, `EffectReader`, `VertexDeclarationReader` and `BasicEffectReader`. It contains two
shared resources.

That last detail found the one real CNA gap. `ModelPart.Effect` is marked
`[ContentSerializer(SharedResource = true)]`, but `ReflectiveTypeReaderBuilder` could only read
ordinary members. `cnanext` now provides `SharedResourceField()`: it reads the 1-based shared-table
index and queues the assignment through `ContentReader::ReadSharedResource`, so assignment occurs
after the root and both effects have been read, as in XNA. The builder requires `RegisterShared()`
for stable object identity and rejects value-shaped `Register()` for such a field. Two focused
tests cover successful deferred assignment and the unsafe registration rejection; the real tank
is the end-to-end case. See `cnanext/plans/plan_xnb.md` XNB-42B.

XNA discovers both reflected field lists from the game assembly. C++ has no equivalent runtime
reflection, so the port declares those exact lists once in
`CustomModelContentReaderRegistrationEXT`; the single registration call is documented in
[`diff.md`](diff.md). It changes neither content nor behavior and is the campaign's established
closed-AOT-reader mechanism.

## Original and native agreement

The unchanged original was built and run under the isolated XNA 4.0 Wine prefix with WineD3D. The
port was built as Release `OPENGLES3` through EasyGL and run under an isolated Xvfb display. Both
open the original `Custom Model` 800x480 window, continuously rotate the fully textured and lit
tank, and produce no runtime error.

Because the angle is a function of `TotalGameTime`, `xna4-diag/` and `cna-diag/` contain audit-only
copies that honor `CNA_TIME`; neither hook is present in the committed source. At exactly 10 s,
the two engines' 800x480 frames differ at only 40721 pixels at any level, only **123 pixels by more
than 8 levels** (**99.9680 % within 8**) and 75 by more than 32 (**99.9805 % within 32**).
ImageMagick's mean absolute error is **18.0685 / 65535** (`0.000275708`). Two captures per engine
are identical within the pinned leg, proving the comparison is not scheduler timing.

This also disproves the old record's “near-plane white line” claim. That artifact belonged to the
hand-converted geometry. The official custom-model XNB renders the same recognizable tank in XNA
and CNA with the original near plane, winding, render state and draw calls; no sample-side culling,
index rewrite or camera change exists.

## WEBGL2 / Chrome

The complete Release `WEBGL2` bundle was built with the real Emscripten toolchain, served over
local HTTP and driven in the system Google Chrome. `evidence/cna-web-webgl2/browser-result.json`
records:

- an 800x480 WebGL2 canvas rendering the textured and lit custom model;
- different hashes 1.5 s apart, proving the model rotates without input;
- the original `Custom Model` title and the `WEBGL2` renderer log;
- Escape reaching the game and shutting down the SDL/Emscripten surface (the canvas becomes 0x0
  and stays there);
- all `.html`, `.js`, `.wasm` and `.data` requests returning 200;
- no unhandled rejection, runtime exception, relevant HTTP error or fatal console message.

The browser image is not a stock `Model` fallback: producing one frame requires the root
reflective reader, the closed private-part list, all vertex/index buffers, both deferred shared
`BasicEffect` resources and the two material textures to load successfully.

## Differences and remaining gaps

- Private reflection-only constructors are public in C++ so `std::make_shared` can invoke them.
- C# reference objects use `std::shared_ptr`; the `foreach (BasicEffect ...)` cast is represented by
  `dynamic_cast` plus `InvalidCastException` on mismatch.
- The closed reader registration in `diff.md` replaces .NET reflection without changing the wire
  data.

No active behavioral, visual, content, input, CNA, sharp-runtime or EasyGL difference remains. The
historical `help.png` is retained only at the sample root and is neither copied nor loaded.

## Reproducible evidence

- `scripts/build-original.sh` and `evidence/build-original.log` — unchanged pipeline/game build.
- `scripts/dump-xnb-readers.py` and `evidence/tank-readers.txt` — real XNB structure.
- `scripts/capture-original.sh` / `capture-cna-native.sh` — live rotating captures.
- `scripts/build-original-diag.sh` / `compare-frozen.sh` and `evidence/frozen/t10/` — pinned parity.
- `scripts/capture-web.sh`, `chrome-smoke.mjs` and
  `evidence/cna-web-webgl2/browser-result.json` — real-Chrome gate.
- `evidence/cna-native-build.log` and `evidence/cna-web-build.log` — final Release builds.

## Current-head analysis — 2026-09-25 (no rebuild or runtime test yet)

The retained `xna4-original/` matches all **19** physical upstream files. The source contains one
runnable game shared by the Windows and Xbox projects, its own `CustomModel` and private
`ModelPart` runtime graph, and two sample-owned content-pipeline source files. Both game projects
declare **Reach**. One listed `tank.fbx` uses `CustomModelProcessor`; the FBX materials pull in two
TGA textures, so the official Windows/Reach result is three XNBs. All three checked-in XNBs are
byte-identical to the retained official Windows output and the retained native Content copies.
The original `CustomModel.htm` is also byte-identical. There are no `#if` branches in the C#
runtime or pipeline units.

The historical port's `Content.Load<CustomModel>("tank")`, reflective field registration,
deferred shared `Effect` resources, original camera/rotation and Escape path are still present in
source. A targeted scan found no old `.model.json`, loose-buffer, F1-overlay, culling or render
workaround in the port. CNA's general `SharedResourceField()` API and its two focused tests are
present on the current dependency head, but the old pixel and Chrome results predate this audit.
No current-head build or run of SAMPLE-052 was made in this analysis.

The port still lacks the physical original's `Microsoft Permissive License.rtf`, `Game.ico` and
`CustomModelSample.png`. Its historical `help.png` is correctly outside `Content`. The retained
native binary's RUNPATH and `MANIFEST.md` point to the former `openeggbert/cnanext` checkout;
`scripts/compare-frozen.sh` points to former `openeggbert/cna-samples` and swaps source into that
checkout, so it cannot safely be used as written. The old original/native capture scripts do not
test Escape and should move their windows into the private display before taking screenshots.
The local gallery currently has no SAMPLE-052 card, detail, screenshots or WEBGL2 bundle.

To requalify: restore the three original files outside Content; rebuild unchanged Windows/Reach
content/game on the active toolchain and check all three XNB hashes; rebuild native OPENGLES3 and
nonthreaded WEBGL2 against current CNA/sharp-runtime; use an isolated frozen-time diagnostic
source tree rather than swapping checked-in files; compare original/native rotation, the custom
model and shared effects, and Escape; run the web bundle and its future gallery copy in system
Chrome. Keep `plan.md` at `🔎` until these current-head checks and publication pass.
