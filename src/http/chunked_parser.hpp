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
#ifndef _http_chunked_parser_hpp
#define _http_chunked_parser_hpp

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

class chunked_parser : public basic_parser<chunked_parser>
{
public:
    chunked_parser();

    chunked_parser(chunked_parser const&) = delete;
    chunked_parser& operator=(chunked_parser const&) = delete;

    void reset();

    boost::tribool consume(char c);

    std::size_t get_chunk_size() const;

private:
    void append_chunk_size_char(int c);

private:
    enum
    {
        state_chunk_size_start,
        state_chunk_size,
        state_expecting_newline_1,
        state_expecting_newline_2,
        state_expecting_newline_3
    } state_;

    std::size_t size_;
    std::size_t chunk_size_;
};

}   // namespace http

#endif  // _http_chunked_parser_hpp
