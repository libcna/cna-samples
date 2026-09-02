#!/usr/bin/env python3
# SPDX-License-Identifier: MS-PL

import math
import pathlib
import sys


def read_ppm(path: pathlib.Path) -> tuple[int, int, bytes]:
    data = path.read_bytes()
    try:
        magic, dimensions, maximum, pixels = data.split(b"\n", 3)
        width_text, height_text = dimensions.split()
    except ValueError as error:
        raise ValueError(f"{path}: malformed binary PPM header") from error
    width = int(width_text)
    height = int(height_text)
    if magic != b"P6" or maximum != b"255":
        raise ValueError(f"{path}: expected P6 with an 8-bit maximum")
    if len(pixels) != width * height * 3:
        raise ValueError(f"{path}: incomplete pixel payload")
    return width, height, pixels


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: compare-static-scene.py <FNA PPM> <CNA PPM>", file=sys.stderr)
        return 2

    fna_width, fna_height, fna = read_ppm(pathlib.Path(sys.argv[1]))
    cna_width, cna_height, cna = read_ppm(pathlib.Path(sys.argv[2]))
    if (fna_width, fna_height) != (cna_width, cna_height):
        print("RACING_STATIC_SCENE_COMPARISON=FAIL dimensions", file=sys.stderr)
        return 1

    differences = [abs(left - right) for left, right in zip(fna, cna)]
    normalized_rmse = math.sqrt(
        sum(value * value for value in differences) / len(differences)
    ) / 255.0
    within_two = sum(value <= 2 for value in differences) / len(differences)

    # FNA uses MojoShader's compatibility-profile GLSL while CNA uses the
    # OPENGL33 core renderer. The bounds retain sub-one-percent aggregate error
    # and reject broad scene, camera, material, texture or geometry divergence.
    passed = normalized_rmse <= 0.01 and within_two >= 0.97
    print(
        "RACING_STATIC_SCENE_COMPARISON={} width={} height={} rmse={:.6f} "
        "channelsWithin2={:.6f} maxChannelDelta={}".format(
            "PASS" if passed else "FAIL",
            fna_width,
            fna_height,
            normalized_rmse,
            within_two,
            max(differences),
        )
    )
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
