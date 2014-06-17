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
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <iostream>
#include <string>


namespace http
{

class status
{
public:
    typedef enum codes_type
    {
        ok                   = 200,
        created              = 201,
        accepted             = 202,
        partial_content      = 206,
        moved_permanently    = 301,
        moved_temporarily    = 302,
        not_modified         = 304,
        bad_request          = 400,
        access_denied        = 401,
        forbidden            = 403,
        not_found            = 404,
        bad_method           = 405,
        proxy_auth_req       = 407,
        request_timeout      = 408,
        server_error         = 500,
        service_unavail      = 503,
        version_not_sup      = 505
    };

public:
    status();
    status(codes_type code);

    status(const status& other);
    status& operator=(const status& other);

    codes_type code() const;
    const std::string& to_string() const;

    friend bool operator==(const status& lhs, const status& rhs);
    friend bool operator<(const status& lhs, const status& rhs);

private:
    codes_type code_;
};

inline bool operator!=(const status& lhs, const status& rhs) { return !(lhs == rhs); }
inline bool operator<=(const status& lhs, const status& rhs) { return !(rhs < lhs); }
inline bool operator>=(const status& lhs, const status& rhs) { return !(lhs < rhs); }
inline bool operator>(const status& lhs, const status& rhs) { return (rhs < lhs); }

inline std::ostream& operator<<(std::ostream& os, const status& status)
{
    os << status.to_string();
    return os;
}

}   // namespace http

#endif  // _http_status_hpp
