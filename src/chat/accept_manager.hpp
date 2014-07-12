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
#ifndef _chat_accept_manager_hpp
#define _chat_accept_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/server_connection.hpp>
#include <http/tcp_connection.hpp>

// BOOST headers
#include <boost/asio.hpp>

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
    typedef std::vector<boost::asio::ip::tcp::acceptor> acceptor_vector;
    typedef std::set<boost::asio::ip::tcp::endpoint> endpoint_vector;

public:
    explicit accept_manager(server_context& context);

    void configure(server_config const& config);

    void start();
    void stop();

private:
    void resolve_endpoint(std::string const& hostname, std::uint16_t port);
    void add_endpoint(boost::asio::ip::tcp::endpoint const& endpoint);

    void create_acceptor(boost::asio::ip::tcp::endpoint const& endpoint);
    void accept_connection(boost::asio::ip::tcp::acceptor& acceptor);

    void pass_connection_to_connection_manager(http::server_connection::pointer connection);
    void handle_accept(boost::asio::ip::tcp::acceptor& acceptor, http::tcp_connection::pointer connection, boost::system::error_code const& error);

private:
    server_context& context_;

    acceptor_vector acceptors_;
    endpoint_vector endpoints_;
};

}   // namespace chat

#endif  // _chat_accept_manager_hpp
