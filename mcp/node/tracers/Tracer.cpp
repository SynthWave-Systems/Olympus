#include "Tracer.hpp"
#include "OpCode.hpp"
#include "4byte.hpp"
#include "Call.hpp"
#include "PreState.hpp"

using namespace mcp;

std::shared_ptr<Tracer> mcp::NewTracer(mcp::json const& _param, mcp::ExecutionResult& _er)
{
    std::string tracerName;
    
    // Handle both formats: "tracer": "tracerName" and "tracer": {"name": "tracerName"}
    if (_param.count("tracer") && !_param["tracer"].empty())
    {
        if (_param["tracer"].is_string())
        {
            tracerName = _param["tracer"].get<std::string>();
        }
        else if (_param["tracer"].is_object() && _param["tracer"].count("name"))
        {
            tracerName = _param["tracer"]["name"].get<std::string>();
        }
    }
    
    if (!tracerName.empty())
    {
        if (tracerName == "noopTracer")
            return std::make_shared<Tracer>();
        else if (tracerName == "4byteTracer")
            return std::make_shared<FourByteTracer>();
        else if (tracerName == "callTracer")
        {
            if (_param.count("tracerConfig"))
                return std::make_shared<CallTracer>(_er, _param["tracerConfig"]);
            else
                return std::make_shared<CallTracer>(_er);
        }
        else if (tracerName == "prestateTracer")
        {
            if (_param.count("tracerConfig"))
                return std::make_shared<PreStateTracer>(_er, _param["tracerConfig"]);
            else
                return std::make_shared<PreStateTracer>(_er);
        }
        // Note: "opTracer" is not a valid tracer name, fall through to default
    }

    // Default to OpCode tracer (struct logs)
    return std::make_shared<OpCode>(_er, _param);
}
