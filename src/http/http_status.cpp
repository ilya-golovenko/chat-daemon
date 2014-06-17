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
#include "http_status.hpp"


namespace http
{
namespace reason_strings
{

const std::string ok                = "200 OK";
const std::string created           = "201 Created";
const std::string accepted          = "202 Accepted";
const std::string partial_content   = "206 Partial Content";
const std::string moved_permanently = "301 Moved Permanently";
const std::string moved_temporarily = "302 Moved Temporarily";
const std::string not_modified      = "304 Not Modified";
const std::string bad_request       = "400 Bad Request";
const std::string access_denied     = "401 Access Denied";
const std::string forbidden         = "403 Forbidden";
const std::string not_found         = "404 Not Found";
const std::string bad_method        = "405 Bad Method";
const std::string proxy_auth_req    = "407 Proxy Auth Required";
const std::string request_timeout   = "408 Request Timeout";
const std::string server_error      = "500 Internal Server Error";
const std::string service_unavail   = "503 Service Unavailable";
const std::string version_not_sup   = "505 Version Not Supported";

}   // namespace reason_strings

status::status() :
    code_(ok)
{
}

status::status(codes_type code) :
    code_(code)
{
}

status::status(const status& other) :
    code_(other.code_)
{
}

status& status::operator=(const status& other)
{
    if(&other != this)
        code_ = other.code_;
    return *this;
}

status::codes_type status::code() const
{
    return code_;
}

const std::string& status::to_string() const
{
    switch(code_)
    {
        case ok:
            return reason_strings::ok;

        case created:
            return reason_strings::created;

        case accepted:
            return reason_strings::accepted;

        case partial_content:
            return reason_strings::partial_content;

        case moved_permanently:
            return reason_strings::moved_permanently;

        case moved_temporarily:
            return reason_strings::moved_temporarily;

        case not_modified:
            return reason_strings::not_modified;

        case bad_request:
            return reason_strings::bad_request;

        case access_denied:
            return reason_strings::access_denied;

        case forbidden:
            return reason_strings::forbidden;

        case not_found:
            return reason_strings::not_found;

        case bad_method:
            return reason_strings::bad_method;

        case proxy_auth_req:
            return reason_strings::proxy_auth_req;

        case request_timeout:
            return reason_strings::request_timeout;

        case server_error:
            return reason_strings::server_error;

        case service_unavail:
            return reason_strings::service_unavail;

        case version_not_sup:
            return reason_strings::version_not_sup;

        default:
            return reason_strings::server_error;
    }
}

bool operator==(const status& lhs, const status& rhs)
{
    return (lhs.code() == rhs.code());
}

bool operator<(const status& lhs, const status& rhs)
{
    return (lhs.code() < rhs.code());
}

}   // namespace http
