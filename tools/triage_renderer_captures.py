#!/usr/bin/env python3
"""Create review aids for a renderer-matrix capture directory.

The metrics in this tool are triage signals only. They can identify missing, solid,
near-black or grossly different frames, but they never turn a frame into a visual pass.
"""

from __future__ import annotations

import argparse
import json
import math
from pathlib import Path
from typing import Any

from PIL import Image, ImageChops, ImageDraw, ImageFont, ImageOps, ImageStat


DEFAULT_RENDERERS = ("OPENGLES3", "OPENGL33", "VULKAN", "WEBGPU", "SDL_GPU", "FNA3D")


def parse_renderers(value: str) -> tuple[str, ...]:
    return tuple(item.strip() for item in value.split(",") if item.strip())


def image_metrics(path: Path) -> dict[str, Any]:
    with Image.open(path) as source:
        rgb = source.convert("RGB")
        gray = rgb.convert("L")
        histogram = gray.histogram()
        pixels = rgb.width * rgb.height
        stats = ImageStat.Stat(gray)
        return {
            "path": str(path),
            "width": rgb.width,
            "height": rgb.height,
            "luma_mean": stats.mean[0],
            "luma_stddev": stats.stddev[0],
            "near_black_fraction": sum(histogram[:8]) / pixels,
            "near_white_fraction": sum(histogram[248:]) / pixels,
        }


def normalized_mae(left_path: Path, right_path: Path) -> float | None:
    with Image.open(left_path) as left_source, Image.open(right_path) as right_source:
        left = left_source.convert("RGB")
        right = right_source.convert("RGB")
        if left.size != right.size:
            return None
        histogram = ImageChops.difference(left, right).histogram()
        absolute_sum = sum((index % 256) * count for index, count in enumerate(histogram))
        return absolute_sum / (left.width * left.height * 3 * 255)


def triage_flags(metrics: dict[str, Any], reference: dict[str, Any] | None) -> list[str]:
    flags: list[str] = []
    if metrics["luma_stddev"] < 2.0:
        flags.append("LOW_VARIANCE")
    if metrics["luma_mean"] < 3.0 and metrics["near_black_fraction"] > 0.95:
        flags.append("NEAR_BLACK")
    if reference is not None:
        if (metrics["width"], metrics["height"]) != (
            reference["width"],
            reference["height"],
        ):
            flags.append("SIZE_MISMATCH")
        reference_mean = reference["luma_mean"]
        if reference_mean > 10.0:
            ratio = metrics["luma_mean"] / reference_mean
            if ratio < 0.2 or ratio > 5.0:
                flags.append("LUMA_OUTLIER")
    return flags


def load_font(size: int) -> ImageFont.ImageFont:
    try:
        return ImageFont.truetype("DejaVuSans.ttf", size)
    except OSError:
        return ImageFont.load_default()


def make_contact_sheets(
    capture_dir: Path,
    output_dir: Path,
    renderers: tuple[str, ...],
    samples: list[str],
    flagged: set[tuple[str, str]],
    rows_per_page: int,
) -> list[Path]:
    tile_width = 240
    image_height = 144
    label_height = 38
    tile_height = image_height + label_height
    font = load_font(13)
    output_paths: list[Path] = []

    for page_index, start in enumerate(range(0, len(samples), rows_per_page), start=1):
        page_samples = samples[start : start + rows_per_page]
        sheet = Image.new(
            "RGB",
            (tile_width * len(renderers), tile_height * len(page_samples)),
            "#202020",
        )
        draw = ImageDraw.Draw(sheet)
        for row, sample in enumerate(page_samples):
            for column, renderer in enumerate(renderers):
                x = column * tile_width
                y = row * tile_height
                capture = capture_dir / renderer / f"{sample}.png"
                if capture.is_file():
                    with Image.open(capture) as source:
                        preview = ImageOps.pad(
                            source.convert("RGB"),
                            (tile_width, image_height),
                            color="#101010",
                        )
                    sheet.paste(preview, (x, y))
                else:
                    draw.rectangle((x, y, x + tile_width, y + image_height), fill="#3a3a3a")
                    draw.text((x + 8, y + 60), "NO CAPTURE", fill="#ffffff", font=font)
                border = "#ff4040" if (sample, renderer) in flagged else "#606060"
                draw.rectangle(
                    (x, y, x + tile_width - 1, y + tile_height - 1),
                    outline=border,
                    width=3 if (sample, renderer) in flagged else 1,
                )
                draw.rectangle(
                    (x, y + image_height, x + tile_width, y + tile_height),
                    fill="#101010",
                )
                draw.text((x + 5, y + image_height + 3), sample, fill="#ffffff", font=font)
                draw.text((x + 5, y + image_height + 20), renderer, fill="#b0d8ff", font=font)

        output_path = output_dir / f"contact-sheet-{page_index:03d}.png"
        sheet.save(output_path)
        output_paths.append(output_path)

    return output_paths


