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
#ifndef _http_version_hpp
#define _http_version_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <iostream>
#include <cstddef>
#include <string>


namespace http
{

class version
{
public:
    version();
    version(std::size_t major, std::size_t minor);

    version(const version& other);
    version& operator=(const version& other);

    std::size_t get_major() const;
    std::size_t get_minor() const;

    const std::string& to_string() const;

    friend bool operator==(const version& lhs, const version& rhs);
    friend bool operator<(const version& lhs, const version& rhs);

public:
    static version _1_0;
    static version _1_1;

private:
    std::size_t major_;
    std::size_t minor_;
    std::string string_;
};

inline bool operator!=(const version& lhs, const version& rhs) { return !(lhs == rhs); }
inline bool operator<=(const version& lhs, const version& rhs) { return !(rhs < lhs); }
inline bool operator>=(const version& lhs, const version& rhs) { return !(lhs < rhs); }
inline bool operator>(const version& lhs, const version& rhs) { return (rhs < lhs); }

inline std::ostream& operator<<(std::ostream& os, const version& version)
{
    os << version.to_string();
    return os;
}

}   // namespace http

#endif  // _http_version_hpp
