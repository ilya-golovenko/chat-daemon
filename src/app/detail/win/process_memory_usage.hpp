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
#ifndef _chat_app_win_process_memory_usage_hpp
#define _chat_app_win_process_memory_usage_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <app/platform.hpp>

#if defined(CHAT_PLATFORM_WINDOWS)

// STL headers
#include <ctime>


namespace chat
{
namespace detail
{

class win_process_memory_usage
{
public:
    win_process_memory_usage();

    win_process_memory_usage(win_process_memory_usage const&) = delete;
    win_process_memory_usage& operator=(win_process_memory_usage const&) = delete;

    unsigned int get_value();

public:
    static unsigned __int64 get_process_memory_size();
    static unsigned __int64 get_free_system_memory_size();
    static unsigned __int64 get_total_system_memory_size();

private:
    unsigned int memory_usage_;
    std::time_t last_update_time_;
};

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_WINDOWS)

#endif  // _chat_app_win_process_memory_usage_hpp
