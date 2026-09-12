# Stock Effects — CompileEffect

This is the CNA C++ port of the original sample's `CompileEffect` command-line Content Pipeline
tool. The upstream package is not a game, so this directory intentionally provides no native game
window and no Web build.

The tool invokes CNA's XNA-shaped `EffectImporter` and `EffectProcessor` exactly like the original:

```text
CompileEffect <targetPlatform> <targetProfile> <input.fx> <output.bin>
```

Valid XNA 4.0 target platforms are `Windows`, `Xbox360`, and `WindowsPhone`; profiles are `Reach`
and `HiDef`. Source compilation needs an `fxc`-compatible compiler. Set `CNA_FXC` to its path and,
when running the Windows executable through Wine, set `CNA_FXC_LAUNCHER=wine`.

The educational `StockEffects` runtime library and its six stock-effect copies are deliberately not
ported here: CNA already implements the XNA stock-effect API, and the project owner selected the
Content Pipeline tool as SAMPLE-004's port boundary.