def build_report(
    capture_dir: Path,
    renderers: tuple[str, ...],
    reference_renderer: str,
) -> dict[str, Any]:
    samples = sorted(
        {
            path.stem
            for renderer in renderers
            for path in (capture_dir / renderer).glob("*.png")
        }
    )
    rows: list[dict[str, Any]] = []
    for sample in samples:
        reference_path = capture_dir / reference_renderer / f"{sample}.png"
        reference_metrics = image_metrics(reference_path) if reference_path.is_file() else None
        for renderer in renderers:
            path = capture_dir / renderer / f"{sample}.png"
            if not path.is_file():
                rows.append(
                    {
                        "sample": sample,
                        "renderer": renderer,
                        "capture": "MISSING",
                        "flags": ["MISSING"],
                    }
                )
                continue
            metrics = image_metrics(path)
            metrics.update(
                {
                    "sample": sample,
                    "renderer": renderer,
                    "capture": "PRESENT",
                    "flags": triage_flags(metrics, reference_metrics),
                    "mae_vs_reference": (
                        normalized_mae(path, reference_path)
                        if reference_metrics is not None and renderer != reference_renderer
                        else 0.0 if renderer == reference_renderer else None
                    ),
                }
            )
            rows.append(metrics)

    return {
        "methodology": (
            "Review aid only: missing/near-black/low-variance/size/luminance signals and "
            "time-sensitive MAE never establish a visual pass."
        ),
        "capture_dir": str(capture_dir),
        "reference_renderer": reference_renderer,
        "renderers": list(renderers),
        "samples": samples,
        "rows": rows,
    }


def write_markdown(report: dict[str, Any], output_path: Path, sheets: list[Path]) -> None:
    rows = report["rows"]
    present = sum(row["capture"] == "PRESENT" for row in rows)
    flagged = [row for row in rows if row["flags"]]
    lines = [
        "# Renderer capture triage",
        "",
        report["methodology"],
        "",
        f"- Samples discovered: {len(report['samples'])}",
        f"- Capture slots: {len(rows)}",
        f"- Captures present: {present}",
        f"- Flagged slots: {len(flagged)}",
        f"- Reference renderer: `{report['reference_renderer']}`",
        f"- Contact sheets: {len(sheets)}",
        "",
        "| Sample | Renderer | Flags | Luma | Stddev | MAE vs reference |",
        "|---|---|---|---:|---:|---:|",
    ]
    for row in flagged:
        lines.append(
            "| {sample} | {renderer} | {flags} | {mean} | {stddev} | {mae} |".format(
                sample=row["sample"],
                renderer=row["renderer"],
                flags=", ".join(row["flags"]),
                mean=f"{row.get('luma_mean', math.nan):.3f}",
                stddev=f"{row.get('luma_stddev', math.nan):.3f}",
                mae=(
                    f"{row['mae_vs_reference']:.6f}"
                    if row.get("mae_vs_reference") is not None
                    else "—"
                ),
            )
        )
    output_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--capture-dir", required=True, type=Path)
    parser.add_argument("--output-dir", type=Path)
    parser.add_argument("--renderers", default=",".join(DEFAULT_RENDERERS))
    parser.add_argument("--reference-renderer", default="OPENGL33")
    parser.add_argument("--rows-per-page", type=int, default=8)
    args = parser.parse_args()

    capture_dir = args.capture_dir.resolve()
    output_dir = (args.output_dir or capture_dir.parent / "visual-triage").resolve()
    renderers = parse_renderers(args.renderers)
    if args.reference_renderer not in renderers:
        parser.error("--reference-renderer must be present in --renderers")
    if args.rows_per_page < 1:
        parser.error("--rows-per-page must be positive")
    output_dir.mkdir(parents=True, exist_ok=True)

    report = build_report(capture_dir, renderers, args.reference_renderer)
    flagged = {
        (row["sample"], row["renderer"])
        for row in report["rows"]
        if row["flags"]
    }
    sheets = make_contact_sheets(
        capture_dir,
        output_dir,
        renderers,
        report["samples"],
        flagged,
        args.rows_per_page,
    )
    (output_dir / "triage.json").write_text(
        json.dumps(report, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    write_markdown(report, output_dir / "triage.md", sheets)
    print(f"Machine triage: {output_dir / 'triage.json'}")
    print(f"Human triage:   {output_dir / 'triage.md'}")
    print(f"Contact sheets: {len(sheets)} in {output_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
