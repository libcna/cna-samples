#!/usr/bin/env python3
# SPDX-License-Identifier: MS-PL

from __future__ import annotations

import argparse
import math
from pathlib import Path


def read_report(path: Path) -> str:
    text = path.read_text(encoding="utf-8-sig")
    if "RESULT PASS" not in text:
        raise ValueError(f"oracle report did not pass: {path}")
    return text


def read_ppm(path: Path) -> tuple[int, int, bytes]:
    data = path.read_bytes()
    if not data.startswith(b"P6"):
        raise ValueError(f"not a binary PPM: {path}")

    offset = 2
    tokens: list[bytes] = []
    while len(tokens) < 3:
        while offset < len(data) and data[offset] in b" \t\r\n":
            offset += 1
        if offset < len(data) and data[offset] == ord("#"):
            while offset < len(data) and data[offset] not in b"\r\n":
                offset += 1
            continue
        start = offset
        while offset < len(data) and data[offset] not in b" \t\r\n":
            offset += 1
        tokens.append(data[start:offset])

    width, height, maximum = map(int, tokens)
    if maximum != 255:
        raise ValueError(f"unsupported PPM maximum {maximum}: {path}")
    while offset < len(data) and data[offset] in b" \t\r\n":
        offset += 1
    pixels = data[offset:]
    if len(pixels) != width * height * 3:
        raise ValueError(f"truncated PPM payload: {path}")
    return width, height, pixels


def normalized_rmse(left: bytes, right: bytes) -> float:
    if len(left) != len(right):
        raise ValueError("cannot compare differently sized images")
    squared_error = sum((a - b) ** 2 for a, b in zip(left, right))
    return math.sqrt(squared_error / len(left)) / 255.0


def image(directory: Path, prefix: str, suffix: str) -> tuple[int, int, bytes]:
    return read_ppm(directory / f"{prefix}-{suffix}.ppm")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Compare Racing Effect pixels from XNA 4, FNA/OpenGL and CNA/OpenGL33"
    )
    parser.add_argument("xna_directory", type=Path)
    parser.add_argument("fna_directory", type=Path)
    parser.add_argument("cna_directory", type=Path)
    args = parser.parse_args()

    read_report(args.xna_directory / "xna-effects.txt")
    read_report(args.fna_directory / "fna-effects.txt")
    read_report(args.cna_directory / "cna-effects.txt")

    suffixes = (
        "normal",
        "normal-clone",
        "blur-horizontal",
        "blur-vertical",
    )
    fna_cna_exact = 0
    for suffix in suffixes:
        fna = image(args.fna_directory, "fna", suffix)
        cna = image(args.cna_directory, "cna", suffix)
        if fna != cna:
            raise ValueError(f"FNA/CNA OPENGL pixel mismatch: {suffix}")
        fna_cna_exact += 1

    xna_normal = image(args.xna_directory, "xna", "normal")
    cna_normal = image(args.cna_directory, "cna", "normal")
    xna_clone = image(args.xna_directory, "xna", "normal-clone")
    cna_clone = image(args.cna_directory, "cna", "normal-clone")
    if xna_normal != cna_normal or xna_clone != cna_clone:
        raise ValueError("XNA/CNA normal-map or clone pixels are not exact")

    blur_rmse: dict[str, float] = {}
    for suffix in ("blur-horizontal", "blur-vertical"):
        xna = image(args.xna_directory, "xna", suffix)
        cna = image(args.cna_directory, "cna", suffix)
        if xna[:2] != cna[:2]:
            raise ValueError(f"XNA/CNA blur dimensions differ: {suffix}")
        value = normalized_rmse(xna[2], cna[2])
        if value > 0.04:
            raise ValueError(
                f"XNA D3D9/CNA OpenGL blur RMSE {value:.6f} exceeds 0.04: {suffix}"
            )
        blur_rmse[suffix] = value

    print(
        "RACING_EFFECT_ORACLE_COMPARISON=PASS "
        f"fna_cna_exact={fna_cna_exact} xna_cna_normal_exact=2 "
        f"xna_gl_horizontal_rmse={blur_rmse['blur-horizontal']:.6f} "
        f"xna_gl_vertical_rmse={blur_rmse['blur-vertical']:.6f}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
