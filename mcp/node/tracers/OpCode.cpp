#include "OpCode.hpp"
#include <libevm/LegacyVM.h>
#include <libinterpreter/VM.h>
#include <mcp/node/evm/ExtVM.h>

using namespace dev::eth;
void mcp::OpCode::CaptureState(uint64_t PC, dev::eth::Instruction inst,
        uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* _vm, dev::eth::ExtVMFace const* voidExt)
{
        auto const& options = debugOptions();

        // check if already accumulated the specified number of logs
        if (options.limit != 0 && options.limit <= m_outValue.size())
                return;

        ExtVM const& ext = dynamic_cast<ExtVM const&>(*voidExt);
        auto vmLegacy = dynamic_cast<LegacyVM const*>(_vm);
        u256s stackData;
        if (vmLegacy)
                stackData = vmLegacy->stack();
        else if (auto interpreter = currentVM())
        {
                auto stackIntx = interpreter->stackIntx();
                stackData.reserve(stackIntx.size());
                for (auto const& word : stackIntx)
                        stackData.emplace_back(intxToU256(word));
        }

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
        if (!options.disableStack && (vmLegacy || currentVM()))
        {
                //mcp::log m_log = { mcp::log("vm") };
                // Try extracting information about the stack from the VM is supported.
                for (auto const& i : stackData)
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

        if (vmLegacy || currentVM())
        {
                bytes const& memory = vmLegacy ? vmLegacy->memory() : currentVM()->memory();

                mcp::json memJson(mcp::json::array());
                if (options.enableMemory)
                {
                        for (unsigned i = 0; i < memory.size(); i += 32)
                        {
                                bytesConstRef memRef(memory.data() + i, 32);
                                memJson.push_back(toHex(memRef));
                        }
                        r["memory"] = memJson;
                }
                r["memSize"] = static_cast<uint64_t>(memory.size());
        }

        if (!options.disableStorage &&
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

mcp::json mcp::OpCode::GetResult()
{
	mcp::json ret;
	ret["gas"] = m_res->gasUsed.convert_to<uint64_t>()/*toJS(t.gas())*/;
	ret["failed"] = m_res->Failed();
	ret["returnValue"] = toHex(m_res->output);
	ret["structLogs"] = m_outValue;
	return ret;
}
