#!/usr/bin/env python3
"""Regression tests for renderer-matrix result classification."""

from __future__ import annotations

from pathlib import Path
import signal
import sys
import unittest


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from run_renderer_matrix import classify_run, sample_executable_path  # noqa: E402


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

    def test_clean_natural_exit_is_a_pass(self) -> None:
        self.assertEqual(
            classify_run("SDL_GPU", "SDL_GPU", False, False, "exited", 0),
            "AUTOMATED_PASS",
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


if __name__ == "__main__":
    unittest.main()
