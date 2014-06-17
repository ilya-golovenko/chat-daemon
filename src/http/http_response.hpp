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
#ifndef _http_response_hpp
#define _http_response_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "http_message.hpp"
#include "http_status.hpp"

// STL headers
#include <string>
#include <vector>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/lexical_cast.hpp>


namespace http
{

class response :
    public message
{
public:
    response();

    explicit response(http::status status);
    response(const version& version, http::status status);

    virtual ~response();

    response(const response& other);
    response& operator=(const response& other);

    void assign(const response& other);

    http::status get_status() const;
    void set_status(http::status status);

    void set_server(const std::string& server);
    boost::optional<std::string> get_server() const;

    void set_cookie(const std::string& cookie);
    boost::optional<std::string> get_cookie() const;

    void to_buffers(buffers_type& buffers) const;
    void dump(std::ostream& os) const;

    static const http::response stock_response(
        http::status status, const std::string& server);

private:
    http::status status_;
};

inline std::ostream& operator<<(std::ostream& os, const response& response)
{
    response.dump(os);
    return os;
}

}   // namespace http

#endif  // _http_response_hpp
