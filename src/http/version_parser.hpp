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
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/parser.hpp>
#include <http/message.hpp>

// STL headers
#include <cstdint>
#include <string>


namespace http
{

class version_parser : public basic_parser<version_parser>
{
public:
    version_parser();

    version_parser(version_parser const&) = delete;
    version_parser& operator=(version_parser const&) = delete;

    void reset();

    parse_result consume(message& message, char c);

private:
    enum
    {
        state_version_h,
        state_version_t_1,
        state_version_t_2,
        state_version_p,
        state_version_slash,
        state_version_major_start,
        state_version_major,
        state_version_minor_start,
        state_version_minor
    } state_;

    std::uint8_t major_;
    std::uint8_t minor_;
};

}   // namespace http

#endif  // _http_version_parser_hpp
