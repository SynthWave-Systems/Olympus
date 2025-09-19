#include "Tracer.hpp"

#include <vector>

namespace mcp
{
namespace tracing
{
namespace
{
thread_local Tracer* s_currentTracer = nullptr;
thread_local std::vector<ExecutionContext> s_contextStack;
}

Tracer* TracerManager::setTracer(Tracer* tracer)
{
    Tracer* previous = s_currentTracer;
    if (s_currentTracer != tracer)
    {
        s_contextStack.clear();
        s_currentTracer = tracer;
    }
    return previous;
}

Tracer* TracerManager::currentTracer()
{
    return s_currentTracer;
}

bool TracerManager::pushContext(dev::eth::ExtVMFace const* ext, unsigned depth)
{
    auto* tracer = s_currentTracer;
    if (!tracer || !ext)
        return false;

    s_contextStack.push_back({ext, depth});
    tracer->onExecutionStart(s_contextStack.back());
    return true;
}

void TracerManager::popContext()
{
    if (!s_currentTracer || s_contextStack.empty())
        return;

    auto ctx = s_contextStack.back();
    s_currentTracer->onExecutionEnd(ctx);
    s_contextStack.pop_back();
}

ExecutionContext const* TracerManager::currentContext()
{
    if (s_contextStack.empty())
        return nullptr;
    return &s_contextStack.back();
}

bool TracerManager::tracerNeedsStack()
{
    auto* tracer = s_currentTracer;
    return tracer && tracer->needsStack();
}

bool TracerManager::tracerNeedsMemory()
{
    auto* tracer = s_currentTracer;
    return tracer && tracer->needsMemory();
}

ScopedTracer::ScopedTracer(Tracer* tracer)
  : m_previous(TracerManager::setTracer(tracer))
{
}

ScopedTracer::~ScopedTracer()
{
    TracerManager::setTracer(m_previous);
}

ContextGuard::ContextGuard(dev::eth::ExtVMFace const* ext, unsigned depth)
{
    m_active = TracerManager::pushContext(ext, depth);
}

ContextGuard::~ContextGuard()
{
    if (m_active)
        TracerManager::popContext();
}

}  // namespace tracing
}  // namespace mcp

