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
#ifndef _chat_room_hpp
#define _chat_room_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/message_dispatcher.hpp>
#include <chat/forwards.hpp>
#include <chat/common.hpp>

// BOOST headers
#include <boost/asio.hpp>

// STL headers
#include <memory>
#include <string>
#include <ctime>


namespace chat
{

class server_context;

class room : public std::enable_shared_from_this<room>
{
public:
    static room_ptr create(server_context& context, std::string const& name, bool persistent);

public:
    room(server_context& context, std::string const& name, bool persistent);

    room(room const&) = delete;
    room& operator=(room const&) = delete;

    std::string const& get_name() const;

    void join(user_ptr user, std::time_t time);
    void accept(user_ptr user, connection_ptr connection);
    void leave(user_ptr user, std::time_t time, std::string const& text);

    void change_user_room(user_ptr user, room_ptr room);

    bool contains(user_ptr user) const;
    bool contains(session_id const& id) const;
    user_ptr get_user(session_id const& id) const;

    void deliver(message_ptr message, bool not_buffered = false);
    void clear_history();

private:
    std::string get_user_list(user_ptr user) const;
    std::string get_lost_users() const;

    message_ptr get_join_message(bool not_buffered, user_ptr user) const;
    message_ptr get_leave_message(bool not_buffered, user_ptr user, std::time_t time, std::string const& text) const;

    void deliver_join_message(user_ptr user);
    void deliver_leave_message(user_ptr user, std::time_t time, std::string const& text);

    std::string substitute_name(std::string const& string) const;

private:
    server_context& context_;

    bool persistent_;
    std::string name_;
    user_map users_;

    message_dispatcher message_dispatcher_;
};

}   // namespace chat

#endif  // _chat_room_hpp
