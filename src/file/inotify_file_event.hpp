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
#ifndef _chat_file_inotify_file_event_hpp
#define _chat_file_inotify_file_event_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <app/platform.hpp>

#if defined(CHAT_PLATFORM_LINUX)

// Application headers
#include <file/file_handle.hpp>

// Linux headers
#include <sys/inotify.h>


namespace chat
{
namespace util
{

class inotify_file_event
{
public:
    explicit inotify_file_event(file_handle& file);
    ~inotify_file_event();

    void initialize();
    void shutdown();

    bool wait();

private:
    static std::size_t const buffer_size = sizeof(struct inotify_event) + FILENAME_MAX;

private:
    file_handle& file_;
    int inotify_fd_;
    int file_wd_;

    char buffer_[buffer_size];
    struct inotify_event* event_;
};

}   // namespace util
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_LINUX)

#endif  // _chat_file_inotify_file_event_hpp
