#include "OPExecutionRecord.hpp"
#include <libevm/LegacyVM.h>
#include <mcp/node/evm/ExtVM.h>

using namespace dev::eth;

mcp::OPExecutionStep::OPExecutionStep() 
    : pc(0), instruction(dev::eth::Instruction::STOP), gas(0), gasCost(0), depth(0) 
{
}

mcp::json mcp::OPExecutionStep::toJson() const
{
    mcp::json r = mcp::json::object();
    
    r["pc"] = pc;
    r["op"] = instructionName;
    r["gas"] = gas;
    r["gasCost"] = gasCost;
    r["depth"] = depth;
    r["stack"] = stack;
    
    if (!memory.empty())
        r["memory"] = memory;
    
    if (!storage.empty())
        r["storage"] = storage;
    
    return r;
}

mcp::OPExecutionRecord::OPExecutionRecord() 
    : totalGasUsed(0), failed(false), exception(mcp::TransactionException::None) 
{
}

void mcp::OPExecutionRecord::addStep(const OPExecutionStep& step)
{
    steps.push_back(step);
}

void mcp::OPExecutionRecord::captureStep(uint64_t PC, dev::eth::Instruction inst, uint64_t gasCost,
                                        uint64_t gas, dev::eth::VMFace const* vm, dev::eth::ExtVMFace const* extVM,
                                        bool includeMemory, bool includeStorage)
{
    if (!extVM)
        return;
        
    ExtVM const& ext = dynamic_cast<ExtVM const&>(*extVM);
    auto legacyVM = dynamic_cast<LegacyVM const*>(vm);
    
    OPExecutionStep step;
    step.pc = PC;
    step.instruction = inst;
    step.instructionName = instructionInfo(inst).name;
    step.gas = gas;
    step.gasCost = gasCost;
    step.depth = ext.depth + 1;  // depth in standard trace is 1-based
    step.contractAddress = ext.myAddress;
    
    // Capture stack state
    if (legacyVM)
    {
        for (auto const& i : legacyVM->stack())
        {
            step.stack.push_back(toCompactHexPrefixedTrim(i));
        }
    }
    
    // Capture memory state if requested
    if (includeMemory && legacyVM)
    {
        bytes const& memory = legacyVM->memory();
        for (unsigned i = 0; i < memory.size(); i += 32)
        {
            bytesConstRef memRef(memory.data() + i, 32);
            step.memory.push_back(toHex(memRef));
        }
    }
    
    // Capture storage state for specific instructions if requested
    if (includeStorage && (inst == Instruction::SLOAD || inst == Instruction::SSTORE))
    {
        for (auto const& i : ext.state().storage(ext.myAddress))
        {
            step.storage[toCompactHex(i.second.first, 32)] = toCompactHex(i.second.second, 32);
        }
    }
    
    steps.push_back(step);
}

const mcp::OPExecutionStep& mcp::OPExecutionRecord::getStep(size_t index) const
{
    if (index >= steps.size())
        throw std::out_of_range("Step index out of range");
    
    return steps[index];
}

mcp::json mcp::OPExecutionRecord::toJson() const
{
    mcp::json ret = mcp::json::object();
    
    ret["gas"] = totalGasUsed;
    ret["failed"] = failed;
    ret["returnValue"] = toHex(output);
    
    mcp::json structLogs = mcp::json::array();
    for (const auto& step : steps)
    {
        structLogs.push_back(step.toJson());
    }
    ret["structLogs"] = structLogs;
    
    return ret;
}

void mcp::OPExecutionRecord::clear()
{
    steps.clear();
    totalGasUsed = 0;
    failed = false;
    output.clear();
    exception = mcp::TransactionException::None;
}