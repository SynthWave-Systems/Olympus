#pragma once
#include <libevm/ExtVMFace.h>
#include <libevm/VMFace.h>
#include <libevm/Logger.h>
#include <mcp/core/common.hpp>
#include <intx/intx.hpp>
#include <limits>

namespace dev
{
namespace eth
{
        class VM;
}
}

namespace mcp
{
        class Tracer : public dev::eth::EVMLogger
        {
                friend class OpCode;

        public:
                struct DebugOptions
                {
                        bool enableMemory = false;      // enable memory capture
                        bool disableStorage = false;    // disable storage capture
                        bool disableStack = false;      // disable stack capture
                        bool debug = false;             // enable verbose debugging output
                        int limit = 0;                  // maximum length of output, zero means unlimited
                };

        public:
                explicit Tracer() {};

		void CaptureTxStart(uint64_t _gasLimit) override {}
		void CaptureTxEnd(uint64_t _restGas) override {}

		void CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to,
			bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value) override {}
		void CaptureEnd(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted) override {}

		void CaptureEnter(dev::eth::Instruction _inst, dev::Address const& _from, dev::Address const& _to, 
			dev::bytes const& _input, uint64_t _gas, std::shared_ptr<dev::u256> _value) override {}
		void CaptureExit(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted) override {}

                void CaptureState(uint64_t PC, dev::eth::Instruction inst,
                        uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) override {}
                void CaptureFault(uint64_t _PC, dev::eth::Instruction _inst,
                        uint64_t _gasCost, uint64_t _gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* _voidExt) override {}

                void SetCurrentVM(dev::eth::VM const* _vm) { m_currentVM = _vm; }

        protected:
                void SetDebugOptions(DebugOptions const& _options) { m_debugOptions = _options; }
                void SetDebugOptions(mcp::json const& _json) { m_debugOptions = parseDebugOptions(_json); }
                DebugOptions const& debugOptions() const { return m_debugOptions; }

                dev::eth::VM const* currentVM() const { return m_currentVM; }

                static DebugOptions parseDebugOptions(mcp::json const& _json)
                {
                        DebugOptions options;
                        if (!_json.is_object() || _json.empty())
                                return options;

                        if (_json.count("enableMemory") && !_json["enableMemory"].empty())
                                options.enableMemory = _json["enableMemory"].get<bool>();
                        if (_json.count("disableStorage") && !_json["disableStorage"].empty())
                                options.disableStorage = _json["disableStorage"].get<bool>();
                        if (_json.count("disableStack") && !_json["disableStack"].empty())
                                options.disableStack = _json["disableStack"].get<bool>();
                        if (_json.count("debug") && !_json["debug"].empty())
                                options.debug = _json["debug"].get<bool>();
                        if (_json.count("limit") && !_json["limit"].empty())
                                options.limit = _json["limit"].get<int>();

                        return options;
                }

                static dev::u256 intxToU256(intx::uint256 const& _value)
                {
                        dev::u256 result = 0;
                        intx::uint256 value = _value;
                        constexpr intx::uint256 mask = intx::uint256{ std::numeric_limits<uint64_t>::max() };
                        unsigned shift = 0;
                        while (value)
                        {
                                uint64_t chunk = static_cast<uint64_t>(value & mask);
                                result |= (dev::u256(chunk) << shift);
                                value >>= 64;
                                shift += 64;
                        }
                        return result;
                }

        public:
                virtual mcp::json GetResult() { return mcp::json::object(); }

        private:
                DebugOptions m_debugOptions;
                dev::eth::VM const* m_currentVM = nullptr;

        };

        std::shared_ptr<Tracer> NewTracer(mcp::json const& _param, mcp::ExecutionResult& _er);
}
