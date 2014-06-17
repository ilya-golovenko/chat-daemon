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
#ifndef _http_url_hpp
#define _http_url_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <string>
#include <iostream>
#include <stdexcept>


namespace http
{

class invalid_url_error :
    public std::invalid_argument
{
public:
    invalid_url_error(const std::string& url) :
        invalid_argument("cannot parse url: " + url)
    {
    }

    virtual ~invalid_url_error() throw()
    {
    }
};

class url
{
public:
    enum components_type
    {
        none_component = 0,

        protocol_component = 1,
        user_info_component = 2,
        hostname_component = 4,
        port_component = 8,
        path_component = 16,
        query_component = 32,
        fragment_component = 64,

        host_info_components = hostname_component | port_component,
        relative_url_components = path_component | query_component | fragment_component,
        all_components = protocol_component | user_info_component | host_info_components | relative_url_components
    };

public:
    static const url from_string(const char* string);
    static const url from_string(const std::string& string);

public:
    url();

    url(const char* string);
    url(const std::string& string);

    url(const url& other);
    url& operator=(const url& other);

    const std::string& protocol() const;
    const std::string& user_info() const;
    const std::string& hostname() const;
    unsigned short port() const;
    const std::string& path() const;
    const std::string& query() const;
    const std::string& fragment() const;

    const std::string host_info(bool escape = true) const;
    const std::string relative_url(bool escape = true) const;

    const std::string to_string(int components = all_components, bool escape = true) const;

    friend bool operator==(const url& lhs, const url& rhs);
    friend bool operator<(const url& lhs, const url& rhs);

private:
    std::string protocol_;
    std::string user_info_;
    std::string hostname_;
    std::string port_;
    std::string path_;
    std::string query_;
    std::string fragment_;
};

inline bool operator!=(const url& lhs, const url& rhs) { return !(lhs == rhs); }
inline bool operator<=(const url& lhs, const url& rhs) { return !(rhs < lhs); }
inline bool operator>=(const url& lhs, const url& rhs) { return !(lhs < rhs); }
inline bool operator>(const url& lhs, const url& rhs) { return (rhs < lhs); }

inline std::ostream& operator<<(std::ostream& os, const url& url)
{
    os << url.to_string();
    return os;
}

}   // namespace http

#endif  // _http_url_hpp
