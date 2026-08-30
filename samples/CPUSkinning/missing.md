# CPU Skinning — port notes

Upstream: `CPUSkinningSample_4_0` (SAMPLE-056). The complete sample is ported: its custom
CPU-model runtime type and reader, exact four-influence CPU vertex algorithm, ordinary GPU
`Model` comparison path, animation, FPS counter, touch/mouse controls, target-specific constructor
branch and original content.

Artifact root: `/rv/tmp/samples/SAMPLE-056-CPUSkinningSample_4_0/`.

## Fresh source and project audit

The historical note incorrectly treated `AnimationClip`, `Keyframe`, `AnimationPlayer`,
`SkinningData`, the custom processor and the absence of `.model.json` data as permanent blockers.
All four animation types are owned by this sample. More importantly, the exact official XNA
pipeline output is an accepted runtime contract: CNA already has the stock XNB pieces and the
generic AOT seam needed to describe sample-owned types without .NET reflection.

Every runtime C# file, both game and data-type projects, both Windows and Phone solutions, the
content project, all processor/writer sources, FBX/texture/font inputs and the one-page Word
document were reviewed. The selected Windows game and its content compile unchanged under XNA
4.0. Windows HiDef and Xbox Reach content builds also pass.

The port retains the full behavior:

- both models use the same `Take 001` clip and `AnimationPlayer` skin transforms;
- the GPU path is a stock `Model` rendered through `SkinnedEffect.SetBoneTransforms`;
- the CPU path blends the exact 12 matrix fields for four bone influences, transforms each source
  position and normal, preserves texture coordinates, uploads the whole result each frame through
  `DynamicVertexBuffer.SetData(..., Discard)`, and draws indexed triangles with `BasicEffect`;
- tap/right-click switches CPU and GPU modes; free-drag/left-drag rotates the camera with the
  original yaw and clamped pitch equations;
- black background, camera, projection, render state, lighting, labels, 30 Hz target and FPS
  component are unchanged;
- the inactive `WINDOWS_PHONE` constructor branch remains in source: full screen at 480x800;
  desktop remains mouse-visible and both targets keep the original touch-gesture path.

The original has no keyboard Escape handler. Automated captures terminate only their diagnostic
host process after collecting evidence; no invented sample input was added.

## Exact official content

The unchanged content project applies `CpuSkinnedModelProcessor` to `dude_cpu.fbx`, which writes a
custom runtime model with CPU vertices and a shared `BasicEffect`. `GpuSkinnedModelProcessor`
applies the canonical skinned-model pipeline to the duplicate `dude_gpu.fbx`. The seven checked-in
Windows Reach products are byte-identical to the retained official build:

| asset | SHA-256 |
|---|---|
| `dude_cpu.xnb` | `bacabe8bd330e933482af691c60088e819180f08bde4dfd9928a76a5bebed24f` |
| `dude_gpu.xnb` | `74738cd3b1c1fbfc301e88c78c782c246a8276d241c924d726660f634dd0d0f3` |
| `font.xnb` | `c32d4d87eb16be5f55a5ebef2e3703ed0823c031db38824d5aae09f57de74a65` |
| `head_0.xnb` | `e933e17b7420c7d42a93549eadf2f86b511afcc81ef9c1f5b17a5eacbcf13c06` |
| `jacket_0.xnb` | `3fb54ff7f3b947b88a6d9b0ecef9c54f5b0330029f900792b9f38b387af5dc80` |
| `pants_0.xnb` | `93158b036d6dd6cf2ad8a927ebe72dd6443ba0575320c92ef5fb896ae6c4eee6` |
| `upBodyC_0.xnb` | `0fbe5d780d18f449b87e0ddb2ac9e9f3f0689add4f7f25f578b19d897edc1c69` |

The reader table was decoded before implementation. `dude_cpu.xnb` has 21 readers and five shared
resources: the two explicit sample readers, `List<CpuSkinnedModelPart>`, reflective `CpuVertex[]`,
an index buffer, reflective `SkinningData`/clip/keyframe collections, the stock scalar/vector/time/
matrix readers and `BasicEffectReader`. `dude_gpu.xnb` has the expected stock model/buffer/
declaration/`SkinnedEffect` graph plus the same reflective animation graph. The port loads both
files directly; there is no raw model, sidecar, reconstructed mesh or runtime pipeline substitute.

## Framework result

