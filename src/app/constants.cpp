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
#include <app/constants.hpp>


namespace strings
{

std::string const empty;

std::string const true_             = "true";
std::string const false_            = "false";

std::string const unknown           = "Unknown";

std::string const not_available     = "Not available";
std::string const not_applicable    = "Not applicable";

}   // namespace strings

namespace files
{

std::string const config            = "spdaemon.conf";
std::string const old_conf          = "dima_chat.conf";

std::string const runstatus         = "runstatus.tmp";
std::string const ignorelist        = "ignorelist.tmp";
std::string const statistics        = "stats.tmp";
std::string const traffic           = "traffic.tmp";

}   // namespace files
