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
#include <http/status.hpp>


namespace http
{
namespace strings
{

std::string const ok                    = "200 OK";
std::string const created               = "201 Created";
std::string const accepted              = "202 Accepted";
std::string const partial_content       = "206 Partial Content";
std::string const moved_permanently     = "301 Moved Permanently";
std::string const moved_temporarily     = "302 Moved Temporarily";
std::string const not_modified          = "304 Not Modified";
std::string const bad_request           = "400 Bad Request";
std::string const access_denied         = "401 Access Denied";
std::string const forbidden             = "403 Forbidden";
std::string const not_found             = "404 Not Found";
std::string const bad_method            = "405 Bad Method";
std::string const proxy_auth_required   = "407 Proxy Auth Required";
std::string const request_timeout       = "408 Request Timeout";
std::string const internal_server_error = "500 Internal Server Error";
std::string const service_unavailable   = "503 Service Unavailable";
std::string const version_not_supported = "505 Version Not Supported";

}   // namespace strings

status::status() :
    code_(ok)
{
}

status::status(std::uint16_t code) :
    code_(code)
{
}

std::uint16_t status::get_code() const
{
    return code_;
}

std::string const& status::to_string() const
{
    switch(code_)
    {
        case ok:
            return strings::ok;

        case created:
            return strings::created;

        case accepted:
            return strings::accepted;

        case partial_content:
            return strings::partial_content;

        case moved_permanently:
            return strings::moved_permanently;

        case moved_temporarily:
            return strings::moved_temporarily;

        case not_modified:
            return strings::not_modified;

        case bad_request:
            return strings::bad_request;

        case access_denied:
            return strings::access_denied;

        case forbidden:
            return strings::forbidden;

        case not_found:
            return strings::not_found;

        case bad_method:
            return strings::bad_method;

        case proxy_auth_required:
            return strings::proxy_auth_required;

        case request_timeout:
            return strings::request_timeout;

        case internal_server_error:
            return strings::internal_server_error;

        case service_unavailable:
            return strings::service_unavailable;

        case version_not_supported:
            return strings::version_not_supported;

        default:
            return strings::internal_server_error;
    }
}

bool operator==(status const& lhs, status const& rhs)
{
    return lhs.get_code() == rhs.get_code();
}

bool operator<(status const& lhs, status const& rhs)
{
    return lhs.get_code() < rhs.get_code();
}

}   // namespace http
