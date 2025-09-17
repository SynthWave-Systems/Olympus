#include "Tracer.hpp"
#include "CallTracer.hpp"
#include "OpcodeTracer.hpp"

namespace mcp
{

std::shared_ptr<Tracer> CreateTracer(std::string const& tracerName, mcp::json const& config)
{
    std::shared_ptr<Tracer> tracer;
    
    if (tracerName == "callTracer") {
        tracer = std::make_shared<CallTracer>();
    }
    else if (tracerName == "opcodeTracer" || tracerName == "structLogs") {
        tracer = std::make_shared<OpcodeTracer>();
    }
    else {
        // Default to opcode tracer for backward compatibility
        tracer = std::make_shared<OpcodeTracer>();
    }
    
    if (tracer) {
        tracer->SetConfig(config);
    }
    
    return tracer;
}

}  // namespace mcp