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
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/message.hpp>
#include <http/status.hpp>

// BOOST headers
#include <boost/asio.hpp>

// STL headers
#include <string>
#include <vector>


namespace http
{

class response : public message
{
public:
    response();

    explicit response(status const& status);
    response(const version& version, status const& status);

    virtual ~response();

    response(response const&) = default;
    response& operator=(response const&) = default;

    status const& get_status() const;
    void set_status(status const& status);

    void set_server(std::string const& server);
    boost::optional<std::string> get_server() const;

    void set_cookie(std::string const& cookie);
    boost::optional<std::string> get_cookie() const;

    void to_buffers(buffers& buffers) const;
    void dump(std::ostream& os) const;

    static response get_stock_response(status const& status, std::string const& server);

private:
    status status_;
};

inline std::ostream& operator<<(std::ostream& os, response const& response)
{
    response.dump(os);
    return os;
}

}   // namespace http

#endif  // _http_response_hpp
