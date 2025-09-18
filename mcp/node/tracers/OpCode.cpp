#include "OpCode.hpp"
#include <mcp/node/evm/ExtVM.h>
#include <libinterpreter/VM.h>
#include <intx/intx.hpp>

using namespace dev::eth;
void mcp::OpCode::CaptureState(uint64_t PC, dev::eth::Instruction inst,
	uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt)
{
	// check if already accumulated the specified number of logs
	if (m_options.limit != 0 && m_options.limit <= m_outValue.size())
		return;

	ExtVM const& ext = dynamic_cast<ExtVM const&>(*voidExt);
	
	mcp::json r = mcp::json::object();

	r["pc"] = PC;
	r["op"] = instructionInfo(inst).name;
	r["gas"] = gas;
	r["gasCost"] = gasCost;
	r["depth"] = ext.depth + 1;  // depth in standard trace is 1-based

	// Try to get stack/memory from libinterpreter VM via global reference
	extern const dev::eth::VM* g_currentInterpreterVM;
	
	// Handle stack capture - use libinterpreter VM directly
	mcp::json stack = mcp::json::array();
	if (!m_options.disableStack && g_currentInterpreterVM)
	{
		// Access stack from libinterpreter VM
		auto stackPtr = g_currentInterpreterVM->getStackPointer();
		auto stackEnd = g_currentInterpreterVM->getStackEnd();
		size_t stackSize = g_currentInterpreterVM->getStackSize();
		
		// Stack grows from high address to low address
		for (size_t i = 0; i < stackSize; ++i) {
			auto& stackItem = stackPtr[i];
			// Convert intx::uint256 to hex string using intx::hex
			std::string hexValue = "0x" + intx::hex(stackItem);
			stack.push_back(hexValue);
		}
	}
	r["stack"] = stack;

	// Handle memory capture - use libinterpreter VM directly
	mcp::json memJson(mcp::json::array());
	if (m_options.enableMemory && g_currentInterpreterVM)
	{
		bytes const& memory = g_currentInterpreterVM->getMemory();
		for (unsigned i = 0; i < memory.size(); i += 32)
		{
			bytesConstRef memRef(memory.data() + i, 32);
			memJson.push_back(toHex(memRef));
		}
	}
	r["memory"] = memJson;

	// Storage capture - this works with ExtVM regardless of VM type
	if (!m_options.disableStorage &&
		(inst == Instruction::SLOAD || inst == Instruction::SSTORE))
	{
		mcp::json storage(mcp::json::object());
		for (auto const& i : ext.state().storage(ext.myAddress))
			storage[toCompactHex(i.second.first, 32)] =
			toCompactHex(i.second.second, 32);
		r["storage"] = storage;
	}

	m_outValue.push_back(r);
}

mcp::json mcp::OpCode::GetResult()
{
	mcp::json ret;
	ret["gas"] = m_res->gasUsed.convert_to<uint64_t>()/*toJS(t.gas())*/;
	ret["failed"] = m_res->Failed();
	ret["returnValue"] = toHex(m_res->output);
	ret["structLogs"] = m_outValue;
	return ret;
}

mcp::OpCode::DebugOptions mcp::OpCode::debugOptions(mcp::json const& _json)
{
	mcp::OpCode::DebugOptions op;
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
