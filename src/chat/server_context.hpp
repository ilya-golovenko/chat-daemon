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
#ifndef _chat_server_context_hpp
#define _chat_server_context_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/skin_manager.hpp>
#include <chat/user_manager.hpp>
#include <chat/room_manager.hpp>
#include <chat/ignore_manager.hpp>
#include <chat/accept_manager.hpp>
#include <chat/filter_manager.hpp>
#include <chat/frontend_manager.hpp>
#include <chat/connection_manager.hpp>
#include <chat/statistics_manager.hpp>
#include <chat/message_parser.hpp>
#include <chat/data_file_reader.hpp>
#include <chat/server_config.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <utility>
#include <random>


namespace chat
{

class server_context
{
public:
    explicit server_context(asio::io_service& io_service);

    server_context(server_context const&) = delete;
    server_context& operator=(server_context const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

    asio::io_service& get_io_service();

    template <typename Handler>
    void dispatch(Handler&& handler);

    std::uint32_t generate_random_number();

    skin_manager& get_skin_manager();
    user_manager& get_user_manager();
    room_manager& get_room_manager();
    ignore_manager& get_ignore_manager();
    accept_manager& get_accept_manager();
    filter_manager& get_filter_manager();
    frontend_manager& get_frontend_manager();
    connection_manager& get_connection_manager();
    statistics_manager& get_statistics_manager();

    message_parser& get_message_parser();
    data_file_reader& get_data_file_reader();

private:
    asio::io_service& io_service_;

    std::random_device random_device_;
    std::minstd_rand random_engine_;

    skin_manager skin_manager_;
    user_manager user_manager_;
    room_manager room_manager_;
    ignore_manager ignore_manager_;
    accept_manager accept_manager_;
    filter_manager filter_manager_;
    frontend_manager frontend_manager_;
    connection_manager connection_manager_;
    statistics_manager statistics_manager_;

    message_parser message_parser_;
    data_file_reader data_file_reader_;
};

template <typename Handler>
void server_context::dispatch(Handler&& handler)
{
    io_service_.dispatch(std::forward<Handler>(handler));
}

}   // namespace chat

#endif  // _chat_server_context_hpp
