#!/usr/bin/env python3
# SPDX-License-Identifier: MS-PL

from __future__ import annotations

import difflib
import pathlib
import sys


EXACT_RECORDS = {
    "LANDSCAPE",
    "TRACK",
    "FIELDHASH",
    "KINEMATIC",
    "CHECKPOINT",
    "PHASEHASH",
    "ROAD",
    "GUARD",
    "COLUMN",
    "HELPER",
    "COMBI",
}


def canonical_records(path: pathlib.Path, require_native_passes: bool) -> list[str]:
    lines = path.read_text(encoding="utf-8").splitlines()
    if not lines or not lines[0].startswith("FORMAT racing-"):
        raise RuntimeError(f"{path}: missing Racing track-oracle format marker")
    if "RESULT PASS" not in lines:
        raise RuntimeError(f"{path}: oracle did not report RESULT PASS")

    records: list[str] = []
    for line in lines:
        record = line.split(" ", 1)[0]
        if record not in EXACT_RECORDS:
            continue
        if require_native_passes:
            if " result=PASS" not in line and record in {
                "LANDSCAPE", "TRACK", "ROAD", "GUARD", "COLUMN", "COMBI"
            }:
                raise RuntimeError(f"{path}: native record did not pass: {line}")
            line = line.replace(" result=PASS", "")
        records.append(line)
    return records


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: compare-track-oracles.py <FNA report> <CNA report>", file=sys.stderr)
        return 2

    fna_path = pathlib.Path(sys.argv[1])
    cna_path = pathlib.Path(sys.argv[2])
    fna = canonical_records(fna_path, False)
    cna = canonical_records(cna_path, True)
    if fna != cna:
        print(
            "\n".join(
                difflib.unified_diff(
                    fna,
                    cna,
                    fromfile=str(fna_path),
                    tofile=str(cna_path),
                    lineterm="",
                )
            ),
            file=sys.stderr,
        )
        return 1

    counts: dict[str, int] = {}
    for line in fna:
        record = line.split(" ", 1)[0]
        counts[record] = counts.get(record, 0) + 1
    summary = " ".join(f"{name}={counts[name]}" for name in sorted(counts))
    print(f"RACING_TRACK_ORACLE_COMPARISON=PASS records={len(fna)} {summary}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
