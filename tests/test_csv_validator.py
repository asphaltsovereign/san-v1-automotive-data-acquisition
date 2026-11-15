"""Characterize recovered host validation; these fixtures are never hardware evidence."""
import contextlib
import csv
import importlib.util
import io
from pathlib import Path
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location(
    'san_validator', ROOT / 'firmware/platformio/tools/validate_csv.py'
)
validator = importlib.util.module_from_spec(spec)
spec.loader.exec_module(validator)


class StructuralCsvChecks(unittest.TestCase):
    def setUp(self):
        with (ROOT / 'data/samples/synthetic_bench.csv').open(newline='') as f:
            self.rows = list(csv.DictReader(f))

    def check(self, rows=None, fields=None):
        with tempfile.TemporaryDirectory() as d:
            path = Path(d) / 'synthetic.csv'
            with path.open('w', newline='') as f:
                writer = csv.DictWriter(f, fieldnames=fields or validator.EXPECTED_COLUMNS)
                writer.writeheader()
                writer.writerows(self.rows if rows is None else rows)
            output = io.StringIO()
            with contextlib.redirect_stdout(output):
                code = validator.validate(path, 10.0)
            return code, output.getvalue()

    def test_synthetic_reference_is_ten_hz(self):
        code, out = self.check()
        self.assertEqual(code, 0)
        self.assertIn('observed_hz: 10.000', out)

    def test_sequence_gap_rejected(self):
        self.rows[4]['sample_seq'] = '99'
        code, out = self.check()
        self.assertEqual(code, 1)
        self.assertIn('sequence jumped', out)

    def test_reversed_uptime_rejected(self):
        self.rows[4]['uptime_ms'] = '100'
        code, out = self.check()
        self.assertEqual(code, 1)
        self.assertIn('uptime is not strictly increasing', out)

    def test_mixed_session_rejected(self):
        self.rows[4]['session_id'] = 'SYNTHETIC_OTHER'
        code, out = self.check()
        self.assertEqual(code, 1)
        self.assertIn('expected one session_id', out)

    def test_mixed_device_rejected(self):
        self.rows[4]['device_id'] = 'DEMO_OTHER'
        code, out = self.check()
        self.assertEqual(code, 1)
        self.assertIn('expected one device_id', out)

    def test_wrong_schema_rejected(self):
        self.rows[4]['schema_version'] = '1.0'
        code, out = self.check()
        self.assertEqual(code, 1)
        self.assertIn('unsupported schema version', out)

    def test_wrong_sampling_rate_rejected(self):
        for i, row in enumerate(self.rows):
            row['uptime_ms'] = str(i * 200)
        code, out = self.check()
        self.assertEqual(code, 1)
        self.assertIn('sample rate 5.000 Hz', out)

    def test_empty_file_rejected(self):
        code, out = self.check(rows=[])
        self.assertEqual(code, 1)
        self.assertIn('no data rows', out)


if __name__ == '__main__':
    unittest.main()
