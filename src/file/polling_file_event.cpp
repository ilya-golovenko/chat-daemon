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
#include <file/polling_file_event.hpp>

#if defined(CHAT_PLATFORM_UNIX)

// Application headers
#include <misc/error_utils.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{
namespace util
{

polling_file_event::polling_file_event(file_handle& file) :
    file_(file),
    terminated_(true)
{
}

polling_file_event::~polling_file_event()
{
    shutdown();
}

void polling_file_event::initialize()
{
    LOG_COMP_TRACE_FUNCTION(polling_file_event);

    if(terminated_)
    {
        LOG_COMP_NOTICE(polling_file_event, "initializing polling file event");
        terminated_ = false;
    }
}

void polling_file_event::shutdown()
{
    LOG_COMP_TRACE_FUNCTION(polling_file_event);

    if(!terminated_)
    {
        LOG_COMP_NOTICE(polling_file_event, "shutting down polling file event");
        terminated_ = true;
    }
}

bool polling_file_event::wait()
{
    LOG_COMP_TRACE_FUNCTION(polling_file_event);

    struct timespec interval;

    interval.tv_sec = 0;
    interval.tv_nsec = 100 * 1000000;

    while(!terminated_)
    {
        if(!file_.eof())
        {
            return true;
        }

        if(::nanosleep(&interval, 0) < 0)
        {
            if(EINTR != errno)
            {
                throw std::runtime_error(util::errno_to_string("nanosleep", errno));
            }
        }
    }

    return false;
}

}   // namespace util
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_UNIX)
