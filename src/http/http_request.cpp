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

// Application headers
#include "http_request.hpp"
#include "http_common.hpp"

// BOOST headers
#include <boost/lexical_cast.hpp>


namespace http
{

request::request() :
    method_(request_methods::get),
    url_(strings::slash)
{
}

request::request(const version& version) :
    message(version),
    method_(request_methods::get),
    url_(strings::slash)
{
}

request::request(const std::string& method, const std::string& url) :
    method_(method),
    url_(url)
{
}

request::request(const std::string& method, const std::string& url, const version& version) :
    message(version),
    method_(method),
    url_(url)
{
}

request::~request()
{
}

request::request(const request& other) :
    message(other)
{
    assign(other);
}

request& request::operator=(const request& other)
{
    assign(other);
    return *this;
}

void request::assign(const request& other)
{
    if(&other != this)
    {
        url_ = other.url_;
        method_ = other.method_;
        message::assign(other);
    }
}

const std::string& request::get_url() const
{
    return url_;
}

void request::set_url(const std::string& url)
{
    url_ = url;
}

void request::set_url(const http::url& url)
{
    url_ = url.to_string();
}

const std::string& request::get_method() const
{
    return method_;
}

void request::set_method(const std::string& method)
{
    method_ = method;
}

void request::set_host(const std::string& host)
{
    set(header_names::host, host);
}

void request::set_host(const std::string& host, unsigned short port)
{
    std::string value = host;

    if(protocol_port != port)
        value += ":" + boost::lexical_cast<std::string>(port);

    set_host(value);
}

boost::optional<std::string> request::get_host() const
{
    return get(header_names::host);
}

void request::set_user_agent(const std::string& user_agent)
{
    set(header_names::user_agent, user_agent);
}

boost::optional<std::string> request::get_user_agent() const
{
    return get(header_names::user_agent);
}

void request::set_cookie(const std::string& cookie)
{
    set(header_names::cookie, cookie);
}

boost::optional<std::string> request::get_cookie() const
{
    return get(header_names::cookie);
}

void request::to_buffers(buffers_type& buffers) const
{
    buffers.push_back(asio::buffer(method_));
    buffers.push_back(asio::buffer(strings::space));
    buffers.push_back(asio::buffer(url_));
    buffers.push_back(asio::buffer(strings::space));
    buffers.push_back(asio::buffer(get_version().to_string()));
    buffers.push_back(asio::buffer(strings::crlf));

    message::to_buffers(buffers);
}

void request::dump(std::ostream& os) const
{
    os << method_;
    os << strings::space;
    os << url_;
    os << strings::space;
    os << get_version();
    os << std::endl;

    message::dump(os);
}

}   // namespace http
