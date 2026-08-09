# CNA gaps relevant to the XNA 4 Racing Game Kit

## Scope and baseline

This file records only framework behavior that is missing, incorrect, backend-limited, or not yet sufficiently verified for this game. It is not a general CNA backlog and it is not an implementation plan.

The primary implementation baseline inspected on 2026-08-09 was the active integration worktree `../cnaintegration` at commit `4ac696c748fb` (`integration/post-audit-phase1`). The checkout requested initially, `../cna` at `ac3aaaeb2a5b`, is about three weeks older. The integration branch is still absorbing 21 feature branches, so every item must be rechecked against the commit selected for the future port.

Important correction to older sample documentation: CNA already has partial XNB support. It can read uncompressed and LZX-compressed XNBs and has real readers for several texture, audio, stock-effect, vertex/index-buffer, and model payloads. The findings below were made from current source and tests, not from the stale claim that “XNB is never supported.”

Status meanings:

- **CONFIRMED GAP**: current source or a focused test proves the required behavior is absent or inconsistent.
- **BACKEND GAP**: the common API may exist, but a specific backend cannot satisfy this game's shader/rendering requirements.
- **PARTIAL**: useful implementation exists, but it does not cover the Racing Game requirement completely.
- **VALIDATION GAP**: no failure is yet established; a representative game-specific experiment is still required.
- **PORT-SIDE ALTERNATIVE**: the game can avoid the CNA gap, but the alternative belongs in the future port or its offline tooling.

## Confirmed framework gaps

### 1. `Rgba64` render-target capability reporting contradicts construction

**Status:** CONFIRMED GAP; affects the examined programmable 3D backends through shared validation. Skia has a separate format path but is not a viable backend for this game.

The Racing Game's `RenderToTexture` asks `GraphicsAdapter.QueryRenderTargetFormat` for `SurfaceFormat.Rgba64` before creating its full-, half-, and quarter-resolution render targets. In current CNA, the non-D3D9 adapter path reports `Rgba64` as supported and can return it, but shared texture validation rejects every non-`Color` format on ordinary GPU backends. `RenderTarget2D` therefore throws after the capability query said the format was usable.

Relevant CNA locations:

- `../cnaintegration/src/Microsoft/Xna/Framework/Graphics/GraphicsAdapter.cpp`
- `../cnaintegration/src/Microsoft/Xna/Framework/Graphics/RenderTarget2D.cpp`
- `../cnaintegration/src/Microsoft/Xna/Framework/Graphics/Texture.cpp`
- `../cnaintegration/tests/Microsoft/Xna/Framework/Graphics/Texture2DTests.cpp`
- `../cnaintegration/tests/Microsoft/Xna/Framework/Graphics/GraphicsAdapterTests.cpp`

A focused test run confirmed that the current expected behavior is to throw while constructing an `Rgba64` texture/render target; the adapter tests do not cover this end-to-end contradiction.

**Required framework action:** make capability selection and construction agree. Either implement a renderable `Rgba64` format on the reference backend and report it, or return a genuinely constructible fallback such as `Color`. Add an end-to-end test that queries a format and then successfully constructs the returned render target.

**PORT-SIDE ALTERNATIVE:** request/use `Color` explicitly for the initial port. This is acceptable for bootstrapping but must be image-compared because it reduces the precision of the glow/menu post-processing chain.

### 2. General compiled custom-effect XNB payloads are intentionally unsupported

**Status:** CONFIRMED GAP; blocks direct use of the original custom-effect/model XNB output.

The Racing Game uses ten custom `.fx` files and its processed models refer to custom material effects. CNA's known-unsupported `EffectReader` path throws for a general compiled effect, and the `Effect` constructor that accepts compiled effect bytes also throws. Stock effects are a separate supported path and do not solve this requirement.

Relevant CNA locations:

- `../cnaintegration/src/Microsoft/Xna/Framework/Content/KnownUnsupportedContentTypeReader.cpp`
- `../cnaintegration/src/Microsoft/Xna/Framework/Graphics/Effect.cpp`
- `../cnaintegration/src/CNA/Internal/Xnb/ModelContentTypeReaders.cpp`

