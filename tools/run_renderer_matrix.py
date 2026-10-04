#!/usr/bin/env python3
"""Run native CNA sample executables across an explicit renderer matrix."""

from __future__ import annotations

import argparse
import csv
import datetime as dt
import json
import os
from pathlib import Path
import re
import shutil
import signal
import subprocess
import sys
import time


PRIMARY_CATEGORIES = {"gallery-runnable", "native-runnable"}
ACTIVE_RENDERER_RE = re.compile(r"CNA: graphics renderer: ([A-Z0-9_]+)")


def parse_csv_set(value: str) -> set[str]:
    return {item.strip() for item in value.split(",") if item.strip()}


def parse_args() -> argparse.Namespace:
    repo_root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(
        description="Run one native executable per sample with each requested CNA renderer."
    )
    parser.add_argument("--build-dir", required=True, type=Path)
    parser.add_argument("--renderers", required=True)
    parser.add_argument(
        "--samples",
        default="",
        help="Comma-separated source directories or target names (default: entire selected corpus)",
    )
    parser.add_argument(
        "--categories",
        default=",".join(sorted(PRIMARY_CATEGORIES)),
        help="Comma-separated manifest categories, or 'all'",
    )
    parser.add_argument(
        "--manifest",
        type=Path,
        default=repo_root / "tools" / "renderer-matrix-corpus.tsv",
    )
    parser.add_argument("--cna-root", type=Path, default=repo_root.parent / "cna")
    parser.add_argument("--timeout", type=float, default=20.0)
    parser.add_argument("--observe-seconds", type=float, default=5.0)
    parser.add_argument("--exit-grace-seconds", type=float, default=3.0)
    parser.add_argument("--output-dir", type=Path)
    parser.add_argument(
        "--no-private-display",
        action="store_true",
        help="Do not enter CNA's private GPU display runner (intended for Windows/macOS)",
    )
    parser.add_argument("--list", action="store_true", help="List selected manifest entries and exit")
    return parser.parse_args()


def enter_private_display(args: argparse.Namespace) -> int | None:
    if (
        args.no_private_display
        or sys.platform != "linux"
        or os.environ.get("CNA_RENDERER_MATRIX_PRIVATE") == "1"
    ):
        return None

    private_runner = args.cna_root.resolve() / "tools" / "platform" / "run_gpu_tests_private.sh"
    if not private_runner.is_file():
        print(f"ENVIRONMENT BLOCKED: private GPU runner not found: {private_runner}", file=sys.stderr)
        return 77

    command = [
        str(private_runner),
        "--exec",
        "env",
        "CNA_RENDERER_MATRIX_PRIVATE=1",
        sys.executable,
        str(Path(__file__).resolve()),
        *sys.argv[1:],
    ]
    return subprocess.run(command, check=False).returncode


def load_manifest(path: Path, categories: set[str] | None, sample_filter: set[str]) -> list[dict[str, str]]:
    entries: list[dict[str, str]] = []
    with path.open(encoding="utf-8", newline="") as handle:
        rows = (line for line in handle if line.strip() and not line.startswith("#"))
        reader = csv.DictReader(
            rows,
            delimiter="\t",
            fieldnames=("category", "source_directory", "target", "note"),
        )
        for row in reader:
            if categories is not None and row["category"] not in categories:
                continue
            if sample_filter and not ({row["source_directory"], row["target"]} & sample_filter):
                continue
            entries.append(row)
    return entries


def read_logs(stdout_path: Path, stderr_path: Path) -> str:
    chunks: list[str] = []
    for path in (stdout_path, stderr_path):
        try:
            chunks.append(path.read_text(encoding="utf-8", errors="replace"))
        except FileNotFoundError:
            pass
    return "\n".join(chunks)


