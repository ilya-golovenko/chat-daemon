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
#include <app/detail/linux/process_memory_usage.hpp>

#if defined(CHAT_PLATFORM_LUNIX)

// Application headers
#include <misc/error_utils.hpp>

// STL headers
#include <stdexcept>
#include <iostream>
#include <fstream>

// CRT headers
#include <errno.h>

// Linux headers
#include <sys/sysinfo.h>


namespace chat
{
namespace detail
{

linux_process_memory_usage::linux_process_memory_usage() :
    memory_usage_(0),
    last_update_time_(0)
{
}

unsigned int linux_process_memory_usage::get_value()
{
    std::time_t this_update_time;

    if(std::time(&this_update_time) < 0)
    {
        throw std::runtime_error(util::errno_to_string("time", errno));
    }

    if(this_update_time - last_update_time_ >= 60)
    {
        unsigned long long const process_size = get_process_memory_size();
        unsigned long long const system_size = get_total_system_memory_size();

        if(system_size > 0)
        {
            memory_usage_ = static_cast<unsigned int>(100 * process_size / system_size);
        }
    }

    return memory_usage_;
}

unsigned long long linux_process_memory_usage::get_process_memory_size()
{
    std::ifstream stat_file("/proc/self/stat");

    if(!stat_file.is_open())
    {
        throw std::runtime_error("cannot open file: /proc/self/stat");
    }

    std::string pid, comm, state, ppid, pgrp, session, tty, tpgid, flags;
    std::string min_flt, cmin_flt, maj_flt, cmaj_flt, utime, stime, cutime;
    std::string cstime, priority, nice, nlwp, alarm, start_time, vsize;

    // skip fields we are not interested in

    stat_file >> pid >> comm >> state >> ppid >> pgrp >> session >> tty >> tpgid >> flags
              >> min_flt >> cmin_flt >> maj_flt >> cmaj_flt >> utime >> stime >> cutime
              >> cstime >> priority >> nice >> nlwp >> alarm >> start_time >> vsize;

    unsigned long rss;
    stat_file >> rss;

    return rss * ::sysconf(_SC_PAGE_SIZE);
}

unsigned long long linux_process_memory_usage::get_free_system_memory_size()
{
    struct sysinfo info;

    if(::sysinfo(&info) < 0)
    {
        throw std::runtime_error(util::errno_to_string("sysinfo", errno));
    }

    return info.freeram * info.mem_unit;
}

unsigned long long linux_process_memory_usage::get_total_system_memory_size()
{
    struct sysinfo info;

    if(::sysinfo(&info) < 0)
    {
        throw std::runtime_error(util::errno_to_string("sysinfo", errno));
    }

    return info.totalram * info.mem_unit;
}

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_LUNIX)
