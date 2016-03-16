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
#ifndef _chat_cron_connection_hpp
#define _chat_cron_connection_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/server_context.hpp>
#include <http/client_connection.hpp>
#include <http/request.hpp>

// ASIO headers
#include <asio/steady_timer.hpp>
#include <asio.hpp>

// STL headers
#include <string>
#include <chrono>


namespace chat
{

class cron_connection
{
public:
    explicit cron_connection(server_context& context);

    cron_connection(cron_connection const&) = delete;
    cron_connection& operator=(cron_connection const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

private:
    http::completion_handler bind_to_timer_hander();
    http::completion_handler bind_to_completion_hander();

    void handle_timer(asio::error_code const& error);
    void handle_complete(asio::error_code const& error);

private:
    server_context& context_;

    std::chrono::seconds interval_;
    asio::steady_timer timer_;

    http::request cron_script_request_;
    http::client_connection::pointer connection_;
};

}   // namespace chat

#endif  // _chat_cron_connection_hpp
