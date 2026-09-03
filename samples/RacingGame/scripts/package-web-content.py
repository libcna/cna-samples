#!/usr/bin/env python3
# SPDX-License-Identifier: MS-PL

import argparse
import json
import os
from pathlib import Path
import shutil


BOOTSTRAP_FILES = {
    "Audio/RacingGameManager.xgs",
    "Audio/Sound Bank.xsb",
    "Audio/Wave Bank.xwb",
    "Shaders/LineRendering.xnb",
    "Textures/ColorSelection.xnb",
    "Textures/GameFont.xnb",
    "Textures/HelpScreenWindows.xnb",
    "Textures/ingame.xnb",
    "Textures/MouseCursor.xnb",
    "Textures/OptionsScreenWindows.xnb",
    "Textures/background.xnb",
    "Textures/buttons.xnb",
    "Textures/headers.xnb",
    "Textures/pokal1.xnb",
    "Textures/pokal2.xnb",
    "Textures/pokal3.xnb",
}

MODEL_SHADERS = {
    "Shaders/LightingShader.xnb",
    "Shaders/NormalMapping.xnb",
    "Shaders/ReflectionSimpleGlass.xnb",
}

POSTPROCESS_FILES = {
    "Shaders/PostScreenGlow.xnb",
    "Shaders/PostScreenMenu.xnb",
    "Shaders/PostScreenShadowBlur.xnb",
    "Shaders/ShadowMap.xnb",
    "Textures/Circle.xnb",
    "Textures/Glow.xnb",
    "Textures/Halo.xnb",
    "Textures/Lens.xnb",
    "Textures/Noise128x128.xnb",
    "Textures/Ring.xnb",
    "Textures/ScreenBorderFadeout.xnb",
    "Textures/ShadowDistanceFadeoutMap.xnb",
    "Textures/Streaks.xnb",
    "Textures/Sun.xnb",
}

PLATFORM_EXCLUSIONS = {
    "Textures/HelpScreenXbox360.xnb",
    "Textures/OptionsScreenXbox360.xnb",
}

EXCLUSION_REASONS = {
    "Audio/Waves/": (
        "XACT build intermediates already compiled into the authentic Wave Bank.xwb"),
    "Textures/HelpScreenXbox360.xnb": (
        "Xbox-only screen; the Web build selects HelpScreenWindows.xnb"),
    "Textures/OptionsScreenXbox360.xnb": (
        "Xbox-only screen; the Web build selects OptionsScreenWindows.xnb"),
}


def classify(relative_path: str) -> str:
    if relative_path.startswith("Audio/Waves/") or relative_path in PLATFORM_EXCLUSIONS:
        return "excluded"
    if relative_path in BOOTSTRAP_FILES:
        return "bootstrap"
    if relative_path in MODEL_SHADERS:
        return "models"
    if relative_path in POSTPROCESS_FILES:
        return "textures"
    if relative_path in {"Models/Car.xnb", "Models/CarSelectionPlate.xnb"}:
        return "models"
    if relative_path.startswith("Textures/RacerCar"):
        return "models"
    if relative_path.startswith("Textures/CarSelectionPlate"):
        return "models"
    if relative_path.startswith("Textures/SkyCubeMap"):
        return "models"
    if relative_path.startswith("Textures/NormalizeCubeMap"):
        return "models"
    return "landscape"


def link_or_copy(source: Path, destination: Path) -> None:
    destination.parent.mkdir(parents=True, exist_ok=True)
    try:
        os.link(source, destination)
    except OSError:
        shutil.copy2(source, destination)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Split authentic Racing XNA output into lossless Web load groups")
    parser.add_argument("--content-root", required=True, type=Path)
    parser.add_argument("--output-root", required=True, type=Path)
    arguments = parser.parse_args()

    content_root = arguments.content_root.resolve()
    output_root = arguments.output_root.resolve()
    if not (content_root / "Models/Car.xnb").is_file():
        parser.error(f"not an authentic Racing Content root: {content_root}")
    if output_root == content_root or content_root in output_root.parents:
        parser.error("output root must not be the canonical Content tree or one of its children")
    if output_root == Path(output_root.anchor) or output_root == Path.home():
        parser.error("refusing to replace a broad output root")

    if output_root.exists():
        shutil.rmtree(output_root)
    output_root.mkdir(parents=True)

    groups = {
        name: {"files": [], "bytes": 0}
        for name in ("bootstrap", "models", "landscape", "textures", "excluded")
    }
    source_files = sorted(path for path in content_root.rglob("*") if path.is_file())
    for source in source_files:
        relative_path = source.relative_to(content_root).as_posix()
        group = classify(relative_path)
        size = source.stat().st_size
        groups[group]["files"].append(relative_path)
        groups[group]["bytes"] += size
        if group != "excluded":
            link_or_copy(source, output_root / group / relative_path)

    canonical_bytes = sum(source.stat().st_size for source in source_files)
    packaged_source_bytes = sum(groups[name]["bytes"] for name in
                                  ("bootstrap", "models", "landscape", "textures"))
    if packaged_source_bytes + groups["excluded"]["bytes"] != canonical_bytes:
        raise RuntimeError("content classification did not account for every source byte")

    missing_bootstrap = sorted(BOOTSTRAP_FILES - set(groups["bootstrap"]["files"]))
    if missing_bootstrap:
        raise RuntimeError(f"missing bootstrap files: {missing_bootstrap}")

    for group in ("models", "landscape", "textures"):
        marker = output_root / group / f".racing-{group}-ready"
        marker.write_text("ready\n", encoding="utf-8")
        groups[group]["files"].append(marker.name)
        groups[group]["bytes"] += marker.stat().st_size

    manifest = {
        "canonicalRoot": str(content_root),
        "canonicalFiles": len(source_files),
        "canonicalBytes": canonical_bytes,
        "packagedSourceBytes": packaged_source_bytes,
        "packagedBytes": sum(groups[name]["bytes"] for name in
                               ("bootstrap", "models", "landscape", "textures")),
        "packageMarkerBytes": sum(groups[name]["bytes"] for name in
                                   ("bootstrap", "models", "landscape", "textures"))
                              - packaged_source_bytes,
        "excludedBytes": groups["excluded"]["bytes"],
        "exclusionReasons": EXCLUSION_REASONS,
        "groups": groups,
    }
    (output_root / "manifest.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    for name, group in groups.items():
        print(f"{name}: {len(group['files'])} files, {group['bytes']} bytes")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
