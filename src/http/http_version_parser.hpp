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
#ifndef _http_version_parser_hpp
#define _http_version_parser_hpp

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

class version_parser :
    public parser<version_parser>
{
public:
    version_parser();
    void reset();

    boost::tribool consume(
        http::message& message, char c);

private:
    void set_message_version(http::message& message);

private:
    enum states_type
    {
        state_http_version_h,
        state_http_version_t_1,
        state_http_version_t_2,
        state_http_version_p,
        state_http_version_slash,
        state_http_version_major_start,
        state_http_version_major,
        state_http_version_minor_start,
        state_http_version_minor
    } state_;

private:
    std::size_t major_;
    std::size_t minor_;
};

}   // namespace http

#endif  // _http_version_parser_hpp
