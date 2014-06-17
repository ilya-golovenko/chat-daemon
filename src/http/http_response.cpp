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
#include "http_response.hpp"
#include "http_mime_types.hpp"
#include "http_common.hpp"


namespace http
{
namespace stock_responses
{

const std::string msie_padding =
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n";

const std::string tail_begin =
    "<hr><center>";

const std::string tail_end =
    "</center>\r\n"
    "</body>\r\n"
    "</html>\r\n";

const std::string bad_request =
    "<html>\r\n"
    "<head><title>400 Bad Request</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>400 Bad Request</h1><center>\r\n";

const std::string access_denied =
    "<html>\r\n"
    "<head><title>401 Access Denied</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>401 Access Denied</h1><center>\r\n";

const std::string forbidden =
    "<html>\r\n"
    "<head><title>403 Forbidden</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>403 Forbidden</h1><center>\r\n";

const std::string not_found =
    "<html>\r\n"
    "<head><title>404 Not Found</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>404 Not Found</h1><center>\r\n";

const std::string bad_method =
    "<html>\r\n"
    "<head><title>405 Bad Method</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>405 Bad Method</h1><center>\r\n";

const std::string request_timeout =
    "<html>\r\n"
    "<head><title>408 Request Timeout</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>408 Request Timeout</h1><center>\r\n";

const std::string server_error =
    "<html>\r\n"
    "<head><title>500 Internal Server Error</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>500 Internal Server Error</h1><center>\r\n";

const std::string service_unavail =
    "<html>\r\n"
    "<head><title>503 Service Unavailable</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>503 Service Unavailable</h1><center>\r\n";

const std::string version_not_sup =
    "<html>\r\n"
    "<head><title>505 Version Not Supported</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>505 Version Not Supported</h1><center>\r\n";

const std::string& to_string(http::status status)
{
    switch(status.code())
    {
        case http::status::bad_request:
            return bad_request;

        case http::status::access_denied:
            return access_denied;

        case http::status::forbidden:
            return forbidden;

        case http::status::bad_method:
            return bad_method;

        case http::status::request_timeout:
            return request_timeout;

        case http::status::server_error:
            return server_error;

        case http::status::service_unavail:
            return service_unavail;

        case http::status::version_not_sup:
            return version_not_sup;

        default:
            return server_error;
    }
}

const std::string get_tail(const std::string& server)
{
    return (tail_begin + server + tail_end);
}

}   // namespace stock_responses 

response::response() :
    status_(http::status::ok)
{
}

response::response(http::status status) :
    status_(status)
{
}

response::response(const version& version, http::status status) :
    message(version),
    status_(status)
{
}

response::~response()
{
}

response::response(const response& other) :
    message(other),
    status_(other.status_)
{
}

response& response::operator=(const response& other)
{
    assign(other);
    return *this;
}

void response::assign(const response& other)
{
    if(&other != this)
    {
        status_ = other.status_;
        message::assign(other);
    }
}

http::status response::get_status() const
{
    return status_;
}

void response::set_status(http::status status)
{
    status_ = status;
}

void response::set_server(const std::string& server)
{
    set(header_names::server, server);
}

boost::optional<std::string> response::get_server() const
{
    return get(header_names::server);
}

void response::set_cookie(const std::string& cookie)
{
    set(header_names::set_cookie, cookie);
}

boost::optional<std::string> response::get_cookie() const
{
    return get(header_names::set_cookie);
}

void response::to_buffers(buffers_type& buffers) const
{
    buffers.push_back(asio::buffer(version_.to_string()));
    buffers.push_back(asio::buffer(strings::space));
    buffers.push_back(asio::buffer(status_.to_string()));
    buffers.push_back(asio::buffer(strings::crlf));

    message::to_buffers(buffers);
}

void response::dump(std::ostream& os) const
{
    os << version_;
    os << strings::space;
    os << status_;
    os << std::endl;

    message::dump(os);
}

const response response::stock_response(http::status status, const std::string& server)
{
    response response(status);

    response.append_body(stock_responses::to_string(status));
    response.append_body(stock_responses::get_tail(server));
    response.append_body(stock_responses::msie_padding);

    response.set_content_type(mime_types::text_html);
    response.set_body_content_length();
    response.set_keep_alive(false);
    response.set_server(server);

    return response;
}

}   // namespace http
