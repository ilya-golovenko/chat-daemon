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
#include <http/request.hpp>
#include <http/common.hpp>


namespace http
{

request::request() :
    method_(request_methods::get),
    url_(strings::slash)
{
}

request::request(version const& version) :
    message(version),
    method_(request_methods::get),
    url_(strings::slash)
{
}

request::request(std::string const& method, std::string const& url) :
    method_(method),
    url_(url)
{
}

request::request(std::string const& method, std::string const& url, const version& version) :
    message(version),
    method_(method),
    url_(url)
{
}

request::~request()
{
}

std::string const& request::get_url() const
{
    return url_;
}

void request::set_url(std::string const& url)
{
    url_ = url;
}

void request::set_url(url const& url)
{
    url_ = url.to_string();
}

std::string const& request::get_method() const
{
    return method_;
}

void request::set_method(std::string const& method)
{
    method_ = method;
}

void request::set_host(std::string const& host)
{
    set(header_names::host, host);
}

void request::set_host(std::string const& host, std::uint16_t port)
{
    std::string value = host;

    if(protocol_port != port)
        value += ":" + std::to_string(port);

    set_host(value);
}

boost::optional<std::string> request::get_host() const
{
    return get(header_names::host);
}

void request::set_user_agent(std::string const& user_agent)
{
    set(header_names::user_agent, user_agent);
}

boost::optional<std::string> request::get_user_agent() const
{
    return get(header_names::user_agent);
}

void request::set_cookie(std::string const& cookie)
{
    set(header_names::cookie, cookie);
}

boost::optional<std::string> request::get_cookie() const
{
    return get(header_names::cookie);
}

void request::to_buffers(buffers& buffers) const
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
