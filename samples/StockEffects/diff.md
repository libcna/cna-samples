# SAMPLE-004 — scope decision and C++ mechanics

The project owner selected a Content Pipeline-only port. The original directory contains no game,
scene, input, or visual executable, so no native or browser demo is invented. Only its directly
runnable `CompileEffect` authoring tool is translated; the separate educational `StockEffects`
runtime library is outside this sample's approved boundary.

C# generic `Enum.TryParse` and `Enum.GetValues` become explicit parsing and name tables for the two
closed XNA enums. C# virtual generic context members become CNA's established non-template virtual
`BuildAndLoadAssetCore`, `BuildAssetCore`, and `ConvertCore` bridge. Those members still throw
`System::NotImplementedException`, exactly like the original custom context, and are not reached by
`EffectProcessor`.

Everything else follows the original flow in the same order: validate four arguments, construct the
logger, import with `EffectImporter`, process with `EffectProcessor`, and write `GetEffectCode()` to
the requested `.bin` path. Compiler discovery is CNA's standard `CNA_FXC` / `CNA_FXC_LAUNCHER`
host configuration rather than a new sample-specific path.
