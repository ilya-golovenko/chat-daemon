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
#ifndef _chat_connection_dispatcher_hpp
#define _chat_connection_dispatcher_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_forwards.hpp"
#include <http_request.hpp>
#include <http_response.hpp>
#include <http_server_connection.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// STL headers
#include <string>


namespace chat
{

class connection_dispatcher :
    private boost::noncopyable,
    public boost::enable_shared_from_this<connection_dispatcher>
{
public:
    connection_dispatcher(chat::server& server);
    void configure();

    void process_connection(http::server::connection::pointer connection);

private:
    void create_chat_header();
    void create_client_http_response();
    void create_document_domain_fix_script();

    std::pair<std::string, bool> process_request(const http::request& request);

    const http::write_handler bind_to_write_handler();
    const http::read_handler bind_to_read_handler(http::server::connection::pointer connection);

    void handle_write(const asio::error_code& error, std::size_t bytes_transferred);
    void handle_read(http::server::connection::pointer, const asio::error_code& error, std::size_t bytes_transferred);

private:
    chat::server& server_;
    http::buffer chat_header_;
    http::buffer daemon_banner_;
    http::buffer reload_irc_frame_;
    http::response client_response_;
    http::buffer document_domain_fix_;
};

typedef boost::shared_ptr<connection_dispatcher> connection_dispatcher_ptr;

}   // namespace chat

#endif  // _chat_connection_dispatcher_hpp
