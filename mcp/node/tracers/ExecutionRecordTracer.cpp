#include "ExecutionRecordTracer.hpp"
#include <libevm/LegacyVM.h>
#include <mcp/node/evm/ExtVM.h>

using namespace dev::eth;

mcp::ExecutionRecordTracer::ExecutionRecordTracer(mcp::ExecutionResult& _er, mcp::json const& _param) noexcept
    : m_res(&_er), m_options(parseOptions(_param)), m_txGasLimit(0)
{
    // Initialize the execution record
    m_executionRecord = std::make_shared<OPExecutionRecord>();
}

void mcp::ExecutionRecordTracer::CaptureTxStart(uint64_t _gasLimit)
{
    m_txGasLimit = _gasLimit;
    if (m_executionRecord)
        m_executionRecord->clear();  // Clear any previous data
}

void mcp::ExecutionRecordTracer::CaptureTxEnd(uint64_t _restGas)
{
    if (m_executionRecord)
    {
        m_executionRecord->totalGasUsed = m_txGasLimit - _restGas;
    }
}

void mcp::ExecutionRecordTracer::CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, 
                                             dev::Address const& _to, bool _create, dev::bytes const& _input, 
                                             uint64_t _gas, dev::u256 _value)
{
    // Transaction-level start - could be used for additional context
}

void mcp::ExecutionRecordTracer::CaptureEnd(dev::bytes const& _output, uint64_t _gasUsed, 
                                           mcp::TransactionException const _excepted)
{
    if (m_executionRecord)
    {
        m_executionRecord->output = _output;
        m_executionRecord->failed = (_excepted != mcp::TransactionException::None);
        m_executionRecord->exception = _excepted;
        m_executionRecord->totalGasUsed = _gasUsed;
    }
}

void mcp::ExecutionRecordTracer::CaptureState(uint64_t PC, dev::eth::Instruction inst,
                                             uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, 
                                             dev::eth::ExtVMFace const* voidExt)
{
    if (!m_executionRecord || !m_options.enableFullTrace)
        return;

    // Check step limit
    if (m_options.stepLimit > 0 && m_executionRecord->getStepCount() >= static_cast<size_t>(m_options.stepLimit))
        return;

    // Capture this execution step
    m_executionRecord->captureStep(PC, inst, gasCost, gas, _vm, voidExt, 
                                   m_options.enableMemory, m_options.enableStorage);
}

mcp::json mcp::ExecutionRecordTracer::GetResult()
{
    if (m_executionRecord)
    {
        return m_executionRecord->toJson();
    }
    
    // Fallback result if no record available
    mcp::json ret = mcp::json::object();
    ret["gas"] = m_res ? m_res->gasUsed.convert_to<uint64_t>() : 0;
    ret["failed"] = m_res ? m_res->Failed() : false;
    ret["returnValue"] = m_res ? toHex(m_res->output) : "0x";
    ret["structLogs"] = mcp::json::array();
    return ret;
}

mcp::ExecutionRecordTracer::Options mcp::ExecutionRecordTracer::parseOptions(mcp::json const& _json)
{
    Options options;
    
    if (!_json.is_object() || _json.empty())
        return options;
    
    if (_json.count("enableMemory") && !_json["enableMemory"].empty())
        options.enableMemory = _json["enableMemory"].get<bool>();
    
    if (_json.count("enableStorage") && !_json["enableStorage"].empty())
        options.enableStorage = _json["enableStorage"].get<bool>();
    
    if (_json.count("enableFullTrace") && !_json["enableFullTrace"].empty())
        options.enableFullTrace = _json["enableFullTrace"].get<bool>();
    
    if (_json.count("stepLimit") && !_json["stepLimit"].empty())
        options.stepLimit = _json["stepLimit"].get<int>();
    
    // Also support legacy option names for compatibility
    if (_json.count("disableStorage") && !_json["disableStorage"].empty())
        options.enableStorage = !_json["disableStorage"].get<bool>();
    
    return options;
}