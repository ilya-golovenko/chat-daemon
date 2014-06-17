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
#ifndef _http_request_hpp
#define _http_request_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "http_message.hpp"
#include "http_url.hpp"

// ASIO headers
#include <asio.hpp>

// STL headers
#include <string>
#include <vector>


namespace http
{

class request :
    public message
{
public:
    request();

    explicit request(const version& version);
    request(const std::string& method, const std::string& url);
    request(const std::string& method, const std::string& url, const version& version);

    virtual ~request();

    request(const request& other);
    request& operator=(const request& other);

    void assign(const request& other);

    const std::string& get_url() const;
    void set_url(const std::string& url);
    void set_url(const http::url& url);

    const std::string& get_method() const;
    void set_method(const std::string& method);

    void set_host(const std::string& host);
    void set_host(const std::string& host, unsigned short port);
    boost::optional<std::string> get_host() const;

    void set_user_agent(const std::string& user_agent);
    boost::optional<std::string> get_user_agent() const;

    void set_cookie(const std::string& cookie);
    boost::optional<std::string> get_cookie() const;

    void to_buffers(buffers_type& buffers) const;
    void dump(std::ostream& os) const;

private:
    std::string method_;
    std::string url_;
};

inline std::ostream& operator<<(std::ostream& os, const request& request)
{
    request.dump(os);
    return os;
}

}   // namespace http

#endif  // _http_request_hpp