The binary `ModelReader` itself is real and tested: it reconstructs bones, mesh parentage, bounding spheres, declarations, buffers, and effects. The failure is specifically the unresolved custom effect inside an otherwise readable model graph. Non-null model tags are also rejected, although the Racing Game custom processor does not generate a `Model.Tag`.

**Required framework action, only if direct original XNB compatibility is chosen:** define and implement a cross-backend representation for custom compiled effects, including parameter/technique/pass metadata and state behavior. Loading DirectX 9 bytecode alone would not make it portable to OpenGL/Vulkan.

**PORT-SIDE ALTERNATIVE (recommended):** do not use original custom-effect XNBs as the primary runtime format. Convert models offline into a Racing-specific package and bind explicitly ported `ShaderEffect` programs and material metadata at runtime.

### 3. CNA has no XNA FX technique/pass/state-block runtime for portable custom shaders

**Status:** PARTIAL / CONFIRMED architectural gap.

CNA's NOXNA `ShaderEffect` is useful and supports named values, matrices, arrays, 2D/cube/3D texture bindings, cloning, custom vertex declarations, and direct 3D/SpriteBatch draws on capable backends. It represents one vertex/fragment program pair. It does not reproduce the XNA `Effect` model used by this game:

- reflected `EffectParameter` collections;
- named and indexed `EffectTechnique` selection;
- ordered multi-pass techniques;
- pass-local render-state blocks;
- loading the old DX9 `.fx` language and shader profiles;
- portable translation of HLSL semantics and sampler declarations.

Relevant CNA locations include:

- `../cnaintegration/include/Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp`
- `../cnaintegration/src/Microsoft/Xna/Framework/Graphics/ShaderEffect.cpp`
- backend `CreateEffectBackend` / shader-program implementations under `../cnaintegration/src/CNA/Internal/Backends/`

This matters for all ten Racing Game effects, particularly the four/five-pass menu/glow chains, two-pass shadow blur, shadow generation/use techniques, and the many one-pass normal-mapping technique variants.

**Required framework action:** none is strictly required for the first reference port if explicit shader programs are acceptable. If XNA-compatible custom `Effect` loading is a CNA goal, this is substantial framework work rather than a missing header.

**PORT-SIDE ALTERNATIVE (recommended):** map each required technique/pass to explicit `ShaderEffect` objects and express all render states and render-target transitions in C++. Preserve the original parameter names in a typed Racing shader interface so comparisons remain possible.

### 4. Loose DDS cube loading cannot read the supplied normalization cube

**Status:** CONFIRMED FORMAT GAP.

The original `NormalizeCubeMap.dds` is an uncompressed RGB888 six-face cube. CNA's loose `TextureCube` DDS loader currently accepts DXT1, DXT3, and DXT5 cube payloads, so this asset cannot be consumed directly. The supplied sky cube is DXT1 and is within the implemented path.

Relevant CNA locations:

- `../cnaintegration/src/Microsoft/Xna/Framework/Graphics/TextureCube.cpp`
- `../cnaintegration/src/Microsoft/Xna/Framework/Content/ContentManager.cpp`

**Required framework action:** add the relevant uncompressed DDS cube formats only if this is desired as a general CNA capability, with face-order and row-pitch tests.

**PORT-SIDE ALTERNATIVE (recommended):** deterministically convert the normalization cube to a supported lossless runtime representation and image/numerically validate its vectors. Loading it from a supported XNB texture-cube payload is another possible experiment.

### 5. Current glTF import is not a semantics-preserving replacement for Racing models

**Status:** PARTIAL; content/importer limitation rather than a general inability to render glTF.

CNA has substantial tested glTF support, including static/skinned geometry, PBR materials, tangent generation, and the `gltf_to_cnj` tool. However, the current runtime collection path records mesh pointers rather than preserving the complete unskinned scene-node identity, transform, and instance graph. It then creates a simplified identity-root/per-mesh bone structure. It also maps materials to stock Basic/PBR effects and does not retain the Racing processor's technique codes or custom effect parameter values.

