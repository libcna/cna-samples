#!/usr/bin/env python3
"""Compare the unchanged XNA 4 and CNA Racing model reflection reports."""

from __future__ import annotations

import argparse
import math
import shlex
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path


IGNORED_RECORDS = {"FORMAT", "FRAMEWORK", "RESULT"}
FLOAT_LIST_FIELDS = {("BONE", "m"), ("MESH", "bounds")}
NULLABLE_STRING_FIELDS = {
    ("MODEL", "rootName"),
    ("BONE", "name"),
    ("MESH", "name"),
    ("PART", "technique"),
}


@dataclass(frozen=True)
class Record:
    kind: str
    positional: tuple[str, ...]
    fields: tuple[tuple[str, str], ...]
    line_number: int


def parse_report(path: Path) -> list[Record]:
    records: list[Record] = []
    text = path.read_text(encoding="utf-8-sig")
    for line_number, raw_line in enumerate(text.splitlines(), 1):
        line = raw_line.strip()
        if not line:
            continue
        tokens = shlex.split(line, posix=True)
        kind = tokens[0]
        if kind in IGNORED_RECORDS:
            continue
        positional: list[str] = []
        fields: list[tuple[str, str]] = []
        for token in tokens[1:]:
            if "=" not in token:
                positional.append(token)
                continue
            key, value = token.split("=", 1)
            if (kind, key) in NULLABLE_STRING_FIELDS and value == "<null>":
                value = ""
            fields.append((key, value))
        records.append(Record(kind, tuple(positional), tuple(fields), line_number))
    return records


def compare_float_lists(expected: str, actual: str) -> bool:
    expected_values = [float(value) for value in expected.split(",")]
    actual_values = [float(value) for value in actual.split(",")]
    return len(expected_values) == len(actual_values) and all(
        math.isclose(left, right, rel_tol=2e-6, abs_tol=2e-7)
        for left, right in zip(expected_values, actual_values)
    )


def compare_records(expected: Record, actual: Record) -> list[str]:
    errors: list[str] = []
    if expected.kind != actual.kind:
        return [f"kind differs: XNA={expected.kind} CNA={actual.kind}"]
    if expected.positional != actual.positional:
        errors.append(
            f"positional fields differ: XNA={expected.positional!r} "
            f"CNA={actual.positional!r}"
        )
    if tuple(key for key, _ in expected.fields) != tuple(
        key for key, _ in actual.fields
    ):
        errors.append(
            f"field names/order differ: XNA={expected.fields!r} "
            f"CNA={actual.fields!r}"
        )
        return errors
    for (key, expected_value), (_, actual_value) in zip(
        expected.fields, actual.fields
    ):
        if (expected.kind, key) in FLOAT_LIST_FIELDS:
            if not compare_float_lists(expected_value, actual_value):
                errors.append(
                    f"{key} differs beyond tolerance: "
                    f"XNA={expected_value} CNA={actual_value}"
                )
        elif expected_value != actual_value:
            errors.append(
                f"{key} differs: XNA={expected_value!r} CNA={actual_value!r}"
            )
    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("xna_report", type=Path)
    parser.add_argument("cna_report", type=Path)
    args = parser.parse_args()

    expected = parse_report(args.xna_report)
    actual = parse_report(args.cna_report)
    errors: list[str] = []
    if len(expected) != len(actual):
        errors.append(f"record count differs: XNA={len(expected)} CNA={len(actual)}")

    for index, (left, right) in enumerate(zip(expected, actual)):
        for detail in compare_records(left, right):
            errors.append(
                f"record {index}: XNA line {left.line_number}, "
                f"CNA line {right.line_number}: {detail}"
            )
            if len(errors) >= 50:
                break
        if len(errors) >= 50:
            break

    expected_counts = Counter(record.kind for record in expected)
    actual_counts = Counter(record.kind for record in actual)
    if expected_counts != actual_counts:
        errors.append(f"record kinds differ: XNA={expected_counts} CNA={actual_counts}")

    if errors:
        print("RACING_MODEL_REPORT_COMPARISON=FAIL", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    counts = " ".join(
        f"{kind}={expected_counts[kind]}"
        for kind in (
            "MODEL",
            "BONE",
            "MESH",
            "PART",
            "ELEMENT",
            "PARAM",
            "TEXTURE2D",
            "TEXTURECUBE",
            "TEXTURE",
        )
        if expected_counts[kind]
    )
    print(
        f"RACING_MODEL_REPORT_COMPARISON=PASS "
        f"records={len(expected)} {counts}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
