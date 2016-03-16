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
#include <app/detail/freebsd/operating_system_info.hpp>

#if defined(CHAT_PLATFORM_FREEBSD)

// Application headers
#include <misc/error_utils.hpp>

// MISSIO headers
#include <missio/format/write.hpp>

// STL headers
#include <stdexcept>

// FreeBSD headers
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <sys/types.h>

// CRT headers
#include <errno.h>


namespace chat
{
namespace detail
{

std::string freebsd_operating_system_info::get_name()
{
    struct utsname name;

    if(::uname(&name) < 0)
    {
        throw std::runtime_error(util::errno_to_string("uname", errno));
    }

    std::string str;

    missio::format::write(str, name.sysname, " ", name.release, " ", name.version, " ", name.machine);

    return str;
}

std::time_t freebsd_operating_system_info::get_uptime()
{
    struct timeval boot_time;
    size_t size = sizeof(boot_time);
    int mib[2] = { CTL_KERN, KERN_BOOTTIME };

    if(::sysctl(mib, 2, &boot_time, &size, 0, 0) < 0)
    {
        throw std::runtime_error(util::errno_to_string("sysctl", errno));
    }

    return static_cast<std::time_t>(std::time(nullptr) - boot_time.tv_sec);
}

std::array<double, 3> freebsd_operating_system_info::get_loadavg()
{
    std::array<double, 3> loadavg;

    int const nelem = ::getloadavg(loadavg.data(), 3);

    if(nelem < 3)
    {
        throw std::runtime_error(util::errno_to_string("getloadavg", errno));
    }

    return loadavg;
}

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_FREEBSD)
