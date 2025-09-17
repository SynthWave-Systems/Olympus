#include "CallTracer.hpp"
#include <mcp/common/jsonHelper.hpp>

using namespace dev;
using namespace dev::eth;

namespace mcp
{

void CallTracer::CaptureState(uint64_t PC, Instruction inst, uint64_t gas, uint64_t cost,
                             VMFace const* vm, ExtVMFace const* extVM)
{
    // For CallTracer, we primarily care about call-related opcodes
    // Individual opcode states are less important than call boundaries
    (void)PC; (void)inst; (void)gas; (void)cost; (void)vm; (void)extVM;
}

void CallTracer::CaptureStart(int depth, Address const& from, Address const& to,
                             bytes const& input, uint64_t gas, u256 const& value)
{
    CallFrame frame;
    frame.from = from.hexPrefixed();
    frame.to = to.hexPrefixed();
    frame.value = toHexPrefixed(value);
    frame.gas = toHexPrefixed(gas);
    frame.input = toHex(input);
    frame.type = "CALL";  // Default, will be refined based on context
    
    if (!m_hasRootCall) {
        // This is the root transaction call
        m_rootCall = frame;
        m_hasRootCall = true;
    } else {
        // This is a sub-call
        m_callStack.push(frame);
    }
}

void CallTracer::CaptureEnd(bytes const& output, uint64_t gasUsed, std::string const& error)
{
    CallFrame* currentFrame = nullptr;
    
    if (m_callStack.empty()) {
        // Root call ending
        currentFrame = &m_rootCall;
    } else {
        // Sub-call ending
        CallFrame frame = m_callStack.top();
        m_callStack.pop();
        
        frame.output = toHex(output);
        frame.gasUsed = toHexPrefixed(gasUsed);
        frame.error = error;
        frame.reverted = !error.empty();
        
        // Add this frame to its parent
        if (m_callStack.empty()) {
            m_rootCall.calls.push_back(frame);
        } else {
            m_callStack.top().calls.push_back(frame);
        }
        return;
    }
    
    if (currentFrame) {
        currentFrame->output = toHex(output);
        currentFrame->gasUsed = toHexPrefixed(gasUsed);
        currentFrame->error = error;
        currentFrame->reverted = !error.empty();
    }
}

void CallTracer::CaptureFault(uint64_t PC, Instruction op, uint64_t gas, uint64_t cost,
                             ExtVMFace const* scope, int depth, std::string const& error)
{
    // Mark current call as failed
    if (!m_callStack.empty()) {
        m_callStack.top().error = error;
        m_callStack.top().reverted = true;
    } else if (m_hasRootCall) {
        m_rootCall.error = error;
        m_rootCall.reverted = true;
    }
}

mcp::json CallTracer::GetResult()
{
    return callFrameToJson(m_rootCall);
}

void CallTracer::SetConfig(mcp::json const& options)
{
    m_config = options;
    // CallTracer typically doesn't need many configuration options
    // But we store them in case future enhancements need them
}

std::string CallTracer::getCallType(Instruction inst) const
{
    switch (inst) {
        case Instruction::CALL: return "CALL";
        case Instruction::CALLCODE: return "CALLCODE";
        case Instruction::DELEGATECALL: return "DELEGATECALL";
        case Instruction::STATICCALL: return "STATICCALL";
        case Instruction::CREATE: return "CREATE";
        case Instruction::CREATE2: return "CREATE2";
        default: return "CALL";
    }
}

mcp::json CallTracer::callFrameToJson(CallFrame const& frame) const
{
    mcp::json result;
    
    result["type"] = frame.type;
    result["from"] = frame.from;
    result["to"] = frame.to;
    result["value"] = frame.value;
    result["gas"] = frame.gas;
    result["gasUsed"] = frame.gasUsed;
    result["input"] = frame.input;
    result["output"] = frame.output;
    
    if (!frame.error.empty()) {
        result["error"] = frame.error;
        result["revertReason"] = frame.error;
    }
    
    if (!frame.calls.empty()) {
        mcp::json calls = mcp::json::array();
        for (auto const& call : frame.calls) {
            calls.push_back(callFrameToJson(call));
        }
        result["calls"] = calls;
    }
    
    return result;
}

}  // namespace mcp