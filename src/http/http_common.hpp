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
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/function.hpp>

// STL headers
#include <string>


namespace http
{

// forward declarations
class buffer;
class response;

namespace strings
{

const std::string empty;

const std::string slash                 = "/";
const std::string space                 = " ";
const std::string separator             = ": ";
const std::string crlf                  = "\r\n"; 

}   // namespace strings

namespace request_methods
{

const std::string get                   = "GET";
const std::string post                  = "POST";
const std::string head                  = "HEAD";

}   // namespace request_methods

namespace header_names
{

// request-only headers
const std::string host                  = "Host";
const std::string range                 = "Range";
const std::string accept                = "Accept";
const std::string referer               = "Referer";
const std::string user_agent            = "User-Agent";
const std::string set_cookie            = "Set-Cookie";
const std::string proxy_authorization   = "Proxy-Authorization";

// response-only headers
const std::string server                = "Server";
const std::string cookie                = "Cookie";
const std::string location              = "Location";
const std::string content_range         = "Content-Range";
const std::string proxy_authenticate    = "Proxy-Authenticate";

// general headers
const std::string date                  = "Date";
const std::string pragma                = "Pragma";
const std::string connection            = "Connection";
const std::string content_type          = "Content-Type";
const std::string cache_control         = "Cache-Control";
const std::string content_length        = "Content-Length";
const std::string proxy_connection      = "Proxy-Connection";
const std::string transfer_encoding     = "Transfer-Encoding";

}   // namespace header_names

namespace cache_tokens
{

const std::string no_cache              = "no-cache";
const std::string no_store              = "no-store";

}   // namespace cache_tokens

namespace accept_tokens
{

const std::string any                   = "*/*";

}   // namespace accept_tokens

namespace connection_tokens
{

const std::string close                 = "close";
const std::string keep_alive            = "keep-alive";

}   // namespace connection_tokens

namespace encoding_tokens
{

const std::string chunked               = "chunked";
const std::string indentity             = "indentity";
const std::string compress              = "compress";
const std::string deflate               = "deflate";
const std::string gzip                  = "gzip";

}   // namespace transfer_tokens

const std::size_t max_redirect_count    = 5;

const unsigned short protocol_port      = 80;
const std::string protocol_name         = "http";

typedef boost::function<void (const asio::error_code&)> timer_handler;

typedef boost::function<void (const asio::error_code&, std::size_t)> read_handler;
typedef boost::function<void (const asio::error_code&, std::size_t)> write_handler;

typedef boost::function<void (const http::buffer&)> data_handler;
typedef boost::function<void (const http::response&)> response_handler;
typedef boost::function<void (const asio::error_code&, std::size_t)> completion_handler;

}   // namespace http

#endif  // _http_common_hpp
