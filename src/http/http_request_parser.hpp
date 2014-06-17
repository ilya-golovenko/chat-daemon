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
#ifndef _http_request_parser_hpp
#define _http_request_parser_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "http_parser.hpp"
#include "http_request.hpp"
#include "http_version_parser.hpp"
#include "http_headers_parser.hpp"

// STL headers
#include <string>


namespace http
{

class request_parser :
    public parser<request_parser>
{
public:
    request_parser();
    void reset();

    boost::tribool consume(
        http::request& request, char c);

private:
    void set_request_method(http::request& request);
    void set_request_url(http::request& request);

private:
    enum states_type
    {
        state_method_start,
        state_method,
        state_url_start,
        state_url,
        state_http_version,
        state_expecting_newline,
        state_headers
    } state_;

private:
    std::size_t size_;
    std::string method_;
    std::string url_;
    http::version_parser version_parser_;
    http::headers_parser headers_parser_;
};

}   // namespace http

#endif  // _http_request_parser_hpp
