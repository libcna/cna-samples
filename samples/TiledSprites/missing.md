# SAMPLE-148 — `TiledSpritesSample_ARCHIVE_3_1` audit

## Status

The complete port and all native qualification are ready. The real system-Chrome WEBGL2 build
also renders correctly, but this row remains **ready/in progress** because its mandatory browser
interaction gate cannot currently be run through the project-required Browser extension route.
The extension and its native-host manifest are absent from the host Chrome profile. No standalone
browser automation or input substitute was used to turn that infrastructure gap into a false
pass.

Artifact root: `/rv/tmp/samples/SAMPLE-148-TiledSpritesSample_ARCHIVE_3_1/`.

## Complete source audit

The upstream delivery is an XNA **3.1** Windows/Xbox game, despite this campaign otherwise targeting
XNA 4.0. Its complete inventory is **10 files / 76,765 bytes**:

- three C# files: the 123-line `Game1`, entry point and assembly metadata;
- one Windows and one Xbox project plus the solution;
- one content project containing a 100×50 RGBA `XNA.png`;
- the thumbnail and Microsoft Permissive License.

The snapshot in `xna3-original/` is byte-identical to the delivery. The game is deliberately small
but complete: it clears an 800×480 backbuffer to CornflowerBlue, draws the XNA logo at `(50, 50)`
at half scale, and samples a source rectangle enlarged by integer `TilesX`/`TilesY` through wrap
addressing. Both counts start at two. It polls only player-one GamePad input: B/X increment/decrement
X, A/Y increment/decrement Y, and Back exits. Input repeats through the original
`double inputdelay = 1000 / 30`; C# performs integer division first, so the retained threshold is
33 milliseconds. The original has no keyboard, mouse, help overlay, clamping or alternate exit
path.

## Bounded XNA 4.0 migration

Compiling the unchanged game against Microsoft's XNA 4.0 assemblies reaches exactly the expected
removed calls:

```text
error CS0103: The name 'SpriteBlendMode' does not exist in the current context
error CS0103: The name 'SaveStateMode' does not exist in the current context
```

The isolated `xna4-migrated/` reference applies only the official transition documented by the
local XNA 4.0 “Tiling a Sprite” documentation: the legacy Begin/save-state sequence and mutable
device sampler slots become `SpriteBatch.Begin(..., SamplerState.LinearWrap, ...)`. Project and
pipeline references move from 3.1/.NET 3.5 to 4.0/.NET 4.0.

One explicit profile selection is also required. This delivery's texture is 100×50, and authentic
XNA 4.0 Reach rejects wrapping a non-power-of-two texture at runtime. Selecting
`GraphicsProfile.HiDef` is therefore part of the truthful XNA4 migration, not a CNA relaxation.
The C++ game makes the same selection. It does not resize, replace or decode the texture into a
different representation.

Microsoft's official XNA 4.0 Windows/Reach TextureProcessor builds the unchanged PNG into the XNB
committed by this sample:

```text
bb7745121c89bd48814dd3e8d7af0394f1b1ba9d50aa722ae39098e6d7b0944e  XNA.xnb
```

The pipeline log records `BuildContent (Windows/Reach) result: True`, and CNA's XNB-to-CNB
diagnostic accepts the same output. No loose image or runtime-generated replacement is used.

## CNA framework repair

The initial translation exposed a CNA const-correctness defect rather than a sample-specific need:
`SpriteBatch::Begin` accepted writable pointers for `SamplerState`, `DepthStencilState` and
`RasterizerState`, even though it only reads/copies them. CNA's built-in presets are `static const`,
forcing callers to cast away constness.

CNA commit `9ca0d4188` changes every applicable Begin overload to accept const state pointers and
adds `SpriteBatchTest.BeginAcceptsConstPresetStates`. The complete CNA graphics suite then passed
**2,289/2,342**, with the remaining 53 tests honestly skipped because they target other renderers
or unavailable multi-renderer configurations. The affected Release graphics module also builds.
The sample now passes `&SamplerState::LinearWrap` directly; no `const_cast`, state clone or hidden
renderer workaround remains.

