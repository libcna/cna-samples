# SAMPLE-099 — ModelImporterSample_4_0 audit

**Current status: `✅`, qualified 2026-09-28. No known runtime differences within the
owner-approved DEC-002 scope.** Fresh unchanged-source Windows/Reach XNA, Release OPENGLES3,
nonthreaded Release WEBGL2 and the exact gallery copy all render the original textured rotating
tank and pass Escape. The icon and active-chain reproduction scripts are restored. The gallery
has its 82nd card, detail page and a screenshot of the running game. No game algorithm, sample
workaround or CNA/Sharp Runtime source changed. The original design-time importer remains
audited reference, not a claimed C++ port. See the current completion section below; prior
qualification and the old threaded static-host failure remain historical evidence.

## Source and behavior

The port retains the original `ObjImporterSample::ObjImporterGame` identity and its
complete runtime behavior:

- `Content.Load<Model>("Tank")` during `LoadContent`;
- base update before Escape/gamepad Back exit handling;
- a Cornflower Blue clear and time-based Y rotation using total seconds divided by
  five;
- the original `(0, 200, 350)` camera and `(0, 35, 0)` look-at point;
- absolute bone-transform copying before drawing every mesh;
- default lighting plus the original world, view and 45-degree perspective matrices;
- base draw after all model meshes have been submitted.

No raw-OBJ runtime loader, alternate model, sample-side parser or other framework
workaround was introduced.

## Authentic source recovery and content

The pre-existing normalized local snapshot lacked `Tank.obj`. The complete official
1.49 MB package was therefore recovered and preserved with SHA-256:

```text
ece748c7f465f04e3e620d7e2881743afa34989896cf6628f6d5108daa6ed8f1  ModelImporterSample_4_0.zip
06964f9ccb3b31498a4218de6c03a10794e8f7f5a3fce1363d99afc9ad7bbd4a  Tank.obj
```

A normalized `diff -qr --strip-trailing-cr` comparison proved that `Tank.obj` was
the only file absent from the local snapshot. The recovered OBJ has 10,910 positions,
14,312 texture coordinates, 10,910 normals, 12 groups, 12 material selections and
21,610 triangular faces.

The unchanged 596-line `ObjImporter.cs` was built as an XNA Content Pipeline
extension and processed the recovered OBJ, MTL and both TGA textures through the
original `ObjImporterSample.ObjImporter -> ModelProcessor` route. The committed
runtime assets are byte-identical to that authentic Windows/Reach output:

```text
11d7d7b5fd7b3bd59be6c1e83bb7f8c8e76f651912350479d26a4c7e374796a1  Content/Tank.xnb
71e35727ae856950f3c524b926246352a8a4ac17d138e5df5340cfe17fbb55c0  Content/engine_diff_tex_small_0.xnb
ee666d84c97951048c898af5aa1b4b2b06a060b8a47fe907873d75ee072d89dc  Content/turret_alt_diff_tex_small_0.xnb
```

The original documentation, image and license are retained at sample root and are
not loaded by the game.

## Design-time boundary

The original importer was reviewed in full. It parses OBJ positions, texture
coordinates, normals, groups, triangle faces, material selections and MTL libraries;
tracks source dependencies; reverses winding for XNA; emits mesh channels and
`BasicMaterialContent`; and resolves texture references while preserving source-line
identity in errors. It warns and skips nontriangle polygons and rejects unsupported line types.

Under the owner-approved `SAMPLES-DEC-002` boundary, this sample qualifies the
runtime game with exact pregenerated XNB output. The 596-line design-time assembly is audited
and preserved as reference evidence, not falsely labelled as ported runtime code. The former
claim that CNA lacks `ContentImporter`/`MeshBuilder` is obsolete: current CNA provides these APIs
and other authoring types. Their existence does not constitute a translation or qualification
of this sample-owned importer; see the current analysis below.

## Historical original XNA reference

The unchanged importer, Windows/Reach content project and game compiled with the
XNA Game Studio 4.0 toolchain. The game executable has SHA-256:

```text
44351e3b0e266746f4ee4acec8b1d294a5d9da2ca6370e90adc066d693e43074  ObjImporterSample.exe
```

An isolated reference run loaded the generated model and textures, displayed the
fully textured tank, visibly rotated it between the two- and five-second captures,
and exited cleanly with Escape.

That executable is now retained at
`evidence/requal-20260928/before-requalification/ObjImporterSample.exe`; the current original
product was freshly rebuilt as recorded below.

Reference source, build output, scripts, logs and captures are preserved under:

```text
/rv/tmp/samples/SAMPLE-099-ModelImporterSample_4_0/
```

