#include "OpCode.hpp"
#include <libinterpreter/VM.h>
#include <mcp/node/evm/ExtVM.h>
#include <algorithm>
#include <sstream>

using namespace dev::eth;
namespace
{
        std::string toCompactHexFromIntx(intx::uint256 value)
        {
                std::ostringstream out;
                out << "0x";

                if (value == 0)
                {
                        out << '0';
                        return out.str();
                }

                std::string hex;
                hex.reserve(64);

                while (value != 0)
                {
                        auto nibble = static_cast<unsigned>(value & intx::uint256{0xF});
                        hex.push_back(nibble < 10 ? static_cast<char>('0' + nibble)
                                                  : static_cast<char>('a' + (nibble - 10)));
                        value >>= 4;
                }

                if (hex.size() % 2 != 0)
                        hex.push_back('0');

                std::reverse(hex.begin(), hex.end());
                out << hex;
                return out.str();
        }
}

void mcp::OpCode::CaptureState(uint64_t PC, dev::eth::Instruction inst,
        uint64_t gasCost, uint64_t gas, dev::eth::VMFace const* /*_vm*/, dev::eth::ExtVMFace const* voidExt)
{
        // check if already accumulated the specified number of logs
        if (m_options.limit != 0 && m_options.limit <= m_outValue.size())
                return;

        auto interpreterVm = m_currentInterpreterVm;
        auto const* ext = dynamic_cast<ExtVM const*>(voidExt);

        mcp::json r = mcp::json::object();

        r["pc"] = PC;
        r["op"] = instructionInfo(inst).name;
        r["gas"] = gas;
        r["gasCost"] = gasCost;
        r["depth"] = ext ? (ext->depth + 1) : 0;  // depth in standard trace is 1-based

        mcp::json stack = mcp::json::array();
        if (interpreterVm && !m_options.disableStack)
        {
                for (auto const& value : interpreterVm->stackIntx())
                        stack.push_back(toCompactHexFromIntx(value));

                r["stack"] = stack;
        }

        if (interpreterVm && !m_options.disableMemory)
        {
                bytes const& memory = interpreterVm->memory();

                mcp::json memJson(mcp::json::array());
                for (unsigned i = 0; i < memory.size(); i += 32)
                {
                        bytesConstRef memRef(memory.data() + i, std::min<unsigned>(32, memory.size() - i));
                        memJson.push_back("0x" + toHex(memRef));
                }
                if (!memJson.empty())
                        r["memory"] = memJson;
        }

        if (!m_options.disableStorage && ext &&
                (inst == Instruction::SLOAD || inst == Instruction::SSTORE))
        {
                mcp::json storage(mcp::json::object());
                for (auto const& i : ext->state().storage(ext->myAddress))
                        storage[toCompactHex(i.second.first, 32)] =
                        toCompactHex(i.second.second, 32);
                r["storage"] = storage;
        }

        m_outValue.push_back(r);
}

mcp::json mcp::OpCode::GetResult()
{
        mcp::json ret;
        ret["gas"] = m_res ? m_res->gasUsed.convert_to<uint64_t>() : 0;
        ret["failed"] = m_res ? m_res->Failed() : true;
        ret["returnValue"] = std::string("0x") + toHex(m_res->output);
        ret["structLogs"] = m_outValue;
        return ret;
}

mcp::OpCode::DebugOptions mcp::OpCode::debugOptions(mcp::json const& _json)
{
        mcp::OpCode::DebugOptions op;
        if (!_json.is_object() || _json.empty())
                return op;

        mcp::json const* options = &_json;
        if (_json.count("tracerConfig") && _json["tracerConfig"].is_object())
                options = &_json["tracerConfig"];

        if (options->count("disableMemory") && !(*options)["disableMemory"].empty())
                op.disableMemory = (*options)["disableMemory"].get<bool>();
        if (options->count("disableStorage") && !(*options)["disableStorage"].empty())
                op.disableStorage = (*options)["disableStorage"].get<bool>();
        if (options->count("disableStack") && !(*options)["disableStack"].empty())
                op.disableStack = (*options)["disableStack"].get<bool>();

        // Backwards compatibility with older "enableMemory" flag.
        if (options->count("enableMemory") && !(*options)["enableMemory"].empty())
                op.disableMemory = !(*options)["enableMemory"].get<bool>();

        if (options->count("debug") && !(*options)["debug"].empty())
                op.debug = (*options)["debug"].get<bool>();
        if (options->count("limit") && !(*options)["limit"].empty())
                op.limit = (*options)["limit"].get<int>();
        return op;
}
