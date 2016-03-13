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
#ifndef _chat_app_win_operating_system_info_hpp
#define _chat_app_win_operating_system_info_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <app/platform.hpp>

#if defined(CHAT_PLATFORM_WINDOWS)

// Application headers
#include <app/symbol_info.hpp>

// STL headers
#include <string>
#include <vector>
#include <ctime>
#include <mutex>


#define CHAT_OSINFO_HAS_BACKTRACE


namespace chat
{
namespace detail
{

class win_operating_system_info
{
public:
    win_operating_system_info() = delete;
    ~win_operating_system_info() = delete;

    static std::string get_name();
    static std::time_t get_uptime();

    static std::vector<symbol_info> get_backtrace();

private:
    struct init_dbghelp_library
    {
        init_dbghelp_library();
        ~init_dbghelp_library();

        init_dbghelp_library(init_dbghelp_library const&) = delete;
        init_dbghelp_library& operator=(init_dbghelp_library const&) = delete;

        static std::mutex& get_mutex();
    };
};

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_WINDOWS)

#endif  // _chat_app_win_operating_system_info_hpp
