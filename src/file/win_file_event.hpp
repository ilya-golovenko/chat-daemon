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
#ifndef _chat_file_win_file_event_hpp
#define _chat_file_win_file_event_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <app/platform.hpp>

#if defined(CHAT_PLATFORM_WINDOWS)

// Application headers
#include <file/file_handle.hpp>

// Windows headers
#include <windows.h>


namespace chat
{
namespace util
{

class win_file_event
{
public:
    explicit win_file_event(file_handle& file);
    ~win_file_event();

    win_file_event(win_file_event const&) = delete;
    win_file_event& operator=(win_file_event const&) = delete;

    void initialize();
    void shutdown();

    bool wait();

private:
    static bool is_valid_handle(HANDLE hande);

private:
    file_handle& file_;
    HANDLE change_handle_;
    volatile bool terminated_;
};

}   // namespace util
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_WINDOWS)

#endif  // _chat_file_win_file_event_hpp
