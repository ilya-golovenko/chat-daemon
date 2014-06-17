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
#ifndef _util_file_event_hpp
#define _util_file_event_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <platform.hpp>

#if defined(_WINDOWS)
#include "win_file_event.hpp"
#elif defined(_LINUX)
#include "inotify_file_event.hpp"
#elif defined(_FREEBSD)
#include "kqueue_file_event.hpp"
#elif defined(_UNIX)
#include "polling_file_event.hpp"
#else
#  error Current operating system is not supported
#endif


namespace util
{

#if defined(_WINDOWS)
typedef win_file_event file_event;
#elif defined(_LINUX)
typedef inotify_file_event file_event;
#elif defined(_FREEBSD)
typedef kqueue_file_event file_event;
#else
typedef polling_file_event file_event;
#endif

}   // namespace util

#endif  // _util_file_event_hpp
