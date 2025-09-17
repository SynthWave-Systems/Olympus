#pragma once

#include <libevm/ExtVMFace.h>
#include <libevm/VMFace.h>
#include <mcp/common/json.hpp>
#include <string>
#include <memory>

namespace mcp
{

/**
 * @brief Base interface for transaction tracers
 * 
 * Tracers implement different ways of collecting and formatting
 * execution trace data during transaction replay. Each tracer
 * provides its own specific output format and focuses on different
 * aspects of execution (calls, opcodes, state changes, etc.).
 */
class Tracer
{
public:
    virtual ~Tracer() = default;

    /**
     * @brief Called for each opcode execution during transaction replay
     * @param steps Step counter
     * @param PC Program counter
     * @param inst Instruction being executed
     * @param newMemSize New memory size after instruction
     * @param gasCost Gas cost of the instruction
     * @param gas Remaining gas
     * @param vm VM instance
     * @param extVM External VM context
     */
    virtual void CaptureState(uint64_t PC, dev::eth::Instruction inst, uint64_t gas, uint64_t cost,
                             dev::eth::VMFace const* vm, dev::eth::ExtVMFace const* extVM) = 0;

    /**
     * @brief Called when entering a new call context
     * @param depth Call depth
     * @param from Caller address
     * @param to Called address
     * @param input Call input data
     * @param gas Gas available for the call
     * @param value Value being transferred
     */
    virtual void CaptureStart(int depth, dev::Address const& from, dev::Address const& to,
                             dev::bytes const& input, uint64_t gas, dev::u256 const& value) = 0;

    /**
     * @brief Called when exiting a call context
     * @param output Call output data
     * @param gasUsed Gas consumed by the call
     * @param error Error message if call failed
     */
    virtual void CaptureEnd(dev::bytes const& output, uint64_t gasUsed, std::string const& error) = 0;

    /**
     * @brief Called when a fault occurs during execution
     * @param PC Program counter where fault occurred
     * @param op Instruction that caused the fault
     * @param gas Remaining gas
     * @param cost Gas cost of the instruction
     * @param scope Current call scope
     * @param depth Call depth
     * @param error Error message
     */
    virtual void CaptureFault(uint64_t PC, dev::eth::Instruction op, uint64_t gas, uint64_t cost,
                             dev::eth::ExtVMFace const* scope, int depth, std::string const& error) = 0;

    /**
     * @brief Get the final trace result as JSON
     * @return JSON representation of the trace data
     */
    virtual mcp::json GetResult() = 0;

    /**
     * @brief Get the tracer type name
     * @return String identifier for this tracer type
     */
    virtual std::string GetTracerName() const = 0;

    /**
     * @brief Configure tracer options from JSON parameters
     * @param options JSON configuration object
     */
    virtual void SetConfig(mcp::json const& options) = 0;
};

/**
 * @brief Factory function to create tracers by name
 * @param tracerName Name of the tracer to create ("callTracer", "opcodeTracer", etc.)
 * @param config Configuration options for the tracer
 * @return Shared pointer to the created tracer, or nullptr if tracer not found
 */
std::shared_ptr<Tracer> CreateTracer(std::string const& tracerName, mcp::json const& config = mcp::json{});

}  // namespace mcp