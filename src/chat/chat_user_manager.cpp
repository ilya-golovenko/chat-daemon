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

// Application headers
#include "chat_user_manager.hpp"
#include "chat_connection.hpp"
#include "chat_exception.hpp"
#include "chat_session.hpp"
#include "chat_server.hpp"
#include "chat_room.hpp"
#include "chat_user.hpp"
#include <config_common.hpp>
#include <path_utils.hpp>
#include <file_utils.hpp>
#include <utilities.hpp>
#include <log_common.hpp>

// BOOST headers
#include <boost/bind.hpp>


namespace chat
{

user_manager::user_manager(chat::server& server) :
    server_(server)
{
}

void user_manager::stop_all_user_sessions()
{
    std::for_each(users_.begin(), users_.end(), boost::bind(
        &user::stop, boost::bind(&user_map_type::value_type::second, _1)));

    users_.clear();
}

void user_manager::accept(chat::connection_ptr connection)
{
    if(contains(connection->session_id()))
        accept(user(connection->session_id()), connection);
}

void user_manager::accept(user_ptr user, connection_ptr connection)
{
    user->room()->accept(user, connection);
}

void user_manager::join(session_ptr session, std::time_t time)
{
    const std::string& session_id = session->id();
    const std::string& room_name = session->room();

    if(!contains(session_id))
    {
        if(!server_.get_room_manager()->contains(room_name))
            server_.get_room_manager()->create(room_name, false);

        room_ptr room = server_.get_room_manager()->room(room_name);
        user_ptr user = chat::user::create(server_, room, session);

        users_.insert(std::make_pair(session_id, user));
        room->join(user, time);
    }
}

void user_manager::join(const std::string& session_id, std::time_t time, const std::string& session_info)
{
    if(!contains(session_id))
        join(chat::session::create(session_id, session_info), time);
}

void user_manager::change_room(user_ptr user, const std::string& room_name)
{
    if(!server_.get_room_manager()->contains(room_name))
        server_.get_room_manager()->create(room_name, false);

    user->room()->change_user_room(user,
        server_.get_room_manager()->room(room_name));
}

void user_manager::change_room(const std::string& session_id, const std::string& room_name)
{
    if(contains(session_id))
        change_room(user(session_id), room_name);
}

void user_manager::leave(user_ptr user, std::time_t time, const std::string& text)
{
    users_.erase(user->session()->id());
    user->room()->leave(user, time, text);
}

void user_manager::leave(const std::string& session_id, std::time_t time, const std::string& text)
{
    if(contains(session_id))
        leave(user(session_id), time, text);
}

bool user_manager::contains(const std::string& session_id) const
{
    return (users_.find(session_id) != users_.end());
}

user_ptr user_manager::user(const std::string& session_id) const
{
    user_map_type::const_iterator it = users_.find(session_id);

    if(it == users_.end())
        throw chat::error("cannot find user session: " + session_id);

    return it->second;
}

std::size_t user_manager::get_ignore_count() const
{
    std::size_t ignore_count = 0;

    user_map_type::const_iterator begin = users_.begin();
    user_map_type::const_iterator end = users_.end();

    for(user_map_type::const_iterator it = begin; it != end; ++it)
        ignore_count += it->second->ignore_count();

    return ignore_count;
}

std::size_t user_manager::get_connection_count() const
{
    std::size_t connection_count = 0;

    user_map_type::const_iterator begin = users_.begin();
    user_map_type::const_iterator end = users_.end();

    for(user_map_type::const_iterator it = begin; it != end; ++it)
    {
        if(it->second->is_connected())
            ++connection_count;
    }

    return connection_count;
}

std::size_t user_manager::get_connection_count(const asio::ip::address& address) const
{
    std::size_t connection_count = 0;

    user_map_type::const_iterator begin = users_.begin();
    user_map_type::const_iterator end = users_.end();

    for(user_map_type::const_iterator it = begin; it != end; ++it)
    {
        if(it->second->is_connected_from(address))
            ++connection_count;
    }

    return connection_count;
}

}   // namespace chat