## Behavioral and visual evidence

`TiledSprites_behavior_probe` derives only to capture the real backbuffer before present and drives
the public GamePad path through CNA's existing canned platform test support. It renders actual
frames and proves:

- initial 2×2 wrapped output;
- B expands only the horizontal bound;
- A expands only the vertical bound;
- X contracts only the horizontal bound;
- Y contracts only the vertical bound;
- Back stops before another Draw instead of relying on an invented Escape path.

The exact repeat count may vary with scheduling because each button is intentionally held across
the real 33ms gate; the assertions test the original directional behavior rather than hard-coding
a scheduler artifact. The final probe passes on real OPENGLES3.

The authentic migrated XNA4 executable and Debug CNA game were captured independently on explicit
Xvfb display `:148`. Both windows report 800×480. Their PNGs have the same SHA-256 and every pixel
matches:

```text
68ca0ad64a1fe4adfd4d0353f7b4f4bc6317f69bd8e2a1b83dbded254273abef
different_pixels=0
total_pixels=384000
```

Debug and Release OPENGLES3 targets build with at most eight workers. The Release executable
initializes the real GLES 3.2 EasyGL renderer and remains in its expected gamepad-only loop during
the bounded smoke run. The broader focused sample qualification previously passed all **206/206**
selected SpriteBatch, sampler, GamePad, GraphicsProfile and XNB/content tests; after the CNA repair,
the full **2,342-test** graphics suite provides the stronger framework regression gate.

## WEBGL2 evidence and outstanding gate

The clean Release WEBGL2 target builds the normal Emscripten game executable and its effective link
command contains:

```text
-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2
```

System Google Chrome was launched only on explicit Xvfb display `:348`, never on the host display.
It fetched all HTML/JS/Wasm/data products with HTTP 200, created the real WebGL2 renderer and drew
the expected 2×2 tiled logos on CornflowerBlue. The current 900×700 browser capture is retained,
and the Chrome log contains no uncaught error, unhandled rejection, abort, shader failure or WebGL
failure.

Visual rendering alone is not this sample's representative interaction gate because all behavior
is GamePad-driven. The required Browser-control route is currently unavailable:

```text
absent  ~/.config/google-chrome/Default/Extensions/hehggada
absent  ~/.config/google-chrome/NativeMessagingHosts/com.openai.codexextension.json
```

The Browser integration's own recovery rule is to reinstall/enable the Browser plugin rather than
constructing an extension or native-host manifest by hand. `/dev/uinput` and `/dev/input` are also
unavailable, so an OS-level virtual controller is not a truthful fallback. Once the owner restores
the supported Browser plugin route, the remaining gate is to exercise B/A/X/Y and Back in the
already-built WEBGL2 game and record the result. This infrastructure blocker does not justify
changing the sample or CNA input behavior.

## Evidence and reproducibility

- `xna3-original/` — immutable complete upstream snapshot;
- `evidence/source-inventory.tsv`, `original.sha256`, `snapshot-diff.txt` — inventory and snapshot
  integrity;
- `evidence/unchanged-game-vs-xna4.*` — unchanged-source compiler boundary;
- `xna4-migrated/`, `scripts/build-original.sh`, `evidence/xna4-*-build.log` — isolated authentic
  XNA4 migration, pipeline output and executable;
- `cna-diagnostic/` — XNB/CNB content diagnostic;
- `cna-native-opengles3/`, `cna-native-opengles3-release/` — Debug/Release native builds;
- `evidence/visual-parity/` and `scripts/capture-reference-and-native.sh` — exact XNA4/CNA pixels;
- `cna-web-webgl2/`, `scripts/build-cna-web.sh`, `scripts/chrome-xvfb-smoke.sh` and
  `evidence/cna-web-webgl2-xvfb-smoke/` — real WEBGL2 build and isolated system-Chrome render.

All supplied build scripts cap parallel work at eight workers. No artifact under `/rv/tmp/samples`
is part of the source commit.
