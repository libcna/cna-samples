# Racing Game content source policy — owner override (2026-09-02)

**This file overrides `plan_racing.md` lines 37, 58 and 147 (and any other place in
`plan_racing.md`, `racing_baseline.md`, `racing_milestone1.md` or elsewhere that says the same
thing). Those lines declared the modern repository's GLB as the "canonical content strategy" and
told the plan to "Remove" `.X` conversion. That decision is wrong and is withdrawn.**

## The rule

**No glTF. No GLB. Not as the canonical source, not as a fallback, not as a convenience path.**

The port's canonical asset source is the **original XNA 4.0 sample**:

```
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/xna4-original/RacingGameWindows1/RacingGame/
```

- 57 models in `RacingGameContent/Models/*.X` (DirectX `.X` format), imported by `XImporter`
  and processed by the sample's own `RacingGameModelProcessor` (see the XNA 4.0
  `RacingGameContent.contentproj`; `ContentWindows.contentproj` is a stale XNA 3.0 project).
- Materials, effects, textures, audio banks: whatever the original `.contentproj` declares.
- The only legitimate path from these sources to a runtime `Model`/`Effect`/`TextureCube` is the
  real XNA/MonoGame **content pipeline**, run externally (Windows / the Win7 VM), producing real
  `.xnb` files, which CNA then loads through `ContentManager::Load<T>` — the same route already
  proven end-to-end by SAMPLE-028.

## What the modern repository (`rds1983/RacingGame`, `modern-fna-original/`) is for

**One thing only: a runnable behavior oracle.** It is useful for comparison screenshots, logs,
and confirming observable behavior (menus, physics, race flow) when the original can't be run.

It must **never** be a source of:
- models, materials, textures, or any other asset
- code that gets translated or copied into the C++ port
- a "canonical content strategy"

If a capability exists only via the modern repo's GLB/FNA-specific path, that is evidence the
capability is missing for the real port — not a reason to route the real port through GLB.

## What this means concretely

- Do **not** build, extend, or rely on a glTF/GLB import path for Racing content in CNA.
  (CNA's existing runtime `.gltf`/`.glb` loader may still exist and be used by *other* samples —
  this policy is scoped to Racing only.)
- `plan_racing.md`'s Milestone 2 ("GLB/material/raw proof") needs to be rewritten around the
  `.X` → content-pipeline → `.xnb` path, or replaced with whatever milestone actually proves that
  path works.
- Keep the existing headless FNA oracle setup (`evidence/fna-reference/`, the Xvfb scripts) —
  that's the legitimate oracle use and is not affected by this policy.

## Status

Sent to the running Codex session (thread `01a0526a-89fb-7450-a688-b82150b52e04`) as a queued
message on 2026-09-02. This file exists so the policy survives independent of that one message —
read it before doing any further Racing content/model work, and update `plan_racing.md` to match
it rather than treating this file as a permanent parallel source of truth.
