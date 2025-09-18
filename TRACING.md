# Tracing Interface Documentation

This document describes the geth-like tracing interface implemented in Olympus.

## Overview

The tracing system allows you to capture detailed execution information during transaction processing, similar to Ethereum's `debug_traceTransaction` functionality.

## Available Tracers

### 1. OpCode Tracer (Default)
Captures detailed opcode-level execution traces including stack, memory, and storage.

**Usage:**
```bash
curl -X POST -H "Content-Type: application/json" --data '{
  "method": "debug_traceTransaction",
  "params": ["0x...", {"tracer": "opCodeTracer"}],
  "id": 1
}' http://localhost:8765
```

**Configuration options:**
```json
{
  "tracer": "opCodeTracer",
  "tracerConfig": {
    "enableMemory": true,
    "disableStack": false,
    "disableStorage": false,
    "limit": 1000
  }
}
```

### 2. Call Tracer
Captures call frames and internal transactions.

**Usage:**
```bash
curl -X POST -H "Content-Type: application/json" --data '{
  "method": "debug_traceTransaction", 
  "params": ["0x...", {"tracer": "callTracer"}],
  "id": 1
}' http://localhost:8765
```

**Configuration options:**
```json
{
  "tracer": "callTracer",
  "tracerConfig": {
    "onlyTopCall": false,
    "withLog": true
  }
}
```

### 3. 4byte Tracer
Tracks function selectors (first 4 bytes) of calls.

**Usage:**
```bash
curl -X POST -H "Content-Type: application/json" --data '{
  "method": "debug_traceTransaction",
  "params": ["0x...", {"tracer": "4byteTracer"}],
  "id": 1
}' http://localhost:8765
```

### 4. PreState Tracer
Captures account state before transaction execution.

**Usage:**
```bash
curl -X POST -H "Content-Type: application/json" --data '{
  "method": "debug_traceTransaction",
  "params": ["0x...", {"tracer": "prestateTracer"}], 
  "id": 1
}' http://localhost:8765
```

### 5. NoOp Tracer
Minimal tracer for performance testing.

**Usage:**
```bash
curl -X POST -H "Content-Type: application/json" --data '{
  "method": "debug_traceTransaction",
  "params": ["0x...", {"tracer": "noopTracer"}],
  "id": 1
}' http://localhost:8765
```

## Default Behavior

When no tracer is specified, the system defaults to the OpCode tracer:

```bash
curl -X POST -H "Content-Type: application/json" --data '{
  "method": "debug_traceTransaction",
  "params": ["0x..."],
  "id": 1
}' http://localhost:8765
```

## Output Format

### OpCode Tracer Output
```json
{
  "gas": 21000,
  "failed": false,
  "returnValue": "0x",
  "structLogs": [
    {
      "pc": 0,
      "op": "PUSH1",
      "gas": 79000,
      "gasCost": 3,
      "depth": 1,
      "stack": ["0x60"],
      "memory": [],
      "storage": {}
    }
  ]
}
```

### Call Tracer Output
```json
{
  "from": "0x...",
  "gas": "0x13880",
  "gasUsed": "0x5208", 
  "to": "0x...",
  "input": "0x",
  "output": "0x",
  "type": "CALL",
  "value": "0x0"
}
```

## Architecture

The tracing system consists of:

1. **Base Tracer Class** (`mcp/node/tracers/Tracer.hpp`) - Defines the EVMLogger interface
2. **Tracer Implementations** - Specific tracer types in `mcp/node/tracers/`
3. **Tracer Factory** (`NewTracer` function) - Creates tracer instances based on parameters
4. **VM Integration** (`Executive.cpp`) - Hooks tracers into VM execution
5. **RPC Interface** (`handler.cpp`) - Exposes tracing via debug_traceTransaction

## Adding New Tracers

To add a new tracer:

1. Create a new tracer class extending `Tracer`
2. Implement required methods (`CaptureState`, `GetResult`, etc.)
3. Add the tracer to the `NewTracer` factory function
4. Include the tracer header in `Tracer.cpp`

Example:
```cpp
class MyTracer : public Tracer {
public:
    void CaptureState(uint64_t PC, dev::eth::Instruction inst, 
                     uint64_t gasCost, uint64_t gas, 
                     dev::eth::VMFace const* vm, 
                     dev::eth::ExtVMFace const* ext) override {
        // Implementation
    }
    
    mcp::json GetResult() override {
        // Return results
    }
};
```