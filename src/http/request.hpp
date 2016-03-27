//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 - 2016 Ilya Golovenko
//    This file is part of Chat.Daemon project
//
//    spchatd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spchatd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spchatd. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------
#ifndef _http_request_hpp
#define _http_request_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/message.hpp>
#include <http/url.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <string>
#include <vector>


namespace http
{

class request : public message
{
public:
    request();

    explicit request(version const& version);
    request(std::string const& method, std::string const& url);
    request(std::string const& method, std::string const& url, version const& version);

    virtual ~request() = default;

    request(request const&) = default;
    request& operator=(request const&) = default;

    request(request&&) = default;
    request& operator=(request&&) = default;

    std::string const& get_url() const;
    void set_url(std::string const& url);
    void set_url(url const& url);

    std::string const& get_method() const;
    void set_method(std::string const& method);

    void set_host(std::string const& host);
    void set_host(std::string const& host, std::uint16_t port);
    boost::optional<std::string> get_host() const;

    void set_user_agent(std::string const& user_agent);
    boost::optional<std::string> get_user_agent() const;

    void set_cookie(std::string const& cookie);
    boost::optional<std::string> get_cookie() const;

    void to_buffers(buffers& buffers) const;
    void dump(std::ostream& os) const;

private:
    std::string method_;
    std::string url_;
};

inline std::ostream& operator<<(std::ostream& os, request const& request)
{
    request.dump(os);
    return os;
}

}   // namespace http

#endif  // _http_request_hpp
