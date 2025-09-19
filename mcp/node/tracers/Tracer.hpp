#pragma once

#include <libdevcore/Common.h>
#include <libevm/ExtVMFace.h>
#include <libevm/Instruction.h>

#include <cstdint>
#include <vector>

#include <intx/intx.hpp>

namespace mcp
{
namespace tracing
{
struct MemoryView
{
    const uint8_t* data = nullptr;
    size_t size = 0;
};

struct OperationState
{
    uint64_t step = 0;
    uint64_t pc = 0;
    dev::eth::Instruction opcode = dev::eth::Instruction::STOP;
    dev::bigint gasCost = 0;
    dev::bigint gasLeft = 0;
    dev::bigint newMemorySize = 0;
    MemoryView memory;
    intx::uint256 const* stackTop = nullptr;
    size_t stackSize = 0;
};

struct ExecutionContext
{
    dev::eth::ExtVMFace const* ext = nullptr;
    unsigned depth = 0;
};

class Tracer
{
public:
    virtual ~Tracer() = default;

    virtual bool needsStack() const { return true; }
    virtual bool needsMemory() const { return true; }

    virtual void onExecutionStart(ExecutionContext const&) {}
    virtual void onExecutionEnd(ExecutionContext const&) {}

    virtual void captureState(OperationState const&, ExecutionContext const&) = 0;
};

class TracerManager
{
public:
    static Tracer* setTracer(Tracer* tracer);
    static Tracer* currentTracer();

    static bool pushContext(dev::eth::ExtVMFace const* ext, unsigned depth);
    static void popContext();
    static ExecutionContext const* currentContext();

    static bool tracerNeedsStack();
    static bool tracerNeedsMemory();
};

class ScopedTracer
{
public:
    explicit ScopedTracer(Tracer* tracer);
    ~ScopedTracer();

    ScopedTracer(ScopedTracer const&) = delete;
    ScopedTracer& operator=(ScopedTracer const&) = delete;

private:
    Tracer* m_previous;
};

class ContextGuard
{
public:
    ContextGuard(dev::eth::ExtVMFace const* ext, unsigned depth);
    ~ContextGuard();

    ContextGuard(ContextGuard const&) = delete;
    ContextGuard& operator=(ContextGuard const&) = delete;

private:
    bool m_active = false;
};

}  // namespace tracing
}  // namespace mcp

