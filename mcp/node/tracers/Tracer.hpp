#pragma once
#include <libevm/ExtVMFace.h>
#include <libevm/VMFace.h>
#include <libevm/Logger.h>
#include <mcp/core/common.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace mcp
{
        class Tracer : public dev::eth::EVMLogger
        {
        public:
                using NamedTracer = std::pair<std::string, std::shared_ptr<Tracer>>;

                explicit Tracer() = default;

                void addTracer(std::string name, std::shared_ptr<Tracer> tracer);
                bool hasChildren() const { return !m_tracers.empty(); }
                size_t childCount() const { return m_tracers.size(); }

                void CaptureTxStart(uint64_t _gasLimit) override;
                void CaptureTxEnd(uint64_t _restGas) override;

                void CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to,
                        bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value) override;
                void CaptureEnd(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted) override;

                void CaptureEnter(dev::eth::Instruction _inst, dev::Address const& _from, dev::Address const& _to,
                        dev::bytes const& _input, uint64_t _gas, std::shared_ptr<dev::u256> _value) override;
                void CaptureExit(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted) override;

                void CaptureState(uint64_t PC, dev::eth::Instruction inst,
                        uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) override;
                void CaptureFault(uint64_t _PC, dev::eth::Instruction _inst,
                        uint64_t _gasCost, uint64_t _gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* _voidExt) override;

                virtual mcp::json GetResult();

        protected:
                std::vector<NamedTracer> const& children() const { return m_tracers; }

        private:
                std::vector<NamedTracer> m_tracers;
        };

        std::shared_ptr<Tracer> NewTracer(mcp::json const& _param, mcp::ExecutionResult& _er);
}