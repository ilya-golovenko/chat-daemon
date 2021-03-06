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
#include <app/detail/freebsd/process_cpu_usage.hpp>

#if defined(CHAT_PLATFORM_FREEBSD)

// Application headers
#include <misc/error_utils.hpp>

// STL headers
#include <stdexcept>
#include <iostream>
#include <fstream>

// UNIX headers
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <sys/types.h>


namespace chat
{
namespace detail
{

freebsd_process_cpu_usage::freebsd_process_cpu_usage() :
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

unsigned int freebsd_process_cpu_usage::get_value()
{
    std::time_t this_update_time;

    if(std::time(&this_update_time) < 0)
    {
        throw std::runtime_error(util::errno_to_string("time", errno));
    }

    if(this_update_time - last_update_time_ >= 60)
    {
        long long const this_system_time = get_system_time();
        long long const this_process_time = get_process_time();

        long long const system_diff = this_system_time - last_system_time_;
        long long const process_diff = this_process_time - last_process_time_;

        if(system_diff > 0)
        {
            long long percent = 100 * process_diff / system_diff;

            if(cpu_count_ > 1)
            {
                percent /= static_cast<long long>(cpu_count_);
            }

            cpu_usage_ = static_cast<unsigned int>(percent);
        }

        last_system_time_ = this_system_time;
        last_process_time_ = this_process_time;
        last_update_time_ = this_update_time;
    }

    return cpu_usage_;
}

unsigned int freebsd_process_cpu_usage::get_cpu_count()
{
    int cpu_count = 0;
    size_t size = sizeof(count);

    if(::sysctlbyname("hw.ncpu", &cpu_count, &size, 0, 0) < 0)
    {
        throw std::runtime_error(util::errno_to_string("sysctlbyname", errno));
    }

    return cpu_count;
}

long long freebsd_process_cpu_usage::get_system_time()
{
    return 0; //TODO: ???
}

long long freebsd_process_cpu_usage::get_process_time()
{
    struct rusage usage;

    if(::getrusage(RUSAGE_SELF, &usage) < 0)
    {
        throw std::runtime_error(util::errno_to_string("rusage", errno));
    }

    long long const user_time = time_to_ticks(usage.ru_utime);
    long long const sys_time = time_to_ticks(usage.ru_stime);

    return user_time + sys_time;
}

long long freebsd_process_cpu_usage::time_to_ticks(struct timeval const& time)
{
    return time.tv_sec * ::sysconf(_SC_CLK_TCK) + time.tv_usec / (1000000 / ::sysconf(_SC_CLK_TCK));
}

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_FREEBSD)
