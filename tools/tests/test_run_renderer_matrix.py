#!/usr/bin/env python3
"""Regression tests for renderer-matrix result classification."""

from __future__ import annotations

from pathlib import Path
import os
import signal
import sys
import tempfile
import unittest
from types import SimpleNamespace
from unittest.mock import patch


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from run_renderer_matrix import (  # noqa: E402
    capture_window,
    classify_run,
    parse_xdotool_geometry,
    sample_executable_path,
)


class ClassifyRunTests(unittest.TestCase):
    def test_sigsegv_after_observation_is_teardown_failure(self) -> None:
        self.assertEqual(
            classify_run("WEBGPU", "WEBGPU", True, False, "sigterm", -signal.SIGSEGV),
            "TEARDOWN_FAIL",
        )

    def test_requested_sigterm_after_observation_is_a_pass(self) -> None:
        self.assertEqual(
            classify_run("VULKAN", "VULKAN", True, False, "sigterm", -signal.SIGTERM),
            "AUTOMATED_PASS",
        )

    def test_clean_escape_after_observation_is_a_pass(self) -> None:
        self.assertEqual(
            classify_run("OPENGLES3", "OPENGLES3", True, False, "escape", 0),
            "AUTOMATED_PASS",
        )

    def test_sigkill_is_not_a_pass(self) -> None:
        self.assertEqual(
            classify_run("FNA3D", "FNA3D", True, False, "sigkill", -signal.SIGKILL),
            "TEARDOWN_FAIL",
        )

    def test_clean_exit_before_observation_is_early_exit(self) -> None:
        self.assertEqual(
            classify_run("SDL_GPU", "SDL_GPU", False, False, "exited", 0),
            "EARLY_EXIT",
        )

    def test_clean_natural_exit_after_observation_is_a_pass(self) -> None:
        self.assertEqual(
            classify_run("SDL_GPU", "SDL_GPU", True, False, "exited", 0),
            "AUTOMATED_PASS",
        )

    def test_timeout_after_renderer_selection_is_not_a_render_failure(self) -> None:
        self.assertEqual(
            classify_run("WEBGPU", "WEBGPU", False, True, "sigterm", -signal.SIGTERM),
            "TIMEOUT",
        )

    def test_crash_before_observation_is_a_render_failure(self) -> None:
        self.assertEqual(
            classify_run("WEBGPU", "WEBGPU", False, False, "exited", -signal.SIGSEGV),
            "RENDER_FAIL",
        )

    def test_wrong_renderer_takes_precedence(self) -> None:
        self.assertEqual(
            classify_run("VULKAN", "OPENGLES3", True, False, "escape", 0),
            "WRONG_RENDERER",
        )

    def test_missing_renderer_uses_timeout_or_init_failure(self) -> None:
        self.assertEqual(
            classify_run("WEBGPU", None, False, True, "sigterm", -signal.SIGTERM),
            "TIMEOUT",
        )
        self.assertEqual(
            classify_run("WEBGPU", None, False, False, "exited", 1),
            "INIT_FAIL",
        )

    def test_requested_windows_termination_after_observation_is_a_pass(self) -> None:
        self.assertEqual(
            classify_run("DIRECTX11", "DIRECTX11", True, False, "terminate", 1),
            "AUTOMATED_PASS",
        )


class SampleExecutablePathTests(unittest.TestCase):
    def test_windows_adds_executable_suffix(self) -> None:
        self.assertEqual(
            sample_executable_path(
                Path("build"), "ShadowMapping", "ShadowMapping_cna_samples", "win32"
            ),
            Path("build/samples/ShadowMapping/ShadowMapping_cna_samples.exe"),
        )

    def test_windows_keeps_explicit_executable_suffix(self) -> None:
        self.assertEqual(
            sample_executable_path(
                Path("build"), "ShadowMapping", "ShadowMapping_cna_samples.exe", "win32"
            ),
            Path("build/samples/ShadowMapping/ShadowMapping_cna_samples.exe"),
        )

    def test_posix_keeps_target_name(self) -> None:
        self.assertEqual(
            sample_executable_path(
                Path("build"), "ShadowMapping", "ShadowMapping_cna_samples", "darwin"
            ),
            Path("build/samples/ShadowMapping/ShadowMapping_cna_samples"),
        )


class XdotoolGeometryTests(unittest.TestCase):
    def test_parses_root_relative_window_geometry(self) -> None:
        self.assertEqual(
            parse_xdotool_geometry(
                "WINDOW=4194306\nX=560\nY=300\nWIDTH=800\nHEIGHT=480\nSCREEN=0\n"
            ),
            (560, 300, 800, 480),
        )

    def test_accepts_partly_offscreen_coordinates(self) -> None:
        self.assertEqual(
            parse_xdotool_geometry("X=-20\nY=-5\nWIDTH=800\nHEIGHT=480\n"),
            (-20, -5, 800, 480),
        )

    def test_rejects_missing_invalid_or_empty_geometry(self) -> None:
        self.assertIsNone(parse_xdotool_geometry("X=1\nY=2\nWIDTH=800\n"))
        self.assertIsNone(parse_xdotool_geometry("X=left\nY=2\nWIDTH=800\nHEIGHT=480\n"))
        self.assertIsNone(parse_xdotool_geometry("X=1\nY=2\nWIDTH=0\nHEIGHT=480\n"))

    def test_capture_reads_composited_root_and_crops_to_window(self) -> None:
        commands: list[list[str]] = []
        with tempfile.TemporaryDirectory() as temporary_directory:
            destination = Path(temporary_directory) / "capture.png"

            def fake_run(command: list[str], **_kwargs: object) -> SimpleNamespace:
                commands.append(command)
                if command[:3] == ["xdotool", "search", "--onlyvisible"]:
                    return SimpleNamespace(returncode=0, stdout="4194306\n")
                if command[:3] == ["xdotool", "getwindowgeometry", "--shell"]:
                    return SimpleNamespace(
                        returncode=0,
                        stdout="X=560\nY=300\nWIDTH=800\nHEIGHT=480\n",
                    )
                if command[0] == "import":
                    destination.write_bytes(b"captured")
                    return SimpleNamespace(returncode=0, stdout="")
                raise AssertionError(f"unexpected command: {command}")

            with (
                patch.dict(os.environ, {"DISPLAY": ":23"}),
                patch("run_renderer_matrix.shutil.which", return_value="/usr/bin/tool"),
                patch("run_renderer_matrix.subprocess.run", side_effect=fake_run),
            ):
                self.assertEqual(capture_window(1234, destination), "CAPTURED")

        self.assertEqual(
            commands[-1],
            [
                "import",
                "-display",
                ":23",
                "-window",
                "root",
                "-crop",
                "800x480+560+300",
                "+repage",
                str(destination),
            ],
        )


if __name__ == "__main__":
    unittest.main()
