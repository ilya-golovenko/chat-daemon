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
#include <app/detail/win/operating_system_info.hpp>

#if defined(CHAT_PLATFORM_WINDOWS)

// Application headers
#include <misc/win_utils.hpp>

// MISSIO headers
#include <missio/format/write.hpp>

// Windows headers
#include <windows.h>
#include <dbghelp.h>

// STL headers
#include <stdexcept>
#include <cstdlib>
#include <cstdint>
#include <string>


namespace chat
{
namespace detail
{

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

std::string win_operating_system_info::get_name()
{
    std::string os_name;
    OSVERSIONINFOA version_info;

    ::ZeroMemory(&version_info, sizeof(OSVERSIONINFOA));
    version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

    if(!::GetVersionExA(&version_info))
    {
        throw std::runtime_error(util::win::error_to_string("GetVersionEx", ::GetLastError()));
    }

    missio::format::write(os_name, "Windows ");

    if(VER_PLATFORM_WIN32_NT == version_info.dwPlatformId)
    {
        missio::format::write(os_name, "NT ");
    }

    missio::format::write(os_name, version_info.dwMajorVersion, ".", version_info.dwMinorVersion);

    if(std::strlen(version_info.szCSDVersion))
    {
        missio::format::write(os_name, " ", version_info.szCSDVersion);
    }

    return os_name;
}

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma warning(pop)
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

std::time_t win_operating_system_info::get_uptime()
{
    return static_cast<std::time_t>(::GetTickCount64() / 1000ULL);
}

std::vector<symbol_info> win_operating_system_info::get_backtrace()
{
    static init_dbghelp_library const init;

    std::lock_guard<std::mutex> lock(init.get_mutex());

    std::vector<symbol_info> stack_trace;

    std::size_t const max_stack_size = 30;
    void* stack_frames[max_stack_size] = { 0 };

    std::size_t const stack_size = ::CaptureStackBackTrace(1, max_stack_size, stack_frames, 0);

    if(stack_size > 0)
    {
        char buffer[sizeof(SYMBOL_INFO) + 1024 - 1] = { 0 };

        SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(buffer);

        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = 1024;

        for(std::size_t i = 0; i < stack_size; ++i)
        {
            symbol_info symbol_info;

            symbol_info.address = reinterpret_cast<std::uint64_t>(stack_frames[i]);

            if(::SymFromAddr(::GetCurrentProcess(), symbol_info.address, 0, symbol))
            {
                symbol_info.name = std::string(symbol->Name, symbol->NameLen);
            }

            stack_trace.push_back(symbol_info);
        }
    }

    return stack_trace;
}

win_operating_system_info::init_dbghelp_library::init_dbghelp_library()
{
    std::lock_guard<std::mutex> lock(get_mutex());

    ::SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

    if(!::SymInitialize(::GetCurrentProcess(), 0, TRUE))
    {
        throw std::runtime_error(util::win::error_to_string("SymInitialize", ::GetLastError()));
    }
}

win_operating_system_info::init_dbghelp_library::~init_dbghelp_library()
{
    std::lock_guard<std::mutex> lock(get_mutex());

    ::SymCleanup(::GetCurrentProcess());
}

std::mutex& win_operating_system_info::init_dbghelp_library::get_mutex()
{
    static std::mutex mutex;
    return mutex;
}

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_WINDOWS)
