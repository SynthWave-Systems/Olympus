#include "Tracer.hpp"
#include "OpCode.hpp"
#include "4byte.hpp"
#include "Call.hpp"
#include "PreState.hpp"

#include <algorithm>
#include <cctype>
#include <string>

using namespace mcp;

namespace
{
        std::string toLowerCopy(std::string value)
        {
                std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
                return value;
        }

        std::shared_ptr<Tracer> createTracerByName(std::string const& tracerName, mcp::ExecutionResult& er, mcp::json const& config)
        {
                auto normalized = toLowerCopy(tracerName);

                if (normalized == "noop" || normalized == "nooptracer")
                        return std::make_shared<Tracer>();

                if (normalized == "4byte" || normalized == "4bytetracer")
                        return std::make_shared<FourByteTracer>();

                if (normalized == "call" || normalized == "calltracer")
                {
                        if (config.is_null() || config.empty())
                                return std::make_shared<CallTracer>(er);
                        return std::make_shared<CallTracer>(er, config);
                }

                if (normalized == "prestate" || normalized == "prestatetracer")
                {
                        if (config.is_null() || config.empty())
                                return std::make_shared<PreStateTracer>(er);
                        return std::make_shared<PreStateTracer>(er, config);
                }

                if (normalized == "opcode" || normalized == "opcodetracer" || normalized == "optracer" || normalized == "structlogger" || normalized == "structlogs")
                        return std::make_shared<OpCode>(er, config);

                // Default tracer falls back to opcode tracing while preserving any configuration
                return std::make_shared<OpCode>(er, config);
        }

        template <class Func>
        void dispatchToAll(std::vector<TracerMultiplexer::NamedTracer>& tracers, Func&& fn)
        {
                for (auto& entry : tracers)
                        fn(entry.second.get());
        }
}

void TracerMultiplexer::CaptureTxStart(uint64_t _gasLimit)
{
        dispatchToAll(m_tracers, [&](Tracer* tracer) { tracer->CaptureTxStart(_gasLimit); });
}

void TracerMultiplexer::CaptureTxEnd(uint64_t _restGas)
{
        dispatchToAll(m_tracers, [&](Tracer* tracer) { tracer->CaptureTxEnd(_restGas); });
}

void TracerMultiplexer::CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to,
        bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value)
{
        dispatchToAll(m_tracers, [&](Tracer* tracer) { tracer->CaptureStart(_voidExt, _from, _to, _create, _input, _gas, _value); });
}

void TracerMultiplexer::CaptureEnd(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted)
{
        dispatchToAll(m_tracers, [&](Tracer* tracer) { tracer->CaptureEnd(_output, _gasUsed, _excepted); });
}

void TracerMultiplexer::CaptureEnter(dev::eth::Instruction _inst, dev::Address const& _from, dev::Address const& _to,
        dev::bytes const& _input, uint64_t _gas, std::shared_ptr<dev::u256> _value)
{
        dispatchToAll(m_tracers, [&](Tracer* tracer) { tracer->CaptureEnter(_inst, _from, _to, _input, _gas, _value); });
}

void TracerMultiplexer::CaptureExit(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted)
{
        dispatchToAll(m_tracers, [&](Tracer* tracer) { tracer->CaptureExit(_output, _gasUsed, _excepted); });
}

void TracerMultiplexer::CaptureState(uint64_t PC, dev::eth::Instruction inst,
        uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt)
{
        dispatchToAll(m_tracers, [&](Tracer* tracer) { tracer->CaptureState(PC, inst, gasCost, gas, _vm, voidExt); });
}

void TracerMultiplexer::CaptureFault(uint64_t _PC, dev::eth::Instruction _inst,
        uint64_t _gasCost, uint64_t _gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* _voidExt)
{
        dispatchToAll(m_tracers, [&](Tracer* tracer) { tracer->CaptureFault(_PC, _inst, _gasCost, _gas, _vm, _voidExt); });
}

mcp::json TracerMultiplexer::GetResult()
{
        if (m_tracers.empty())
                return mcp::json::object();

        mcp::json result = mcp::json::object();
        for (auto const& entry : m_tracers)
        {
                result[entry.first] = entry.second->GetResult();
        }
        return result;
}

std::shared_ptr<Tracer> mcp::NewTracer(mcp::json const& _param, mcp::ExecutionResult& _er)
{
        if (_param.is_object() && _param.count("tracers"))
        {
                auto const& tracerArray = _param["tracers"];
                if (tracerArray.is_array() && !tracerArray.empty())
                {
                        std::vector<TracerMultiplexer::NamedTracer> tracers;
                        tracers.reserve(tracerArray.size());

                        for (auto const& definition : tracerArray)
                        {
                                std::string tracerName;
                                std::string resultKey;
                                mcp::json config;

                                if (definition.is_string())
                                {
                                        tracerName = definition.get<std::string>();
                                        resultKey = tracerName;
                                }
                                else if (definition.is_object())
                                {
                                        if (definition.count("tracer") && definition["tracer"].is_string())
                                                tracerName = definition["tracer"].get<std::string>();
                                        else if (definition.count("type") && definition["type"].is_string())
                                                tracerName = definition["type"].get<std::string>();

                                        if (definition.count("config"))
                                                config = definition["config"];
                                        else if (definition.count("tracerConfig"))
                                                config = definition["tracerConfig"];

                                        if (definition.count("name") && definition["name"].is_string())
                                                resultKey = definition["name"].get<std::string>();
                                        else if (!tracerName.empty())
                                                resultKey = tracerName;
                                }

                                if (tracerName.empty())
                                        continue;

                                auto tracer = createTracerByName(tracerName, _er, config);
                                if (!tracer)
                                        continue;

                                if (resultKey.empty())
                                        resultKey = tracerName;

                                tracers.emplace_back(resultKey, tracer);
                        }

                        if (!tracers.empty())
                                return std::make_shared<TracerMultiplexer>(std::move(tracers));
                }
        }

        if (_param.count("tracer") && !_param["tracer"].empty())
        {
                mcp::json config;
                if (_param.count("tracerConfig"))
                        config = _param["tracerConfig"];

                return createTracerByName(_param["tracer"], _er, config);
        }

        return std::make_shared<OpCode>(_er, _param);
}
