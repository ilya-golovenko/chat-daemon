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
#ifndef _chat_app_constants_hpp
#define _chat_app_constants_hpp

//TODO: Add error checking macroses (try..catch..), add stack tracing.
//TODO: Reuse boost.exception library or write own classes.
//TODO: Create common exception classes: file_error, win32_error, posix_error, convert_error.
//TODO: Implement chat::dispatcher for routing and dispatching of messages, connections, etc.

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <string>


namespace strings
{

extern std::string const empty;

extern std::string const true_;
extern std::string const false_;

extern std::string const unknown;

extern std::string const not_available;
extern std::string const not_applicable;

}   // namespace strings

namespace files
{

extern std::string const config;
extern std::string const old_conf;

extern std::string const runstatus;
extern std::string const ignorelist;
extern std::string const statistics;
extern std::string const traffic;

}   // namespace files

#endif  // _chat_app_constants_hpp
