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
#include <file/inotify_file_event.hpp>

#if defined(CHAT_PLATFORM_LINUX)

// Application headers
#include <misc/error_utils.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{
namespace util
{

inotify_file_event::inotify_file_event(file_handle& file) :
    file_(file),
    inotify_fd_(-1),
    file_wd_(-1)
{
    event_ = reinterpret_cast<struct inotify_event*>(buffer_);
}

inotify_file_event::~inotify_file_event()
{
    shutdown();
}

void inotify_file_event::initialize()
{
    LOG_COMP_TRACE_FUNCTION(inotify_file_event);

    if(inotify_fd_ < 0)
    {
        LOG_COMP_NOTICE(inotify_file_event, "initializing inotify");

        inotify_fd_ = ::inotify_init();

        if(inotify_fd_ < 0)
        {
            throw std::runtime_error(util::errno_to_string("inotify_init", errno));
        }

        file_wd_ = ::inotify_add_watch(inotify_fd_, file_.filename().c_str(), IN_MODIFY);

        if(file_wd_ < 0)
        {
            throw std::runtime_error(util::errno_to_string("inotify_add_watch", errno));
        }
    }
}

void inotify_file_event::shutdown()
{
    LOG_COMP_TRACE_FUNCTION(inotify_file_event);

    if(inotify_fd_ >= 0)
    {
        LOG_COMP_NOTICE(inotify_file_event, "shutting down inotify");

        ::inotify_rm_watch(inotify_fd_, file_wd_);

        ::close(inotify_fd_);
        inotify_fd_ = -1;
    }
}

bool inotify_file_event::wait()
{
    LOG_COMP_TRACE_FUNCTION(inotify_file_event);

    if(::read(inotify_fd_, buffer_, sizeof(buffer_)) > 0)
    {
        return event_->mask & IN_MODIFY == IN_MODIFY;
    }

    return false;
}

}   // namespace util
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_LINUX)
