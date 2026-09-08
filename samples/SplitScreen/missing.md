# SAMPLE-076 — Split Screen audit

**Status: complete — no known behavior or content differences from the XNA 4.0 original.**

The historical `.model.json` blocker was stale. The port now loads the authentic official-pipeline
`tank.xnb`; live CNA's XNB `ModelReader` supplies every named bone, its hierarchy and each mesh's
parent bone. No sidecar model, raw buffer path or sample workaround is present.

Artifact root:
`/rv/tmp/samples/SAMPLE-076-SplitScreenSample_4_0/`

## Original surface audited

The selected product is the upstream Windows XNA 4.0 Debug/x86/HiDef project. Every runtime and
assembly unit was reviewed line by line:

- `Program.cs`
- `SplitScreenGame.cs`
- `Tank.cs`
- `Properties/AssemblyInfo.cs`

The Windows, Xbox and Phone project declarations, content project, FBX model, both TGA textures,
HTML documentation, solution files and inactive `WINDOWS_PHONE` timing/fullscreen branch were also
audited. The port retains the original 800x480 presentation, fixed top camera, time-orbiting bottom
camera, two independent viewport/projection pairs, two-pixel borders, input/exit order and all tank
animation formulas.

`Tank` again resolves and caches the nine original named bones, preserves their bind transforms,
updates four wheels, both steering bones, turret, cannon and hatch, calls
`CopyAbsoluteBoneTransformsTo`, selects each mesh's parent-bone transform, configures its
`BasicEffect`, enables default lighting and draws the same model twice. The only C++ differences
are ownership/value mechanics and required logical runtime type naming.

## Authentic content

`scripts/build-original.sh` builds the unchanged content project with XNA Game Studio 4.0's
official Windows/HiDef pipeline. The three checked-in XNBs are byte-identical to that retained
output:

| File | Bytes | SHA-256 |
|---|---:|---|
| `engine_diff_tex_0.xnb` | 699,291 | `bbb181f9095e2a953a57bac81aea675e6892e46bfa56cbf77e02fa18e6f64c61` |
| `tank.xnb` | 840,175 | `da6f9a6cb0993984b8bffeb87e6fe7f23af8055834e6c91d9046a80f27211f5b` |
| `turret_alt_diff_tex_0.xnb` | 699,291 | `9e86f9fe773ff333d3f0e20ce8f54e6082619732d53cfc798031b98042adf288` |

The model's reader graph contains the stock `ModelReader`, vertex/index/declaration readers and
`BasicEffectReader`, with its material textures as external references. `Content/` contains only
these exact XNA artifacts; there are no loose textures, converted geometry, generated headers or
JSON/binary sidecars.

## Qualification

All CNA builds used `CCACHE_DIR=/rv/cnaccache` and at most eight parallel jobs.

- The unchanged Windows/HiDef XNA executable built and ran under the isolated XNA 4.0 Wine
  environment with WineD3D. At two and four seconds it rendered the animated tank through the
  expected fixed top and orbiting bottom views, including the black divider, then exited cleanly
  through the original Escape path.
- Debug OPENGLES3 built from `cna-native-opengles3/`. On a real Mesa OpenGL ES 3.2 context it
  loaded all three XNBs through `Content.Load<Model>("tank")`, rendered the same two animated
  views at 800x480 and exited with code 0 after Escape.
- Release OPENGLES3 built independently in `cna-native-opengles3-release/` and passed the same
  real-context visual, animation, title and clean-exit gates.
- WEBGL2 built the complete `.html`, `.js`, `.wasm` and `.data` bundle. The system Google Chrome
  obtained `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, rendered both viewports at 800x480 with distinct
  two/four-second frame hashes, completed 600 additional `requestAnimationFrame` callbacks and
  reported no runtime exception, unhandled rejection, fatal console message or relevant HTTP
  error.
- Original, Debug, Release and browser captures show the same geometry, materials, bone animation,
  camera behavior, viewport placement and divider. Capture timing is intentionally live rather
  than frozen, so the orbiting lower camera is at slightly different angles between processes.
- The targeted scan found only the original platform conditionals and the required
  `CNAEXT GetTypeName()` declaration. No CNA or Sharp Runtime change was required.

## Retained evidence

- Exact source snapshot and hashes: `xna4-original/`, `original-manifest.txt`,
  `original-sha256.txt`
- Original official build: `xna4-build/windows-hidef/`, `evidence/build-original.log`
- Original run: `evidence/original-windows-hidef/`
- Debug native run: `evidence/cna-native-opengles3-qualified/`
- Release native run: `evidence/cna-native-opengles3-release-qualified/`
- Browser run and machine-readable result: `evidence/cna-web-webgl2-qualified/`
- Reproducible original/native/web build and capture drivers: `scripts/`

---

## Re-audited 2026-09-08: the captures are not reproducible, and the port matches anyway

Every frame of this sample is a function of `gameTime.TotalGameTime.TotalSeconds` --
`SplitScreenGame.cs:128-136` drives the wheels, steering, turret, cannon and hatch from it, and the
camera orbits on `Cos(time)`/`Sin(time)`. `capture-cna-native.sh` shoots after `sleep 2` and
`sleep 2` again, so the capture lands wherever process startup happens to put the animation phase.

Measured by re-running the same script:

| frame | CNA run vs CNA run | CNA vs XNA (recorded) | CNA vs XNA (re-run) |
| --- | --- | --- | --- |
| `01-split-screen-2s` | 0.110 | 0.055 | 0.115 |
| `02-split-screen-4s` | 0.108 | 0.077 | 0.114 |

**Two runs of the port differ from each other as much as the port differs from XNA**, so those
numbers measure the harness. The recorded 0.055 and 0.077 were a run that happened to land closer;
nothing regressed. `missing.md` never claimed a pixel match, which was the right call.

### With the phase matched, the port reproduces XNA

`scripts/probe-timing-sweep.sh` captures 48 frames at 0.15 s intervals across the animation and the
closest to each original capture is taken:

| original | closest swept frame | RMSE |
| --- | --- | --- |
| `01-split-screen-2s` | `t10` | 0.0425 |
| `02-split-screen-4s` | `t19` | **0.00486** |

At 0.0049 the two frames are indistinguishable: both viewports, both tanks in the same pose, the
same two-pixel borders, the same camera angles. That is the same order as SAMPLE-074's deterministic
start frame (0.0031), and it is a demonstration of equivalence the wall-clock captures could not
give. The `01` frame's 0.0425 is the sweep's own granularity -- 0.15 s is a long time for an
animation this fast -- not a second finding.

Nothing in the sample or the framework needed changing.
