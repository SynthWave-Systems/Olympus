#include "OpCode.hpp"
#include <libevm/LegacyVM.h>
#include <mcp/node/evm/ExtVM.h>

using namespace dev::eth;

void mcp::OpCodeTracer::CaptureState(uint64_t PC, dev::eth::Instruction inst,
	uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt)
{
	// check if already accumulated the specified number of logs
	if (m_options.limit != 0 && m_options.limit <= m_outValue.size())
		return;

	ExtVM const& ext = dynamic_cast<ExtVM const&>(*voidExt);
	auto vm = dynamic_cast<LegacyVM const*>(_vm);

	mcp::json r = mcp::json::object();

	r["pc"] = PC;
	r["op"] = instructionInfo(inst).name;
	//r["op"] = static_cast<uint8_t>(inst);
	//if (m_showMnemonics)
	//	r["opName"] = instructionInfo(inst).name;
	r["gas"] = gas/*.convert_to<uint64_t>()*//*toString(gas)*/;
	r["gasCost"] = gasCost/*.convert_to<uint64_t>()*//*toString(gasCost)*/;
	r["depth"] = ext.depth + 1;  // depth in standard trace is 1-based
	//if (!!newMemSize)
	//	r["memexpand"] = toString(newMemSize);

	mcp::json stack = mcp::json::array();
	if (vm && !m_options.disableStack)
	{
		//mcp::log m_log = { mcp::log("vm") };
		// Try extracting information about the stack from the VM is supported.
		for (auto const& i : vm->stack())
		{
			//LOG(m_log.info) << i << " : " << toCompactHexPrefixed(i, 1);
			stack.push_back(toCompactHexPrefixedTrim(i));
		}

		r["stack"] = stack;
	}

	//bool newContext = false;
	//Instruction lastInst = Instruction::STOP;

	////assert_x(ext.depth > 0);
	//if (m_lastInst.size() == ext.depth /*- 1*/)
	//{
	//	// starting a new context
	//	assert(m_lastInst.size() == ext.depth/* - 1*/);
	//	m_lastInst.push_back(inst);
	//	newContext = true;
	//}
	//else if (m_lastInst.size() == ext.depth + 2)
	//{
	//	m_lastInst.pop_back();
	//	lastInst = m_lastInst.back();
	//}
	//else if (m_lastInst.size() == ext.depth + 1)
	//{
	//	// continuing in previous context
	//	lastInst = m_lastInst.back();
	//	m_lastInst.back() = inst;
	//}
	//else
	//{
	//	cwarn << "GAA!!! Tracing VM and more than one new/deleted stack frame between steps!";
	//	cwarn << "Attmepting naive recovery...";
	//	m_lastInst.resize(ext.depth + 1);
	//}

	if (vm)
	{
		bytes const& memory = vm->memory();

		mcp::json memJson(mcp::json::array());
		if (m_options.enableMemory)
		{
			for (unsigned i = 0; i < memory.size(); i += 32)
			{
				bytesConstRef memRef(memory.data() + i, 32);
				memJson.push_back(toHex(memRef));
			}
			r["memory"] = memJson;
		}
		//r["memSize"] = static_cast<uint64_t>(memory.size());
	}

	if (!m_options.disableStorage &&
		(inst == Instruction::SLOAD || inst == Instruction::SSTORE)
		/*(m_options.fullStorage || changesStorage(lastInst) || newContext)*/)
	{
		mcp::json storage(mcp::json::object());
		for (auto const& i : ext.state().storage(ext.myAddress))
			storage[toCompactHex(i.second.first, 32)] =
			toCompactHex(i.second.second, 32);
		r["storage"] = storage;
	}

	m_outValue.push_back(r);
}

mcp::json mcp::OpCodeTracer::GetResult()
{
	mcp::json ret;
	ret["gas"] = m_res->gasUsed.convert_to<uint64_t>();
	ret["failed"] = m_res->Failed();
	ret["returnValue"] = toHex(m_res->output);
	ret["structLogs"] = m_outValue;
	return ret;
}

mcp::OpCodeTracer::DebugOptions mcp::OpCodeTracer::debugOptions(mcp::json const& _json)
{
	mcp::OpCodeTracer::DebugOptions op;
	if (!_json.is_object() || _json.empty())
		return op;
	if (_json.count("enableMemory") && !_json["enableMemory"].empty())
		op.enableMemory = _json["enableMemory"].get<bool>();
	if (_json.count("disableStorage") && !_json["disableStorage"].empty())
		op.disableStorage = _json["disableStorage"].get<bool>();
	if (_json.count("disableStack") && !_json["disableStack"].empty())
		op.disableStack = _json["disableStack"].get<bool>();
	//if (_json.count("full_storage") && !_json["full_storage"].empty())
	//	op.fullStorage = _json["full_storage"].get<bool>();
	if (_json.count("debug") && !_json["debug"].empty())
		op.debug = _json["debug"].get<bool>();
	if (_json.count("limit") && !_json["limit"].empty())
		op.limit = _json["limit"].get<int>();
	return op;
}

// Enhanced method for direct VM integration
void mcp::OpCodeTracer::CaptureOpcodeExecution(uint64_t pc, dev::eth::Instruction op, const std::string& opName, const dev::eth::VM* vm, dev::eth::ExtVMFace const* ext)
{
	// check if already accumulated the specified number of logs
	if (m_options.limit != 0 && m_options.limit <= m_outValue.size())
		return;

	if (!ext)
		return;

	ExtVM const& extVM = dynamic_cast<ExtVM const&>(*ext);

	mcp::json r = mcp::json::object();

	r["pc"] = pc;
	r["op"] = opName;
	r["depth"] = extVM.depth + 1;  // depth in standard trace is 1-based
	
	// For direct VM integration, we may not have gas information readily available
	// This is a limitation of the direct approach, but we can set reasonable defaults
	r["gas"] = static_cast<uint64_t>(0); // TODO: Extract from VM state if possible
	r["gasCost"] = static_cast<uint64_t>(0); // TODO: Extract from VM state if possible

	// Try to extract additional state information if VM is accessible
	// Note: The libinterpreter VM might not expose all the same interfaces as LegacyVM
	// For now, we'll focus on basic opcode tracking and can enhance later
	
	// TODO: If we can access VM internals, add:
	// - Stack information
	// - Memory information (if enableMemory is true)
	// - Storage information (if not disableStorage and relevant opcodes)
	
	// Basic implementation - captures opcode execution without detailed VM state
	// This provides the core transaction execution tracing capability
	
	m_outValue.push_back(r);
}

// Create a callback function that can be used with VM's setOpcodeLogCallback
dev::eth::OpcodeLogCallback mcp::OpCodeTracer::CreateCallback(dev::eth::ExtVMFace const* ext)
{
	return [this, ext](uint64_t pc, dev::eth::Instruction op, const std::string& opName, const dev::eth::VM* vm) {
		this->CaptureOpcodeExecution(pc, op, opName, vm, ext);
	};
}
