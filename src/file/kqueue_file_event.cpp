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
#include <file/kqueue_file_event.hpp>

#if defined(CHAT_PLATFORM_FREEBSD)

// Application headers
#include <misc/error_utils.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{
namespace util
{

kqueue_file_event::kqueue_file_event(file_handle& file) :
    file_(file),
    kqueue_fd_(-1)
{
}

kqueue_file_event::~kqueue_file_event()
{
    shutdown();
}

void kqueue_file_event::initialize()
{
    LOG_COMP_TRACE_FUNCTION(kqueue_file_event);

    if(kqueue_fd_ < 0)
    {
        LOG_COMP_NOTICE(kqueue_file_event, "initializing kqueue");

        kqueue_fd_ = ::kqueue();

        if(kqueue_fd_ < 0)
        {
            throw std::runtime_error(util::errno_to_string("kqueue", errno));
        }

        EV_SET(&event_, file_, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_WRITE, 0, 0);

        if(::kevent(kqueue_fd_, &event_, 1, 0, 0, 0) < 0)
        {
            throw std::runtime_error(util::errno_to_string("kevent", errno));
        }

        ::bzero(&event_, sizeof(event_));
    }
}

void kqueue_file_event::shutdown()
{
    LOG_COMP_TRACE_FUNCTION(kqueue_file_event);

    if(kqueue_fd_ >= 0)
    {
        LOG_COMP_NOTICE(kqueue_file_event, "shutting down kqueue");

        ::close(kqueue_fd_);
        kqueue_fd_ = -1;
    }
}

bool kqueue_file_event::wait()
{
    LOG_COMP_TRACE_FUNCTION(kqueue_file_event);

    return ::kevent(kqueue_fd_, 0, 0, &event_, 1, 0) > 0;
}

}   // namespace util
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_FREEBSD)
