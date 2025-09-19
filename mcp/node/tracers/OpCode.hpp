#pragma once
#include "Tracer.hpp"
#include <libinterpreter/VM.h>

namespace mcp
{
	class OpCodeTracer: public Tracer
	{
	public:
		struct DebugOptions
		{
			bool enableMemory = false;// enable memory capture
			bool disableStorage = false;// disable stack capture
			//bool disableMemory = false;
			bool disableStack = false;// disable storage capture
			//bool fullStorage = false;
			bool debug = false; // print output during capture end. for expand.
			int limit = 0;// maximum length of output, but zero means unlimited
		};

		explicit OpCodeTracer(mcp::ExecutionResult& _er, mcp::json const& _param = mcp::json()) noexcept :
			m_res{ &_er },
			m_options(debugOptions(_param)) {}

		// Primary method for capturing opcode execution state
		void CaptureState(uint64_t PC, dev::eth::Instruction inst,
			uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt) override;

		// Enhanced method for direct VM integration - captures execution state with VM context
		void CaptureOpcodeExecution(uint64_t pc, dev::eth::Instruction op, const std::string& opName, const dev::eth::VM* vm, dev::eth::ExtVMFace const* ext);

		mcp::json GetResult() override;

		// Create a callback function that can be used with VM's setOpcodeLogCallback
		dev::eth::OpcodeLogCallback CreateCallback(dev::eth::ExtVMFace const* ext);

	private:
		OpCodeTracer::DebugOptions debugOptions(mcp::json const& _json);
		DebugOptions m_options;
		mcp::json m_outValue{ mcp::json::array() };
		ExecutionResult* m_res = nullptr;
	};
}