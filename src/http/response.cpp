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
#include <http/response.hpp>
#include <http/mime_types.hpp>
#include <http/common.hpp>


namespace http
{
namespace stock_responses
{

std::string const msie_padding =
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n";

std::string const tail_begin =
    "<hr><center>";

std::string const tail_end =
    "</center>\r\n"
    "</body>\r\n"
    "</html>\r\n";

std::string const bad_request =
    "<html>\r\n"
    "<head><title>400 Bad Request</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>400 Bad Request</h1><center>\r\n";

std::string const access_denied =
    "<html>\r\n"
    "<head><title>401 Access Denied</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>401 Access Denied</h1><center>\r\n";

std::string const forbidden =
    "<html>\r\n"
    "<head><title>403 Forbidden</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>403 Forbidden</h1><center>\r\n";

std::string const not_found =
    "<html>\r\n"
    "<head><title>404 Not Found</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>404 Not Found</h1><center>\r\n";

std::string const bad_method =
    "<html>\r\n"
    "<head><title>405 Bad Method</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>405 Bad Method</h1><center>\r\n";

std::string const request_timeout =
    "<html>\r\n"
    "<head><title>408 Request Timeout</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>408 Request Timeout</h1><center>\r\n";

std::string const internal_server_error =
    "<html>\r\n"
    "<head><title>500 Internal Server Error</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>500 Internal Server Error</h1><center>\r\n";

std::string const service_unavailable =
    "<html>\r\n"
    "<head><title>503 Service Unavailable</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>503 Service Unavailable</h1><center>\r\n";

std::string const version_not_supported =
    "<html>\r\n"
    "<head><title>505 Version Not Supported</title></head>\r\n"
    "<body bgcolor=\"white\">\r\n"
    "<center><h1>505 Version Not Supported</h1><center>\r\n";

std::string const& to_string(status const& status)
{
    switch(status.get_code())
    {
        case status::bad_request:
            return bad_request;

        case status::access_denied:
            return access_denied;

        case status::forbidden:
            return forbidden;

        case status::bad_method:
            return bad_method;

        case status::request_timeout:
            return request_timeout;

        case status::internal_server_error:
            return internal_server_error;

        case status::service_unavailable:
            return service_unavailable;

        case status::version_not_supported:
            return version_not_supported;

        default:
            return internal_server_error;
    }
}

std::string get_tail(std::string const& server)
{
    return tail_begin + server + tail_end;
}

}   // namespace stock_responses 

response::response() :
    status_(status::ok)
{
}

response::response(status const& status) :
    status_(status)
{
}

response::response(version const& version, status const& status) :
    message(version),
    status_(status)
{
}

response::~response()
{
}

status const& response::get_status() const
{
    return status_;
}

void response::set_status(status const& status)
{
    status_ = status;
}

void response::set_server(std::string const& server)
{
    set(header_names::server, server);
}

boost::optional<std::string> response::get_server() const
{
    return get(header_names::server);
}

void response::set_cookie(std::string const& cookie)
{
    set(header_names::set_cookie, cookie);
}

boost::optional<std::string> response::get_cookie() const
{
    return get(header_names::set_cookie);
}

void response::to_buffers(buffers& buffers) const
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

response response::get_stock_response(status const& status, std::string const& server)
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
