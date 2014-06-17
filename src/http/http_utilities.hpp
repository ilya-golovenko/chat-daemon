//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 Ilya Golovenko
//    This file is part of libsphttp.
//
//    libsphttp is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    libsphttp is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with libsphttp. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------
#ifndef _http_utilities_hpp
#define _http_utilities_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <string>


namespace http_util
{

struct string_iless :
    public std::binary_function<std::string, std::string, bool>
{
    bool operator()(const std::string& left, const std::string& right) const
    {
        return boost::ilexicographical_compare(left, right);
    }
};

unsigned char hex_to_int(unsigned char hex);

const std::string escape(const std::string& input);
bool unescape(const std::string& input, std::string& output);

const std::string combine_url_path(const std::string& path1, const std::string& path2);

}   // namespace http_util

#endif  // _http_utilities_hpp
