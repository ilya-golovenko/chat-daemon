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
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/parser.hpp>
#include <http/message.hpp>

// BOOST headers
#include <boost/logic/tribool.hpp>

// STL headers
#include <string>


namespace http
{

class headers_parser : public basic_parser<headers_parser>
{
public:
    headers_parser();

    headers_parser(headers_parser const&) = delete;
    headers_parser& operator=(headers_parser const&) = delete;

    void reset();

    boost::tribool consume(message& message, char c);

private:
    void set_message_header(message& message);

private:
    enum
    {
        state_header_line_start,
        state_header_whitespace,
        state_header_name,
        state_space_before_header_value,
        state_header_value_start,
        state_header_value,
        state_expecting_newline_1,
        state_expecting_newline_2
    } state_;

    std::string name_;
    std::string value_;
};

}   // namespace http

#endif  // _http_headers_parser_hpp
