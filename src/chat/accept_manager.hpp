//---------------------------------------------------------------------------
//
//    Copyright (C) 2008 - 2016 Ilya Golovenko
//    This file is part of Chat.Daemon project
//
//    spchatd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spchatd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spchatd. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------
#ifndef _chat_accept_manager_hpp
#define _chat_accept_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/server_connection.hpp>
#include <http/tcp_connection.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <string>
#include <vector>
#include <set>


namespace chat
{

class server_context;
struct server_config;

class accept_manager
{
public:
    typedef std::vector<asio::ip::tcp::acceptor> acceptor_vector;
    typedef std::set<asio::ip::tcp::endpoint> endpoint_vector;

public:
    explicit accept_manager(server_context& context);

    void configure(server_config const& config);

    void start();
    void stop();

private:
    void resolve_endpoint(std::string const& hostname, std::uint16_t port);
    void add_endpoint(asio::ip::tcp::endpoint const& endpoint);

    void create_acceptor(asio::ip::tcp::endpoint const& endpoint);
    void accept_connection(asio::ip::tcp::acceptor& acceptor);

    void handle_connection(http::server_connection::pointer connection);

    void handle_accept(asio::ip::tcp::acceptor& acceptor, tcp::connection& connection, asio::error_code const& error);

private:
    server_context& context_;

    acceptor_vector acceptors_;
    endpoint_vector endpoints_;
};

}   // namespace chat

#endif  // _chat_accept_manager_hpp
