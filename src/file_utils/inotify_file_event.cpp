//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009 Ilya Golovenko
//    This file is part of spdaemon.
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
#include "inotify_file_event.hpp"
#include <utilities.hpp>
#include <log_common.hpp>


namespace util
{

#if defined(_LINUX)

inotify_file_event::inotify_file_event(file& file) :
    file_(file),
    inotify_fd_(-1),
    file_wd_(-1)
{
    event_ = reinterpret_cast<struct inotify_event*>(&*buffer_);
}

inotify_file_event::~inotify_file_event()
{
    shutdown();
}

void inotify_file_event::initialize()
{
    if(inotify_fd_ < 0)
    {
        LOGNOT("initializing inotify");

        inotify_fd_ = inotify_init();

        if(inotify_fd_ < 0)
            throw std::runtime_error(errno_to_string("inotify_init", errno));

        file_wd_ = inotify_add_watch(inotify_fd_, file_.filename().c_str(), IN_MODIFY);

        if(file_wd_ < 0)
            throw std::runtime_error(errno_to_string("inotify_add_watch", errno));
    }
}

void inotify_file_event::shutdown()
{
    if(inotify_fd_ >= 0)
    {
        LOGNOT("shutting down inotify");

        inotify_rm_watch(inotify_fd_, file_wd_);

        close(inotify_fd_);
        inotify_fd_ = -1;
    }
}

bool inotify_file_event::wait()
{
    if(read(inotify_fd_, buffer_, sizeof(buffer_)) > 0)
        return ((event_->mask & IN_MODIFY) == IN_MODIFY);

    return false;
}

#endif  // defined(_LINUX)

}   // namespace util
