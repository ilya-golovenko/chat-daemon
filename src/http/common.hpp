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
#ifndef _http_common_hpp
#define _http_common_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/asio.hpp>

// STL headers
#include <functional>
#include <string>


namespace http
{

// forward declarations
class buffer;
class response;

namespace strings
{

std::string const empty;

std::string const slash                 = "/";
std::string const space                 = " ";
std::string const separator             = ": ";
std::string const crlf                  = "\r\n"; 

}   // namespace strings

namespace request_methods
{

std::string const get                   = "GET";
std::string const post                  = "POST";
std::string const head                  = "HEAD";

}   // namespace request_methods

namespace header_names
{

// request-only headers
std::string const host                  = "Host";
std::string const range                 = "Range";
std::string const accept                = "Accept";
std::string const referer               = "Referer";
std::string const user_agent            = "User-Agent";
std::string const set_cookie            = "Set-Cookie";
std::string const proxy_authorization   = "Proxy-Authorization";

// response-only headers
std::string const server                = "Server";
std::string const cookie                = "Cookie";
std::string const location              = "Location";
std::string const content_range         = "Content-Range";
std::string const proxy_authenticate    = "Proxy-Authenticate";

// general headers
std::string const date                  = "Date";
std::string const pragma                = "Pragma";
std::string const connection            = "Connection";
std::string const content_type          = "Content-Type";
std::string const cache_control         = "Cache-Control";
std::string const content_length        = "Content-Length";
std::string const proxy_connection      = "Proxy-Connection";
std::string const transfer_encoding     = "Transfer-Encoding";

}   // namespace header_names

namespace cache_tokens
{

std::string const no_cache              = "no-cache";
std::string const no_store              = "no-store";

}   // namespace cache_tokens

namespace accept_tokens
{

std::string const any                   = "*/*";

}   // namespace accept_tokens

namespace connection_tokens
{

std::string const close                 = "close";
std::string const keep_alive            = "keep-alive";

}   // namespace connection_tokens

namespace encoding_tokens
{

std::string const chunked               = "chunked";
std::string const identity              = "identity";
std::string const compress              = "compress";
std::string const deflate               = "deflate";
std::string const gzip                  = "gzip";

}   // namespace encoding_tokens

std::size_t const max_redirect_count    = 5;

std::uint16_t const protocol_port       = 80;
std::string const protocol_name         = "http";

typedef std::function<void (boost::system::error_code const&)> stop_handler;
typedef std::function<void (boost::system::error_code const&)> timer_handler;
typedef std::function<void (boost::system::error_code const&)> completion_handler;
typedef std::function<void (boost::system::error_code const&, std::size_t)> read_handler;
typedef std::function<void (boost::system::error_code const&, std::size_t)> write_handler;


}   // namespace http

#endif  // _http_common_hpp
