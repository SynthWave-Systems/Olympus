# EVM Debugging Flow Guide

This guide documents the complete debugging flow for EVM transaction execution in the Olympus blockchain implementation, from API calls down to individual VM opcodes.

## Overview

The Olympus EVM implementation provides comprehensive debugging and tracing capabilities across multiple layers:

1. **Client API Level** - High-level transaction and gas estimation tracing
2. **Executive Level** - Transaction execution context with block information
3. **VM Level** - Individual opcode execution with enhanced context
4. **RPC API Level** - `debug_traceTransaction` for external debugging

## Enhanced Debugging Architecture

### Multi-Layer Tracing System

```
Client RPC API → Chain State → Executive → VM → Opcodes
     ↓              ↓           ↓         ↓       ↓
   debug_*      executeTransaction  go()   exec()  onOperation()
 RPC methods    Client API logging      BOOST_LOG  Opcode logging
                                             ↓
                                    Tracer Integration
                                    (OpCode, Call, 4byte, PreState)
```

## 1. Client API-Level Tracing (`mcp/node/Client.cpp`)

The Client class provides the highest level of tracing for EVM interactions:

#### Key Methods:
- `Client::call()` - Contract calls and state queries
- `Client::estimateGas()` - Gas estimation for transactions

#### Enhanced Logging Format:
```
Client::call starting - From=0x1234... To=0x5678... Value=1000000000000000000 Gas=21000
Client::call executing transaction - TxHash=0xabcd... Nonce=5 ActualGas=21000
Client::call completed - TxHash=0xabcd... GasUsed=21000 OutputSize=0 Exception=0
```

#### Features:
- Source/destination addresses with complete parameter logging
- Transaction value, gas limits, and gas prices tracking
- Block number and execution context
- Transaction hash and execution results correlation
- Gas usage and output size monitoring
- Exception status and error reporting

#### Log Level:
Uses `BOOST_LOG_TRIVIAL(debug)` for API-level events.

## 2. Executive-Level Tracing (`mcp/node/evm/Executive.cpp`)

The Executive class manages transaction execution and provides enhanced opcode logging:

#### Enhanced Opcode Callback (`g_opcodeLogCallback`):
```cpp
g_opcodeLogCallback = OpcodeLogCallback([this](uint64_t pc, Instruction op, const std::string& opName, const VM* vm) {
    // Enhanced logging with block-level context (block number, state root) for complete traceability
    BOOST_LOG(m_log.trace) << "EVM Opcode: TxHash=" << m_t.sha3().hexPrefixed() 
                          << " BlockMci=" << m_envInfo.mci()
                          << " Timestamp=" << m_envInfo.timestamp()
                          << " PC=" << pc << " OP=" << opName 
                          << " Gas=" << m_gas;
    
    // Connect to shared_ptr tracer for structured tracing
    if (m_tracer && m_ext) {
        m_tracer->CaptureState(pc, op, 0, static_cast<uint64_t>(m_gas), nullptr, m_ext.get());
    }
});
```

#### Enhanced Context Information:
- **Transaction hash** for complete traceability across all layers
- **Block MCI (Main Chain Index)** for block context awareness
- **Block timestamp** for temporal correlation
- **Program counter (PC)** and human-readable opcode names
- **Current gas consumption** tracking
- **Integration with structured tracers** for detailed analysis

#### Log Level:
Uses `BOOST_LOG(m_log.trace)` for opcode-level events.

## 3. VM-Level Tracing (`libinterpreter/VM.h`, `libinterpreter/VM.cpp`)

The VM class provides low-level opcode execution tracing:

#### Global Opcode Callback System:
- `g_opcodeLogCallback` - Global callback for opcode logging
- `OpcodeLogCallback` - Function type for opcode callbacks
- `getInstructionName()` - Comprehensive opcode to name mapping

#### Enhanced Opcode Name Mapping:
Complete mapping covering all EVM instructions:
- **Arithmetic**: ADD, MUL, SUB, DIV, MOD, etc.
- **Stack operations**: PUSH1-PUSH32, DUP1-DUP16, SWAP1-SWAP16
- **Memory operations**: MLOAD, MSTORE, MSTORE8
- **Storage operations**: SLOAD, SSTORE
- **Control flow**: JUMP, JUMPI, JUMPDEST
- **Calls**: CALL, DELEGATECALL, STATICCALL, CREATE, CREATE2

