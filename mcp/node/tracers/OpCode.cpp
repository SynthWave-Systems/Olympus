#include "OpCode.hpp"
#include <libinterpreter/VM.h>
#include <mcp/node/evm/ExtVM.h>
#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>

using namespace dev::eth;
namespace
{
        std::string toCompactHexFromIntx(intx::uint256 const& value)
        {
                auto bytes = intx::be::store<std::array<uint8_t, 32>>(value);
                size_t firstNonZero = 0;
                while (firstNonZero < bytes.size() && bytes[firstNonZero] == 0)
                        ++firstNonZero;

                std::ostringstream out;
                out << "0x";
                if (firstNonZero == bytes.size())
                {
                        out << '0';
                }
                else
                {
                        out << std::hex << std::nouppercase << std::setfill('0');
                        for (size_t i = firstNonZero; i < bytes.size(); ++i)
                                out << std::setw(2) << static_cast<unsigned>(bytes[i]);
                }
                return out.str();
        }
}

void mcp::OpCode::CaptureState(uint64_t PC, dev::eth::Instruction inst,
        uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* /*_vm*/, dev::eth::ExtVMFace const* voidExt)
{
        // check if already accumulated the specified number of logs
        if (m_options.limit != 0 && m_options.limit <= m_outValue.size())
                return;

        ExtVM const& ext = dynamic_cast<ExtVM const&>(*voidExt);
        auto interpreterVm = m_currentInterpreterVm;

        mcp::json r = mcp::json::object();

        r["pc"] = PC;
        r["op"] = instructionInfo(inst).name;
        r["gas"] = gas;
        r["gasCost"] = gasCost;
        r["depth"] = ext.depth + 1;  // depth in standard trace is 1-based

        mcp::json stack = mcp::json::array();
        if (interpreterVm && !m_options.disableStack)
        {
                for (auto const& value : interpreterVm->stackIntx())
                        stack.push_back(toCompactHexFromIntx(value));

                r["stack"] = stack;
        }

        if (interpreterVm)
        {
                bytes const& memory = interpreterVm->memory();

                if (m_options.enableMemory)
                {
                        mcp::json memJson(mcp::json::array());
                        for (unsigned i = 0; i < memory.size(); i += 32)
                        {
                                bytesConstRef memRef(memory.data() + i, std::min<unsigned>(32, memory.size() - i));
                                memJson.push_back(toHex(memRef));
                        }
                        r["memory"] = memJson;
                }
        }

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
        //if (_json.count("full_storage") && !_json["full_storage"].empty())
        //      op.fullStorage = _json["full_storage"].get<bool>();
        if (_json.count("disableStack") && !_json["disableStack"].empty())
                op.disableStack = _json["disableStack"].get<bool>();
        //if (_json.count("full_storage") && !_json["full_storage"].empty())
        //      op.fullStorage = _json["full_storage"].get<bool>();
        if (_json.count("debug") && !_json["debug"].empty())
                op.debug = _json["debug"].get<bool>();
        if (_json.count("limit") && !_json["limit"].empty())
                op.limit = _json["limit"].get<int>();
        return op;
}
