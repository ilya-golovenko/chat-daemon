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
#include "chat_room.hpp"
#include "chat_user.hpp"
#include "chat_server.hpp"
#include "chat_common.hpp"
#include "chat_message.hpp"
#include "chat_connection.hpp"
#include "chat_exception.hpp"
#include <file_utils.hpp>
#include <utilities.hpp>
#include <config_common.hpp>
#include <log_common.hpp>
#include <version.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>


namespace chat
{

room_ptr room::create(chat::server& server, const std::string& name, bool persistent)
{
    return boost::make_shared<room>(boost::ref(server), name, persistent);
}

const std::string& room::name() const
{
    return name_;
}

void room::join(user_ptr user, std::time_t /*time*/)
{
    if(!contains(user))
    {
        user->start();

        deliver_join_message(user);

        // Add newcomer to the user list
        users_.insert(std::make_pair(user->session()->id(), user));

        LOGNOT("%1% joined into the room: %2%") << user->name() << name_;
    }
}

void room::accept(user_ptr user, connection_ptr connection)
{
    if(contains(user))
    {
        // Start user session
        user->start(connection);

        user->deliver(substitute_name(config.chat.messages.join_room));

        // Deliver chat user list
        user->deliver(get_user_list(user));

        user->deliver(scripts::start_events);

        // Deliver list of lost users
        user->deliver(get_lost_users());

        // Deliver recent messages
        message_dispatcher_.deliver_recent_messages(user);

        user->deliver(config.chat.messages.end_of_buffer);

        // Deliver chat topic
        user->deliver(server_.get_chat_topic());

        // Set user joined status
        user->update(update::connection, status::joined);
    }
}

void room::leave(user_ptr user, std::time_t time, const std::string& text)
{
    if(contains(user))
    {
        LOGNOT("%1% left the room: %2%") << user->name() << name_;

        deliver_leave_message(user, time, text);

        // Remove user from the room
        users_.erase(user->session()->id());

        // Remove room if it is empty and is not persistent
        if(!persistent_ && users_.empty())
            server_.get_room_manager()->remove(name_);

        // Stop user session
        user->stop();
    }
}

void room::change_user_room(user_ptr user, room_ptr room)
{
    if(contains(user))
    {
        // Remove user from the room
        users_.erase(user->session()->id());

        // Remove room if it is empty and is not persistent
        if(!persistent_ && users_.empty())
            server_.get_room_manager()->remove(name_);

        deliver_leave_message(user, std::time(0),
            room->substitute_name(config.chat.messages.change_room));

        // Deliver script which will force the browser to refresh
        user->deliver(config.chat.scripts.reload_this_frame);

        room->join(user, std::time(0));
        user->set_room(room);
    }
}

bool room::contains(user_ptr user) const
{
    return contains(user->session()->id());
}

bool room::contains(const std::string& session_id) const
{
    return (users_.find(session_id) != users_.end());
}

user_ptr room::user(const std::string& session_id) const
{
    user_map_type::const_iterator it = users_.find(session_id);

    if(it == users_.end())
        throw chat::error("no user with session '" + session_id + "' found in the room: " + name_);

    return it->second;
}

void room::deliver(message_ptr message, bool not_buffered)
{
    message_dispatcher_.deliver_message(message, not_buffered);
}

void room::clear_history()
{
    message_dispatcher_.clear_recent_messages();
}

room::room(server& server, const std::string& name, bool persistent) :
    server_(server),
    persistent_(persistent),
    name_(name),
    users_(),
    message_dispatcher_(users_)
{
}

const std::string room::get_user_list(user_ptr user) const
{
    std::ostringstream users;
    users << scripts::userlist_begin;

    user_map_type::const_iterator begin = users_.begin();
    user_map_type::const_iterator end = users_.end();

    for(user_map_type::const_iterator it = begin; it != end; ++it)
    {
        if(it != users_.begin())
            users << ',';

        users << '[' << it->second->session()->to_string() << ']';
    }

    users << scripts::userlist_end;

    if(user->session()->access() >= config.chat.admin.view_ip)
        return chat::message::get_admin_text(users.str());

    return chat::message::get_user_text(users.str());
}

const std::string room::get_lost_users() const
{
    std::ostringstream users;

    user_map_type::const_iterator begin = users_.begin();
    user_map_type::const_iterator end = users_.end();

    for(user_map_type::const_iterator it = begin; it != end; ++it)
    {
        if(!it->second->is_connected() && !it->second->is_bot())
            users << it->second->get_update_message(update::connection, status::lost);
    }

    return users.str();
}

void room::deliver_join_message(user_ptr user)
{
    // Deliver user join message
    deliver(get_join_message(true, user), true);

    // Add user join message to recent messages list
    message_dispatcher_.buffer_message(get_join_message(false, user));
}

void room::deliver_leave_message(user_ptr user, std::time_t time, const std::string& text)
{
    // Deliver user leave message
    deliver(get_leave_message(true, user, time, text), true);

    // Add user leave message to recent messages list
    message_dispatcher_.buffer_message(get_leave_message(false, user, time, text));
}

message_ptr room::get_join_message(bool not_buffered, user_ptr user) const
{
    boost::format formatter(formatters::user_join);

    formatter % not_buffered;
    formatter % server_.get_skin_manager()->get_join_message(user);
    formatter % user->session()->to_string();

    return chat::message::create(formatter.str());
}

message_ptr room::get_leave_message(bool not_buffered,
    user_ptr user, std::time_t time, const std::string& text) const
{
    boost::format formatter(formatters::user_leave);

    formatter % not_buffered;
    formatter % server_.get_skin_manager()->get_leave_message(user);
    formatter % user->session()->nickname();
    formatter % time;
    formatter % text;

    return chat::message::create(formatter.str());
}

const std::string room::substitute_name(const std::string& string) const
{
    return boost::replace_first_copy(string, room_name_placeholder, name_);
}

}   // namespace chat
