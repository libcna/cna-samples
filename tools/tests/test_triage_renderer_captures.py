import importlib.util
from pathlib import Path
import tempfile
import unittest

from PIL import Image


MODULE_PATH = Path(__file__).resolve().parents[1] / "triage_renderer_captures.py"
SPEC = importlib.util.spec_from_file_location("triage_renderer_captures", MODULE_PATH)
TRIAGE = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
SPEC.loader.exec_module(TRIAGE)


class CaptureTriageTests(unittest.TestCase):
    def test_black_frame_is_flagged_without_becoming_a_pass(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "black.png"
            Image.new("RGB", (4, 4), "black").save(path)
            metrics = TRIAGE.image_metrics(path)
            self.assertEqual(
                TRIAGE.triage_flags(metrics, None),
                ["LOW_VARIANCE", "NEAR_BLACK"],
            )

    def test_normalized_mae_is_exact_for_known_pixels(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            black = root / "black.png"
            white = root / "white.png"
            Image.new("RGB", (2, 2), "black").save(black)
            Image.new("RGB", (2, 2), "white").save(white)
            self.assertEqual(TRIAGE.normalized_mae(black, white), 1.0)

    def test_report_records_missing_renderer_capture(self):
        with tempfile.TemporaryDirectory() as directory:
            capture_dir = Path(directory)
            (capture_dir / "OPENGL33").mkdir()
            (capture_dir / "VULKAN").mkdir()
            Image.new("RGB", (4, 4), "red").save(
                capture_dir / "OPENGL33" / "Sample.png"
            )
            report = TRIAGE.build_report(
                capture_dir,
                ("OPENGL33", "VULKAN"),
                "OPENGL33",
            )
            missing = report["rows"][1]
            self.assertEqual(missing["capture"], "MISSING")
            self.assertEqual(missing["flags"], ["MISSING"])


if __name__ == "__main__":
    unittest.main()
