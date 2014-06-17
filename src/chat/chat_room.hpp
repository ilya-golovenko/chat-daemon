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
#ifndef _chat_room_hpp
#define _chat_room_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_message_dispatcher.hpp"
#include "chat_forwards.hpp"
#include "chat_common.hpp"

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

// STL headers
#include <string>
#include <ctime>


namespace chat
{

class room :
    private boost::noncopyable,
    public boost::enable_shared_from_this<room>
{
public:
    static room_ptr create(chat::server& server, const std::string& name, bool persistent);

public:
    room(server& server, const std::string& name, bool persistent);

    const std::string& name() const;

    void join(user_ptr user, std::time_t time);
    void accept(user_ptr user, connection_ptr connection);
    void leave(user_ptr user, std::time_t time, const std::string& text);

    void change_user_room(user_ptr user, room_ptr room);

    bool contains(user_ptr user) const;
    bool contains(const std::string& session_id) const;
    user_ptr user(const std::string& session_id) const;

    void deliver(message_ptr message, bool not_buffered = false);
    void clear_history();

private:
    const std::string get_user_list(user_ptr user) const;
    const std::string get_lost_users() const;

    message_ptr get_join_message(bool not_buffered, user_ptr user) const;

    message_ptr get_leave_message(bool not_buffered, user_ptr user,
        std::time_t time, const std::string& text) const;

    void deliver_join_message(user_ptr user);

    void deliver_leave_message(user_ptr user,
        std::time_t time, const std::string& text);

    const std::string substitute_name(const std::string& string) const;

private:
    chat::server& server_;

    bool persistent_;
    std::string name_;
    user_map_type users_;

    message_dispatcher message_dispatcher_;
};

}   // namespace chat

#endif  // _chat_room_hpp
