#include "working.hpp"
#include <boost/dll/runtime_symbol_info.hpp>

boost::filesystem::path mcp::app_path() {
    auto p = boost::dll::program_location();
    return p.parent_path();
}