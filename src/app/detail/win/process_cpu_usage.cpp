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
#include <app/detail/win/process_cpu_usage.hpp>

#if defined(CHAT_PLATFORM_WINDOWS)

// Application headers
#include <misc/error_utils.hpp>
#include <misc/win_utils.hpp>

// STL headers
#include <stdexcept>


namespace chat
{
namespace detail
{

win_process_cpu_usage::win_process_cpu_usage() :
    cpu_usage_(0),
    cpu_count_(get_cpu_count()),
    last_system_time_(get_system_time()),
    last_process_time_(get_process_time())
{
    if(std::time(&last_update_time_) < 0)
    {
        throw std::runtime_error(util::errno_to_string("time", errno));
    }
}

unsigned int win_process_cpu_usage::get_value()
{
    std::time_t this_update_time;

    if(std::time(&this_update_time) < 0)
    {
        throw std::runtime_error(util::errno_to_string("time", errno));
    }

    if(this_update_time - last_update_time_ >= 60)
    {
        __int64 const this_system_time = get_system_time();
        __int64 const this_process_time = get_process_time();

        __int64 const system_diff = this_system_time - last_system_time_;
        __int64 const process_diff = this_process_time - last_process_time_;

        if(system_diff > 0)
        {
            __int64 percent = 100 * process_diff / system_diff;

            if(cpu_count_ > 1)
            {
                percent /= static_cast<__int64>(cpu_count_);
            }

            cpu_usage_ = static_cast<unsigned int>(percent);
        }

        last_system_time_ = this_system_time;
        last_process_time_ = this_process_time;
        last_update_time_ = this_update_time;
    }

    return cpu_usage_;
}

unsigned int win_process_cpu_usage::get_cpu_count()
{
    SYSTEM_INFO system_info;

    ::GetSystemInfo(&system_info);

    return system_info.dwNumberOfProcessors;
}

__int64 win_process_cpu_usage::get_system_time()
{
    FILETIME sys_idle_time;
    FILETIME sys_kernel_time;
    FILETIME sys_user_time;

    if(!::GetSystemTimes(&sys_idle_time, &sys_kernel_time, &sys_user_time))
    {
        throw std::runtime_error(util::win::error_to_string("GetSystemTimes", ::GetLastError()));
    }

    LARGE_INTEGER kernel_time;
    LARGE_INTEGER user_time;

    kernel_time.HighPart = sys_kernel_time.dwHighDateTime;
    kernel_time.LowPart = sys_kernel_time.dwLowDateTime;
    user_time.HighPart = sys_user_time.dwHighDateTime;
    user_time.LowPart = sys_user_time.dwLowDateTime;

    return kernel_time.QuadPart + user_time.QuadPart;
}

__int64 win_process_cpu_usage::get_process_time()
{
    FILETIME sys_creation_time;
    FILETIME sys_exit_time;
    FILETIME sys_kernel_time;
    FILETIME sys_user_time;

    if(!::GetProcessTimes(::GetCurrentProcess(), &sys_creation_time, &sys_exit_time, &sys_kernel_time, &sys_user_time))
    {
        throw std::runtime_error(util::win::error_to_string("GetProcessTimes", ::GetLastError()));
    }

    LARGE_INTEGER kernel_time;
    LARGE_INTEGER user_time;

    kernel_time.HighPart = sys_kernel_time.dwHighDateTime;
    kernel_time.LowPart = sys_kernel_time.dwLowDateTime;
    user_time.HighPart = sys_user_time.dwHighDateTime;
    user_time.LowPart = sys_user_time.dwLowDateTime;

    return kernel_time.QuadPart + user_time.QuadPart;
}

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_WINDOWS)