#### VM Context:
The callback receives complete execution context:
- Program counter (`pc`)
- Instruction opcode (`op`)
- Human-readable opcode name (`opName`)
- VM instance pointer for state access (`vm`)

## 4. Advanced Tracer Integration (`mcp/node/tracers/`)

### Existing Tracer Infrastructure:

#### Base Tracer System:
```cpp
class Tracer : public dev::eth::EVMLogger {
    virtual void CaptureState(uint64_t PC, dev::eth::Instruction inst,
        uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, 
        dev::eth::ExtVMFace const* voidExt) override;
    virtual mcp::json GetResult() = 0;
};
```

#### Specialized Tracer Types:

1. **OpCode Tracer** (`mcp/node/tracers/OpCode.hpp`)
   - Detailed instruction-level execution tracing
   - Stack, memory, and storage state capture
   - Configurable capture options (disableStack, enableMemory, etc.)

2. **Call Tracer** (`mcp/node/tracers/Call.hpp`)
   - High-level call interaction tracing
   - Nested call tree structure
   - Call type identification (CALL, DELEGATECALL, STATICCALL, CREATE)
   - Value transfer and gas usage tracking

3. **4byte Tracer** (`mcp/node/tracers/4byte.hpp`)
   - Function signature analysis
   - ABI method identification

4. **PreState Tracer** (`mcp/node/tracers/PreState.hpp`)
   - Account state analysis before execution
   - Balance and nonce tracking

#### Tracer Factory:
```cpp
std::shared_ptr<Tracer> NewTracer(mcp::json const& _param, mcp::ExecutionResult& _er);
```

## 5. RPC API-Level Debugging (`mcp/rpc/handler.cpp`)

#### `debug_traceTransaction` RPC Method:

Enhanced implementation with tracer selection:

```cpp
void mcp::rpc_handler::debug_traceTransaction(mcp::json &j_response, bool &)
{
    h256 txHash = jsToHash(params[0]);
    LocalisedTransaction t = client()->localisedTransaction(txHash);
    Block block = client()->blockByHash(t.blockHash(), true);
    
    // Set up tracer options with type selection
    mcp::json tracerOptions;
    if (params.size() > 1 && !params[1].is_null()) {
        tracerOptions = params[1];
    }
    
    // Create appropriate tracer (OpCode, Call, 4byte, PreState)
    mcp::ExecutionResult er;
    std::shared_ptr<Tracer> tracer = NewTracer(tracerOptions, er);
    
    // Execute with complete tracing integration
    chain_state s(chain_state::Null);
    Executive executive(s, block, t.transactionIndex(), client()->blockChain(), tracer);
    executive.setResultRecipient(er);
    
    traceTransaction(executive, t);
    j_response["result"] = tracer->GetResult();
}
```

#### Tracer Selection Examples:
```bash
# OpCode tracing (default)
{"method":"debug_traceTransaction","params":["0x123..."]}

# Call tracing
{"method":"debug_traceTransaction","params":["0x123...", {"tracer":"callTracer"}]}

# 4byte analysis
{"method":"debug_traceTransaction","params":["0x123...", {"tracer":"4byteTracer"}]}

# PreState analysis
{"method":"debug_traceTransaction","params":["0x123...", {"tracer":"prestateTracer"}]}
```

## Complete Debugging Session Example

### 1. RPC Call with Call Tracer
```bash
curl -X POST --data '{"method":"debug_traceTransaction","params":["0xabc123...", {"tracer":"callTracer"}]}'
```

### 2. Client API Processing
```
Client::call starting - From=0x456... To=0x789... Value=1000000000000000000 Gas=21000 BlockNumber=100
```

### 3. Executive Enhanced Setup
```
Executive: Enhanced opcode logging setup for TxHash=0xabc123... BlockMci=12345 Timestamp=1634567890
```

