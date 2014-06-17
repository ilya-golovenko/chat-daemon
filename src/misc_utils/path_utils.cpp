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
#include "path_utils.hpp"
#include <utilities.hpp>
#include <platform.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>


namespace util
{
namespace path
{

const std::string posix_separator   = "/";
const std::string win32_separator   = "\\";

#if defined(_WINDOWS)
const std::string separator     = win32_separator;
#else
const std::string separator     = posix_separator;
#endif  // defined(_WINDOWS)

const std::string initial_path  = current();

const std::string current()
{
    char* buffer = getcwd(0, 0);

    if(!buffer)
        throw std::runtime_error(errno_to_string("getcwd", errno));

    std::string path(buffer);

    free(buffer);
    return path;
}

const std::string& initial()
{
    return initial_path;
}

bool exists(const std::string& path)
{
    struct stat path_stat;

    if(!stat(path.c_str(), &path_stat))
    {
        if(path_stat.st_mode & S_IFDIR)
            return true;
    }

    return false;
}

const std::string native(const std::string& path)
{
#if defined(_WINDOWS)
    return boost::replace_all_copy(path, posix_separator, separator);
#else
    return boost::replace_all_copy(path, win32_separator, separator);
#endif  // defined(_WINDOWS)
}

const std::string remove_filespec(const std::string& path)
{
    const std::string native_path = native(path);
    std::size_t pos = native_path.find_last_of(separator);
    return native_path.substr(0, pos);
}

const std::string combine(const std::string& path1, const std::string& path2)
{
    if(path1.empty())
        return native(path2);

    if(path2.empty())
        return native(path1);

    const std::string native_path1 = native(path1);
    const std::string native_path2 = native(path2);

    bool has_end = boost::ends_with(native_path1, separator);
    bool has_start = boost::starts_with(native_path2, separator);

    if(has_end && has_start)
        return (native_path1 + native_path2.substr(1));

    if(has_end || has_start)
        return (native_path1 + native_path2);

    return (native_path1 + separator + native_path2);
}

}   // namespace path
}   // namespace util