Relevant CNA locations:

- `../cnaintegration/src/CNA/Internal/GltfImport/GltfImportCore.cpp` (`CollectMeshGroups`)
- `../cnaintegration/src/Microsoft/Xna/Framework/Content/ContentManager.cpp` (`ReadGltfModel` and associated model construction)
- `../cnaintegration/tools/gltf_to_cnj`

This is critical because the game uses parent bones/pivots for car wheels and windmill blades, mesh naming, model bounding spheres, material-effect values, and an appended per-part technique number.

**Required framework action:** only if generic CNA glTF import is selected as the final runtime loader: preserve node transforms/instances and expose extensible material metadata/effect binding.

**PORT-SIDE ALTERNATIVE (recommended):** use glTF/Assimp only as a geometry conversion component and write a deterministic sidecar/manifest that preserves hierarchy, pivots, mesh/material mapping, bounds, texture bindings, colors, shininess, alpha, and Racing technique IDs.

## Backend-specific gaps

### 6. Vulkan's current custom `ShaderEffect` binding ABI is too restricted

**Status:** BACKEND GAP; unsuitable for the initial Racing Game port.

The Vulkan backend's custom-effect path consumes SPIR-V rather than the GLSL source intended for the recommended OpenGL backend. Its current named-value handling is reduced to a constrained push-constant layout rather than the large collection of matrices, vectors, scalar arrays, 2D textures, and cube textures required by the Racing shaders.

Relevant CNA location:

- `../cnaintegration/src/CNA/Internal/Backends/Vulkan/VulkanGraphicsBackend.cpp`

**Required framework action before Vulkan enablement:** a reflected descriptor/uniform binding model sufficient for all Racing shader parameters, plus render-target, sampler, custom-vertex-layout, and state regression tests. This does not block an `OPENGL33` reference implementation.

### 7. Bgfx cannot compile the Racing shader sources through the current `ShaderEffect` path

**Status:** BACKEND GAP; unsuitable for the initial Racing Game port.

The Bgfx effect backend's source-program compilation path currently returns failure and expects backend-specific precompiled shaders. The Racing port would need a separate shader build toolchain and binding strategy, or CNA backend work.

Relevant CNA location:

- `../cnaintegration/include/CNA/Internal/Backends/Bgfx/BgfxGraphicsBackend.hpp`
- `../cnaintegration/src/CNA/Internal/Backends/Bgfx/BgfxGraphicsBackend.cpp`

### 8. Newly integrated and 2D/fixed-function backends are not valid initial targets

**Status:** BACKEND LIMITATION / maturity risk.

LLGL was merged into the inspected integration branch on the audit date and has not been exercised with this game. Other newly added general-purpose backends may eventually work but lack a Racing-sized shader/render-target proof. SDL_Renderer, Direct2D, Canvas, HTML DOM, Skia, ASCII, GDI, and similar 2D backends do not provide the programmable 3D pipeline this game requires. OpenGL ES 1 and other fixed-function legacy paths cannot faithfully represent the shader set.

No work on these backends should be included in the first port milestone.

## Validation gaps (not yet proven CNA defects)

### 9. Exact Racing custom vertex layouts need a live reference-backend proof

Current OpenGL/EasyGL code supports generic vertex declarations and its tests cover a custom 48-byte layout. Racing uses a 44-byte tangent vertex (`Vector3 position`, `Vector2 UV`, `Vector3 normal`, `Vector3 tangent`) and 32-bit landscape indices. These appear implementable and are **likely ready**, but a representative 257x257 landscape buffer and exact declaration must be rendered before promotion to READY.

### 10. Racing XACT banks and authored behavior have not been tested end to end

CNA has real XACT parsers/runtime behavior and relevant tests for categories, cue instance limits, RPC curves, variables, loops/random variation, and MS-ADPCM. Those tests passed in the inspected integration tree. The Racing source actually needs these facilities and no positional audio.

