#pragma once
#include "Tracer.hpp"

namespace mcp
{
    /**
     * @brief Tracer that captures detailed execution records and makes them available to custom tracers
     * 
     * This tracer creates a comprehensive OPExecutionRecord containing all execution steps,
     * which can then be accessed by custom tracers for detailed analysis.
     */
    class ExecutionRecordTracer : public Tracer
    {
    public:
        struct Options
        {
            bool enableMemory = false;      ///< Whether to capture memory state
            bool enableStorage = true;      ///< Whether to capture storage state
            bool enableFullTrace = true;    ///< Whether to capture full execution trace
            int stepLimit = 0;              ///< Maximum number of steps to record (0 = unlimited)
        };

        explicit ExecutionRecordTracer(mcp::ExecutionResult& _er, mcp::json const& _param = mcp::json()) noexcept;

        void CaptureTxStart(uint64_t _gasLimit) override;
        void CaptureTxEnd(uint64_t _restGas) override;

        void CaptureStart(dev::eth::ExtVMFace const* _voidExt, dev::Address const& _from, dev::Address const& _to,
            bool _create, dev::bytes const& _input, uint64_t _gas, dev::u256 _value) override;
        void CaptureEnd(dev::bytes const& _output, uint64_t _gasUsed, mcp::TransactionException const _excepted) override;

        void CaptureState(uint64_t PC, dev::eth::Instruction inst,
            uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) override;

        mcp::json GetResult() override;

    private:
        ExecutionResult* m_res;
        Options m_options;
        uint64_t m_txGasLimit;

        /**
         * @brief Parse options from JSON configuration
         * @param _json JSON configuration object
         * @return Parsed options
         */
        Options parseOptions(mcp::json const& _json);
    };
}