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
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <iostream>
#include <cstdint>
#include <string>


namespace http
{

class version
{
public:
    version();
    version(std::uint8_t major, std::uint8_t minor);

    version(version const&) = default;
    version& operator=(version const&) = default;

    std::uint8_t get_major() const;
    std::uint8_t get_minor() const;

    std::string const& to_string() const;

    friend bool operator==(version const& lhs, version const& rhs);
    friend bool operator<(version const& lhs, version const& rhs);

public:
    static version const http_1_0;
    static version const http_1_1;

private:
    std::uint8_t major_;
    std::uint8_t minor_;
    std::string str_;
};

inline bool operator!=(version const& lhs, version const& rhs) { return !operator==(lhs, rhs); }
inline bool operator<=(version const& lhs, version const& rhs) { return !operator<(rhs, lhs); }
inline bool operator>=(version const& lhs, version const& rhs) { return !operator<(lhs, rhs); }
inline bool operator>(version const& lhs, version const& rhs) { return operator<(rhs, lhs); }

inline std::ostream& operator<<(std::ostream& os, version const& version)
{
    os << version.to_string();
    return os;
}

}   // namespace http

#endif  // _http_version_hpp