## Historical CNA qualification

- Debug OPENGLES3 build and real X11 run: passed; model and both external textures
  loaded, two- and five-second frames differed, and Escape exited cleanly.
- Clean Release OPENGLES3 build and real X11 run: passed with the same visible model,
  rotation and clean exit.
- Focused CNA XNB/model/texture/external-reference regression: 22/22 tests passed on
  a real OPENGLES3 context.
- WebGL2 Release build: passed.
- Real Chrome: the 800x480 canvas used WebGL 2.0, displayed the textured tank with
  more than 100 distinct captured colours, changed frame hash as the model rotated,
  and completed 600/600 `requestAnimationFrame` callbacks. There were no runtime
  exceptions, unhandled promise rejections, fatal console messages or relevant HTTP
  errors, and all HTML/JS/Wasm/data artifacts returned HTTP 200.

No CNA, Sharp Runtime, EasyGL or MetaGL source change was required by this sample.
All browser and native evidence, including reusable qualification scripts, remains
in the artifact directory above.

## Current-head re-analysis — 2026-09-28

Starting heads: cna-samples `develop dab1a16`, CNA `next b2fd47a45`, Sharp Runtime `next fc033a0e`.
SAMPLE-098 is owner-confirmed, pruned and pushed; gallery `main 25005b6` is pushed as well.
This turn analyzes SAMPLE-099; no sample/framework source, new game build or gallery entry was added.

### Package and complete source review

This is one runtime game with Windows/x86 and Xbox 360 **Reach** projects, plus a shared 596-line
OBJ/MTL design-time importer library. It has no Phone, audio, touch, networking or application-thread
path. The game loads `Tank`, automatically rotates it about Y, copies absolute bone transforms,
uses default `BasicEffect` lighting and the original camera/projection, and exits on Escape/Back.
There is no player-controlled camera or tank movement. No mouse-to-touch opt-in is needed.

The physical local directory still has 17 files / 1,612,433 bytes and lacks `Tank.obj`. All its
files equal the retained source after CRLF normalization. The retained recovered official ZIP
has its known `ece748c7f465…` hash; **all 18 files / 3,830,679 bytes** of `xna4-original/` are
byte-identical to the archive. Thus the complete reference package is available without editing
or reconstructing the OBJ. Its `06964f9ccb3b…` hash still verifies.

The game, entry point, both assembly files, all projects/content declarations, original documentation,
MTL and complete importer were reviewed. The OBJ contains 10,910 positions, 14,312 UVs, 10,910
normals and 21,610 triangles. Its 25 `g` records comprise 12 named groups and 13 anonymous groups;
the earlier “12 groups” describes the named meshes, not the total group records. The importer flips
V, reverses winding, retains source-line error identities, tracks MTL dependencies and constructs
materials/mesh channels. It ignores oversized polygon faces with a warning and rejects unknown
line types; these original limitations are not CNA deficiencies.

The complete runtime C++ translation preserves update/draw order, `TotalSeconds/5`, camera
`(0,200,350)`, look-at `(0,35,0)`, 45-degree projection and mesh/effect traversal. `std::optional`
represents the initially null model; the matrix vector matches CNA's public array-shaped API.
The targeted bypass scan finds only the required managed type identity extension and original
Load/Draw/default-lighting calls. There is no raw OBJ parser, alternate model, backend branch,
overlay, extra input or sample workaround. Both original projects select Reach, which also matches
current CNA's default project profile. The original `Game.ico` is absent from the port root and
should be restored unchanged during qualification; the original thumbnail is already retained.

### Exact content and retained desktop products

All three checked-in XNBs remain byte-identical to the retained official Windows/Reach outputs
and native deployment, with the hashes above. `Tank.xnb` uses stock Model, String, VertexBuffer,
VertexDeclaration, IndexBuffer and BasicEffect readers; it has 26 bones and four shared resources.
No sample-owned runtime reader or reflective registration is required. Its two textures are the
original external-reference products, not runtime loose-file substitutes.

The retained original is actually named `xna4-build/bin/ObjImporterSample.exe`; its `44351e3b0e…`
hash verifies. The old executable-name typo above has been corrected. A fresh run uses the
established Wine prefix, `WINEDLLOVERRIDES=d3d9=b` and its own Xvfb display. The retained Release
OPENGLES3 product runs separately on a private display. Both render the fully textured tank at
800×480, change frames between two/five seconds, and exit with code 0 after a 600 ms Escape hold.
The captures were visually inspected. Their animation clocks were not frozen, so no pixel-exact
frame comparison is claimed. These are fresh runs of existing binaries, not fresh current-head builds.

### Static HTTP browser failure

