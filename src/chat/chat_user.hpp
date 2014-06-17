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
#ifndef _chat_user_hpp
#define _chat_user_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_ignore_manager.hpp"
#include "chat_forwards.hpp"
#include "chat_session.hpp"
#include "chat_common.hpp"
#include <http_common.hpp>
#include <log_source.hpp>
#include <utilities.hpp>
#include <string_utils.hpp>
#include <async_timer.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/enable_shared_from_this.hpp>

// STL headers
#include <string>


namespace chat
{

class user :
    private boost::noncopyable,
    public log_source<user>,
    public boost::enable_shared_from_this<user>
{
public:
    static user_ptr create(chat::server& server,
        room_ptr room, session_ptr session);

public:
    user(chat::server& server, room_ptr room, session_ptr session);
    ~user();

    session_ptr session() const;
    const std::string& name() const;

    room_ptr room() const;
    void set_room(room_ptr room);

    void deliver(const http::buffer& text);
    void deliver(const std::string& text);

    bool is_connected() const;
    bool is_connected_from(const asio::ip::address& address) const;

    void start(connection_ptr connection);
    void start();
    void stop();

    bool is_bot() const;
    void register_as_bot();

    template <typename T>
    inline void update(update::type update_type, const T& value);
    void update(update::type update_type, const std::string& value);

    template <typename T>
    inline const std::string get_update_message(update::type update_type, const T& value) const;
    const std::string get_update_message(update::type update_type, const std::string& value) const;

    void add_ignore(const std::string& session_id);
    void remove_ignore(const std::string& session_id);
    void clear_ignores();

    bool is_ignored(const std::string& session_id) const;
    std::size_t ignore_count() const;

public:
    const std::string& get_log_source_name() const;

private:
    void rebuild_user_name();

    void start_ping_timer();
    void start_leave_timer();

    void remove_session_file();
    void update_session_field(update::type update_type, const std::string& value);

    const chat::completion_handler bind_to_write_handler();

    void handle_write(const asio::error_code& error);
    void handle_leave(const asio::error_code& error);
    void handle_ping(const asio::error_code& error);

private:
    std::string name_;
    chat::server& server_;

    util::async_timer timer_;

    bool bot_;
    bool active_;

    room_ptr room_;
    session_ptr session_;
    connection_ptr connection_;

    ignore_manager ignore_manager_;
};

template <typename T>
inline void user::update(update::type update_type, const T& value)
{
    update(update_type, util::to_string(value));
}

template <typename T>
inline const std::string user::get_update_message(update::type update_type, const T& value) const
{
    return get_update_message(update_type, util::to_string(value));
}

}   // namespace chat

#endif  // _chat_user_hpp
