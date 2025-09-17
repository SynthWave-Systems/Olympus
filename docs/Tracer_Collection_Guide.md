# Tracer Collection Usage Guide

This guide demonstrates how to use the comprehensive tracer collection functionality in Olympus EVM debugging system.

## Overview

The Olympus EVM debugging system supports multiple tracer types that can be selected via the `debug_traceTransaction` RPC API. Each tracer provides different perspectives on transaction execution, allowing users to choose the most appropriate analysis tool for their debugging needs.

## Available Tracers

### 1. **OpCode Tracer** (Default)
- **Purpose**: Detailed instruction-level execution tracing
- **Use Cases**: Low-level debugging, gas optimization, understanding EVM execution
- **Output**: Complete opcode trace with stack, memory, and storage state

### 2. **Call Tracer**
- **Purpose**: High-level call interaction tracing
- **Use Cases**: Contract interaction analysis, call tree visualization, transaction flow understanding
- **Output**: Nested call structure with input/output data and gas usage

### 3. **4byte Tracer**
- **Purpose**: Function signature analysis
- **Use Cases**: ABI method identification, function call frequency analysis
- **Output**: Mapping of 4-byte function selectors to call counts

### 4. **PreState Tracer**
- **Purpose**: Account state analysis before execution
- **Use Cases**: State change analysis, balance tracking, nonce verification
- **Output**: Account states (balance, nonce, code) before transaction execution

## Usage Examples

### Default OpCode Tracer

When no tracer is specified, the system defaults to the OpCode tracer:

```bash
curl -X POST http://localhost:8765 \
  -H "Content-Type: application/json" \
  --data '{
    "jsonrpc":"2.0",
    "method":"debug_traceTransaction",
    "params":["0xe18a384a972c18fd514653e3c97e2722cf1fe3eb274cd9b4c70f2a043d290346"],
    "id":1
  }'
```

**Response:**
```json
{
  "result": {
    "gas": "0x5208",
    "failed": false,
    "returnValue": "0x",
    "structLogs": [
      {"pc": 0, "op": "PUSH1", "gas": 21000, "stack": []},
      {"pc": 2, "op": "PUSH1", "gas": 20997, "stack": ["0x01"]},
      {"pc": 4, "op": "MSTORE", "gas": 20994, "stack": ["0x01", "0x80"]}
    ]
  }
}
```

### Call Tracer

For high-level call analysis:

```bash
curl -X POST http://localhost:8765 \
  -H "Content-Type: application/json" \
  --data '{
    "jsonrpc":"2.0",
    "method":"debug_traceTransaction",
    "params":[
      "0xe18a384a972c18fd514653e3c97e2722cf1fe3eb274cd9b4c70f2a043d290346",
      {"tracer": "callTracer"}
    ],
    "id":1
  }'
```

**Response:**
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

### 4byte Tracer

For function signature analysis:

```bash
curl -X POST http://localhost:8765 \
  -H "Content-Type: application/json" \
  --data '{
    "jsonrpc":"2.0",
    "method":"debug_traceTransaction",
    "params":[
      "0xe18a384a972c18fd514653e3c97e2722cf1fe3eb274cd9b4c70f2a043d290346",
      {"tracer": "4byteTracer"}
    ],
    "id":1
  }'
```

**Response:**
```json
{
  "result": {
    "0xa9059cbb": 2,
    "0x23b872dd": 1,
    "0x095ea7b3": 1
  }
}
```

### PreState Tracer

For account state analysis:

```bash
curl -X POST http://localhost:8765 \
  -H "Content-Type: application/json" \
  --data '{
    "jsonrpc":"2.0",
    "method":"debug_traceTransaction",
    "params":[
      "0xe18a384a972c18fd514653e3c97e2722cf1fe3eb274cd9b4c70f2a043d290346",
      {"tracer": "prestateTracer"}
    ],
    "id":1
  }'
```

**Response:**
```json
{
  "result": {
    "0x456...": {
      "balance": "0x152d02c7e14af6800000",
      "nonce": 5
    },
    "0x789...": {
      "balance": "0x0",
      "nonce": 1,
      "code": "0x608060405234801561001057600080fd5b50..."
    }
  }
}
```

## Advanced Configuration

### Call Tracer with Configuration

