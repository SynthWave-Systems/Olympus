#pragma once
#include "Tracer.hpp"

namespace mcp
{
	class OpCode: public Tracer
	{
	public:
                struct DebugOptions
                {
                        bool disableMemory = false;   ///< Skip memory dumps when true.
                        bool disableStorage = false;  ///< Skip storage reads when true.
                        bool disableStack = false;    ///< Skip stack snapshots when true.
                        bool debug = false;           ///< Emit verbose logging (currently unused).
                        int limit = 0;                ///< Maximum number of struct logs (0 = unlimited).
                };

                explicit OpCode(mcp::ExecutionResult& _er, mcp::json const& _param = mcp::json()) noexcept :
                        //Tracer(_er),
                        m_res{ &_er },
                        m_options(debugOptions(_param)) {}

                void SetCurrentVM(dev::eth::VM const* _vm) override { m_currentInterpreterVm = _vm; }

                void CaptureState(uint64_t PC, dev::eth::Instruction inst,
                        uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) override;

                mcp::json GetResult() override;

        private:
                OpCode::DebugOptions debugOptions(mcp::json const& _json);
                DebugOptions m_options;
                mcp::json m_outValue{ mcp::json::array() };
                ExecutionResult* m_res = nullptr;
                dev::eth::VM const* m_currentInterpreterVm = nullptr;
        };
}
