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
#include <misc/error_utils.hpp>

// MISSIO headers
#include <missio/format/print.hpp>

// STL headers
#include <cstring>


namespace chat
{
namespace util
{

std::string errno_to_string(char const* function, int errnum)
{
    std::string str;

    //TODO: missio::format::print(str, "{0} failed: {1} ({2})", function, std::strerror(errnum), errnum);

    return str;
}

std::string errno_to_string(char const* function, std::string const& filename, int errnum)
{
    std::string str;

    //TODO: missio::format::print(str, "cannot {0} {1}: {2} ({3})", function, filename, std::strerror(errnum), errnum);

    return str;
}

}   // namespace util
}   // namespace chat
