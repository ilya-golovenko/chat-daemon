//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2015 Ilya Golovenko
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
#ifndef _chat_connection_manager_hpp
#define _chat_connection_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/session_id.hpp>
#include <http/server_connection.hpp>
#include <http/response.hpp>
#include <http/request.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <cstddef>


namespace chat
{

class server_context;
struct server_config;

class connection_manager
{
public:
    explicit connection_manager(server_context& context);

    connection_manager(connection_manager const&) = delete;
    connection_manager& operator=(connection_manager const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

    void process_connection(http::server_connection::pointer connection);

private:
    void create_client_http_response();

    session_id extract_session_id(http::request const& request);

    http::completion_handler bind_to_write_handler();
    http::completion_handler bind_to_read_handler(http::server_connection::pointer connection);

    void handle_write(asio::error_code const& error);
    void handle_read(http::server_connection::pointer, asio::error_code const& error);

private:
    server_context& context_;
    std::string session_name_;
    std::string session_path_;
    http::buffer chat_header_;
    http::buffer daemon_banner_;
    http::buffer reload_irc_frame_;
    http::response client_response_;
    http::buffer document_domain_fix_;
    std::chrono::seconds connect_timeout_;
};

}   // namespace chat

#endif  // _chat_connection_manager_hpp
