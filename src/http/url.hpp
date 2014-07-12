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
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <stdexcept>
#include <iostream>
#include <cstdint>
#include <string>


namespace http
{

class url
{
public:
    enum components
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
    static url from_string(std::string const& str);

public:
    url();

    explicit url(std::string const& str);

    url(url const&) = default;
    url& operator=(url const&) = default;

    std::string const& get_protocol() const;
    std::string const& get_user_info() const;
    std::string const& get_hostname() const;
    std::uint16_t get_port() const;
    std::string const& get_path() const;
    std::string const& get_query() const;
    std::string const& get_fragment() const;

    std::string get_host_info(bool escape = true) const;
    std::string get_relative_url(bool escape = true) const;

    std::string to_string(int components = all_components, bool escape = true) const;

    friend bool operator==(url const& lhs, url const& rhs);
    friend bool operator<(url const& lhs, url const& rhs);

private:
    std::string protocol_;
    std::string user_info_;
    std::string hostname_;
    std::string port_;
    std::string path_;
    std::string query_;
    std::string fragment_;
};

inline bool operator!=(url const& lhs, url const& rhs) { return !operator==(lhs, rhs); }
inline bool operator<=(url const& lhs, url const& rhs) { return !operator<(rhs, lhs); }
inline bool operator>=(url const& lhs, url const& rhs) { return !operator<(lhs, rhs); }
inline bool operator>(url const& lhs, url const& rhs) { return operator<(rhs, lhs); }

inline std::ostream& operator<<(std::ostream& os, url const& url)
{
    os << url.to_string();
    return os;
}

}   // namespace http

#endif  // _http_url_hpp
