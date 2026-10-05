# Native renderer qualification

Updated 2026-10-05 for the owner-authorized native multi-renderer campaign. The starting revisions
were CNA `b0e97bb1bb876f9b3edd6f4ff1ef3067908ae8ac` on branch `samples` and cna-samples
`5db32e6a2631f216e85082b1c390321548924aa2` on branch `develop`. Read current revisions and logical
campaign commits from Git. The completed corpus campaign through cna-samples `d34edebd7` and CNA
`504bf0a06` was pushed at the owner's explicit request; later work must still follow the normal
no-push-without-request rule.

## Architecture

A native configuration builds one executable per sample. CNA's existing renderer registry embeds
the requested implementations and `CNA_GRAPHICS_RENDERER` selects one when the process starts. A
requested renderer must initialize and appear in the startup log; the matrix runner treats a
missing or different active identity as a failure and never falls back silently.

The root CMake configuration preserves the convenient defaults only when neither renderer variable
was supplied: native builds use `OPENGLES3`, while the established Emscripten build uses `WEBGL2`.
Explicit `CNA_GRAPHICS_RENDERER` and `CNA_GRAPHICS_RENDERERS` values pass through unchanged. The
selected set enables only its applicable compiled-XNA-effect implementations:

- EasyGL (`OPENGLES2`, `OPENGLES3`, `OPENGL33`, `WEBGL1`, `WEBGL2`);
- Vulkan, WebGPU, SDL_GPU, DirectX 9, DirectX 11 and DirectX 12;
- FNA3D's own FNA3D/MojoShader path, which has no separate option.

Metal has no compiled-XNA-effect implementation and is not reported at parity. The common
`cna_add_sample()` helper calls CNA's `cna_copy_renderer_runtime()` once per target. Windows also
deploys SDL and MinGW runtime DLLs through CNA-owned helpers. This includes `wgpu_native`; no
individual sample carries renderer-specific copy logic.

## Authoritative corpus

[`tools/renderer-matrix-corpus.tsv`](tools/renderer-matrix-corpus.tsv) is the machine-readable
manifest. It contains:

| Category | Count | Matrix role |
|---|---:|---|
| `gallery-runnable` | 89 | Published gallery executables and the primary corpus. |
| `native-runnable` | 2 | `ClientServerSample` and `NetRumble`; included in the primary native corpus. |
| supporting/partial/test/experimental | 9 | Available by explicit category, not counted as gallery acceptance. |
| `special-standalone` | 1 | RacingGame; governed by its separate plan and excluded here. |

Thus the primary native matrix has 91 executables. Documentation-only, rights-blocked and
non-runnable dispositions remain recorded in [`plan.md`](plan.md); a repository directory is not
automatically a runnable matrix row.

### RacingGame special standalone

The separately governed RacingGame is not counted among those 91 rows, but task
`RACING-MULTI-001` now gives it the same Linux runtime-selection architecture. One executable embeds
`OPENGLES3`, `OPENGL33`, `VULKAN`, `WEBGPU`, `SDL_GPU` and `FNA3D`; all six pass actual-product
startup plus captured drive, menu and device-reset probes using authentic XNA4 content and compiled
effects. See [`plan_racing.md`](plan_racing.md) and
[`samples/RacingGame/missing.md`](samples/RacingGame/missing.md) for commands, visual evidence and
the still-open audio/platform gates.

## Linux qualification

The clean Release multi-renderer tree contains `OPENGLES3`, `OPENGL33`, `VULKAN`, `WEBGPU`,
`SDL_GPU` and `FNA3D`. The test host uses Debian GCC 14.2.0, CMake 3.31.6, Ninja 1.12.1, Linux
6.12, an AMD Phoenix/Radeon 780M-class GPU and Mesa/RADV 25.0.7. Every launch used CNA's private
GPU display runner, not the owner's desktop.

An `AUTOMATED_PASS` means the requested renderer was logged as active, initialization completed and
the application remained stable through the observation interval. It does not by itself prove
visual correctness. A clean process exit before that interval is reported separately as
`EARLY_EXIT`; it is not promoted to a pass merely because its exit status is zero.

| Renderer | Built once | Automated pass | Render fail | Failure classification |
|---|---:|---:|---:|---|
| `OPENGLES3` | 91 | 90 | 1 | `Yacht`: renderer-independent unavailable gamer-services transport. |
| `OPENGL33` | 91 | 90 | 1 | `Yacht`, as above. |
| `VULKAN` | 91 | 90 | 1 | `Yacht`, as above. |
| `WEBGPU` | 91 | 90 | 1 | `Yacht`, as above. |
| `SDL_GPU` | 91 | 89 | 2 | `LensFlare`: upstream query API absent; `Yacht`. |
| `FNA3D` (default SDL_GPU driver) | 91 | 89 | 2 | `LensFlare`: selected internal driver has no query; `Yacht`. |

