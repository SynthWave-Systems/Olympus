#pragma once

#include "Tracer.hpp"
#include <stack>
#include <vector>

namespace mcp
{

/**
 * @brief Call tracer that tracks contract calls and their results
 * 
 * This tracer focuses on high-level call interactions rather than individual opcodes.
 * It produces a nested call tree showing:
 * - Call type (CALL, DELEGATECALL, STATICCALL, CREATE, etc.)
 * - From/to addresses
 * - Input data and return data
 * - Gas usage
 * - Value transfers
 * - Success/failure status
 */
class CallTracer : public Tracer
{
public:
    struct CallFrame
    {
        std::string type;           // "CALL", "DELEGATECALL", "STATICCALL", "CREATE", "CREATE2"
        std::string from;           // Caller address
        std::string to;             // Called address (empty for CREATE)
        std::string value;          // Value transferred (hex string)
        std::string gas;            // Gas available for call (hex string)
        std::string gasUsed;        // Gas consumed by call (hex string)
        std::string input;          // Call input data (hex string)
        std::string output;         // Call output/return data (hex string)
        std::string error;          // Error message if call failed
        std::vector<CallFrame> calls; // Nested sub-calls
        bool reverted = false;      // Whether this call reverted
    };

private:
    std::stack<CallFrame> m_callStack;
    CallFrame m_rootCall;
    bool m_hasRootCall = false;
    mcp::json m_config;

public:
    CallTracer() = default;
    virtual ~CallTracer() = default;

    void CaptureState(uint64_t PC, dev::eth::Instruction inst, uint64_t gas, uint64_t cost,
                     dev::eth::VMFace const* vm, dev::eth::ExtVMFace const* extVM) override;

    void CaptureStart(int depth, dev::Address const& from, dev::Address const& to,
                     dev::bytes const& input, uint64_t gas, dev::u256 const& value) override;

    void CaptureEnd(dev::bytes const& output, uint64_t gasUsed, std::string const& error) override;

    void CaptureFault(uint64_t PC, dev::eth::Instruction op, uint64_t gas, uint64_t cost,
                     dev::eth::ExtVMFace const* scope, int depth, std::string const& error) override;

    mcp::json GetResult() override;

    std::string GetTracerName() const override { return "callTracer"; }

    void SetConfig(mcp::json const& options) override;

private:
    std::string getCallType(dev::eth::Instruction inst) const;
    mcp::json callFrameToJson(CallFrame const& frame) const;
};

}  // namespace mcp