def send_escape(pid: int) -> bool:
    if shutil.which("xdotool") is None or not os.environ.get("DISPLAY"):
        return False
    try:
        search = subprocess.run(
            ["xdotool", "search", "--onlyvisible", "--pid", str(pid)],
            check=False,
            capture_output=True,
            text=True,
            timeout=1.0,
        )
        window_ids = [line.strip() for line in search.stdout.splitlines() if line.strip()]
        if not window_ids:
            return False
        subprocess.run(
            ["xdotool", "key", "--window", window_ids[-1], "Escape"],
            check=False,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            timeout=1.0,
        )
        return True
    except (OSError, subprocess.TimeoutExpired):
        return False


def stop_process(process: subprocess.Popen[bytes], grace_seconds: float) -> str:
    if process.poll() is not None:
        return "exited"
    if send_escape(process.pid):
        try:
            process.wait(timeout=grace_seconds)
            return "escape"
        except subprocess.TimeoutExpired:
            pass
    try:
        os.killpg(process.pid, signal.SIGTERM)
    except ProcessLookupError:
        return "exited"
    try:
        process.wait(timeout=grace_seconds)
        return "sigterm"
    except subprocess.TimeoutExpired:
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        process.wait()
        return "sigkill"


def run_one(
    entry: dict[str, str],
    renderer: str,
    build_dir: Path,
    logs_dir: Path,
    timeout: float,
    observe_seconds: float,
    exit_grace_seconds: float,
) -> dict[str, object]:
    source_directory = entry["source_directory"]
    target = entry["target"]
    executable = build_dir / "samples" / source_directory / target
    result: dict[str, object] = {
        "sample": source_directory,
        "target": target,
        "category": entry["category"],
        "renderer_requested": renderer,
        "renderer_active": None,
        "result": "BUILD_MISSING",
        "visual": "MANUAL_REQUIRED",
        "exit_status": None,
        "stop_method": "not-started",
        "duration_seconds": 0.0,
    }
    if target == "-" or not executable.is_file() or not os.access(executable, os.X_OK):
        result["executable"] = str(executable)
        return result

    sample_log_dir = logs_dir / renderer / source_directory
    sample_log_dir.mkdir(parents=True, exist_ok=True)
    stdout_path = sample_log_dir / "stdout.log"
    stderr_path = sample_log_dir / "stderr.log"
    result["executable"] = str(executable)
    result["stdout"] = str(stdout_path)
    result["stderr"] = str(stderr_path)

    environment = os.environ.copy()
    environment["CNA_GRAPHICS_RENDERER"] = renderer
    started = time.monotonic()
    selected_at: float | None = None
    timed_out = False
    survived_observation = False

    with stdout_path.open("wb") as stdout_handle, stderr_path.open("wb") as stderr_handle:
        process = subprocess.Popen(
            [str(executable)],
            cwd=executable.parent,
            env=environment,
            stdout=stdout_handle,
            stderr=stderr_handle,
            start_new_session=True,
        )
        while True:
            now = time.monotonic()
            output = read_logs(stdout_path, stderr_path)
            matches = ACTIVE_RENDERER_RE.findall(output)
            if matches:
                result["renderer_active"] = matches[-1]
                if selected_at is None:
                    selected_at = now
            return_code = process.poll()
            if return_code is not None:
                break
            if selected_at is not None and now - selected_at >= observe_seconds:
                survived_observation = True
                break
            if now - started >= timeout:
                timed_out = True
                break
            time.sleep(0.1)

        result["stop_method"] = stop_process(process, exit_grace_seconds)
        result["exit_status"] = process.returncode

    result["duration_seconds"] = round(time.monotonic() - started, 3)
    active = result["renderer_active"]
    if active is not None and active != renderer:
        result["result"] = "WRONG_RENDERER"
    elif active == renderer and (survived_observation or process.returncode == 0):
        result["result"] = "AUTOMATED_PASS"
    elif active == renderer:
        result["result"] = "RENDER_FAIL"
    elif timed_out:
        result["result"] = "TIMEOUT"
    else:
        result["result"] = "INIT_FAIL"
    return result


