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
#ifndef _common_hpp
#define _common_hpp

//TODO: Add error checking macroses (try..catch..), add stack tracing.
//TODO: Reuse boost.exception library or write own classes.
//TODO: Create common exception classes: file_error, win32_error, posix_error, convert_error.
//TODO: Implement chat::dispatcher for routing and dispatching of messages, connections, etc.
//TODO: Modify logging to allow usage of multi-line log messages.

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "platform.hpp"

// STL headers
#include <string>


namespace strings
{

const std::string empty;

const std::string true_             = "true";
const std::string false_            = "false";

const std::string unknown           = "Unknown";

const std::string not_available     = "Not available";
const std::string not_applicable    = "Not applicable";

}   // namespace strings

namespace files
{

const std::string config            = "spdaemon.conf";
const std::string old_conf          = "dima_chat.conf";

const std::string runstatus         = "runstatus.tmp";
const std::string ignorelist        = "ignorelist.tmp";
const std::string statistics        = "stats.tmp";
const std::string traffic           = "traffic.tmp";

}   // namespace files

#endif  // _common_hpp
