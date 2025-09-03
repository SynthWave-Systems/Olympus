#include "Tracer.hpp"
#include "OpCode.hpp"
#include "4byte.hpp"
#include "Call.hpp"
#include "PreState.hpp"
#include "ExecutionRecordTracer.hpp"

using namespace mcp;

std::shared_ptr<Tracer> mcp::NewTracer(mcp::json const& _param, mcp::ExecutionResult& _er)
{
    if (_param.count("tracer") && !_param["tracer"].empty())
    {
        if (_param["tracer"] == "noopTracer")
            return std::make_shared<Tracer>();
        else if (_param["tracer"] == "4byteTracer")
            return std::make_shared<FourByteTracer>();
        else if (_param["tracer"] == "callTracer")
        {
            if (_param.count("tracerConfig"))
                return std::make_shared<CallTracer>(_er, _param["tracerConfig"]);
            else
                return std::make_shared<CallTracer>(_er);
        }
        else if (_param["tracer"] == "prestateTracer")
        {
            if (_param.count("tracerConfig"))
                return std::make_shared<PreStateTracer>(_er, _param["tracerConfig"]);
            else
                return std::make_shared<PreStateTracer>(_er);
        }
        else if (_param["tracer"] == "executionRecordTracer")
        {
            if (_param.count("tracerConfig"))
                return std::make_shared<ExecutionRecordTracer>(_er, _param["tracerConfig"]);
            else
                return std::make_shared<ExecutionRecordTracer>(_er);
        }
        else if (_param["tracer"] == "OpTracer")
        {
            // OpTracer is an alias for the default opcode tracer
            return std::make_shared<OpCode>(_er, _param.count("tracerConfig") ? _param["tracerConfig"] : _param);
        }
            
    }

    return std::make_shared<OpCode>(_er, _param);
}
