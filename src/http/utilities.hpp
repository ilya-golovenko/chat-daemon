//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 - 2016 Ilya Golovenko
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
#ifndef _http_utilities_hpp
#define _http_utilities_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <functional>
#include <cstdint>
#include <string>


namespace http
{

struct string_iless : std::binary_function<std::string, std::string, bool>
{
    bool operator()(std::string const& lhs, std::string const& rhs) const
    {
        return boost::algorithm::ilexicographical_compare(lhs, rhs);
    }
};

std::string escape_string(std::string const& str);
std::string unescape_string(std::string const& str);

std::string combine_url_path(std::string const& path1, std::string const& path2);

}   // namespace http

#endif  // _http_utilities_hpp