These results exercise the same sample executable repeatedly with a different environment
selection. They do not represent six separately generated copies.

### Visual comparison

The high-discrimination set is `BloomSample`, `Graphics3D`, `InstancedModel`, `LensFlare`,
`NormalMappingEffect`, `Particles3D`, `ReachGraphicsDemo`, `ShadowMapping`, `ShatterEffect` and
`SpriteSheet`. It covers 2D, stock and compiled effects, render targets/postprocessing, normalized
formats, instancing/multiple streams, particles, shadows and occlusion queries.

The post-fix pass captured 58 frames and manually compared them with the OPENGLES3/OPENGL33
references. Fifty-seven are visual passes. The per-renderer result is:

| Renderer | Manual visual result |
|---|---|
| `OPENGLES3` | 9 pass; `LensFlare` is an upstream/API precision mismatch. |
| `OPENGL33` | 10 pass. |
| `VULKAN` | 10 pass. |
| `WEBGPU` | 10 pass. |
| `SDL_GPU` | 9 pass; `LensFlare` has no query and therefore no capture. |
| `FNA3D` default driver | 9 pass; `LensFlare` has no query and therefore no capture. |

OPENGLES3 supplies a real `GL_ANY_SAMPLES_PASSED` boolean, not an exact pixel count. The authentic
sample divides the returned `1` by a 100x100 query area, so its flare chain is almost invisible.
OPENGL33, Vulkan and WebGPU use real count queries and render the expected flare. SDL_GPU exposes
no public occlusion-query/query-pool facility, and FNA3D's default SDL_GPU driver inherits that
limit. CNA does not invent a count or synchronously rasterize a software surrogate: either would
violate the asynchronous XNA query contract. A supplemental FNA3D run using its OpenGL driver
uses a real query and passes the visual comparison.

The full 91x6 corpus still needs broader manual interaction/visual coverage; its automated result
must not be presented as 546 manual visual passes.

### OPENGLES2 profile

The separate OPENGLES2 build records 80 automated passes and 11 render failures. Ten failures are
truthful profile limits: compiled-effect render-target sampling, unsupported `NormalizedByte2` or
`NormalizedByte4` storage, hardware instancing, or occlusion query. The eleventh is the same
renderer-independent `Yacht` failure. OPENGLES2 is a compatible subset, not a HiDef full-gallery
target, and its capability flags were not inflated to turn those rows green.

## Matrix runner

[`tools/run_renderer_matrix.py`](tools/run_renderer_matrix.py) reads the authoritative manifest,
selects samples/categories/renderers, launches each executable with an explicit
`CNA_GRAPHICS_RENDERER`, retains stdout/stderr, enforces an observation interval and timeout,
checks the active-renderer log, optionally captures a window, and writes JSON plus Markdown
summaries. The shell wrapper enters CNA's private Linux GPU runner automatically. Windows paths,
`.exe` suffixes and process termination are covered by the runner's unit tests.

On Linux, captures come from the compositor's root image cropped to the sample window's measured
geometry. Direct X11 window-pixmap capture is intentionally avoided: under Xwayland it can return
a stale or incomplete Vulkan backing image even while the compositor displays the complete frame.
The capture delay still begins when CNA logs the active renderer, so allow enough time for content
loading; four seconds is the current conservative gallery-review value.

The composed full-corpus sweep exposed one runner classification gap: nine consecutive GLES3
processes exited cleanly before the six-second observation interval, and the old classifier treated
exit status zero as a pass even though no frame was captured. They are now reported as
`EARLY_EXIT`. A focused repeat completed all nine observation intervals with the requested GLES3
renderer active and captured all nine frames; the original early exits are not used as visual
evidence.

Example:

```bash
./tools/run_renderer_matrix.sh \
  --build-dir cmake-build-qual-multi \
  --renderers OPENGLES3,OPENGL33,VULKAN,WEBGPU,SDL_GPU,FNA3D
```

Use `--samples` for a focused set, `--categories all` for non-primary projects and
`--capture-after-seconds N` for manual visual evidence. Generated evidence is deliberately not a
pixel-perfect oracle because several samples contain time-dependent animation.

## Reproduction commands

All campaign builds use at most 12 jobs.

