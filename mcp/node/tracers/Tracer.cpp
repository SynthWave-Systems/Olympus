#include "Tracer.hpp"
#include "OpCode.hpp"
#include "4byte.hpp"
#include "Call.hpp"
#include "PreState.hpp"

using namespace mcp;

std::shared_ptr<Tracer> mcp::NewTracer(mcp::json const& _param, mcp::ExecutionResult& _er)
{
    // Check if a specific tracer type is requested
    if (_param.count("tracer") && !_param["tracer"].empty())
    {
        std::string tracerType = _param["tracer"].get<std::string>();
        
        if (tracerType == "noopTracer")
            return std::make_shared<Tracer>();
        else if (tracerType == "4byteTracer")
            return std::make_shared<FourByteTracer>();
        else if (tracerType == "callTracer")
        {
            if (_param.count("tracerConfig"))
                return std::make_shared<CallTracer>(_er, _param["tracerConfig"]);
            else
                return std::make_shared<CallTracer>(_er);
        }
        else if (tracerType == "prestateTracer")
        {
            if (_param.count("tracerConfig"))
                return std::make_shared<PreStateTracer>(_er, _param["tracerConfig"]);
            else
                return std::make_shared<PreStateTracer>(_er);
        }
        else if (tracerType == "opCodeTracer")
        {
            if (_param.count("tracerConfig"))
                return std::make_shared<OpCode>(_er, _param["tracerConfig"]);
            else
                return std::make_shared<OpCode>(_er);
        }
        // If requested tracer type is unknown, fall back to OpCode tracer
    }

    // Default to OpCode tracer (structured logs like geth's debug_traceTransaction)
    return std::make_shared<OpCode>(_er, _param);
}
