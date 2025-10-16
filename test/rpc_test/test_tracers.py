import json
import os
import pathlib
import unittest
from typing import Optional, Dict, Any

import requests


DEFAULT_RPC_URL = "http://127.0.0.1:8765"
CALLS_FILE = pathlib.Path(__file__).resolve().parents[2] / "calls.txt"


def _find_default_tx_hash() -> Optional[str]:
    env_candidates = [
        os.getenv("TRACE_TX_HASH"),
        os.getenv("TRACE_TRANSACTION_HASH"),
        os.getenv("TRACE_TX"),
    ]
    for candidate in env_candidates:
        if candidate and candidate.startswith("0x") and len(candidate) == 66:
            return candidate

    if CALLS_FILE.exists():
        with CALLS_FILE.open("r", encoding="utf-8") as handle:
            for line in handle:
                line = line.strip()
                if line.startswith("0x") and len(line) == 66:
                    return line
    return None


class TracerIntegrationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.rpc_url = os.getenv("TRACE_RPC_URL", DEFAULT_RPC_URL)
        cls.tx_hash = _find_default_tx_hash()
        if not cls.tx_hash:
            raise unittest.SkipTest("No transaction hash available for tracer smoke tests.")

    def _debug_trace(self, tracer: str, config: Optional[Dict[str, Any]] = None) -> Any:
        payload = {
            "jsonrpc": "2.0",
            "method": "debug_traceTransaction",
            "params": [self.tx_hash, {"tracer": tracer}],
            "id": tracer,
        }
        if config:
            payload["params"][1]["tracerConfig"] = config

        response = requests.post(self.rpc_url, json=payload, timeout=30)
        response.raise_for_status()
        message = response.json()

        if "error" in message:
            self.fail(f"debug_traceTransaction returned error for {tracer}: {message['error']}")

        self.assertIn("result", message, f"Malformed response for tracer {tracer}: {message}")
        return message["result"]

    def test_opcode_tracer_struct_logs(self) -> None:
        result = self._debug_trace("opCodeTracer")
        self.assertIsInstance(result, dict)
        self.assertIn("structLogs", result)
        self.assertIsInstance(result["structLogs"], list)

    def test_call_tracer_root_fields(self) -> None:
        result = self._debug_trace("callTracer")
        self.assertIsInstance(result, dict)
        self.assertIn("type", result)
        self.assertIn("from", result)
        self.assertIn("gas", result)

    def test_fourbyte_tracer_counts(self) -> None:
        result = self._debug_trace("4byteTracer")
        self.assertIsInstance(result, dict)
        self.assertIn("_totalRecords", result)
        self.assertGreaterEqual(result["_totalRecords"], 0)


if __name__ == "__main__":
    unittest.main()