No CNA, SharpRuntime, EasyGL or MetaGL repair was necessary. SAMPLE-051/054 had already established
the generic reflective XNB path and complete stock skinned-model support; SAMPLE-056's distinct
explicit readers and CPU vertex shape fit those general contracts. Only the sample-owned closed
reader registrations described in `diff.md` are added.

## Native XNA/CNA fidelity

The unchanged XNA Windows game and the Release OPENGLES3 port both run at 800x480 with the original
`CPU Skinning` title. Ordinary captures show the animated textured Dude, `GPU skinning` and
`CPU skinning` labels, right-click switching and left-button camera rotation in both engines.

Audit-only source copies pin the clip to identical absolute times. Each GPU and CPU frame is
byte-identical across a two-second interval, while the 0.5 and 0.9 second legs change bounds and
centroid. The comparison mask excludes only the independently timed FPS text and lower-left mode
label, leaving 275,600 model/background pixels:

| clip time | path | exact pixels | within 8 | within 16 | after 4 px blur |
|---|---|---:|---:|---:|---:|
| 0.5 s | GPU | 92.15% | **99.96%** | 99.99% | **100.00%** |
| 0.5 s | CPU | 93.30% | **99.99%** | 99.99% | **100.00%** |
| 0.9 s | GPU | 90.95% | **99.97%** | 99.99% | **100.00%** |
| 0.9 s | CPU | 93.28% | **99.99%** | 99.99% | **100.00%** |

At 0.5 s both engines have model bounds `330–497 x 90–479`; centroids differ by at most 0.02 px.
At 0.9 s the bounds are `327–481 x 90–479`; centroids differ by at most 0.04 px. Within unchanged
XNA, GPU and CPU pixels differ by at most one color level at both times. Within CNA, the two paths
are 99.97/99.98% within eight levels (maximum differences 14/11), demonstrating that the CPU
implementation produces the same deformation rather than displaying the GPU model in both modes.

## Web

The complete Emscripten `WEBGL2` bundle builds and runs in the system Google Chrome. The browser
gate verifies an 800x480 WebGL 2 canvas, the `WEBGL2` renderer log, original document/window title,
advancing animation, right-click CPU switching and left-button camera drag. All `.html`, `.js`,
`.wasm` and `.data` requests succeed, with no promise rejection, runtime exception, relevant HTTP
failure or fatal console message. The captured GPU and CPU frames visibly contain the corresponding
mode labels and correctly deformed textured model.

## Intentional C++ mappings

- C# arrays/lists/dictionaries map to `std::vector`/`std::unordered_map`; the public part view uses
  SharpRuntime's `ReadOnlyCollection` to preserve the original read-only property contract.
- Sample-owned serialized reference types derive `System::Object` and use `std::shared_ptr` so
  explicit/reflective readers and shared XNB resources retain reference semantics.
- Private parameterless deserializer constructors become public C++ constructors so the declared
  AOT readers can construct the same final objects.
- C# reference fields map to RAII (`std::unique_ptr`, `std::shared_ptr`, `std::optional`), and
  `typeof(VertexPositionNormalTexture)` maps to the type's stock vertex declaration.
- C# effect casts become checked `dynamic_cast`s; a mismatched effect throws
  `InvalidCastException`/`ContentLoadException` instead of silently changing behavior.
- `static void Main()` maps to `int main()`, and properties use CNA's established getter/setter
  naming convention.

## Documentation and evidence

The kit contains no HTML topic. Its original `Cpu Skinning Demo Description.doc` is retained
byte-identically (SHA-256 `8906aa8922e433aaa990e0c6a497fe225dc644559e456288320b057f9aa436e1`),
and `CPUSkinning.htm` is a hand-authored semantic HTML rendition of that one-page document.

- Original: unchanged Windows Reach game/content build and WineD3D run; Windows HiDef and Xbox
  Reach content builds also pass.
- CNA: Release OPENGLES3 target builds and passes animation, CPU/GPU and drag checks.
- Browser: Release WEBGL2 bundle passes the real-Chrome renderer/content/behavior/error gate.
- Artifact evidence: `scripts/`, `xna4-original/`, `xna4-build/`, `xna4-diag/`, `cna-diag/`,
  `evidence/{xna-original,cna-native-opengles3,cna-web-webgl2,frozen}/` and both complete CNA build
  trees under the artifact root.

There is no remaining SAMPLE-056 blocker and no sample-side workaround.
