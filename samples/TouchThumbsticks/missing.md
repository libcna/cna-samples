# SAMPLE-080 — Touch Thumbsticks audit

**Status: complete — no known behavior or content differences from the XNA 4.0 original.**

The historical port was not an acceptable endpoint. It changed all four content identifiers,
substituted loose PNG files for compiled content, added independent keyboard/mouse controls,
published TouchPanel dimensions manually, added Escape/F1 behavior and loaded the documentation
image at runtime. All of those workarounds are removed.

Artifact root:
`/rv/tmp/samples/SAMPLE-080-TouchThumbsticksSample_4_0/`

## Original surface audited

All 22 files in the Windows Phone/Reach XNA 4.0 product were retained and reviewed. The complete
game surface consists of `TouchThumbsticksGame`, `VirtualThumbsticks`, `Ship`, `PlayerShip`,
`EnemyShip` and `Bullet`, plus the platform-provided entry boundary and assembly metadata. The
solution/project, both phone manifests, four source textures, icons, thumbnail, HTML document and
license were also included in the audit.

The port preserves the original namespace and class/member names. `Ship` remains abstract;
reference ownership in the C# `List<Bullet>` is represented by `unique_ptr` elements so the
readonly bullet rotation and deferred-removal behavior remain intact.

The original target requests 800x480, fullscreen on Windows Phone and a 30 Hz target time. It reads
only GamePad Back and raw `TouchPanel` snapshots. There is no keyboard, mouse, gesture recognizer,
Escape exit, F1 overlay or desktop fallback. The first touch in each screen half establishes that
stick's center, the touch ID remains tracked across later snapshots, an available previous location
is used as the earliest center, and displacement is divided by exactly 60 pixels and normalized
only beyond unit length.

The left stick applies 0.75 acceleration per update followed by 0.98 drag. The right stick aims
above magnitude 0.3 and fires a 20-pixel-per-update bullet every 0.15 seconds; otherwise the left
stick controls rotation above magnitude 0.2. The 1000x1000 clamp, bullet cleanup, enemy radius,
four-pixel border, camera transform, 1000 time-seeded stars, immediate first spawn, subsequent
two-second spawn interval, one-or-two-enemy `Random.Next(1, 3)` result, homing, collision/removal
order and both draw batches are direct translations of the original formulas and ordering.

`TouchThumbSticks.htm` is byte-identical to the upstream document. The repository's legacy
documentation-only `help.png` is retained beside it and is neither packaged nor loaded.

## Authentic content

The unchanged official XNA Game Studio 4.0 content pipeline built both Windows/Reach fixtures for
the desktop reference and Windows Phone/Reach fixtures for the authentic target. `Content/`
contains only the four retained Phone outputs, and each is byte-identical to the artifact:

| File | Bytes | SHA-256 |
|---|---:|---|
| `alien.xnb` | 4,283 | `6d581238223fd436431fa45b20811b67424ced275bc5d90e3e5a110ef5cb3448` |
| `bullet.xnb` | 699 | `bab5819fc71182e14541ba267b551c83473d331e26bdce3ff7f3b0caeb1e281a` |
| `player1.xnb` | 4,283 | `0f65e5347d30ba35510336a67b05e9f92cb6992788343708819b59f9bfe467fa` |
| `thumbstick.xnb` | 14,587 | `2cfe77736745edb5ee3561f315b1620684e08590649206ec5e1759c224d08793` |

The original asset names `alien`, `bullet`, `player1` and `thumbstick` are restored. The copied
`Images/*.png` files and sample-local help content are gone.

## Qualification

All CNA builds used `CCACHE_DIR=/rv/cnaccache` and no more than eight parallel jobs.

- The unchanged seven C# units and assembly metadata compiled against the XNA 4.0 Windows
  assemblies with the authentic Phone conditional active. The resulting reference executable ran
  through WineD3D at exactly 800x480 using the official Windows/Reach XNBs. Ordinary X11 pointer
  input produced neither virtual-stick indicator nor player control, as expected for the original
  touch-only implementation.
- Debug and Release OPENGLES3 builds both loaded the four authentic Phone XNBs on a real Mesa
  OpenGL ES 3.2 context. An external artifact-only SDL adapter supplied two independent finger IDs
  below CNA; it is not linked into or shipped with the sample.
- Both native configurations tracked the contacts simultaneously. The centered left contact
  produced about 1,548 green-dominant indicator pixels; dragging it shifted the world/camera.
  While it remained active, the right contact produced about 1,518–1,548 blue-dominant pixels,
  rotation and multiple red bullets. Releasing both removed both indicators, and queued SDL quit
  completed teardown without a runtime error.
- The complete Release WEBGL2 bundle ran in system Google Chrome. CDP supplied two simultaneous
  browser touch IDs. Chrome obtained `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, measured 1,986 green
  pixels for the left stick and 1,994 blue pixels with both active, increased red-dominant pixels
  from 1,972 to 3,335 while firing, removed the indicators on release, completed 600 additional
  `requestAnimationFrame` callbacks and reported no exception, rejection, fatal console message or
  relevant HTTP error.
- The starfield and enemies intentionally use a time-seeded static `Random`, so whole frames are
  nondeterministic. The deterministic 282-pixel player-ship mask at the initial centered position
  nevertheless matched the XNA reference pixel-for-pixel in Debug, Release and browser output;
  all three exact source colors matched at every player pixel.
- No CNA or Sharp Runtime change was required for this sample.

## Retained evidence

- Exact source snapshot and hashes: `xna4-original/`, `original-manifest.txt`,
  `original-sha256.txt`
- Official pipeline outputs and unchanged reference executable: `xna4-build/`,
  `evidence/xna-content-sha256.txt`
- Original run: `evidence/xna-original/`
- Debug and Release native multi-touch runs: `evidence/cna-native-opengles3/`,
  `evidence/cna-native-opengles3-release/`
- Real-browser result and captures: `evidence/cna-web-webgl2-qualified/`
- Deterministic visual comparison: `evidence/visual-comparison.json`
- Reproducible original/native/web build and capture drivers: `scripts/`