### Linux

```bash
cmake -S . -B cmake-build-qual-multi -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCNA_GRAPHICS_RENDERER=OPENGLES3 \
  -DCNA_GRAPHICS_RENDERERS='OPENGLES3;OPENGL33;VULKAN;WEBGPU;SDL_GPU;FNA3D'
cmake --build cmake-build-qual-multi --parallel 12
./tools/run_renderer_matrix.sh \
  --build-dir cmake-build-qual-multi \
  --renderers OPENGLES3,OPENGL33,VULKAN,WEBGPU,SDL_GPU,FNA3D
```

The pinned wgpu-native package can be supplied with `CNA_WEBGPU_ROOT`; otherwise CNA's documented
dependency acquisition applies.

### Native Windows follow-up

Linux MinGW cross-compilation produced all 106 repository executables, including the complete
91-row primary corpus, with seven renderers in each executable. `wgpu_native.dll`, SDL3,
SDL3_image, SDL3_mixer and the required MinGW runtime DLLs were deployed beside every output.
This is a cross-build result, not native Windows qualification.

Supplemental Wine evidence passes the DirectX 9 and DirectX 11 `ShadowMapping` startup/stability
smoke after compiled-effect repairs. DirectX 12 reaches vkd3d-proton device creation but is
environment-blocked at the real HWND swap chain; off-screen DirectX 12 tests pass. None of this is
reported as a native Windows GPU pass.

Run the following in a Developer PowerShell with a target-native Vulkan SDK and wgpu-native package:

```powershell
cmake -S . -B build-windows-multi -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DCNA_GRAPHICS_RENDERER=DIRECTX11 `
  -DCNA_GRAPHICS_RENDERERS="DIRECTX9;DIRECTX11;DIRECTX12;VULKAN;WEBGPU;SDL_GPU;FNA3D" `
  -DCNA_WEBGPU_ROOT=C:\path\to\wgpu-native
cmake --build build-windows-multi --parallel 12
py .\tools\run_renderer_matrix.py `
  --build-dir build-windows-multi `
  --renderers DIRECTX9,DIRECTX11,DIRECTX12,VULKAN,WEBGPU,SDL_GPU,FNA3D `
  --no-private-display
```

Review the ten-sample visual set first, then the complete matrix. Native Windows qualification
must record the GPU, driver and Windows version and must not reuse the Linux cross-build label.

### Native macOS follow-up

Linux work can validate only portable contracts. Current Metal source still lacks compiled XNA
effects, exact occlusion queries, multiple vertex streams and instancing, and the adapted
Objective-C++ implementation has not been compiled or executed on the owner's Mac mini M4. Metal
therefore remains unqualified. OpenGL is deprecated on macOS; its actual context/toolchain result
must be recorded rather than assumed.

On the Mac mini, with Xcode command-line tools, Ninja and target-native dependencies installed:

```bash
cmake -S . -B build-macos-multi -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCNA_GRAPHICS_RENDERER=METAL \
  -DCNA_GRAPHICS_RENDERERS='METAL;OPENGLES3;OPENGL33;WEBGPU;SDL_GPU;FNA3D' \
  -DCNA_WEBGPU_ROOT=/path/to/wgpu-native
cmake --build build-macos-multi --parallel 12
python3 tools/run_renderer_matrix.py \
  --build-dir build-macos-multi \
  --renderers METAL,OPENGLES3,OPENGL33,WEBGPU,SDL_GPU,FNA3D \
  --no-private-display
```

Run CNA's Metal and common graphics tests before the sample runner, then manually inspect the same
ten-sample set. A configure/build failure is evidence to repair, not permission to drop a
requested renderer silently. If current macOS cannot supply a usable OpenGL profile, record that
row as an OS/toolchain limitation and rerun the remaining identities without claiming OPENGL33.

## Web scope

The established WebGL1/WebGL2 gallery pipeline was not redesigned. Common native CMake changes
preserve its `WEBGL2` default and effect support. Optional WebGPU-under-Emscripten work was not
attempted in this native campaign.

## Remaining external gates

- Native Windows GPU and visual qualification on actual Windows hardware.
- Native macOS compilation and execution, including Metal on the Mac mini M4.
- A real SDL_GPU occlusion-query API before direct SDL_GPU can implement XNA query semantics.
- Wider manual visual/interaction review beyond the ten-sample discriminator set.
- Metal compiled-XNA-effect and other currently truthful capability gaps.

Retired renderer identities such as `OPENGL4` are intentionally absent and must not be restored.
