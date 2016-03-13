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
#ifndef _chat_app_linux_process_cpu_usage_hpp
#define _chat_app_linux_process_cpu_usage_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <app/platform.hpp>

#if defined(CHAT_PLATFORM_LUNIX)

// STL headers
#include <ctime>

// UNIX headers
#include <sys/time.h>


namespace chat
{
namespace detail
{

class linux_process_cpu_usage
{
public:
    linux_process_cpu_usage();

    linux_process_cpu_usage(linux_process_cpu_usage const&) = delete;
    linux_process_cpu_usage& operator=(linux_process_cpu_usage const&) = delete;

    unsigned int get_value();

private:
    static unsigned int get_cpu_count();

    static long long get_system_time();
    static long long get_process_time();

    static long long time_to_ticks(struct timeval const& time);

private:
    unsigned int cpu_usage_;
    unsigned int cpu_count_;
    long long last_system_time_;
    long long last_process_time_;
    std::time_t last_update_time_;
};

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_LUNIX)

#endif  // _chat_app_linux_process_cpu_usage_hpp
