#pragma once

#include "Tracer.hpp"

#include <mcp/common/json.hpp>

namespace mcp
{
namespace tracing
{
class OPCodeTracer : public Tracer
{
public:
    struct Options
    {
        bool disableStorage = false;
        bool disableMemory = false;
        bool disableStack = false;
        bool fullStorage = false;
    };

    explicit OPCodeTracer(Options options = Options{});

    void setShowMnemonics(bool value) { m_showMnemonics = value; }

    static Options fromJson(mcp::json const& json);

    mcp::json const& logs() const { return m_trace; }

    bool needsStack() const override;
    bool needsMemory() const override;

    void captureState(OperationState const& state, ExecutionContext const& context) override;

private:
    Options m_options;
    bool m_showMnemonics = false;
    std::vector<dev::eth::Instruction> m_lastInst;
    mcp::json m_trace;
};

}  // namespace tracing
}  // namespace mcp

