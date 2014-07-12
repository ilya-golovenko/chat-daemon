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
#include <app/version.hpp>

// MISSIO headers
#include <missio/format/print.hpp>


namespace chat
{

static char const short_name[] = "spdaemon";
static char const long_name[]  = "spdaemon - server for chat.php.spb.ru chat";
static char const copyright[]  = "Copyright (C) 2008, 2009, 2014 Ilya Golovenko";

static char const timestamp[]  = __TIMESTAMP__;

static char const version_format[] = "{0}/{1}.{2}.{3}";

static char const license_format[] =
    "{0}\r\n"
    "{1}\r\n"
    "\r\n"
    "Version: {2}.{3}.{4} ({5})\r\n"
    "\r\n"
    "This program is free software: you can redistribute it and/or modify\r\n"
    "it under the terms of the GNU General Public License as published by\r\n"
    "the Free Software Foundation, either version 3 of the License, or\r\n"
    "(at your option) any later version.\r\n"
    "\r\n"
    "This program is distributed in the hope that it will be useful,\r\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\r\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\r\n"
    "GNU General Public License for more details.\r\n"
    "\r\n"
    "You should have received a copy of the GNU General Public License\r\n"
    "along with this program. If not, see <http://www.gnu.org/licenses/>.\r\n";

static char const banner_format[] =
    "{0}\r\n"
    "{1}\r\n"
    "\r\n"
    "Version: {2}.{3}.{4} ({5})\r\n"
    "\r\n"
    "Visit our web site: <http://daemon.spchat.ru>\r\n"
    "Contact us via e-mail: daemon@spchat.ru\r\n";

std::string build_version_text()
{
    std::string str;

    missio::format::print(str, version_format, short_name, version_major, version_minor, version_build);

    return str;
}

std::string build_license_text()
{
    std::string str;

    missio::format::print(str, license_format, long_name, copyright, version_major, version_minor, version_build, timestamp);

    return str;
}

std::string build_banner_text()
{
    std::string str;

    missio::format::print(str, banner_format, long_name, copyright, version_major, version_minor, version_build, timestamp);

    return str;
}

}   // namespace chat
