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
#ifndef _http_url_query_parser_hpp
#define _http_url_query_parser_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/url_query.hpp>
#include <http/url.hpp>

// STL headers
#include <string>


namespace http
{

class url_query_parser
{
public:
    url_query_parser();

    url_query_parser(url_query_parser const&) = delete;
    url_query_parser& operator=(url_query_parser const&) = delete;

    url_query parse(url const& url);

private:
    void reset();
    bool consume(char c);

private:
    enum
    {
        state_name_start,
        state_name,
        state_value
    } state_;

    std::string name_;
    std::string value_;

    url_query url_query_;
};

}   // namespace http

#endif  // _http_url_query_parser_hpp
