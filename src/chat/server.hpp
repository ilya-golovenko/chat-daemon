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
#ifndef _chat_server_hpp
#define _chat_server_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <chat/cron_connection.hpp>
#include <chat/common.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <thread>


namespace chat
{

class server
{
public:
    server();

    server(server const&) = delete;
    server& operator=(server const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

    asio::io_service& get_io_service();

private:
    void run();

    void deliver_stop_message();
    void handle_stop_request();

private:
    std::thread worker_thread_;

    asio::io_service io_service_;

    server_context context_;

    cron_connection cron_connection_;
};

}   // namespace chat

#endif  // _chat_server_hpp
