#pragma once

#include <vector>
#include <string>
#include <map>
#include <mcp/core/common.hpp>

namespace dev { namespace eth { 
class VMFace; 
class ExtVMFace; 
enum class Instruction : uint8_t; 
}}

namespace mcp
{
    /**
     * @brief Structure representing a single opcode execution step during transaction tracing
     * 
     * This struct captures all relevant information about one execution step,
     * providing a structured interface for custom tracers to access execution data.
     */
    struct OPExecutionStep
    {
        uint64_t pc;                           ///< Program counter at this step
        dev::eth::Instruction instruction;     ///< The executed instruction/opcode
        std::string instructionName;           ///< Human-readable instruction name
        uint64_t gas;                          ///< Remaining gas before this operation
        uint64_t gasCost;                      ///< Gas cost of this operation
        uint64_t depth;                        ///< Call depth (1-based)
        std::vector<std::string> stack;        ///< Stack state (hex strings)
        std::vector<std::string> memory;       ///< Memory state (32-byte hex chunks)
        std::map<std::string, std::string> storage; ///< Storage state for this address
        dev::Address contractAddress;          ///< Address of the contract being executed
        
        /**
         * @brief Default constructor
         */
        OPExecutionStep();
        
        /**
         * @brief Convert this step to JSON format for compatibility
         * @return JSON representation of this execution step
         */
        mcp::json toJson() const;
    };

    /**
     * @brief Complete execution record for a transaction, containing all opcode execution steps
     * 
     * This struct aggregates all execution steps from a transaction trace,
     * providing custom tracers with structured access to the complete execution history.
     */
    struct OPExecutionRecord
    {
        std::vector<OPExecutionStep> steps;    ///< All execution steps in order
        uint64_t totalGasUsed;                 ///< Total gas consumed by the transaction
        bool failed;                           ///< Whether the transaction failed
        dev::bytes output;                     ///< Transaction output/return value
        mcp::TransactionException exception;   ///< Exception type if transaction failed
        
        /**
         * @brief Default constructor
         */
        OPExecutionRecord();
        
        /**
         * @brief Add a new execution step to the record
         * @param step The execution step to add
         */
        void addStep(const OPExecutionStep& step);
        
        /**
         * @brief Add a new execution step by capturing current VM state
         * @param PC Program counter
         * @param inst Instruction being executed
         * @param gasCost Gas cost of the instruction
         * @param gas Remaining gas
         * @param vm Pointer to the VM interface
         * @param extVM Pointer to the external VM interface
         * @param includeMemory Whether to capture memory state
         * @param includeStorage Whether to capture storage state
         */
        void captureStep(uint64_t PC, dev::eth::Instruction inst, uint64_t gasCost, 
                        uint64_t gas, dev::eth::VMFace const* vm, dev::eth::ExtVMFace const* extVM,
                        bool includeMemory = false, bool includeStorage = true);
        
        /**
         * @brief Get the number of execution steps recorded
         * @return Number of steps in this record
         */
        size_t getStepCount() const { return steps.size(); }
        
        /**
         * @brief Get a specific execution step by index
         * @param index Index of the step to retrieve
         * @return Const reference to the execution step
         */
        const OPExecutionStep& getStep(size_t index) const;
        
        /**
         * @brief Convert the entire execution record to JSON format
         * @return JSON representation compatible with existing trace formats
         */
        mcp::json toJson() const;
        
        /**
         * @brief Clear all recorded steps and reset the record
         */
        void clear();
    };
}