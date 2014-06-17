//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009 Ilya Golovenko
//    This file is part of spdaemon.
//
//    spdaemon is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spdaemon is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spdaemon. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------
#ifndef _frontend_manager_hpp
#define _frontend_manager_hpp

// Application headers
#include <http_server_connection.hpp>
#include <http_request.hpp>
#include <chat_forwards.hpp>
#include <config_common.hpp>
#include <utilities.hpp>
#include <log_common.hpp>

// BOOST headers
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <string>


class frontend_manager :
    private boost::noncopyable,
    public log_source<frontend_manager>
{
DEFINE_LOG_SOURCE_NAME("frontend manager")

public:
    frontend_manager(chat::server& server);
    void configure();

    void process_connection(http::server::connection::pointer connection);
    bool is_connected_via_frontend(http::server::connection::pointer connection) const;

private:
    void add_frontend_address(const std::string& address);

private:
    typedef boost::unordered_set<asio::ip::address,
        util::address_hash> address_set_type;

private:
    chat::server& server_;

    bool enabled_;
    std::string http_header_;
    address_set_type addresses_;
};

typedef boost::shared_ptr<frontend_manager> frontend_manager_ptr;

#endif  // _frontend_manager_hpp
