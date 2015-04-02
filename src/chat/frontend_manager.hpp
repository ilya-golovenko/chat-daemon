//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2014 Ilya Golovenko
//    This file is part of Chat.Daemon project
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
#ifndef _chat_frontend_manager_hpp
#define _chat_frontend_manager_hpp

// Application headers
#include <http/server_connection.hpp>
#include <misc/hash_utils.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <unordered_set>
#include <string>


namespace chat
{

class server_context;
struct server_config;

class frontend_manager
{
public:
    explicit frontend_manager(server_context& context);

    frontend_manager(frontend_manager const&) = delete;
    frontend_manager& operator=(frontend_manager const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

    void process_connection(http::server_connection::pointer connection);
    bool is_connected_via_frontend(http::server_connection::pointer connection) const;

private:
    typedef std::unordered_set<asio::ip::address> address_set;

private:
    server_context& context_;

    bool enabled_;
    std::string http_header_;
    address_set addresses_;
};

}   // namespace chat

#endif  // _chat_frontend_manager_hpp
