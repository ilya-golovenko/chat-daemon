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
#ifndef _http_headers_parser_hpp
#define _http_headers_parser_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "http_parser.hpp"
#include "http_message.hpp"

// STL headers
#include <string>


namespace http
{

class headers_parser :
    public parser<headers_parser>
{
public:
    headers_parser();
    void reset();

    boost::tribool consume(
        http::message& message, char c);

private:
    void set_message_header(http::message& message);

private:
    enum states_type
    {
        state_header_line_start,
        state_header_whitespace,
        state_header_name,
        state_space_before_header_value,
        state_header_value,
        state_expecting_newline_1,
        state_expecting_newline_2
    } state_;

private:
    std::string name_;
    std::string value_;
};

}   // namespace http

#endif  // _http_headers_parser_hpp