The repository contains the `.xap` authoring project and source WAVs but no generated `.xgs`, `.xsb`, or `.xwb` files. Until genuine Windows XACT output is obtained, CNA has not been tested against this exact project: gear cue replacement, Music/Gears instance policies, the cue-scoped `Pitch` RPC, crossfades, loop behavior, and the PC MS-ADPCM banks remain LIKELY READY rather than READY.

### 11. Render-target chains and readback need an integrated `OPENGL33` test

The individual APIs exist, but the exact sequence of full/half/quarter render targets, repeated resolve/rebind, cube sampling, custom blending, depth changes, and final `GetBackBufferData`/JPEG screenshot should be captured in one focused test. The known `Rgba64` issue must be resolved or deliberately bypassed first.

### 12. Storage needs an ordinary Racing-shaped round-trip test

`StorageDevice`, `StorageContainer`, selector/open-container async facades, and preference-path storage are implemented. Existing focused coverage is stronger for containment/deletion than for the exact settings/high-score/replay cycle. Test XML settings, binary replay data, overwrite, reconnect behavior, and screenshot paths. Racing uses controlled file names, so this is not a known blocker.

### 13. Ongoing integration can invalidate this list

Because `integration/post-audit-phase1` is actively merging 21 feature branches, the future implementation session must pin and record a CNA commit, rerun the focused tests, and diff every file cited above. “Fixed on a feature branch” is not equivalent to available in the chosen build.

## Port-side dependencies that should not be misreported as CNA gaps

These require deliberate adaptation, but CNA should not necessarily implement them:

- Replacing `XmlSerializer`, `[Serializable]`, and implicit C# reflection with explicit parsers for `.Track`, `.CombiModel`, and settings schemas.
- Converting the 57 legacy ASCII `.x` models and reproducing `RacingGameModelProcessor` semantics.
- Porting ten DX9-era `.fx` files to explicit GLSL/HLSL shader variants.
- Replacing Windows Forms message boxes/window hiding, `[STAThread]`, legacy path rules, and XNA GamerServices UI conventions.
- Ensuring GPU objects are created on the render thread instead of copying the original background loading thread literally.
- Normalizing content case and backslashes for Linux.
- Replacing static/global C# ownership and garbage-collector assumptions with RAII.
- Fixing or consciously preserving defects in the original XNA 4 conversion (for example replay fallback state, one-sided mouse movement detection, and questionable custom blend-state initialization).

## Verified capabilities that are not missing

Do not recreate stale blockers for these items without a new failing experiment:

- Partial XNB reading, including uncompressed/LZX payloads.
- Binary XNB model reconstruction with bones, mesh parents, bounds, buffers, declarations, and stock/readable effects.
- XNB/loose Texture2D support for the game's common images; loose `.tga` is recognized.
- XNB texture cubes and compressed DXT DDS cube loading.
- Real XACT `AudioEngine`, `WaveBank`, `SoundBank`, `Cue`, categories, cue variables/RPCs, and PC MS-ADPCM decoding.
- Keyboard, mouse, and gamepad APIs used by the game.
- `SpriteBatch`, render targets, vertex/index buffers, 32-bit indices, states, and generic custom vertex declarations on the OpenGL/EasyGL family, subject to the focused validations above.
- The historical SpriteBatch-before-3D ordering defect: current integration contains a regression example stating it is fixed across backends. Re-test in the pinned build, but do not list it as an open gap merely because older documentation still does.

## Priority for framework work

1. Resolve the `Rgba64` capability/construction contradiction and add the end-to-end test.
2. Prove the full Racing render-target/custom-vertex subset on `OPENGL33`.
3. Obtain and run the genuine Racing XACT banks; fix CNA only if a minimized failure is demonstrated.
4. Keep the first port on explicit `ShaderEffect` programs and a game-specific offline content package.
5. Consider generic custom-effect XNB, richer glTF scene preservation, and additional backend shader support only after the reference game is complete and their broader CNA value is established.
