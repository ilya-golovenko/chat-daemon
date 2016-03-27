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

// Application headers
#include <http/status.hpp>


namespace http
{
namespace reason_strings
{

static std::string const continue_                        = "Continue";
static std::string const switching_protocols              = "Switching Protocols";

static std::string const ok                               = "OK";
static std::string const created                          = "Created";
static std::string const accepted                         = "Accepted";
static std::string const non_authoritative                = "Non-Authoritative Information";
static std::string const no_content                       = "No Content";
static std::string const reset_content                    = "Reset Content";
static std::string const partial_content                  = "Partial Content";

static std::string const multiple_choices                 = "Multiple Choices";
static std::string const moved                            = "Moved";
static std::string const found                            = "Found";
static std::string const see_other                        = "See Other";
static std::string const not_modified                     = "Not Modified";
static std::string const use_proxy                        = "Use Proxy";
static std::string const temporary_redirect               = "Temporary Redirect";

static std::string const bad_request                      = "Bad Request";
static std::string const unauthorized                     = "Unauthorized";
static std::string const forbidden                        = "Forbidden";
static std::string const not_found                        = "Not Found";
static std::string const method_not_allowed               = "Method Not Allowed";
static std::string const not_acceptable                   = "Not Acceptable";
static std::string const proxy_authentication_required    = "Proxy Authentication Required";
static std::string const request_timeout                  = "Request Timeout";
static std::string const conflict                         = "Conflict";
static std::string const gone                             = "Gone";
static std::string const length_required                  = "Length Required";
static std::string const precondition_failed              = "Precondition Failed";
static std::string const request_entity_too_large         = "Request Entity Too Large";
static std::string const request_uri_too_long             = "Request URI Too Long";
static std::string const unsupported_media_type           = "Unsupported Media Type";
static std::string const requested_range_not_satisfiable  = "Requested Range Not Satisfiable";
static std::string const expectation_failed               = "Expectation Failed";

static std::string const internal_server_error            = "Internal Server Error";
static std::string const not_implemented                  = "Not Implemented";
static std::string const bad_gateway                      = "Bad Gateway";
static std::string const service_unavailable              = "Service Unavailable";
static std::string const gateway_timeout                  = "Gateway Timeout";
static std::string const version_not_supported            = "Version Not Supported";

std::string const& to_string(std::uint16_t code)
{
    switch(code)
    {
        // 1XX codes

        case status::continue_:
            return continue_;

        case status::switching_protocols:
            return switching_protocols;

        // 2XX codes

        case status::ok:
            return ok;

        case status::created:
            return created;

        case status::accepted:
            return accepted;

        case status::non_authoritative:
            return non_authoritative;

        case status::no_content:
            return no_content;

        case status::reset_content:
            return reset_content;

        case status::partial_content:
            return partial_content;

        // 3XX codes

        case status::multiple_choices:
            return multiple_choices;

        case status::moved:
            return moved;

        case status::found:
            return found;

        case status::see_other:
            return see_other;

        case status::not_modified:
            return not_modified;

        case status::use_proxy:
            return use_proxy;

        case status::temporary_redirect:
            return temporary_redirect;

        // 4XX codes

        case status::bad_request:
            return bad_request;

        case status::unauthorized:
            return unauthorized;

        case status::forbidden:
            return forbidden;

        case status::not_found:
            return not_found;

        case status::method_not_allowed:
            return method_not_allowed;

        case status::not_acceptable:
            return not_acceptable;

        case status::proxy_authentication_required:
            return proxy_authentication_required;

        case status::request_timeout:
            return request_timeout;

        case status::conflict:
            return conflict;

        case status::gone:
            return gone;

        case status::length_required:
            return length_required;

        case status::precondition_failed:
            return precondition_failed;

        case status::request_entity_too_large:
            return request_entity_too_large;

        case status::request_uri_too_long:
            return request_uri_too_long;

        case status::unsupported_media_type:
            return unsupported_media_type;

        case status::requested_range_not_satisfiable:
            return requested_range_not_satisfiable;

        case status::expectation_failed:
            return expectation_failed;

        // 5XX codes

        case status::internal_server_error:
            return internal_server_error;

        case status::not_implemented:
            return not_implemented;

        case status::bad_gateway:
            return bad_gateway;

        case status::service_unavailable:
            return service_unavailable;

        case status::gateway_timeout:
            return gateway_timeout;

        case status::version_not_supported:
            return version_not_supported;

        default:
            return internal_server_error;
    }
}

}   // namespace reason_strings

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
    return reason_strings::to_string( code_ );
}

bool status::is_provisional() const
{
    return code_ >= continue_ && code_ < ok;
}

bool status::is_successful() const
{
    return code_ >= ok && code_ < multiple_choices;
}

bool status::is_redirected() const
{
    return code_ >= multiple_choices && code_ < bad_request;
}

bool status::is_request_error() const
{
    return code_ >= bad_request && code_ < internal_server_error;
}

bool status::is_server_error() const
{
    return code_ >= internal_server_error;
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
