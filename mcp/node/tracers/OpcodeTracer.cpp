#include "OpcodeTracer.hpp"
#include <mcp/rpc/jsonHelper.hpp>
#include <mcp/node/evm/ExtVM.h>
#include <libevm/LegacyVM.h>

using namespace dev;
using namespace dev::eth;

namespace mcp
{

void OpcodeTracer::CaptureState(uint64_t PC, Instruction inst, uint64_t gas, uint64_t cost,
                               VMFace const* vm, ExtVMFace const* extVM)
{
    OpcodeStep step;
    step.pc = PC;
    step.op = getInstructionName(inst);
    step.gas = gas;
    step.gasCost = cost;
    step.depth = extVM ? extVM->depth : 0;

    // Capture stack if enabled and VM supports it
    if (!m_disableStack && vm) {
        auto legacyVM = dynamic_cast<LegacyVM const*>(vm);
        if (legacyVM) {
            for (auto const& item : legacyVM->stack()) {
                step.stack.push_back(dev::toCompactHex(item, 32));
            }
        }
    }

    // Capture memory if enabled and relevant
    if (!m_disableMemory && shouldCaptureMemory(inst) && vm) {
        auto legacyVM = dynamic_cast<LegacyVM const*>(vm);
        if (legacyVM) {
            auto const& memory = legacyVM->memory();
            for (size_t i = 0; i < memory.size(); i += 32) {
                bytesConstRef chunk(memory.data() + i, std::min(size_t(32), memory.size() - i));
                step.memory.push_back(dev::toHex(chunk));
            }
        }
    }

    // Capture storage if enabled and relevant
    if (!m_disableStorage && (m_fullStorage || shouldCaptureStorage(inst)) && extVM) {
        auto extVMImpl = dynamic_cast<mcp::ExtVM const*>(extVM);
        if (extVMImpl) {
            step.storage = mcp::json::object();
            for (auto const& item : extVMImpl->state().storage(extVMImpl->myAddress)) {
                step.storage[dev::toCompactHexPrefixed(item.second.first, 1)] = 
                    dev::toCompactHex(item.second.second, 32);
            }
        }
    }

    m_steps.push_back(step);
}

void OpcodeTracer::CaptureStart(int depth, Address const& from, Address const& to,
                               bytes const& input, uint64_t gas, u256 const& value)
{
    // For opcode tracer, call boundaries are less important
    // The actual CALL/CREATE opcodes will be captured in CaptureState
    (void)depth; (void)from; (void)to; (void)input; (void)gas; (void)value;
}

void OpcodeTracer::CaptureEnd(bytes const& output, uint64_t gasUsed, std::string const& error)
{
    // Mark the last step with return information if there was an error
    if (!error.empty() && !m_steps.empty()) {
        m_steps.back().error = error;
    }
    (void)output; (void)gasUsed;
}

void OpcodeTracer::CaptureFault(uint64_t PC, Instruction op, uint64_t gas, uint64_t cost,
                               ExtVMFace const* scope, int depth, std::string const& error)
{
    // Create a step for the faulting instruction
    OpcodeStep step;
    step.pc = PC;
    step.op = getInstructionName(op);
    step.gas = gas;
    step.gasCost = cost;
    step.depth = depth;
    step.error = error;
    
    m_steps.push_back(step);
}

mcp::json OpcodeTracer::GetResult()
{
    mcp::json result = mcp::json::object();
    mcp::json structLogs = mcp::json::array();
    
    for (auto const& step : m_steps) {
        mcp::json logEntry = mcp::json::object();
        
        logEntry["pc"] = step.pc;
        logEntry["op"] = step.op;
        logEntry["gas"] = step.gas;
        logEntry["gasCost"] = step.gasCost;
        logEntry["depth"] = step.depth;
        
        if (!step.stack.empty()) {
            logEntry["stack"] = step.stack;
        }
        
        if (!step.memory.empty()) {
            logEntry["memory"] = step.memory;
        }
        
        if (!step.storage.empty()) {
            logEntry["storage"] = step.storage;
        }
        
        if (!step.error.empty()) {
            logEntry["error"] = step.error;
        }
        
        structLogs.push_back(logEntry);
    }
    
    result["structLogs"] = structLogs;
    return result;
}

void OpcodeTracer::SetConfig(mcp::json const& options)
{
    m_config = options;
    m_disableStack = options.value("disableStack", false);
    m_disableMemory = options.value("disableMemory", false);
    m_disableStorage = options.value("disableStorage", false);
    m_fullStorage = options.value("fullStorage", false);
}

std::string OpcodeTracer::getInstructionName(Instruction inst) const
{
    // Reuse the same logic from VM::getInstructionName
    switch (inst) {
        case Instruction::STOP: return "STOP";
        case Instruction::ADD: return "ADD";
        case Instruction::MUL: return "MUL";
        case Instruction::SUB: return "SUB";
        case Instruction::DIV: return "DIV";
        case Instruction::SDIV: return "SDIV";
        case Instruction::MOD: return "MOD";
        case Instruction::SMOD: return "SMOD";
        case Instruction::ADDMOD: return "ADDMOD";
        case Instruction::MULMOD: return "MULMOD";
        case Instruction::EXP: return "EXP";
        case Instruction::SIGNEXTEND: return "SIGNEXTEND";
        case Instruction::LT: return "LT";
        case Instruction::GT: return "GT";
        case Instruction::SLT: return "SLT";
        case Instruction::SGT: return "SGT";
        case Instruction::EQ: return "EQ";
        case Instruction::ISZERO: return "ISZERO";
        case Instruction::AND: return "AND";
        case Instruction::OR: return "OR";
        case Instruction::XOR: return "XOR";
        case Instruction::NOT: return "NOT";
        case Instruction::BYTE: return "BYTE";
        case Instruction::SHL: return "SHL";
        case Instruction::SHR: return "SHR";
        case Instruction::SAR: return "SAR";
        case Instruction::KECCAK256: return "KECCAK256";
        case Instruction::ADDRESS: return "ADDRESS";
        case Instruction::BALANCE: return "BALANCE";
        case Instruction::ORIGIN: return "ORIGIN";
        case Instruction::CALLER: return "CALLER";
        case Instruction::CALLVALUE: return "CALLVALUE";
        case Instruction::CALLDATALOAD: return "CALLDATALOAD";
        case Instruction::CALLDATASIZE: return "CALLDATASIZE";
        case Instruction::CALLDATACOPY: return "CALLDATACOPY";
        case Instruction::CODESIZE: return "CODESIZE";
        case Instruction::CODECOPY: return "CODECOPY";
        case Instruction::GASPRICE: return "GASPRICE";
        case Instruction::EXTCODESIZE: return "EXTCODESIZE";
        case Instruction::EXTCODECOPY: return "EXTCODECOPY";
        case Instruction::RETURNDATASIZE: return "RETURNDATASIZE";
        case Instruction::RETURNDATACOPY: return "RETURNDATACOPY";
        case Instruction::EXTCODEHASH: return "EXTCODEHASH";
        case Instruction::BLOCKHASH: return "BLOCKHASH";
        case Instruction::COINBASE: return "COINBASE";
        case Instruction::TIMESTAMP: return "TIMESTAMP";
        case Instruction::NUMBER: return "NUMBER";
        case Instruction::DIFFICULTY: return "DIFFICULTY";
        case Instruction::GASLIMIT: return "GASLIMIT";
        case Instruction::CHAINID: return "CHAINID";
        case Instruction::SELFBALANCE: return "SELFBALANCE";
        case Instruction::BASEFEE: return "BASEFEE";
        case Instruction::POP: return "POP";
        case Instruction::MLOAD: return "MLOAD";
        case Instruction::MSTORE: return "MSTORE";
        case Instruction::MSTORE8: return "MSTORE8";
        case Instruction::SLOAD: return "SLOAD";
        case Instruction::SSTORE: return "SSTORE";
        case Instruction::JUMP: return "JUMP";
        case Instruction::JUMPI: return "JUMPI";
        case Instruction::PC: return "PC";
        case Instruction::MSIZE: return "MSIZE";
        case Instruction::GAS: return "GAS";
        case Instruction::JUMPDEST: return "JUMPDEST";
        case Instruction::PUSH0: return "PUSH0";
        case Instruction::LOG0: return "LOG0";
        case Instruction::LOG1: return "LOG1";
        case Instruction::LOG2: return "LOG2";
        case Instruction::LOG3: return "LOG3";
        case Instruction::LOG4: return "LOG4";
        case Instruction::CREATE: return "CREATE";
        case Instruction::CALL: return "CALL";
        case Instruction::CALLCODE: return "CALLCODE";
        case Instruction::RETURN: return "RETURN";
        case Instruction::DELEGATECALL: return "DELEGATECALL";
        case Instruction::CREATE2: return "CREATE2";
        case Instruction::STATICCALL: return "STATICCALL";
        case Instruction::REVERT: return "REVERT";
        case Instruction::SELFDESTRUCT: return "SELFDESTRUCT";
        default: {
            int opcode = static_cast<int>(inst);
            if (opcode >= 0x60 && opcode <= 0x7f) {
                return "PUSH" + std::to_string(opcode - 0x5f);
            } else if (opcode >= 0x80 && opcode <= 0x8f) {
                return "DUP" + std::to_string(opcode - 0x7f);
            } else if (opcode >= 0x90 && opcode <= 0x9f) {
                return "SWAP" + std::to_string(opcode - 0x8f);
            }
            return "OPCODE_" + std::to_string(opcode);
        }
    }
}

bool OpcodeTracer::shouldCaptureMemory(Instruction inst) const
{
    return inst == Instruction::MSTORE || inst == Instruction::MSTORE8 || 
           inst == Instruction::MLOAD || inst == Instruction::CREATE ||
           inst == Instruction::CALL || inst == Instruction::CALLCODE ||
           inst == Instruction::KECCAK256 || inst == Instruction::CALLDATACOPY ||
           inst == Instruction::CODECOPY || inst == Instruction::EXTCODECOPY ||
           inst == Instruction::DELEGATECALL;
}

bool OpcodeTracer::shouldCaptureStorage(Instruction inst) const
{
    return inst == Instruction::SSTORE;
}

}  // namespace mcp