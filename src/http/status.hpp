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
#ifndef _http_status_hpp
#define _http_status_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <iostream>
#include <cstdint>
#include <string>


namespace http
{

class status
{
public:
    static std::uint16_t const ok                      = 200;
    static std::uint16_t const created                 = 201;
    static std::uint16_t const accepted                = 202;
    static std::uint16_t const partial_content         = 206;
    static std::uint16_t const moved_permanently       = 301;
    static std::uint16_t const moved_temporarily       = 302;
    static std::uint16_t const not_modified            = 304;
    static std::uint16_t const bad_request             = 400;
    static std::uint16_t const access_denied           = 401;
    static std::uint16_t const forbidden               = 403;
    static std::uint16_t const not_found               = 404;
    static std::uint16_t const bad_method              = 405;
    static std::uint16_t const proxy_auth_required     = 407;
    static std::uint16_t const request_timeout         = 408;
    static std::uint16_t const internal_server_error   = 500;
    static std::uint16_t const service_unavailable     = 503;
    static std::uint16_t const version_not_supported   = 505;

public:
    status();

    status(std::uint16_t code);

    status(status const&) = default;
    status& operator=(status const&) = default;

    std::uint16_t get_code() const;
    std::string const& to_string() const;

    friend bool operator==(status const& lhs, status const& rhs);
    friend bool operator<(status const& lhs, status const& rhs);

private:
    std::uint16_t code_;
};

inline bool operator!=(status const& lhs, status const& rhs) { return !operator==(lhs, rhs); }
inline bool operator<=(status const& lhs, status const& rhs) { return !operator<(rhs, lhs); }
inline bool operator>=(status const& lhs, status const& rhs) { return !operator<(lhs, rhs); }
inline bool operator>(status const& lhs, status const& rhs) { return operator<(rhs, lhs); }

inline std::ostream& operator<<(std::ostream& os, status const& status)
{
    os << status.to_string();
    return os;
}

}   // namespace http

#endif  // _http_status_hpp
