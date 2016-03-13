//---------------------------------------------------------------------------
//
//    Copyright (C) 2008 - 2016 Ilya Golovenko
//    This file is part of Chat.Daemon project
//
//    spchatd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spchatd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spchatd. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include <app/detail/linux/operating_system_info.hpp>

#if defined(CHAT_PLATFORM_LUNIX)

// Application headers
#include <misc/error_utils.hpp>

// MISSIO headers
#include <missio/format/write.hpp>

// STL headers
#include <stdexcept>
#include <sstream>
#include <memory>

// Linux headers
#include <sys/utsname.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>

// CRT headers
#include <string.h>
#include <stdlib.h>
#include <errno.h>


namespace chat
{
namespace detail
{

std::string linux_operating_system_info::get_name()
{
    struct utsname name;

    if(::uname(&name) < 0)
        throw std::runtime_error(util::errno_to_string("uname", errno));

    std::string str;

    missio::format::write(str, name.sysname, " ", name.release, " ", name.version, " ", name.machine);

    return str;
}

std::time_t linux_operating_system_info::get_uptime()
{
    struct sysinfo info;

    if(::sysinfo(&info) < 0)
        throw std::runtime_error(util::errno_to_string("sysinfo", errno));

    return static_cast<std::time_t>(info.uptime);
}

std::array<double, 3> linux_operating_system_info::get_loadavg()
{
    std::array<double, 3> loadavg;

    int const nelem = ::getloadavg(loadavg.data(), 3);

    if(nelem < 3)
    {
        throw std::runtime_error(util::errno_to_string("getloadavg", errno));
    }

    return loadavg;
}

std::vector<symbol_info> linux_operating_system_info::get_backtrace()
{
    int const max_stack_size = 31;
    void* stack_frames[max_stack_size] = { nullptr };

    int const stack_size = ::backtrace(stack_frames, max_stack_size);

    if(stack_size < 0)
        throw std::runtime_error(util::errno_to_string("backtrace", errno));

    std::vector<symbol_info> stack_trace;

    for(int i = 1; i < stack_size; ++i)
    {
        symbol_info symbol_info;

        symbol_info.address = reinterpret_cast<std::uint64_t>(stack_frames[i]);

        if(char const* symbol = get_symbol_string(stack_frames[i]))
            symbol_info.name = demangle_symbol(symbol);

        stack_trace.push_back(symbol_info);
    }

    return stack_trace;
}

char const* linux_operating_system_info::get_symbol_string(void* address)
{
    Dl_info dl_info;

    if(nullptr != ::dladdr(address, &dl_info))
        return dl_info.dli_sname;

    return nullptr;
}

std::string linux_operating_system_info::demangle_symbol(char const* symbol)
{
    std::unique_ptr<char, decltype(&::free)> demangled(::abi::__cxa_demangle(symbol, 0, 0, 0), ::free);

    return demangled ? std::string(demangled.get()) : std::string(symbol);
}

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_LUNIX)
