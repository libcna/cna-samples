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

## Framework defects found and fixed

All three were found by running this sample, measured with gdb rather than inferred, and fixed in
`cnanext` (`plans/plan_fx.md` `FX-129`, `FX-130`, `FX-131`).

### FX-129 — a compiled pass's shader pair never became the bound program

The first gameplay frame threw

```
CNA EasyGL: this compiled effect's vertex shader requires attribute 'vs_v3' (usage 7, index 0),
but none of the 1 vertex stream(s) supplied to this draw declares an element with that usage
and usage index.
```

about an attribute `Particle.fx` does not have. `MOJOSHADER_glBindShaders` returns silently when a
pair fails to link -- `program = MOJOSHADER_glLinkProgram(v, p); if (program == NULL) return;` --
leaving the previous program bound, so `BindCompiledEffectForDrawEXT` read back `NormalMapping`'s
shader and validated the particle draw against it. A gdb trace of every `ApplyPass` and every
`MOJOSHADER_glBindShaders` call showed the sequence exactly: the Particle pass binds its own
shader, and the FX-098 bounce then rebinds NormalMapping's. `ApplyPass` now refuses a pass whose
selected pair is not the bound one, naming the GL link log.

### FX-130 — a Direct3D 9 pixel shader input that no vertex shader writes

The link failed for a real reason: `error: fragment shader input 'io_5_0' has no matching output
in the previous stage`. `Particle.fx`'s pixel shader reads `TEXCOORD0` because Direct3D generated
it in the rasterizer for point sprites; its vertex shader outputs only `POSITION`, `PSIZE`,
`COLOR0` and `COLOR1`. D3D9 links the stages by register and tolerates that; GLSL links by name and
refuses it. XNA 4.0 removed point sprites -- which is why this sample's own `ParticleManager` draws
`LineList` with the point-sprite draw commented out beside it -- and still runs the effect
unchanged. A sixth pinned MojoShader patch gives such inputs a producer-free definition at link
time, where the pairing that decides which are unmatched is known.

### FX-131 — a short `SetData` shrank the buffer a later draw may read

With the program bound the draw was refused instead: *"The requested primitive range exceeds the
bound vertex buffer (Parameter 'primitiveCount') Actual value was 100."* XNA fixes
`VertexBuffer.VertexCount` at construction and `SetData(data, startIndex, elementCount)` writes a
prefix; CNA tracked the last upload both in the draw validation and in the GL allocation.
`ParticleManager` allocates 8192 vertices, re-uploads only the live particles, and then draws
`LineList` passing the vertex count as the primitive count -- an upstream quirk that asks for 200
vertices out of 8192, and is reproduced here rather than corrected.

## Verification

Native `OPENGLES3` on Xvfb 1920x1080, window 1280x720: the game runs intro, ship selection, level
selection, live gameplay and the end screen, and exits cleanly (`exit=0`). The gameplay frame
matches `evidence/original/05-game.png` corridor for corridor -- same normal-mapped walls, lava
windows, hazard frames, central bloom, crosshair and HUD -- and ship selection matches
`evidence/original/02-player.png` apart from the ship's continuous rotation phase.

## Deviations

See `diff.md`.
