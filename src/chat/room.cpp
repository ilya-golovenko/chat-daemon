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

// Application headers
#include <chat/room.hpp>
#include <chat/server_context.hpp>
#include <chat/connection.hpp>
#include <chat/exception.hpp>
#include <chat/message.hpp>
#include <chat/user.hpp>
#include <chat/common.hpp>
#include <app/version.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>
#include <missio/format/print.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <functional>


namespace chat
{

room_ptr room::create(server_context& context, std::string const& name, bool persistent)
{
    return std::make_shared<room>(std::ref(context), name, persistent);
}

room::room(server_context& context, std::string const& name, bool persistent) :
    context_(context),
    persistent_(persistent),
    name_(name),
    message_dispatcher_(context_, users_)
{
}

std::string const& room::get_name() const
{
    return name_;
}

void room::join(user_ptr user, std::time_t /*time*/)
{
    LOG_COMP_TRACE_FUNCTION(room);

    if(!contains(user))
    {
        user->start();

        deliver_join_message(user);

        // Add newcomer to the user list
        users_.emplace(user->get_session_id(), user);

        LOG_COMP_NOTICE(room, "user ", user->get_nickname(), " joined into room ", name_);
    }
}

void room::accept(user_ptr user, connection_ptr connection)
{
    LOG_COMP_TRACE_FUNCTION(room);

    if(contains(user))
    {
        // Start user session
        user->start(connection);

        user->deliver(substitute_name(config.join_room_message));

        // Deliver chat user list
        user->deliver(get_user_list(user));

        user->deliver(scripts::start_events);

        // Deliver list of lost users
        user->deliver(get_lost_users());

        // Deliver recent messages
        message_dispatcher_.deliver_recent_messages(user);

        user->deliver(config.end_of_buffer_message);

        // Deliver chat topic
        user->deliver(context_.get_room_manager().get_chat_topic());

        // Set user joined status
        user->update(update::connection, status::joined);
    }
}

void room::leave(user_ptr user, std::time_t time, std::string const& text)
{
    LOG_COMP_TRACE_FUNCTION(room);

    if(contains(user))
    {
        LOG_COMP_NOTICE(room, "user ", user->get_nickname(), " left room ", name_);

        deliver_leave_message(user, time, text);

        // Remove user from the room
        users_.erase(user->get_session_id());

        // Remove room if it is empty and is not persistent
        if(!persistent_ && users_.empty())
            context_.get_room_manager().remove(name_);

        // Stop user session
        user->stop();
    }
}

void room::change_user_room(user_ptr user, room_ptr room)
{
    LOG_COMP_TRACE_FUNCTION(room);

    if(contains(user))
    {
        // Remove user from the room
        users_.erase(user->get_session_id());

        // Remove room if it is empty and is not persistent
        if(!persistent_ && users_.empty())
            context_.get_room_manager().remove(name_);

        deliver_leave_message(user, std::time(nullptr), room->substitute_name(config.change_room_message));

        // Deliver script which will force the browser to refresh
        user->deliver(config.reload_this_frame_script);

        room->join(user, std::time(nullptr));
        user->set_room(room);
    }
}

bool room::contains(user_ptr user) const
{
    return contains(user->get_session_id());
}

bool room::contains(session_id const& id) const
{
    return users_.find(id) != users_.end();
}

user_ptr room::get_user(session_id const& id) const
{
    user_map::const_iterator it = users_.find(id);

    if(it == users_.end())
        throw exception("no user with session ", id, " found in room ", name_);

    return it->second;
}

void room::deliver(message_ptr message, bool not_buffered)
{
    LOG_COMP_TRACE_FUNCTION(room);

    message_dispatcher_.deliver_message(message, not_buffered);
}

void room::clear_history()
{
    LOG_COMP_TRACE_FUNCTION(room);

    message_dispatcher_.clear_recent_messages();
}

std::string room::get_user_list(user_ptr user) const
{
    std::ostringstream users;

    users << scripts::userlist_begin;

    user_map::const_iterator begin = users_.begin();
    user_map::const_iterator end = users_.end();

    for(user_map::const_iterator it = begin; it != end; ++it)
    {
        if(it != users_.begin())
            users << ',';

        users << '[' << it->second->get_session()->to_string() << ']';
    }

    users << scripts::userlist_end;

    if(user->get_access() >= config.admin_view_ip)
        return chat::message::get_admin_text(users.str());

    return chat::message::get_user_text(users.str());
}

std::string room::get_lost_users() const
{
    LOG_COMP_TRACE_FUNCTION(room);

    std::ostringstream users;

    user_map::const_iterator begin = users_.begin();
    user_map::const_iterator end = users_.end();

    for(user_map::const_iterator it = begin; it != end; ++it)
    {
        if(!it->second->is_connected() && !it->second->is_bot())
            users << it->second->get_update_message(update::connection, status::lost);
    }

    return users.str();
}

void room::deliver_join_message(user_ptr user)
{
    LOG_COMP_TRACE_FUNCTION(room);

    // Deliver user join message
    deliver(get_join_message(true, user), true);

    // Add user join message to recent messages list
    message_dispatcher_.buffer_message(get_join_message(false, user));
}

void room::deliver_leave_message(user_ptr user, std::time_t time, std::string const& text)
{
    LOG_COMP_TRACE_FUNCTION(room);

    // Deliver user leave message
    deliver(get_leave_message(true, user, time, text), true);

    // Add user leave message to recent messages list
    message_dispatcher_.buffer_message(get_leave_message(false, user, time, text));
}

message_ptr room::get_join_message(bool not_buffered, user_ptr user) const
{
    LOG_COMP_TRACE_FUNCTION(room);

    std::string message_text;

    std::string const& session = user->get_session()->to_string();
    std::string const& join_text = context_.get_skin_manager().get_join_message(user);

    missio::format::print(message_text, formatters::user_join, not_buffered, join_text, session);

    return chat::message::create(message_text);
}

message_ptr room::get_leave_message(bool not_buffered, user_ptr user, std::time_t time, std::string const& text) const
{
    LOG_COMP_TRACE_FUNCTION(room);

    std::string message_text;

    std::string const& nickname = user->get_session()->get_nickname();
    std::string const& leave_text = context_.get_skin_manager().get_leave_message(user);

    missio::format::print(message_text, formatters::user_leave, not_buffered, leave_text, nickname, time, text);

    return chat::message::create(message_text);
}

std::string room::substitute_name(std::string const& string) const
{
    return boost::algorithm::replace_first_copy(string, room_name_placeholder, name_);
}

}   // namespace chat
