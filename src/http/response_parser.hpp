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
#ifndef _http_response_parser_hpp
#define _http_response_parser_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/parser.hpp>
#include <http/response.hpp>
#include <http/version_parser.hpp>
#include <http/headers_parser.hpp>

// STL headers
#include <string>


namespace http
{

class response_parser : public basic_parser<response_parser>
{
public:
    response_parser();

    response_parser(response_parser const&) = delete;
    response_parser& operator=(response_parser const&) = delete;

    void reset();

    parse_result consume(response& response, char c);

private:
    void set_response_status(response& response);

private:
    enum
    {
        state_version,
        state_status_code_start,
        state_status_code,
        state_status_reason,
        state_expecting_newline,
        state_headers
    } state_;

    std::size_t size_;
    std::uint16_t status_;

    version_parser version_parser_;
    headers_parser headers_parser_;
};

}   // namespace http

#endif  // _http_response_parser_hpp
