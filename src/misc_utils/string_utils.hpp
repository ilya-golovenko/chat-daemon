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
#ifndef _string_utils_hpp
#define _string_utils_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

// STL headers
#include <string>


namespace util
{

template <typename Source>
inline const std::string to_string(const Source& source)
{
    return boost::lexical_cast<std::string>(source);
}

template <typename Target>
inline const Target to_type(const std::string& source)
{
    return boost::lexical_cast<Target>(source);
}

struct string_iless :
    public std::binary_function<std::string, std::string, bool>
{
    bool operator()(const std::string& left, const std::string& right) const
    {
        return boost::ilexicographical_compare(left, right);
    }
};

}   // namespace util

#endif  // _string_utils_hpp
