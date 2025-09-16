# Complete Debugging Flow Documentation

This document describes the complete debugging flow from API calls to VM opcodes in the Olympus blockchain implementation.

## Overview

The debugging system provides complete traceability from high-level RPC API calls down to individual EVM opcode execution, with transaction hash correlation at every level.

## Debugging Flow Architecture

```
Client RPC API → Chain State → Executive → VM → Opcodes
     ↓              ↓           ↓         ↓       ↓
   debug_*      executeTransaction  go()   exec()  onOperation()
 RPC methods    Client API logging      BOOST_LOG  Opcode logging
```

## 1. Client API Level (RPC Layer)

### Entry Points
- **debug_traceTransaction**: Main debugging RPC endpoint
- **Location**: `mcp/rpc/handler.cpp:debug_traceTransaction()`
- **Purpose**: Serves structured trace data for transaction analysis

### Features
- Transaction hash validation and lookup
- Flexible tracer options (disableStorage, disableMemory, disableStack)
- Structured JSON response with gas usage, execution status, and opcodes
- Complete error handling for missing transactions/blocks

### Usage Example
```bash
curl -X POST -H "Content-Type: application/json" \
  --data '{"jsonrpc":"2.0","method":"debug_traceTransaction","params":["0x123..."],"id":1}' \
  http://localhost:8765
```

## 2. Chain State Level (API Execution)

### Location
- **File**: `mcp/node/chain_state.cpp:executeTransaction()`
- **Purpose**: Client API-level tracing for complete execution path debugging

### Logging Format
```
Client API: Starting transaction execution TxHash=0x... From=0x... To=0x... Value=...
Client API: Transaction execution completed TxHash=0x... Result=SUCCESS GasUsed=21000
```

### Features
- Transaction initialization and parameter logging
- Execution result and gas consumption tracking
- Error state capture and logging

## 3. Executive Level (Transaction Execution)

### Location
- **File**: `mcp/node/evm/Executive.cpp:go()`
- **Purpose**: Set up opcode logging callbacks and manage VM execution

### Features
- Global opcode callback setup: `g_opcodeLogCallback`
- Enhanced context including block number and execution depth
- Proper callback lifecycle management (setup/cleanup)
- Integration with BOOST_LOG trace level

### Callback Setup
```cpp
g_opcodeLogCallback = OpcodeLogCallback([this](uint64_t pc, Instruction op, const std::string& opName, const VM* vm) {
    BOOST_LOG(m_log.trace) << "EVM Opcode: TxHash=" << m_t.sha3().hexPrefixed() 
                          << " BlockNum=" << m_envInfo.number()
                          << " PC=" << pc << " OP=" << opName 
                          << " (0x" << std::hex << static_cast<int>(op) << std::dec << ")"
                          << " Depth=" << (m_ext ? m_ext->depth : 0);
});
```

## 4. VM Level (EVM Execution)

### Location
- **Files**: `libinterpreter/VM.h`, `libinterpreter/VM.cpp`
- **Purpose**: Individual opcode execution and logging

### Key Components

#### OpcodeLogCallback Type
```cpp
using OpcodeLogCallback = std::function<void(uint64_t pc, Instruction op, const std::string& opName, const VM* vm)>;
```

#### Global Callback
```cpp
extern OpcodeLogCallback g_opcodeLogCallback;
```

#### onOperation() Method
Enhanced to call logging callbacks with complete context:
```cpp
void onOperation() {
    if (m_opcodeLogCallback) {
        std::string opName = getInstructionName(m_OP);
        m_opcodeLogCallback(m_PC, m_OP, opName, this);
    } else if (g_opcodeLogCallback) {
        std::string opName = getInstructionName(m_OP);
        g_opcodeLogCallback(m_PC, m_OP, opName, this);
    }
}
```

## 5. Opcode Level (Individual Instructions)

### Opcode Name Mapping
Complete mapping of EVM opcodes to human-readable names:
- Arithmetic: ADD, MUL, SUB, DIV, MOD, etc.
- Stack operations: PUSH1-PUSH32, DUP1-DUP16, SWAP1-SWAP16
- Memory operations: MLOAD, MSTORE, MSTORE8
- Storage operations: SLOAD, SSTORE
- Control flow: JUMP, JUMPI, JUMPDEST
- Calls: CALL, DELEGATECALL, STATICCALL, CREATE, CREATE2

### Log Format
```
EVM Opcode: TxHash=0x1234... BlockNum=12345 PC=42 OP=SSTORE (0x55) Depth=1
```

## Complete Debugging Session Example

### 1. RPC Call
```bash
curl -X POST --data '{"method":"debug_traceTransaction","params":["0xabc123..."]}'
```

### 2. Chain State Log
```
Client API: Starting transaction execution TxHash=0xabc123... From=0x456... To=0x789...
```

### 3. Executive Setup
```
Executive: Setting up opcode logging for TxHash=0xabc123... BlockNum=12345
```

### 4. VM Opcode Execution
```
EVM Opcode: TxHash=0xabc123... BlockNum=12345 PC=0 OP=PUSH1 (0x60) Depth=1
EVM Opcode: TxHash=0xabc123... BlockNum=12345 PC=2 OP=PUSH1 (0x60) Depth=1
EVM Opcode: TxHash=0xabc123... BlockNum=12345 PC=4 OP=MSTORE (0x52) Depth=1
EVM Opcode: TxHash=0xabc123... BlockNum=12345 PC=5 OP=SSTORE (0x55) Depth=1
```

### 5. Chain State Completion
```
Client API: Transaction execution completed TxHash=0xabc123... Result=SUCCESS GasUsed=21204
```

### 6. RPC Response
```json
{
  "result": {
    "gas": "0x52cc",
    "failed": false,
    "returnValue": "0x",
    "structLogs": [
      {"pc": 0, "op": "PUSH1", "gas": 21000, "stack": [], ...},
      {"pc": 2, "op": "PUSH1", "gas": 20997, "stack": ["0x01"], ...},
      ...
    ]
  }
}
```

## Transaction Hash Traceability

The transaction hash serves as the correlation key across all logging levels:

1. **RPC Level**: Validates and processes the transaction hash from client request
2. **Chain State Level**: Uses `ts.sha3().hexPrefixed()` for API-level logging
3. **Executive Level**: Uses `m_t.sha3().hexPrefixed()` for opcode callback setup
4. **VM Level**: Receives transaction context through callback for opcode logging

## Block-Level Context Enhancement

Enhanced logging includes:
- **Block Number**: `m_envInfo.number()` for temporal context
- **Execution Depth**: Contract call depth for call stack tracing
- **State Roots**: Available for MPT debugging (can be extended)

## Error Handling

Complete error handling at each level:
- **RPC Level**: Invalid transaction hash, transaction not found
- **Chain State Level**: Insufficient balance, account state errors
- **Executive Level**: VM exceptions, gas exhaustion
- **VM Level**: Invalid opcodes, stack overflow/underflow

## Performance Considerations

- Opcode logging is controlled by BOOST_LOG trace level
- Callback setup/cleanup minimizes performance impact
- Structured logging allows selective enabling of debug features
- Memory-efficient opcode name mapping

## Future Enhancements

Potential areas for expansion:
- State root logging for MPT analysis
- Gas cost breakdown per opcode
- Memory/storage change tracking
- Call graph visualization
- Performance profiling integration

This debugging system provides complete visibility into EVM execution while maintaining performance and modularity.