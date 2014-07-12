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
#ifndef _chat_file_kqueue_file_event_hpp
#define _chat_file_kqueue_file_event_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <app/platform.hpp>

#if defined(CHAT_PLATFORM_FREEBSD)

// Application headers
#include <file/file_handle.hpp>


namespace chat
{
namespace util
{

class kqueue_file_event
{
public:
    explicit kqueue_file_event(file_handle& file);
    ~kqueue_file_event();

    kqueue_file_event(kqueue_file_event const&) = delete;
    kqueue_file_event& operator=(kqueue_file_event const&) = delete;

    void initialize();
    void shutdown();

    bool wait();

private:
    file_handle& file_;
    struct kevent event_;
    int kqueue_fd_;
};

}   // namespace util
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_FREEBSD)

#endif  // _chat_file_kqueue_file_event_hpp
