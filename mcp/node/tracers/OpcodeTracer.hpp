#pragma once

#include "Tracer.hpp"
#include <vector>

namespace mcp
{

/**
 * @brief Opcode tracer that captures detailed instruction-level execution
 * 
 * This tracer provides comprehensive opcode-level tracing similar to StandardTrace
 * but implementing the new Tracer interface. It captures:
 * - Every opcode execution with PC, gas, and stack state
 * - Memory changes when relevant
 * - Storage changes when relevant
 * - Call depth and context
 */
class OpcodeTracer : public Tracer
{
public:
    struct OpcodeStep
    {
        uint64_t pc;                    // Program counter
        std::string op;                 // Opcode name
        uint64_t gas;                   // Remaining gas
        uint64_t gasCost;               // Cost of this operation
        int depth;                      // Call stack depth
        std::vector<std::string> stack; // Stack contents (hex strings)
        std::vector<std::string> memory; // Memory contents (32-byte chunks)
        mcp::json storage;              // Storage changes
        std::string error;              // Error if operation failed
    };

private:
    std::vector<OpcodeStep> m_steps;
    mcp::json m_config;
    bool m_disableStack = false;
    bool m_disableMemory = false;
    bool m_disableStorage = false;
    bool m_fullStorage = false;

public:
    OpcodeTracer() = default;
    virtual ~OpcodeTracer() = default;

    void CaptureState(uint64_t PC, dev::eth::Instruction inst, uint64_t gas, uint64_t cost,
                     dev::eth::VMFace const* vm, dev::eth::ExtVMFace const* extVM) override;

    void CaptureStart(int depth, dev::Address const& from, dev::Address const& to,
                     dev::bytes const& input, uint64_t gas, dev::u256 const& value) override;

    void CaptureEnd(dev::bytes const& output, uint64_t gasUsed, std::string const& error) override;

    void CaptureFault(uint64_t PC, dev::eth::Instruction op, uint64_t gas, uint64_t cost,
                     dev::eth::ExtVMFace const* scope, int depth, std::string const& error) override;

    mcp::json GetResult() override;

    std::string GetTracerName() const override { return "opcodeTracer"; }

    void SetConfig(mcp::json const& options) override;

private:
    std::string getInstructionName(dev::eth::Instruction inst) const;
    bool shouldCaptureMemory(dev::eth::Instruction inst) const;
    bool shouldCaptureStorage(dev::eth::Instruction inst) const;
};

}  // namespace mcp