```bash
curl -X POST http://localhost:8765 \
  -H "Content-Type: application/json" \
  --data '{
    "jsonrpc":"2.0",
    "method":"debug_traceTransaction",
    "params":[
      "0xe18a384a972c18fd514653e3c97e2722cf1fe3eb274cd9b4c70f2a043d290346",
      {
        "tracer": "callTracer",
        "tracerConfig": {
          "OnlyTopCall": false,
          "WithLog": true
        }
      }
    ],
    "id":1
  }'
```

### OpCode Tracer with Custom Options

```bash
curl -X POST http://localhost:8765 \
  -H "Content-Type: application/json" \
  --data '{
    "jsonrpc":"2.0",
    "method":"debug_traceTransaction",
    "params":[
      "0xe18a384a972c18fd514653e3c97e2722cf1fe3eb274cd9b4c70f2a043d290346",
      {
        "enableMemory": true,
        "disableStorage": false,
        "disableStack": false,
        "limit": 1000
      }
    ],
    "id":1
  }'
```

## Implementation Details

### Tracer Factory System

The system uses a factory pattern implemented in `mcp/node/tracers/Tracer.cpp`:

```cpp
std::shared_ptr<Tracer> mcp::NewTracer(mcp::json const& _param, mcp::ExecutionResult& _er)
{
    if (_param.count("tracer") && !_param["tracer"].empty())
    {
        if (_param["tracer"] == "noopTracer")
            return std::make_shared<Tracer>();
        else if (_param["tracer"] == "4byteTracer")
            return std::make_shared<FourByteTracer>();
        else if (_param["tracer"] == "callTracer")
            return std::make_shared<CallTracer>(_er, _param.value("tracerConfig", mcp::json()));
        else if (_param["tracer"] == "prestateTracer")
            return std::make_shared<PreStateTracer>(_er, _param.value("tracerConfig", mcp::json()));
    }
    
    // Default to OpCode tracer
    return std::make_shared<OpCode>(_er, _param);
}
```

### RPC Integration

The `debug_traceTransaction` RPC method in `mcp/rpc/handler.cpp`:

```cpp
void mcp::rpc_handler::debug_traceTransaction(mcp::json &j_response, bool &)
{
    // Get transaction and block information
    h256 txHash = jsToHash(params[0]);
    LocalisedTransaction t = client()->localisedTransaction(txHash);
    Block block = client()->blockByHash(t.blockHash(), true);
    
    // Set up tracer options from params[1]
    mcp::json tracerOptions;
    if (params.size() > 1 && !params[1].is_null()) {
        tracerOptions = params[1];
    }
    
    // Create execution result and tracer
    mcp::ExecutionResult er;
    std::shared_ptr<Tracer> tracer = NewTracer(tracerOptions, er);
    
    // Execute transaction with selected tracer
    // ... execution logic ...
    
    // Return structured trace results
    j_response["result"] = tracer->GetResult();
}
```

## Best Practices

### Choosing the Right Tracer

1. **OpCode Tracer**: Use for detailed gas optimization, understanding EVM mechanics, or debugging complex logic
2. **Call Tracer**: Use for understanding contract interactions, analyzing transaction flow, or debugging cross-contract calls
3. **4byte Tracer**: Use for analyzing function usage patterns, identifying frequently called methods
4. **PreState Tracer**: Use for understanding state changes, debugging balance issues, or analyzing account interactions

### Performance Considerations

- **OpCode Tracer**: Most detailed but also most resource-intensive
- **Call Tracer**: Good balance of detail and performance for most use cases
- **4byte Tracer**: Lightweight, suitable for batch analysis
- **PreState Tracer**: Moderate resource usage, focused on state analysis

### Error Handling

The system gracefully handles:
- Unknown tracer types (falls back to OpCode tracer)
- Invalid tracer configurations (uses defaults)
- Missing transaction data (returns appropriate error messages)

## Integration with Enhanced Logging

The tracer system integrates seamlessly with the enhanced logging system:

- **Block Context**: All tracers receive block MCI, timestamp, and state information
- **Transaction Correlation**: Complete transaction hash traceability across all layers
- **Client API Integration**: Works with enhanced Client API-level tracing
- **BOOST_LOG Integration**: Structured logging with appropriate log levels

## Conclusion

The Olympus EVM debugging system provides a comprehensive, production-ready tracer collection that enables users to:

- **Select appropriate analysis tools** via simple RPC parameters
- **Get structured, detailed output** in standard formats
- **Integrate with existing debugging workflows** using familiar APIs
- **Scale from detailed opcode analysis to high-level call tracing** as needed

The system is already fully implemented and ready for use exactly as requested in the original comment.