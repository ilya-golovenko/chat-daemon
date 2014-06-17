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
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "http_common.hpp"
#include "http_url.hpp"

// BOOST headers
#include <boost/noncopyable.hpp>

// STL headers
#include <string>


namespace http
{

class url_builder :
    private boost::noncopyable
{
public:
    url_builder();

    url_builder(const http::url& url);
    url_builder(const std::string& url);

    const std::string& protocol() const;
    void set_protocol(const std::string& protocol);

    const std::string& user_info() const;
    void set_user_info(const std::string& user_info);

    const std::string& hostname() const;
    void set_hostname(const std::string& hostname);

    unsigned short port() const;
    void set_port(unsigned short port);

    const std::string& path() const;
    void set_path(const std::string& path);

    const std::string& query() const;
    void set_query(const std::string& query);

    const std::string& fragment() const;
    void set_fragment(const std::string& fragment);

    const std::string to_string(int components = http::url::all_components, bool escape = false) const;
    const http::url get_url(int components = http::url::all_components, bool escape = false) const;

private:
    void initialize_from_url(const http::url& url);

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
