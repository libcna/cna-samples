#!/usr/bin/env python3
# SPDX-License-Identifier: MS-PL

import pathlib
import sys


def records(path: pathlib.Path) -> dict[str, str]:
    lines = path.read_text(encoding="utf-8").splitlines()
    if not lines or not lines[0].startswith("FORMAT racing-"):
        raise ValueError(f"{path}: missing Racing physics format record")
    if not lines or lines[-1] != "RESULT PASS":
        raise ValueError(f"{path}: oracle did not pass")
    result: dict[str, str] = {}
    for line in lines[1:-1]:
        key, separator, payload = line.partition(" ")
        if not separator or key in result:
            raise ValueError(f"{path}: malformed or duplicate record: {line}")
        result[key] = payload
    return result


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: compare-physics-oracle.py FNA_REPORT CNA_REPORT",
              file=sys.stderr)
        return 2
    fna = records(pathlib.Path(sys.argv[1]))
    cna = records(pathlib.Path(sys.argv[2]))
    if fna != cna:
        print("RACING_PHYSICS_ORACLE_COMPARISON=FAIL", file=sys.stderr)
        print(f"FNA={fna}", file=sys.stderr)
        print(f"CNA={cna}", file=sys.stderr)
        return 1
    print("RACING_PHYSICS_ORACLE_COMPARISON=PASS "
          f"records={len(fna)} vector={fna['VECTOR']} spring={fna['SPRING']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
