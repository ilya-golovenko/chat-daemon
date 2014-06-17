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
#ifndef _banner_hpp
#define _banner_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "version.hpp"

// STL headers
#include <string>


const std::string daemon_text_banner =
    "\r\n"
    "spdaemon - server for chat.php.spb.ru chat\r\n"
    "Copyright (C) 2008, 2009 Ilya Golovenko\r\n"
    "\r\n"
    "Version: " + daemon_version + "\r\n"
    "Release Date: " + daemon_release_date + "\r\n"
    "\r\n"
    "Visit us at: http://daemon.spchat.ru\r\n"
    "Contact us via: daemon@spchat.ru\r\n";

const std::string daemon_html_banner =
    "<!--\r\n"
    + daemon_text_banner + "\r\n"
    "-->\r\n";

const std::string daemon_license_text =
    "\r\n"
    "spdaemon - server for chat.php.spb.ru chat\r\n"
    "Copyright (C) 2008, 2009 Ilya Golovenko\r\n"
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

#endif  // _banner_hpp
