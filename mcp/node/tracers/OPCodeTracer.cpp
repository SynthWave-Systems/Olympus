#include "OPCodeTracer.hpp"

#include "../evm/ExtVM.h"

#include <libdevcore/CommonData.h>
#include <libevm/Instruction.h>
#include <libevm/InstructionInfo.h>

#include <algorithm>
#include <cstring>
#include <vector>

#include <evmc/evmc.h>

namespace mcp
{
namespace tracing
{
namespace
{
using dev::eth::Instruction;

bool changesMemory(Instruction inst)
{
    switch (inst)
    {
    case Instruction::MSTORE:
    case Instruction::MSTORE8:
    case Instruction::MLOAD:
    case Instruction::CREATE:
    case Instruction::CALL:
    case Instruction::CALLCODE:
    case Instruction::SHA3:
    case Instruction::CALLDATACOPY:
    case Instruction::CODECOPY:
    case Instruction::EXTCODECOPY:
    case Instruction::DELEGATECALL:
        return true;
    default:
        return false;
    }
}

bool changesStorage(Instruction inst)
{
    return inst == Instruction::SSTORE;
}

dev::h256 toH256(intx::uint256 const& value)
{
    evmc_uint256be be = intx::be::store<evmc_uint256be>(value);
    dev::h256 out;
    std::memcpy(out.data(), be.bytes, sizeof(be.bytes));
    return out;
}
}

OPCodeTracer::OPCodeTracer(Options options)
  : m_options(options), m_trace(mcp::json::array())
{
}

OPCodeTracer::Options OPCodeTracer::fromJson(mcp::json const& json)
{
    Options opts;
    if (!json.is_object())
        return opts;

    if (json.contains("disableStorage"))
        opts.disableStorage = json["disableStorage"].get<bool>();
    if (json.contains("disableMemory"))
        opts.disableMemory = json["disableMemory"].get<bool>();
    if (json.contains("disableStack"))
        opts.disableStack = json["disableStack"].get<bool>();
    if (json.contains("full_storage"))
        opts.fullStorage = json["full_storage"].get<bool>();
    return opts;
}

bool OPCodeTracer::needsStack() const
{
    return !m_options.disableStack;
}

bool OPCodeTracer::needsMemory() const
{
    return !m_options.disableMemory;
}

void OPCodeTracer::captureState(OperationState const& state, ExecutionContext const& context)
{
    mcp::json entry = mcp::json::object();

    if (!m_options.disableStack && state.stackTop)
    {
        mcp::json stack = mcp::json::array();
        for (size_t i = 0; i < state.stackSize; ++i)
        {
            stack.push_back(toCompactHex(toH256(state.stackTop[i]), 32));
        }
        entry["stack"] = stack;
    }

    bool newContext = false;
    Instruction lastInst = Instruction::STOP;

    unsigned depth = context.depth;
    if (depth == 0)
        depth = 1;

    if (m_lastInst.size() == depth - 1)
    {
        m_lastInst.push_back(state.opcode);
        newContext = true;
    }
    else if (!m_lastInst.empty() && m_lastInst.size() == depth)
    {
        lastInst = m_lastInst.back();
        m_lastInst.back() = state.opcode;
    }
    else if (!m_lastInst.empty() && m_lastInst.size() == depth + 1)
    {
        m_lastInst.pop_back();
        lastInst = m_lastInst.back();
        m_lastInst.back() = state.opcode;
    }
    else
    {
        m_lastInst.resize(depth);
        if (!m_lastInst.empty())
        {
            lastInst = m_lastInst.back();
            m_lastInst.back() = state.opcode;
        }
        else
        {
            m_lastInst.push_back(state.opcode);
            newContext = true;
        }
    }

    if (!m_options.disableMemory && state.memory.data && state.memory.size > 0 &&
        (newContext || changesMemory(lastInst)))
    {
        mcp::json memory = mcp::json::array();
        for (size_t i = 0; i < state.memory.size; i += 32)
        {
            size_t chunkSize = std::min<size_t>(32, state.memory.size - i);
            if (chunkSize == 32)
            {
                memory.push_back(toHex(dev::bytesConstRef(state.memory.data + i, chunkSize)));
            }
            else
            {
                dev::bytes temp(state.memory.data + i, state.memory.data + i + chunkSize);
                temp.resize(32, 0);
                memory.push_back(toHex(dev::bytesConstRef(temp.data(), temp.size())));
            }
        }
        entry["memory"] = memory;
    }

    auto const* ext = dynamic_cast<mcp::ExtVM const*>(context.ext);
    if (!m_options.disableStorage && ext &&
        (m_options.fullStorage || newContext || changesStorage(lastInst)))
    {
        mcp::json storage = mcp::json::object();
        for (auto const& item : ext->state().storage(ext->myAddress))
        {
            storage[toCompactHexPrefixed(item.second.first, 1)] =
                toCompactHex(item.second.second, 32);
        }
        entry["storage"] = storage;
    }

    if (m_showMnemonics)
        entry["op"] = dev::eth::instructionInfo(state.opcode).name;

    entry["pc"] = dev::toString(state.pc);
    entry["gas"] = dev::toString(state.gasLeft);
    entry["gasCost"] = dev::toString(state.gasCost);
    entry["depth"] = dev::toString(depth);
    if (state.newMemorySize != 0)
        entry["memexpand"] = dev::toString(state.newMemorySize);

    m_trace.push_back(entry);
}

}  // namespace tracing
}  // namespace mcp

