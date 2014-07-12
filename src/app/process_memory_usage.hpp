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
#ifndef _chat_app_process_memory_usage_hpp
#define _chat_app_process_memory_usage_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <app/platform.hpp>


#if defined(CHAT_PLATFORM_WINDOWS)
#include <app/detail/win/process_memory_usage.hpp>
#elif defined(CHAT_PLATFORM_LUNIX)
#include <app/detail/linux/process_memory_usage.hpp>
#elif defined(CHAT_PLATFORM_FREEBSD)
#include <app/detail/freebsd/process_memory_usage.hpp>
#else
#error Current operating system is not supported.
#endif


namespace chat
{

#if defined(CHAT_PLATFORM_WINDOWS)
typedef detail::win_process_memory_usage process_memory_usage;
#elif defined(CHAT_PLATFORM_LUNIX)
typedef detail::linux_process_memory_usage process_memory_usage;
#elif defined(CHAT_PLATFORM_FREEBSD)
typedef detail::freebsd_process_memory_usage process_memory_usage;
#else
#error Current operating system is not supported.
#endif

}   // namespace chat

#endif  // _chat_app_process_memory_usage_hpp
