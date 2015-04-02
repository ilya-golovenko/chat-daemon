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
#ifndef _chat_user_manager_hpp
#define _chat_user_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/session_id.hpp>
#include <chat/common.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <unordered_map>
#include <string>
#include <ctime>


namespace chat
{

class server_context;
struct server_config;

class user_manager
{
public:
    explicit user_manager(server_context& context);

    user_manager(user_manager const&) = delete;
    user_manager& operator=(user_manager const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

    void accept(connection_ptr connection);
    void accept(user_ptr user, connection_ptr connection);

    void join(session_ptr session, std::time_t time);
    void join(session_id const& id, std::time_t time, std::string const& session_info);

    void change_room(user_ptr user, std::string const& room_name);
    void change_room(session_id const& id, std::string const& room_name);

    void leave(user_ptr user, std::time_t time, std::string const& text);
    void leave(session_id const& id, std::time_t time, std::string const& text);

    void update(user_ptr user, update::type update_type, std::string const& text);
    void update(session_id const& id, update::type update_type, std::string const& text);

    void deliver_common_message(user_ptr user, std::string const& text);
    void deliver_common_message(session_id const& id, std::string const& text);

    void deliver_personal_message(user_ptr user, std::string const& text, bool not_buffered);
    void deliver_personal_message(session_id const& id, std::string const& text, bool not_buffered);

    void register_bot(user_ptr user);
    void register_bot(session_id const& id);

    bool contains(session_id const& id) const;
    user_ptr get_user(session_id const& id) const;

    std::string const& resolve(session_id const& id) const;

    std::size_t get_connection_count() const;
    std::size_t get_connection_count(asio::ip::address const& address) const;

private:
    typedef std::unordered_map<std::string, connection_ptr> connection_map;

private:
    server_context& context_;

    user_map users_;
    connection_map connections_;
};

}   // namespace chat

#endif  // _chat_user_manager_hpp
