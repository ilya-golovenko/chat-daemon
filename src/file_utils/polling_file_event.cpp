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
#include "polling_file_event.hpp"
#include <utilities.hpp>
#include <log_common.hpp>


namespace util
{

#if defined(_UNIX)

polling_file_event::polling_file_event(file& file) :
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
    if(terminated_)
    {
        LOGNOT("initializing polling file event");
        terminated_ = false;
    }
}

void polling_file_event::shutdown()
{
    if(!terminated_)
    {
        LOGNOT("shutting down polling file event");
        terminated_ = true;
    }
}

bool polling_file_event::wait()
{
    struct timespec interval;

    interval.tv_sec = 0;
    interval.tv_nsec = 100 * 1000000;

    while(!terminated_)
    {
        if(!file_.eof())
            return true;

        if(nanosleep(&interval, 0) < 0)
        {
            if(EINTR != errno)
                throw std::runtime_error(errno_to_string("nanosleep", errno));
        }
    }

    return false;
}

#endif  // defined(_UNIX)

}   // namespace util
