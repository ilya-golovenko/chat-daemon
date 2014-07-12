//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2014 Ilya Golovenko
//    This file is part of Chat.Daemon project
//
//    spdaemon is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spdaemon is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spdaemon. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include <app/detail/win/process_memory_usage.hpp>

#if defined(CHAT_PLATFORM_WINDOWS)

// Application headers
#include <misc/error_utils.hpp>
#include <misc/win_utils.hpp>

// STL headers
#include <stdexcept>

// Windows headers
#include <psapi.h>


namespace chat
{
namespace detail
{

win_process_memory_usage::win_process_memory_usage() :
    memory_usage_(0),
    last_update_time_(0)
{
}

unsigned int win_process_memory_usage::get_value()
{
    std::time_t this_update_time;

    if(std::time(&this_update_time) < 0)
        throw std::runtime_error(util::errno_to_string("time", errno));

    if(this_update_time - last_update_time_ >= 60)
    {
        unsigned __int64 const process_size = get_process_memory_size();
        unsigned __int64 const system_size = get_total_system_memory_size();

        if(system_size > 0)
            memory_usage_ = static_cast<unsigned int>(100 * process_size / system_size);
    }

    return memory_usage_;
}

unsigned __int64 win_process_memory_usage::get_process_memory_size()
{
    PROCESS_MEMORY_COUNTERS proc_mem_counters;

    proc_mem_counters.cb = sizeof(PROCESS_MEMORY_COUNTERS);

    if(!::GetProcessMemoryInfo(::GetCurrentProcess(), &proc_mem_counters, sizeof(PROCESS_MEMORY_COUNTERS)))
        throw std::runtime_error(util::win::error_to_string("GetProcessMemoryInfo", ::GetLastError()));

    return proc_mem_counters.WorkingSetSize;
}

unsigned __int64 win_process_memory_usage::get_free_system_memory_size()
{
    MEMORYSTATUSEX memory_status;

    memory_status.dwLength = sizeof(MEMORYSTATUSEX);

    if(!::GlobalMemoryStatusEx(&memory_status))
        throw std::runtime_error(util::win::error_to_string("GlobalMemoryStatusEx", ::GetLastError()));

    return memory_status.ullAvailPhys;
}

unsigned __int64 win_process_memory_usage::get_total_system_memory_size()
{
    MEMORYSTATUSEX memory_status;

    memory_status.dwLength = sizeof(MEMORYSTATUSEX);

    if(!::GlobalMemoryStatusEx(&memory_status))
        throw std::runtime_error(util::win::error_to_string("GlobalMemoryStatusEx", ::GetLastError()));

    return memory_status.ullTotalPhys;
}

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_WINDOWS)
