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
#ifndef _chat_accept_manager_hpp
#define _chat_accept_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_forwards.hpp"
#include <config_common.hpp>
#include <tcp_connection.hpp>
#include <http_server_connection.hpp>
#include <io_thread.hpp>
#include <log_common.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// STL headers
#include <string>
#include <vector>
#include <set>


namespace chat
{

class accept_manager :
    private boost::noncopyable,
    public log_source<accept_manager>,
    public boost::enable_shared_from_this<accept_manager>
{
DEFINE_LOG_SOURCE_NAME("accept manager")

public:
    typedef boost::shared_ptr<asio::ip::tcp::acceptor> acceptor_ptr;

    typedef std::vector<acceptor_ptr> acceptor_vector_type;
    typedef std::set<asio::ip::tcp::endpoint> endpoint_vector_type;

public:
    accept_manager(chat::server& server);
    ~accept_manager();

    void configure();
    void start();
    void stop();

private:
    void resolve_endpoint(const std::string& endpoint);
    void add_endpoint(const asio::ip::tcp::endpoint& endpoint);

    acceptor_ptr create_new_acceptor(const asio::ip::tcp::endpoint& endpoint);
    void create_acceptor(const asio::ip::tcp::endpoint& endpoint);
    void accept_connection(acceptor_ptr acceptor);

    void pass_connection_to_connection_dispatcher(http::server::connection::pointer connection);
    void handle_accept(acceptor_ptr acceptor, tcp::connection::pointer connection, const asio::error_code& error);

private:
    chat::server& server_;
    util::io_thread io_thread_;

    acceptor_vector_type acceptors_;
    endpoint_vector_type endpoints_;
};

typedef boost::shared_ptr<accept_manager> accept_manager_ptr;

}   // namespace chat

#endif  // _chat_accept_manager_hpp
