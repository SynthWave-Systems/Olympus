#pragma once
#include "Tracer.hpp"

namespace mcp
{
        class OpCode: public Tracer
        {
        public:
                explicit OpCode(mcp::ExecutionResult& _er, mcp::json const& _param = mcp::json()) noexcept :
                        //Tracer(_er),
                        m_res{ &_er }
                {
                        SetDebugOptions(_param);
                }

		void CaptureState(uint64_t PC, dev::eth::Instruction inst,
			uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) override;

		mcp::json GetResult() override;

        private:
                mcp::json m_outValue{ mcp::json::array() };
                ExecutionResult* m_res = nullptr;
        };
}
