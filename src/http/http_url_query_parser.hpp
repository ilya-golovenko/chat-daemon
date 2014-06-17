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
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "http_url_query.hpp"
#include "http_url.hpp"

// BOOST headers
#include <boost/noncopyable.hpp>

// STL headers
#include <stdexcept>
#include <string>
#include <map>


namespace http
{

class url_query_parser :
    private boost::noncopyable
{
public:
    url_query_parser();

    const http::url_query parse(const http::url& url);

private:
    void reset();
    bool consume(char c);
    void add_query_parameter();

private:
    enum
    {
        state_name_start,
        state_name,
        state_value
    } state_;

    std::string name_;
    std::string value_;

    http::url_query url_query_;
};

}   // namespace http

#endif  // _http_url_query_parser_hpp