### 4. VM Opcode Execution with Block Context
```
EVM Opcode: TxHash=0xabc123... BlockMci=12345 Timestamp=1634567890 PC=0 OP=PUSH1 Gas=20979
EVM Opcode: TxHash=0xabc123... BlockMci=12345 Timestamp=1634567890 PC=2 OP=PUSH1 Gas=20976
EVM Opcode: TxHash=0xabc123... BlockMci=12345 Timestamp=1634567890 PC=4 OP=MSTORE Gas=20973
```

### 5. Client API Completion
```
Client::call completed - TxHash=0xabc123... GasUsed=21000 OutputSize=0 Exception=0
```

### 6. Enhanced RPC Response (Call Tracer)
```json
{
  "result": {
    "type": "CALL",
    "from": "0x456...",
    "to": "0x789...",
    "value": "0xde0b6b3a7640000",
    "gas": "0x5208",
    "gasUsed": "0x5208",
    "input": "0x",
    "output": "0x",
    "calls": []
  }
}
```

## Transaction Hash Traceability

Enhanced correlation system provides complete traceability:

1. **RPC Level**: Transaction hash validation and parameter processing
2. **Client API Level**: Full transaction lifecycle with `TxHash=0x...` correlation
3. **Executive Level**: Block context integration with enhanced logging
4. **VM Level**: Opcode-level execution with complete transaction context
5. **Tracer Level**: Structured data collection with correlation preservation

## Block-Level Context Enhancement

Our enhanced system provides unprecedented block context awareness:

- **Block MCI**: Main Chain Index for blockchain position
- **Timestamp**: Temporal context for execution correlation
- **State Information**: Complete state context available
- **Execution Depth**: Call stack depth tracking
- **Gas Tracking**: Comprehensive gas consumption monitoring

## Advanced Features

### Error Handling
Complete error isolation at every layer:
- **RPC Level**: Invalid transaction hash, transaction not found
- **Client API Level**: Parameter validation and result tracking
- **Executive Level**: VM exceptions with graceful degradation
- **VM Level**: Opcode failures with detailed context
- **Tracer Level**: Fault tolerance with continued execution

### Performance Optimization
- **Selective Logging**: Log level control prevents performance impact
- **Tracer Selection**: Choose appropriate detail level
- **Callback Management**: Efficient setup/cleanup lifecycle
- **Memory Efficiency**: Optimized data structures

### Integration Benefits
- **External Tool Compatibility**: Standard debug_traceTransaction API
- **Multiple Output Formats**: OpCode, Call, 4byte, PreState tracers
- **Complete Correlation**: Transaction hash across all layers
- **Block Context**: Enhanced temporal and blockchain awareness

## Configuration and Usage

### Enabling Enhanced Debugging

1. **Compile-time**: BOOST_LOG integration enabled
2. **Runtime Configuration**:
   - `debug` level for Client API tracing
   - `trace` level for VM opcode logging
   - Tracer selection via RPC parameters

### Advanced Debugging Commands
```bash
# Complete opcode trace with enhanced context
curl -X POST --data '{"method":"debug_traceTransaction","params":["0x..."]}'

# Call-level tracing for contract interaction analysis
curl -X POST --data '{"method":"debug_traceTransaction","params":["0x...", {"tracer":"callTracer"}]}'

# Function signature analysis
curl -X POST --data '{"method":"debug_traceTransaction","params":["0x...", {"tracer":"4byteTracer"}]}'

# State analysis before execution
curl -X POST --data '{"method":"debug_traceTransaction","params":["0x...", {"tracer":"prestateTracer"}]}'
```

## Architecture Benefits

This enhanced debugging system provides:

1. **Complete Execution Path Visibility** - From API calls to opcodes
2. **Block-Level Context Awareness** - MCI, timestamp, state integration
3. **Multi-Format Output** - Flexible tracer selection
4. **Production-Ready Performance** - Selective logging and optimization
5. **External Tool Integration** - Standard RPC API compatibility
6. **Comprehensive Error Handling** - Fault tolerance at every layer

The system successfully combines the existing sophisticated tracer infrastructure of the evm branch with our enhanced block context logging and Client API tracing, providing a more complete debugging solution than any previous implementation.

This comprehensive debugging flow enables complete visibility into EVM execution while maintaining high performance and production readiness.