The old JavaScript contains `PThread` and shared WebAssembly memory. Fresh system Chrome on an
isolated display/profile, served by an ordinary `ThreadingHTTPServer` without isolation headers,
reproduces:

```text
DataCloneError: Failed to execute 'postMessage' on 'Worker':
SharedArrayBuffer transfer requires self.crossOriginIsolated.
```

The canvas stays 300×150, the shell reports an exception, and the WEBGL2 renderer/game never
starts. Required requests have no HTTP error; this is a thread/isolation ABI failure. The older
COOP/COEP qualification is preserved as historical evidence. The original game has no thread use,
so a fresh `CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF` build is the appropriate ordinary-static-host
product, followed by a real Chrome rendering/rotation/Escape gate. No failed path was hidden.

### Current authoring scope and remaining work

Current CNA has real `ContentImporter<T>`, `MeshBuilder`, `BasicMaterialContent`, `ModelProcessor`
and supporting content types. Source inspection retracts the obsolete blanket “APIs missing”
claim; it does not prove this 596-line importer already compiles or produces equivalent output
through CNA. No importer translation or new authoring compatibility test was performed. DEC-002's
accepted exact-XNB runtime boundary still applies; extending the authoring scope is a separate task.

Native RUNPATH and reproduction scripts still reference retired `openeggbert/cnanext` checkouts.
The old native product runs using available system SDL libraries. The web build script lacks the
current Sharp Runtime root and `CCACHE_BASEDIR`, and caps `--parallel` at eight. The old capture
scripts also have incomplete cleanup and use an ad-hoc `/tmp` browser profile. The new analysis
helpers use owned processes/displays and keep evidence/profile lifetime under this artifact root.

Next qualification should refresh the reproduction commands to the active libcna chain, shared
ccache and all cores; restore the exact icon; build current Release OPENGLES3 and nonthreaded
Release WEBGL2; verify the original scene, rotation, Escape and browser errors; and add the gallery
card/detail page with a screenshot of the running tank. No ModelImporter gallery entry exists today.
No newly demonstrated large CNA runtime subsystem or owner scope decision blocks this work.
The historical 22/22 focused model tests were not rerun or presented as a current-head result.

Evidence: artifact `evidence/current-head-analysis-20260928/`, including `inventory.json`,
`no-workaround-scan.txt`, `retained-runs/{result.json,*.png,*-run.log}` and
`static-web/{result.json,static-page.png,console.log,server.log}`. Reproduce with
`scripts/analyze-retained-runs-20260928.py` and `scripts/probe-static-web-20260928.py`.
The preceding products and evidence remain intact; no SAMPLE-099 prune was authorized or applied.


## Current-head completion — 2026-09-28

Starting heads were samples `develop 184d39f`, CNA `next b2fd47a45`, Sharp Runtime `next
fc033a0e` and gallery `main 25005b6`. The task restores the exact original `Game.ico`;
all three runtime C++ files remain byte-identical to the starting samples commit. CNA and
Sharp Runtime source/heads are unchanged. The accepted DEC-002 boundary remains the full
runtime game with original XNA-generated content.

### Source, original build and content

The complete retained ZIP and all **18 files / 3,830,679 bytes** of the original snapshot again
match byte for byte. Physical upstream still has 17 files and lacks `Tank.obj`; every present
file matches after CRLF normalization. The game, projects, assembly files, full 596-line importer,
content declarations and original documentation were reviewed. The runtime retains exact loading,
update/draw order, time-based rotation, camera, projection, default lighting and Escape/Back.

`build-original.sh` freshly compiles the unchanged importer and uses the official Windows/Reach
`BuildContent`/`ModelProcessor` route. All three resulting model/external-texture XNBs remain
byte-identical to the checked-in files and native deployment, with the hashes above. No runtime
OBJ parser, alternate asset or sample-specific content reader is required. The new original
compiler invocation also includes the exact `Game.ico` requested by the Windows project.
Fresh Windows/x86 Debug Reach executable SHA-256:

```text
d704273f9a8d3617ddfba8d71af219f4245fea3d141733e2d0f169603c3c6cec
```

The original runs through `/home/robertvokac/.wine-cna-xna40`, `WINEDLLOVERRIDES=d3d9=b` and an
owned Xvfb display. The capture verifies a textured 800×480 tank, changing two/five-second frame
pixels and clean Escape exit code 0. The earlier original executable and old threaded bundle
are preserved under `evidence/requal-20260928/before-requalification/`.

### Current Release OPENGLES3

The build uses the active libcna CNA/Sharp Runtime roots, static CNA, Ninja, Release,
`CNA_SAMPLES_ONLY=ModelImporterSample`, shared ccache, `CCACHE_BASEDIR=/rv` and all CPU cores.
The current product is:

