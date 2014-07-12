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
#include <misc/path_utils.hpp>
#include <misc/error_utils.hpp>
#include <app/platform.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <memory>

#if defined(CHAT_PLATFORM_WINDOWS)
// Windows headers
#include <direct.h>
#endif

// CRT headers
#include <sys/types.h>
#include <sys/stat.h>


namespace chat
{
namespace util
{
namespace path
{

char const win_separator[]   = "\\";
char const posix_separator[] = "/";

#if defined(CHAT_PLATFORM_WINDOWS)
char const native_separator[] = "\\";
#else
char const native_separator[] = "/";
#endif

std::string const initial_path = current();

std::string current()
{
    std::unique_ptr<void, decltype(&::free)> buffer(::getcwd(0, 0), ::free);

    if(!buffer)
        throw std::runtime_error(util::errno_to_string("getcwd", errno));

    return std::string(static_cast<char const*>(buffer.get()));
}

std::string const& initial()
{
    return initial_path;
}

bool exists(std::string const& path)
{
    struct stat path_stat;

    if(!::stat(path.c_str(), &path_stat))
    {
        if(path_stat.st_mode & S_IFDIR)
            return true;
    }

    return false;
}

std::string native(std::string const& path)
{
#if defined(CHAT_PLATFORM_WINDOWS)
    return boost::algorithm::replace_all_copy(path, posix_separator, native_separator);
#else
    return boost::algorithm::replace_all_copy(path, win_separator, native_separator);
#endif
}

std::string remove_filespec(std::string const& path)
{
    std::string const native_path = native(path);
    std::size_t pos = native_path.find_last_of(native_separator);
    return native_path.substr(0, pos);
}

std::string combine(std::string const& path1, std::string const& path2)
{
    if(path1.empty())
        return native(path2);

    if(path2.empty())
        return native(path1);

    std::string const native_path1 = native(path1);
    std::string const native_path2 = native(path2);

    bool const has_end = boost::algorithm::ends_with(native_path1, native_separator);
    bool const has_start = boost::algorithm::starts_with(native_path2, native_separator);

    if(has_end && has_start)
        return native_path1 + native_path2.substr(1);

    if(has_end || has_start)
        return native_path1 + native_path2;

    return native_path1 + native_separator + native_path2;
}

}   // namespace path
}   // namespace util
}   // namespace chat
