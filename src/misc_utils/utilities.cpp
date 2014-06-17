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

// Application headers
#include "utilities.hpp"
#include <file_utils.hpp>
#include <win_utils.hpp>
#include <log_common.hpp>
#include <common.hpp>
#include <platform.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>
#include <boost/scoped_array.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>

// STL headers
#include <stdexcept>


namespace util
{

#if defined(_WINDOWS)

#define EPOCH_BIAS  116444736000000000ULL

boost::uint64_t get_milli_time()
{
    union
    {
        FILETIME file_time;
        boost::uint64_t scalar;
    } time_value;

    GetSystemTimeAsFileTime(&time_value.file_time);

    time_value.scalar -= EPOCH_BIAS;
    time_value.scalar /= 10000ULL;

    return time_value.scalar;
}

const std::string get_os_name()
{
    std::ostringstream buffer;
    OSVERSIONINFO version_info;

    ZeroMemory(&version_info, sizeof(OSVERSIONINFO));
    version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if(!GetVersionEx(&version_info))
        throw std::runtime_error(win32_error_to_string("GetVersionEx", GetLastError()));

    bool is_win_nt = (VER_PLATFORM_WIN32_NT == version_info.dwPlatformId);

    buffer << "Windows ";

    if(is_win_nt)
        buffer << "NT ";

    buffer << version_info.dwMajorVersion << '.';
    buffer << version_info.dwMinorVersion;

    if(strlen(version_info.szCSDVersion))
        buffer << ' ' << version_info.szCSDVersion;

    return buffer.str();
}

const std::string get_os_info()
{
    return strings::not_available;
}

#endif  // defined(_WINDOWS)

#if defined(_UNIX)

boost::uint64_t get_milli_time()
{
    struct timespec timespec;

    if(clock_gettime(CLOCK_MONOTONIC, &timespec) < 0)
        throw std::runtime_error(errno_to_string("clock_gettime", errno));

    boost::uint64_t value;

    value = timespec.tv_sec * 1000;
    value += timespec.tv_nsec / 1000000;

    return value;
}

const std::string get_os_name()
{
    std::ostringstream buffer;

    struct utsname info;
    if(uname(&info) < 0)
        throw std::runtime_error(errno_to_string("uname", errno));

    buffer << info.sysname << ' ';
    buffer << info.release << ' ';
    buffer << info.version << ' ';
    buffer << info.machine;

    return buffer.str();
}

const std::string get_os_info()
{
    enum
    {
        seconds_per_minute = 60,
        seconds_per_hour = 3600,
        seconds_per_day = 86400
    };

    // system uptime information

    std::ostringstream buffer;

#if defined(_FREEBSD)

    struct timeval boot_time;
    std::size_t size = sizeof(boot_time);
    int mib[2] = { CTL_KERN, KERN_BOOTTIME };

    if(sysctl(mib, 2, &boot_time, &size, 0, 0) < 0)
        throw std::runtime_error(errno_to_string("sysctl", errno));

    std::time_t uptime;
    std::time(&uptime);

    uptime -= boot_time.tv_sec;

#elif defined(_LINUX)

    struct sysinfo info;
    if(sysinfo(&info) < 0)
        throw std::runtime_error(errno_to_string("sysinfo", errno));

    std::time_t uptime = static_cast<std::time_t>(info.uptime);

#endif  // elif defined(_LINUX)

    if(uptime < seconds_per_hour)
    {
        // uptime in minutes
        buffer << "uptime: " << uptime / seconds_per_minute << " minutes; ";
    }
    else if(uptime < seconds_per_day)
    {
        // uptime in hours
        buffer << "uptime: " << uptime / seconds_per_hour << " hours; ";
    }
    else
    {
        // uptime in days
        buffer << "uptime: " << uptime / seconds_per_day << " days; ";
    }

    // load average information

    int nelem = -1;
    double loadavg[3];

    nelem = getloadavg(loadavg, 3);
    if(nelem < 3)
        throw std::runtime_error(errno_to_string("getloadavg", errno));

    buffer << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    buffer << "load average: " << loadavg[0] << ", " << loadavg[1] << ", " << loadavg[2];

    return buffer.str();
}
#endif  // defined(_UNIX)

const std::string& get_random_string(const std::vector<std::string>& strings)
{
    if(strings.empty())
        throw std::runtime_error("strings vector is empty");

    return strings.at(std::rand() % strings.size());
}

const std::string errno_to_string(const std::string& function, int errnum)
{
    return std::string(function + " failed: " + strerror(errnum));
}

const std::string file_errno_to_string(const std::string& function, const std::string& filename, int errnum)
{
    return std::string("cannot " + function + " " + filename + ": " + strerror(errnum));
}

}   // namespace util
