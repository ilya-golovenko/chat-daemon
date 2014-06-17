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
#ifndef _platform_hpp
#define _platform_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

#if defined(__unix__) || defined(__unix)
#  define _UNIX
#  define _PLATFORM_SUPPORTED
#endif

#if defined(__linux__) || defined(__linux)
#  define _LINUX
#  define _PLATFORM_SUPPORTED
#endif

#if defined(_WIN32) || defined(_WIN64)
#  define _WINDOWS
#  define _PLATFORM_SUPPORTED
#endif

#if defined(__FreeBSD__)
#  define _FREEBSD
#  define _PLATFORM_SUPPORTED
#endif

#if !defined(_PLATFORM_SUPPORTED)
#  error Current operating system is not supported
#endif

#if !defined(_UNIX)
#  if defined(_LINUX) || defined(_FREEBSD)
#    define _UNIX
#  endif
#endif

#if defined(_UNIX)
#include <sys/utsname.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#endif

#if defined(_LINUX)
#include <sys/sysinfo.h>
#include <sys/inotify.h>
#endif

#if defined(_FREEBSD)
#include <sys/sysctl.h>
#include <sys/event.h>
#include <sys/time.h>
#endif

#if defined(_WINDOWS)
#include <windows.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#endif

// CRT headers
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#endif  // _platform_hpp
