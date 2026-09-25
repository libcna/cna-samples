# Renderer candidates for EasyGL-qualified samples

Analysis recorded 2026-09-25 against CNA `9bb6dc0a7e03ddcca60f5d496f23b302d332dcf8`
and this repository `6393322`. This is a capability assessment, not a claim that
another renderer has passed the sample campaign. No alternative-renderer sample
build or run was performed for this assessment.

## Scope and acceptance bar

The sample campaign uses native `OPENGLES3` and browser `WEBGL2`. These are two
profiles of the same internal EasyGL implementation. Its renderer boundary in
[`rules.md`](rules.md#renderer-boundary) and [`plan.md`](plan.md#renderer-boundary-for-the-sample-campaign)
remains in force; the candidates below are for a possible later qualification
effort. The root [CMake configuration](CMakeLists.txt) currently forces those
two identities and enables `CNA_EASYGL_COMPILED_EFFECTS`, so passing a different
`CNA_GRAPHICS_RENDERER` cache value is not enough to build this repository with
another renderer.

"Can run all EasyGL-qualified samples" means running their unchanged C++ game
logic and original content through the public CNA/XNA API, including representative
interaction and visual comparison. A capability flag or a renderer's own test
suite does not establish that result. The most discriminating existing samples
include:

- [SAMPLE-038 ShadowMapping](samples/ShadowMapping/missing.md): a 2048 x 2048
  `SurfaceFormat.Single` render target, sampled by a compiled effect and drawn
  with `SpriteBatch`.
- [SAMPLE-040 InstancedModel](samples/InstancedModel/missing.md): a compiled
  two-technique effect, multiple vertex streams and hardware instancing.
- [SAMPLE-041 LensFlare](samples/LensFlare/missing.md): an occlusion query whose
  **pixel count**, not just visible/hidden state, controls the sun and flares.
  EasyGL's boolean count is an owner-accepted limitation for this one sample;
  it is not general approval of that behavior elsewhere.
- [SAMPLE-042 ShatterEffect](samples/ShatterEffect/missing.md) and
  [SAMPLE-043 Particles3D](samples/Particles3D/missing.md): original compiled
  XNA effects, custom vertex data and effect-driven animation.

## Candidates

| CNA identity | Current potential | Work or qualification still needed |
|---|---|---|
| `OPENGL33` | Closest native profile: it shares EasyGL's implementation with `OPENGLES3` and `WEBGL2`. | Desktop GL shader/profile and driver differences still need sample-level checks. It provides neither an independent renderer implementation nor a browser target. |
| **`WEBGPU`** | Strongest **independent identity spanning native and browser**. Both targets have executed stock 3D effects and compiled XNA effects; the renderer also has multiple vertex streams, instancing, float targets, MRT and occlusion queries. | Enable `CNA_WEBGPU_COMPILED_EFFECTS`; exercise every sample effect on both targets. The browser's SPIR-V-to-WGSL translator accepts a measured subset and refuses constructs outside it. An occlusion query spanning several render-pass segments records only its first segment. No sample-wide parity run exists. The measured `cna-street` scene still took 104 ms per WebGPU frame versus 29.7 ms on EasyGL after the latest buffer work. |
| **`OPENGL4`** | Strong independent native desktop candidate. Classic XNA features include compiled effects, multiple streams, instancing, float targets and exact occlusion counts. | Enable `CNA_OPENGL4_COMPILED_EFFECTS`; qualify the actual samples and supported desktop drivers. There is no browser target. |
| **`VULKAN`** | Strong native candidate with compiled effects, multiple streams, instancing, float targets and occlusion queries. | Enable `CNA_VULKAN_COMPILED_EFFECTS`; run the sample effects and scenes. Precise occlusion counts depend on the selected physical device's feature. No browser target is established. |
| **`FNA3D`** | XNA-shaped native middleware with compiled effects always on, native multiple-stream input, driver-dependent hardware instancing, render targets and occlusion queries. | Qualify the selected FNA3D driver and each sample. Source-string `ShaderEffect` is unsupported, although the samples above use compiled XNA effects. No CNA browser qualification exists for this identity. |
| **`DIRECTX11`, `DIRECTX12`** | Strong Windows-native candidates; their optional compiled-effect paths cover multiple streams, instancing, `SpriteBatch` and 2D/cube/volume sampling. | Enable the respective `CNA_DIRECTX*_COMPILED_EFFECTS` option and run the sample matrix. Existing graphics evidence relies largely on Wine/DXVK or vkd3d; native Windows hardware remains a separate gate. Neither is a browser renderer. |
| **`SOFTWARE`** | Independent CPU rasterizer with optional compiled-effect execution, multiple streams, instancing, float targets and exact occlusion counts. | Enable `CNA_SOFTWARE_COMPILED_EFFECTS`. It owns a windowless CPU framebuffer and `Present()` does not show an image; an onscreen/browser presentation path and acceptable game speed would be substantial additional work. |
| `DIRECTX9` | Windows-native XNA-era path with optional compiled effects and native occlusion queries. | Its compiled-effect conformance still skips renderer-wide multiple-stream input, which SAMPLE-040 needs. Windows and sample-level verification also remain. |
| `SDL_GPU` | Native GPU path with optional compiled effects. **Current code** reports both multiple streams and instancing, despite older compiled-effect tables saying otherwise. | It currently reports no occlusion-query support and no float render-target support. Those block the SAMPLE-041 query and SAMPLE-038 target. Vendored SDL_gpu has no occlusion-query API, so a faithful count needs more than a capability switch. No browser sample route is established. |

`METAL` currently lacks compiled XNA effects, occlusion queries, multiple streams
and instancing; it also needs fresh native macOS validation. `OPENGLES2` and
`WEBGL1` share EasyGL but deliberately refuse MSAA, MRT, occlusion queries,
volume textures, instancing and multiple streams. `PORTABLEGL` is a bounded CPU
3D renderer without render targets, general effects or instancing. The 2D-only,
headless and stub identities cannot cover the 3D/effect samples without becoming
substantially different renderers.

## Practical conclusion

For **one independent renderer serving both native and browser products**, start
with `WEBGPU`. For native-only coverage, `OPENGL4`, `VULKAN` and `FNA3D` are the
most direct cross-platform candidates; `DIRECTX11`/`DIRECTX12` are strong
Windows-specific candidates. `OPENGL33` is useful as a near-EasyGL native control,
not as an independent alternative. `SOFTWARE` is valuable as an independent pixel
oracle, with presentation and performance work needed for playable products.

A later implementation effort should first make the sample build's renderer
selectable, enable the chosen renderer's compiled-effect option, then run the
unchanged high-discrimination samples above on their intended platforms. Fix
framework defects in CNA and keep sample code free of renderer workarounds.
Only a complete per-sample native/browser gate could justify a claim that a
candidate serves *all* EasyGL-qualified samples.

## CNA evidence used

The source and renderer-specific documents in the sibling CNA checkout at the
revision named above are the evidence for this assessment:

- `cmake/RendererIdentities.cmake`, `cmake/RendererSelection.cmake`, and
  `docs/renderer-registry.md` for identities, platform boundaries and build options.
- `docs/webgpu-renderer.md`, `docs/opengl4-renderer.md`,
  `plans/plan_opengl4_modern_graphics.md`, `docs/vulkan-renderer.md`, and
  `docs/fna3d-renderer.md` for the strongest candidates.
- `docs/directx11-renderer.md`, `docs/directx12-renderer.md`,
  `docs/software-renderer.md`, `docs/portablegl-renderer.md`, and
  `plans/plan_fx.md` for the remaining effect and capability boundaries.
- `modules/renderers/sdl-gpu/src/SdlGpuRenderer.cpp` for SDL_GPU's **current**
  capability answers; `plans/plan_street_perf.md` for the measured five-renderer
  workload. Some older summary tables predate the newer compiled-effect and
  SDL_GPU multi-stream/instancing implementations and should not override these
  sources.
