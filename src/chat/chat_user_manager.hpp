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
#ifndef _chat_user_manager_hpp
#define _chat_user_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_forwards.hpp"
#include "chat_common.hpp"

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

// STL headers
#include <string>
#include <ctime>


namespace chat
{

class user_manager :
    private boost::noncopyable
{
public:
    user_manager(chat::server& server);
    void stop_all_user_sessions();

    void accept(chat::connection_ptr connection);
    void accept(user_ptr user, connection_ptr connection);

    void join(session_ptr session, std::time_t time);
    void join(const std::string& session_id, std::time_t time, const std::string& session_info);

    void change_room(user_ptr user, const std::string& room_name);
    void change_room(const std::string& session_id, const std::string& room_name);

    void leave(user_ptr user, std::time_t time, const std::string& text);
    void leave(const std::string& session_id, std::time_t time, const std::string& text);

    bool contains(const std::string& session_id) const;
    user_ptr user(const std::string& session_id) const;

    std::size_t get_ignore_count() const;
    std::size_t get_connection_count() const;

    std::size_t get_connection_count(const asio::ip::address& address) const;

private:
    typedef boost::unordered_map<std::string, chat::connection_ptr> connection_map_type;

private:
    chat::server& server_;
    user_map_type users_;
    connection_map_type connections_;
};

typedef boost::shared_ptr<user_manager> user_manager_ptr;

}   // namespace chat

#endif  // _chat_user_manager_hpp
