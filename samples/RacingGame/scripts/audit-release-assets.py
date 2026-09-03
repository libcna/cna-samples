#!/usr/bin/env python3
"""Verify the retained original Racing snapshot and authentic XNA build."""

from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import sys


EXPECTED_SOURCE_ASSETS = {
    ".x": 57,
    ".tga": 129,
    ".png": 11,
    ".dds": 2,
    ".fx": 10,
    ".wav": 28,
    ".xap": 1,
    ".track": 3,
    ".combimodel": 10,
    ".data": 1,
}
EXPECTED_XNB_GROUPS = {
    "Audio": 28,
    "Models": 57,
    "Shaders": 10,
    "Textures": 244,
}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def verify_manifest(manifest: Path, root: Path) -> int:
    count = 0
    for line_number, line in enumerate(
        manifest.read_text(encoding="utf-8").splitlines(), start=1
    ):
        if not line:
            continue
        try:
            expected, relative = line.split("  ", 1)
        except ValueError as error:
            raise RuntimeError(
                f"malformed manifest line {manifest}:{line_number}"
            ) from error
        path = root / relative
        if not path.is_file():
            raise RuntimeError(f"manifest file is missing: {path}")
        actual = sha256(path)
        if actual != expected:
            raise RuntimeError(
                f"manifest hash mismatch: {path} expected={expected} actual={actual}"
            )
        count += 1
    return count


def suffix_counts(root: Path) -> dict[str, int]:
    result: dict[str, int] = {}
    for path in root.rglob("*"):
        if path.is_file():
            suffix = path.suffix.lower()
            result[suffix] = result.get(suffix, 0) + 1
    return result


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "artifact_root",
        nargs="?",
        type=Path,
        default=Path(
            "/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master"
        ),
    )
    args = parser.parse_args()
    artifact = args.artifact_root.resolve()
    original = artifact / "xna4-original"
    content_source = (
        original / "RacingGameWindows1/RacingGame/RacingGameContent"
    )
    authentic = artifact / "evidence/xna4-authentic-build"
    authentic_content = authentic / "Debug/Content"

    source_files = verify_manifest(
        artifact / "evidence/xna4-original.sha256", original
    )
    if source_files != 325:
        raise RuntimeError(f"expected 325 source files, found {source_files}")
    print(f"SOURCE_MANIFEST=PASS files={source_files}")

    authentic_files = verify_manifest(
        authentic / "SHA256SUMS-debug.txt", authentic
    )
    if authentic_files != 358:
        raise RuntimeError(
            f"expected 358 authentic build files, found {authentic_files}"
        )
    print(f"AUTHENTIC_XNA4_MANIFEST=PASS files={authentic_files}")

    counts = suffix_counts(content_source)
    for suffix, expected in EXPECTED_SOURCE_ASSETS.items():
        actual = counts.get(suffix, 0)
        if actual != expected:
            raise RuntimeError(
                f"source asset count mismatch for {suffix}: "
                f"expected={expected} actual={actual}"
            )
        print(f"SOURCE_ASSET {suffix[1:]}={actual}")

    xnb_groups: dict[str, int] = {}
    for path in authentic_content.rglob("*.xnb"):
        group = path.relative_to(authentic_content).parts[0]
        xnb_groups[group] = xnb_groups.get(group, 0) + 1
    if xnb_groups != EXPECTED_XNB_GROUPS:
        raise RuntimeError(
            f"authentic XNB inventory mismatch: {xnb_groups!r}"
        )
    print(
        "AUTHENTIC_XNB "
        + " ".join(
            f"{group}={count}"
            for group, count in EXPECTED_XNB_GROUPS.items()
        )
        + f" total={sum(xnb_groups.values())}"
    )

    required_outputs = {
        ".xgs": 1,
        ".xsb": 1,
        ".xwb": 1,
        ".track": 3,
        ".combimodel": 10,
        ".data": 1,
    }
    output_counts = suffix_counts(authentic_content)
    for suffix, expected in required_outputs.items():
        actual = output_counts.get(suffix, 0)
        if actual != expected:
            raise RuntimeError(
                f"authentic output count mismatch for {suffix}: "
                f"expected={expected} actual={actual}"
            )
    print("AUTHENTIC_NON_XNB=PASS files=17")

    license_names = ("license", "eula", "copying")
    license_files = [
        path
        for path in original.rglob("*")
        if path.is_file()
        and (
            path.suffix.lower() == ".rtf"
            or any(name in path.name.lower() for name in license_names)
        )
    ]
    if license_files:
        print("CANONICAL_LICENSE_FILES=REVIEW")
        for path in license_files:
            print(path.relative_to(original))
    else:
        print("CANONICAL_LICENSE_FILES=0")
    print("SOURCE_NOTICE=Microsoft Corporation; All rights reserved")
    print("REDISTRIBUTION_STATUS=BLOCKED_MISSING_CANONICAL_LICENSE")
    print("RESULT PASS")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError) as error:
        print(f"RESULT FAIL: {error}", file=sys.stderr)
        raise SystemExit(1)
