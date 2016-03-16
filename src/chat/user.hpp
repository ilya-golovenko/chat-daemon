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
#ifndef _chat_user_hpp
#define _chat_user_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/forwards.hpp>
#include <chat/session.hpp>
#include <chat/common.hpp>
#include <http/common.hpp>

// ASIO headers
#include <asio/steady_timer.hpp>
#include <asio.hpp>

// STL headers
#include <memory>
#include <string>


namespace chat
{

class server_context;

class user : public std::enable_shared_from_this<user>
{
public:
    static user_ptr create(server_context& context, room_ptr room, session_ptr session);

public:
    user(server_context& context, room_ptr room, session_ptr session);
    ~user();

    user(user const&) = delete;
    user& operator=(user const&) = delete;

    session_ptr get_session() const;

    session_id const& get_session_id() const;
    std::string const& get_nickname() const;

    std::size_t get_access() const;
    sex::type get_sex() const;

    room_ptr get_room() const;
    void set_room(room_ptr room);

    void deliver(http::buffer const& text);
    void deliver(std::string const& text);

    bool is_connected() const;
    bool is_connected_from(asio::ip::address const& address) const;

    void start(connection_ptr connection);
    void start();
    void stop();

    bool is_bot() const;
    void register_bot();

    template <typename T>
    inline void update(update::type update_type, T const& value);
    void update(update::type update_type, std::string const& value);

    template <typename T>
    inline std::string get_update_message(update::type update_type, T const& value) const;
    std::string get_update_message(update::type update_type, std::string const& value) const;

private:
    void start_ping_timer();
    void start_leave_timer();

    void update_session_field(update::type update_type, std::string const& value);

    chat::completion_handler bind_to_write_handler();

    void handle_write(asio::error_code const& error);
    void handle_leave(asio::error_code const& error);
    void handle_ping(asio::error_code const& error);

private:
    server_context& context_;

    asio::steady_timer timer_;

    bool bot_;
    bool active_;

    room_ptr room_;
    session_ptr session_;
    connection_ptr connection_;
};

template <typename T>
inline void user::update(update::type update_type, T const& value)
{
    update(update_type, std::to_string(value));
}

template <typename T>
inline std::string user::get_update_message(update::type update_type, T const& value) const
{
    return get_update_message(update_type, std::to_string(value));
}

}   // namespace chat

#endif  // _chat_user_hpp
