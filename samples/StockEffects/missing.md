# SAMPLE-004 audit — `StockEffectsSample_4_0`

Status: owner decision required (`🛑`). No C++ port or sample-side substitute has been committed.

## Classification

This upstream directory is not a runnable `Game` and has no scene, window, controls or visual
output. It contains two Windows/Xbox authoring projects:

- `CompileEffect`: a command-line program that invokes the XNA Content Pipeline's
  `EffectImporter` and `EffectProcessor` and writes their raw compiled effect bytes;
- `StockEffects`: a library in the separate `StockEffects` namespace containing educational
  copies of `BasicEffect`, `SkinnedEffect`, `EnvironmentMapEffect`, `DualTextureEffect`,
  `AlphaTestEffect` and `SpriteEffect`, with the six compiled blobs embedded as resources.

The executable/library portion is 369 lines of compiler/context C#, 2,680 lines of effect-wrapper
C#, and generated resource/assembly code. The shader portion is 2,013 lines across six `.fx` files
and four `.fxh` includes. A complete port is therefore a tool/library project, not another game
sample.

## Original evidence

All artifacts are under `/rv/tmp/samples/SAMPLE-004-StockEffectsSample_4_0`.

- `xna4-original/StockEffects`: exact upstream source snapshot;
- `xna4-build/CompileEffect/CompileEffect.exe`: unchanged original compiler source built against
  the official XNA 4.0 Windows/x86 assemblies;
- `xna4-build/effect-bin`: all six `.bin` files produced successfully by that executable under the
  prepared 32-bit XNA Wine prefix;
- `xna4-build/StockEffectsWindows/StockEffects.dll`: unchanged original effect-wrapper source built
  with all six generated resources embedded.

The source files were compiled directly because the extracted SDK assemblies are available on
Linux while the Visual Studio/XNA MSBuild targets are not installed in that Wine prefix. No source
logic was modified. There is no original visual executable to capture or compare.

## Current CNA evidence

CNA already implements the six XNA stock effect classes in
`Microsoft::Xna::Framework::Graphics`, and its public `Effect(GraphicsDevice&, effectCode)` path
accepts raw compiled Effect Framework bytes. This does not automatically port the sample's
separate `StockEffects` classes, their wrapper logic or their authoring tool.

The six exact blobs generated from this sample differ byte-for-byte from CNA's retained FNA
fixtures, so they were tested directly:

- the MojoShader probe parsed all six, reporting 66 shader objects and 86,700 bytes of Shader
  Model bytecode with no parse failure;
- the OPENGLES3 compiled-effect benchmark constructed the exact SpriteEffect, BasicEffect and
  SkinnedEffect blobs, cloned/applied BasicEffect and rendered 500 compiled-effect draws
  successfully.

There is no CNA or sharp-runtime implementation of the design-time
`Microsoft.Xna.Framework.Content.Pipeline` surface used here: `EffectImporter`, `EffectProcessor`,
`EffectContent`, `CompiledEffectContent`, `ContentImporterContext`, `ContentProcessorContext` and
their dependency/build contracts. CNA's runtime compiled-effect parser is not an HLSL Effect
Framework source compiler.

No native sample target or WEBGL2 bundle was invented: upstream has no runnable program to map to
those gates, and the owner must first choose the intended tool/library scope and browser outcome.

## Owner decision — `SAMPLES-DEC-002`

1. **End this audited row as a non-game tool/library.** Retain the original C# build and evidence,
   make no C++ alias or fake browser demo, and continue with `SAMPLE-005`.
2. **Port only the educational `StockEffects` library.** Translate all seven wrapper/helper files
   into the original separate namespace and embed reproducible compiled blobs, while explicitly
   accepting that `CompileEffect` is outside this row. A purpose-built verification harness and a
   definition of the web artifact would still be needed because upstream supplies none.
3. **Port the complete package.** First implement a reusable CNA content-pipeline authoring subset
   and cross-platform effect-source compiler, then port the CLI, six wrapper classes, resource
   generation, native tests and an owner-approved web tool interface. This is the largest option
   but could later support other `SAMPLES-DEC-002` rows.

Using CNA's built-in effects as aliases for the sample classes, dropping the CLI without approval,
or inventing a game around the library would be a workaround and is not an accepted fourth option.
