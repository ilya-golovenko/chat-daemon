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
#ifndef _http_url_builder_hpp
#define _http_url_builder_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/common.hpp>
#include <http/url.hpp>

// STL headers
#include <cstdint>
#include <string>


namespace http
{

class url_builder
{
public:
    url_builder();

    explicit url_builder(url const& url);
    explicit url_builder(std::string const& str);

    url_builder(url_builder const&) = delete;
    url_builder& operator=(url_builder const&) = delete;

    std::string const& get_protocol() const;
    void set_protocol(std::string const& protocol);

    std::string const& get_user_info() const;
    void set_user_info(std::string const& user_info);

    std::string const& get_hostname() const;
    void set_hostname(std::string const& hostname);

    std::uint16_t get_port() const;
    void set_port(std::uint16_t port);

    std::string const& get_path() const;
    void set_path(std::string const& path);

    std::string const& get_query() const;
    void set_query(std::string const& query);

    std::string const& get_fragment() const;
    void set_fragment(std::string const& fragment);

    url get_url(int components = url::all_components, bool escape = false) const;
    std::string to_string(int components = url::all_components, bool escape = false) const;

private:
    void initialize_from_url(url const& url);

private:
    std::string protocol_;
    std::string user_info_;
    std::string hostname_;
    std::string port_;
    std::string path_;
    std::string query_;
    std::string fragment_;
};

}   // namespace http

#endif  // _http_url_builder_hpp
