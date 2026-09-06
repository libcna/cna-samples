# SAMPLE-066 — ShipGame (XNA 4.0 Ship Game Starter Kit)

Artifact root: `/rv/tmp/samples/SAMPLE-066-ShipGame_4_0/`
Status: 🛠 in progress.

## Upstream product

One runnable product, `ShipGameWindows.sln`, built from three projects:

| Project | Units | Ported to |
|---|---|---|
| `ShipGame` | 28 `.cs` | `src/` |
| `BoxCollider` | 9 `.cs` | `src/BoxCollider/` |
| `NormalMappingModelProcessor` | 1 `.cs` | content-pipeline only — not runtime code |

Every C# unit of the two runtime projects is ported and the executable links.

## Original build and capture (retained)

The unchanged original builds and runs on this host:

- `scripts/build-original.sh` compiles the unchanged `NormalMappingModelProcessor` with XNA 4.0's
  own `csc.exe` under Wine, then drives the unchanged `ShipGameContentWindows.contentproj` through
  the official `BuildContent` MSBuild task (`XnaPipelineRunner.exe`, mono-compiled).
  Output: 159 `.xnb` + `sounds.xgs` + `Wave Bank.xwb` + `Sound Bank.xsb` + 10 loose `.xml`.
- `scripts/build-original-game.sh` builds `ShipGameWindows.exe`.
- `scripts/capture-original.sh` runs it (prefix `~/.wine-cna-xna40`, `WINEDLLOVERRIDES=d3d9=b`)
  and captures intro, ship select, level select and live gameplay:
  `evidence/original/0{1..5}-*.png`.

`Content/` in this sample is that pipeline output, copied byte-for-byte from
`xna4-build/bin-windows/Content/`.

## Content directory spelling

The original spells its own content directory two ways and NTFS makes them one directory:

- `Content.RootDirectory = "Content"` — every `Content.Load<T>()`;
- `"content/levels/…"`, `"content/ships/…"`, `"content/screens/…"`, `"content/sounds/…"` —
  literal `System.IO` paths in `EntityList.Load`, `LightList.Load` and the XACT constructors.

On a case-sensitive filesystem only one spelling can exist. `System.IO` is the strict reader
(sharp-runtime's `File` is a faithful .NET reimplementation and .NET on Unix is case-sensitive),
while CNA's `ContentManager` already resolves an asset path case-insensitively, so the deployed
directory takes the `System.IO` spelling and `ContentManager` resolves the other one. This is a
packaging decision, not a behaviour change: `cna_add_sample` gained an optional `CONTENT_NAME`
argument that defaults to `Content`, and this sample passes `content`.

## Framework gaps found so far

### GFX — a compiled effect's pass does not become the bound GL program (open)

Reproducer: enter gameplay (single player, any level). The first `ParticleManager.Draw` throws

```
CNA EasyGL: this compiled effect's vertex shader requires attribute 'vs_v3' (usage 7, index 0),
but none of the 1 vertex stream(s) supplied to this draw declares an element with that usage
and usage index.
```

Measured with gdb, not inferred:

- The device's `currentEffect_` at the failing draw **is** `shaders/Particle`
  (same pointer as `ParticleManager`'s own `effect_`), and its compiled runtime's technique is
  `Particle`, index 0, `passActive_ = true`.
- That pass's two states are `MOJOSHADER_RS_VERTEXSHADER` → object 3 and
  `MOJOSHADER_RS_PIXELSHADER` → object 4; both objects have the right `MOJOSHADER_SYMTYPE_*`,
  `is_preshader == 0`, and a non-null shader. `MOJOSHADER_effectBeginPass` therefore sets
  `effect->current_vert` to Particle's own vertex shader — a trace of every `ApplyPass` confirms
  it (`APPLYPASS tech=Particle curVert=0x…`).
- Yet `MOJOSHADER_glGetBoundShaders` at the draw returns a **NormalMapping** vertex shader
  (reflection: `POSITION0, TEXCOORD0, NORMAL0, BINORMAL0, TANGENT0` — exactly
  `NormalMappingVS`), and the particle stream is `VertexPositionNormalTexture`, so the binormal
  input has no source.
- A breakpoint on `MOJOSHADER_glBindShaders` shows **no** call between Particle's `ApplyPass` and
  the draw except the two the FX-098 bounce makes inside `BindCompiledEffectForDrawEXT` itself —
  and those rebind what `MOJOSHADER_glGetBoundShaders` already returned. So
  `MOJOSHADER_effectBeginPass` set the effect's own `current_vert` without that reaching the GL
  context's bound program.

Consequence: only the *first* compiled effect used in a frame is really bound; every later draw
with a different compiled effect silently runs the previous one's program, and fails loudly only
when the two vertex layouts disagree. The menus hide it because `Blur` is the only compiled effect
there.

Next step: determine what `effect->ctx.bindShaders` is wired to for a CNA-created
`MOJOSHADER_effect`, and whether the effect runtime and the GL adapter are sharing one shader
context at all.

## Deviations

See `diff.md`.