```text
cna-native-opengles3/samples/ModelImporterSample/ModelImporterSample_cna_samples
```

Its RUNPATH points to the active CNA prebuilt SDL. Its three deployed XNBs are exact original
pipeline output. On a separate owned display, it renders the textured tank, rotates it and exits
with code 0 on Escape. Original/native captures were visually compared. Their animation clocks
were independent; the recorded two/five-second comparisons are not a claim of pixel-exact frame
identity. The old `cna-native-opengles3-release/` product remains historical and untouched.

### Nonthreaded Release WEBGL2 and actual exit

The original has no application thread use, so the web build sets
`CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF`. The **7,838,284-byte** WASM contains no `debug_info`;
JavaScript contains no `PThread`, `shared:true` or `new SharedArrayBuffer` token. The data bundle
is the exact concatenation of the three original XNBs. The Emscripten shell is unmodified.

Fresh system Google Chrome with an owned display/profile runs over ordinary static HTTP:

- `crossOriginIsolated=false`, real WebGL 2 and an 800×480 game canvas;
- the original textured scene and different frame pixels as the tank rotates;
- 600 further browser animation frames;
- trusted Escape delivered to the focused canvas;
- graphics context count **1 → 0** and identical post-exit frames two seconds apart;
- no runtime exceptions, unhandled rejections, required-asset HTTP errors or fatal messages.

The first diagnostic gates incorrectly required the compiled-effect teardown log. A renderer
with only built-in BasicEffect has no compiled-effect context and does not print that log on
normal shutdown. The focused repeat confirmed trusted Escape delivery; the corrected gate
observes actual context disposal and stable exited pixels. No game/framework fix or workaround
was needed. These earlier false-negative probe records remain in `web-before-window-focus/`
and `web-before-exit-observation/`; a pre-visible-window harness attempt is retained separately.
Capture helpers now wait for and focus their own Chrome window. They do not replace input
providers, alter game memory or change application processing.

### Gallery delivery

The four-file current bundle is byte-identical to
`../samples.libcna.com/ModelImporterSample/`. An independent Chrome test serves the whole gallery
root and opens the actual nested game URL; it repeats rendering, rotation, 600 frames and clean
Escape. Every game/gallery request succeeds. Chrome then renders the detail page and last gallery
card; both screenshots were visually inspected.

The gallery has **82 unique cards**, correct page ranges, source link, Play URL and both adjacent
navigation links from Microphone Echo. All affected local links resolve. The full image is an
exact copy of the final game's `web/01-frame-2s.png`; its thumbnail is a resize of that capture.
It shows the game, not a menu or an upstream promotional illustration.

Local gallery commit: `08b8409` (`SAMPLE-099: add verified model importer WebGL2 gallery entry`).
Publication awaits an owner-requested push.

### Audit record, reproduction and limits

The targeted bypass scan was manually reviewed against the original and contains only the
managed identity mechanic and original Load/default-lighting calls. There is no sample workaround.
No CNA or Sharp Runtime component changed, so no additional framework regression gate was required;
the earlier 22/22 focused tests remain labelled historical rather than presented as a new result.
Original, native and both real-browser product gates provide the current runtime verification.
No physical Xbox/gamepad run or new C++ authoring-importer qualification is claimed.

Artifact root: `/rv/tmp/samples/SAMPLE-099-ModelImporterSample_4_0/`.
Current evidence is `evidence/requal-20260928/`: `inventory.json`, `effective-build-config.json`,
`frame-comparison.json`, `no-workaround-scan.txt`, fresh build logs, `desktop/`, `web/`,
`gallery-web/` and preserved earlier probes. Closing local repository heads are recorded in
`final-heads.json`. Earlier static-host failure and analysis remain under
`evidence/current-head-analysis-20260928/`. `MANIFEST.md` distinguishes current/historical products
and preserves the earlier prune record.

Reproduction: `build-original.sh`, `build-cna-native.sh`, `build-cna-web.sh`, `capture-native.py`,
`capture-web.py` and `inventory-current.py`. Shell capture wrappers use these safe helpers.
The original build copies deployment files atomically, preserving earlier hardlinked products.
Browser profiles are temporary inside the evidence directory; owned processes/displays are cleaned
up. Both build trees remain reusable. No new SAMPLE-099 prune or push was authorized/applied.

The completion dry run proposes approximately **201.4 MB** of build intermediates (250.9 →
49.5 MB before stripping/deduplication). Its exact report is
`evidence/requal-20260928/prune-dry-run.log`; it deletes nothing.
