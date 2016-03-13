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
#ifndef _chat_app_platform_hpp
#define _chat_app_platform_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

#if defined(__unix__) || defined(__unix)
#define CHAT_PLATFORM_UNIX
#define CHAR_SUPPORTED_PLATFORM
#endif

#if defined(__linux__) || defined(__linux)
#define CHAT_PLATFORM_LINUX
#define CHAT_SUPPORTED_PLATFORM
#endif

#if defined(_WIN32) || defined(_WIN64)
#define CHAT_PLATFORM_WINDOWS
#define CHAT_SUPPORTED_PLATFORM
#endif

#if defined(__FreeBSD__)
#define CHAT_PLATFORM_FREEBSD
#define CHAT_SUPPORTED_PLATFORM
#endif

#if defined(__POSIX__)
#define CHAT_PLATFORM_POSIX
#define CHAT_SUPPORTED_PLATFORM
#endif

#if !defined(CHAT_SUPPORTED_PLATFORM)
#error Current operating system is not supported.
#endif

#if !defined(CHAT_PLATFORM_POSIX) && defined(CHAT_PLATFORM_UNIX)
#include<unistd.h>
#if defined(_POSIX_VERSION)
#define CHAT_PLATFORM_POSIX
#endif
#endif

#endif  // _chat_app_platform_hpp