def write_summaries(
    output_dir: Path,
    build_dir: Path,
    renderers: list[str],
    categories: set[str] | None,
    results: list[dict[str, object]],
) -> None:
    totals: dict[str, dict[str, int]] = {}
    for result in results:
        renderer = str(result["renderer_requested"])
        state = str(result["result"])
        totals.setdefault(renderer, {})[state] = totals.setdefault(renderer, {}).get(state, 0) + 1

    document = {
        "schema_version": 1,
        "generated_at_utc": dt.datetime.now(dt.timezone.utc).isoformat(),
        "build_dir": str(build_dir),
        "renderers": renderers,
        "categories": sorted(categories) if categories is not None else ["all"],
        "totals": totals,
        "results": results,
    }
    (output_dir / "summary.json").write_text(
        json.dumps(document, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )

    lines = [
        "# CNA sample renderer matrix",
        "",
        f"Build: `{build_dir}`",
        "",
        "An `AUTOMATED_PASS` proves startup, the logged active renderer, and a stable observation "
        "interval. It does not constitute a manual visual pass.",
        "",
        "| Sample | Requested | Active | Automated result | Exit | Stop | Visual |",
        "|---|---|---|---|---:|---|---|",
    ]
    for result in results:
        lines.append(
            "| {sample} | {renderer_requested} | {renderer_active} | {result} | {exit_status} | "
            "{stop_method} | {visual} |".format(**result)
        )
    lines.extend(["", "## Totals", ""])
    for renderer in renderers:
        counts = ", ".join(f"{name}: {count}" for name, count in sorted(totals.get(renderer, {}).items()))
        lines.append(f"- {renderer}: {counts or 'no results'}")
    (output_dir / "summary.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    args = parse_args()
    build_dir = args.build_dir.resolve()
    manifest = args.manifest.resolve()
    renderers = sorted(parse_csv_set(args.renderers))
    sample_filter = parse_csv_set(args.samples)
    categories = None if args.categories == "all" else parse_csv_set(args.categories)
    if not renderers:
        print("No renderers selected", file=sys.stderr)
        return 2
    if args.timeout <= 0 or args.observe_seconds <= 0 or args.observe_seconds > args.timeout:
        print("Timeouts must be positive and observe-seconds must not exceed timeout", file=sys.stderr)
        return 2
    if not manifest.is_file():
        print(f"Manifest not found: {manifest}", file=sys.stderr)
        return 2

    entries = load_manifest(manifest, categories, sample_filter)
    if not entries:
        print("No manifest entries matched the requested filters", file=sys.stderr)
        return 2
    if args.list:
        for entry in entries:
            print(f"{entry['category']}\t{entry['source_directory']}\t{entry['target']}")
        return 0

    private_status = enter_private_display(args)
    if private_status is not None:
        return private_status

    timestamp = dt.datetime.now().strftime("%Y%m%d-%H%M%S")
    output_dir = (args.output_dir or build_dir / "renderer-matrix-results" / timestamp).resolve()
    logs_dir = output_dir / "logs"
    logs_dir.mkdir(parents=True, exist_ok=True)

    results: list[dict[str, object]] = []
    for renderer in renderers:
        for entry in entries:
            print(f"[{renderer}] {entry['source_directory']}", flush=True)
            result = run_one(
                entry,
                renderer,
                build_dir,
                logs_dir,
                args.timeout,
                args.observe_seconds,
                args.exit_grace_seconds,
            )
            results.append(result)
            print(
                f"  {result['result']} (active={result['renderer_active']}, "
                f"exit={result['exit_status']}, {result['duration_seconds']}s)",
                flush=True,
            )

    write_summaries(output_dir, build_dir, renderers, categories, results)
    print(f"Machine summary: {output_dir / 'summary.json'}")
    print(f"Human summary:   {output_dir / 'summary.md'}")
    return 0 if all(result["result"] == "AUTOMATED_PASS" for result in results) else 1


if __name__ == "__main__":
    raise SystemExit(main())
