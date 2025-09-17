# EVM Debugging Flow Guide

This guide documents the complete debugging flow for EVM transaction execution in the Olympus blockchain implementation, from API calls down to individual VM opcodes.

## Overview

The Olympus EVM implementation provides comprehensive debugging and tracing capabilities across multiple layers:

1. **Client API Level** - High-level transaction and gas estimation tracing
2. **Executive Level** - Transaction execution context with block information
3. **VM Level** - Individual opcode execution with enhanced context
4. **RPC API Level** - `debug_traceTransaction` for external debugging

## Debugging Architecture

### 1. Client API-Level Tracing (`mcp/node/Client.cpp`)

The Client class provides the highest level of tracing for EVM interactions:

#### Key Methods:
- `Client::call()` - Contract calls and state queries
- `Client::estimateGas()` - Gas estimation for transactions

#### Logged Information:
- Source/destination addresses
- Transaction value, gas limits, and gas prices
- Block number and context
- Transaction hash and execution results
- Gas usage and output size
- Exception status

#### Log Level:
Uses `BOOST_LOG_TRIVIAL(debug)` for API-level events.

### 2. Executive-Level Tracing (`mcp/node/evm/Executive.cpp`)

The Executive class manages transaction execution and provides enhanced opcode logging:

#### Enhanced Opcode Callback (`g_opcodeLogCallback`):
```cpp
g_opcodeLogCallback = OpcodeLogCallback([this](uint64_t pc, Instruction op, const std::string& opName, const VM* vm) {
    // Enhanced logging with block-level context
    BOOST_LOG(m_log.trace) << "EVM Opcode: TxHash=" << m_t.sha3().hexPrefixed() 
                          << " BlockMci=" << m_envInfo.mci()
                          << " Timestamp=" << m_envInfo.timestamp()
                          << " PC=" << pc << " OP=" << opName 
                          << " Gas=" << m_gas;
    
    // Connect to tracer for structured tracing
    if (m_tracer && m_ext) {
        m_tracer->CaptureState(pc, op, 0, static_cast<uint64_t>(m_gas), nullptr, m_ext.get());
    }
});
```

#### Logged Information:
- Transaction hash for complete traceability
- Block MCI (Main Chain Index) for block context
- Block timestamp
- Program counter (PC) and opcode name
- Current gas consumption
- Integration with structured tracers

#### Log Level:
Uses `BOOST_LOG(m_log.trace)` for opcode-level events.

### 3. VM-Level Tracing (`libinterpreter/VM.h`, `libinterpreter/VM.cpp`)

The VM class provides low-level opcode execution tracing:

#### Global Opcode Callback:
- `g_opcodeLogCallback` - Global callback for opcode logging
- `OpcodeLogCallback` - Function type for opcode callbacks
- `getInstructionName()` - Maps opcodes to human-readable names

#### VM Context:
The callback receives:
- Program counter (`pc`)
- Instruction opcode (`op`)
- Human-readable opcode name (`opName`)
- VM instance pointer for context (`vm`)

### 4. RPC API-Level Debugging (`mcp/rpc/handler.cpp`)

#### `debug_traceTransaction` RPC Method:

Provides external access to transaction tracing:

```cpp
void mcp::rpc_handler::debug_traceTransaction(mcp::json &j_response, bool &)
{
    // Get transaction and block information
    h256 txHash = jsToHash(params[0]);
    LocalisedTransaction t = client()->localisedTransaction(txHash);
    Block block = client()->blockByHash(t.blockHash(), true);
    
    // Create tracer and executive
    mcp::ExecutionResult er;
    std::shared_ptr<Tracer> tracer = NewTracer(tracerOptions, er);
    Executive executive(s, block, t.transactionIndex(), client()->blockChain(), tracer);
    
    // Execute with tracing
    traceTransaction(executive, t);
    
    // Return structured results
    j_response["result"] = tracer->GetResult();
}
```

## Complete Debugging Flow

### 1. External RPC Call
```json
{
  "method": "debug_traceTransaction",
  "params": ["0x1234...abcd", {"tracer": "callTracer"}]
}
```

### 2. Client API Processing
```
Client::call() -> BOOST_LOG_TRIVIAL(debug) logs:
- From/To addresses
- Transaction value and gas parameters
- Block context
```

### 3. Executive Transaction Setup
```
Executive constructor -> Enhanced block context:
- Block MCI and timestamp
- Transaction hash
- Environment setup
```

### 4. VM Opcode Execution
```
VM::onOperation() -> g_opcodeLogCallback triggers:
- Individual opcode execution
- Program counter tracking
- Gas consumption monitoring
- Complete transaction context
```

### 5. Structured Response
```json
{
  "result": {
    "gas": "0x5208",
    "failed": false,
    "returnValue": "0x",
    "structLogs": [...]
  }
}
```

## Configuration and Usage

### Enabling Debug Logging

1. **Compile-time**: Ensure BOOST_LOG is properly configured
2. **Runtime**: Set appropriate log levels:
   - `debug` for Client API tracing
   - `trace` for VM opcode tracing

### Log Output Format

#### Client API Logs:
```
Client::call starting - From=0x1234... To=0x5678... Value=1000000000000000000 Gas=21000
Client::call completed - TxHash=0xabcd... GasUsed=21000 OutputSize=0 Exception=0
```

#### VM Opcode Logs:
```
EVM Opcode: TxHash=0xabcd... BlockMci=12345 Timestamp=1634567890 PC=0 OP=PUSH1 Gas=20979
EVM Opcode: TxHash=0xabcd... BlockMci=12345 Timestamp=1634567890 PC=2 OP=PUSH1 Gas=20976
```

## Integration with External Tools

### Tracer Integration
The system integrates with:
- Call tracers for call stack analysis
- 4byte tracers for function identification
- Custom tracers for specialized debugging

### Error Handling
- Transaction failures are logged with exception details
- Tracer failures are isolated to prevent VM execution disruption
- Block and transaction not found errors are handled gracefully

## Best Practices

1. **Performance**: Use appropriate log levels in production
2. **Context**: Always include transaction hash for traceability
3. **Block Context**: Include block information for complete debugging
4. **Error Isolation**: Ensure tracer failures don't affect execution
5. **Structured Output**: Use consistent formats for automated analysis

## Troubleshooting

### Common Issues:
1. **Missing logs**: Check log level configuration
2. **Incomplete traces**: Verify tracer setup in debug_traceTransaction
3. **Performance issues**: Consider filtering by transaction hash or block range

### Debug Commands:
```bash
# Enable debug logging
export LOG_LEVEL=debug

# Trace specific transaction
curl -X POST -H "Content-Type: application/json" \
  --data '{"method":"debug_traceTransaction","params":["0x..."],"id":1}' \
  http://localhost:8765
```

This comprehensive debugging flow enables complete visibility from high-level API calls down to individual EVM opcode execution, supporting both development debugging and production monitoring.