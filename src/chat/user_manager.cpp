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
#include <chat/user_manager.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <chat/connection.hpp>
#include <chat/exception.hpp>
#include <chat/session.hpp>
#include <chat/message.hpp>
#include <chat/room.hpp>
#include <chat/user.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{

user_manager::user_manager(server_context& context) :
     context_(context)
{
}

void user_manager::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    LOG_COMP_NOTICE(user_manager, "configuring");

}

void user_manager::start()
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    LOG_COMP_NOTICE(user_manager, "starting");

    LOG_COMP_NOTICE(user_manager, "started");
}

void user_manager::stop()
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    LOG_COMP_NOTICE(user_manager, "stopping");

    for(auto const& user : users_)
    {
        user.second->stop();
    }

    users_.clear();

    LOG_COMP_NOTICE(user_manager, "stopped");
}

void user_manager::accept(connection_ptr connection)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    session_id const& id = connection->get_session_id();

    if(contains(id))
        accept(get_user(id), connection);
}

void user_manager::accept(user_ptr user, connection_ptr connection)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    user->get_room()->accept(user, connection);
}

void user_manager::join(session_ptr session, std::time_t time)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    if(!contains(session->get_id()))
    {
        context_.get_room_manager().create(session->get_room(), false);

        room_ptr room = context_.get_room_manager().get_room(session->get_room());
        user_ptr user = chat::user::create(context_, room, session);

        users_.emplace(session->get_id(), user);
        room->join(user, time);
    }
}

void user_manager::join(session_id const& id, std::time_t time, std::string const& session_info)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    if(!contains(id))
        join(chat::session::create(id, session_info), time);
}

void user_manager::change_room(user_ptr user, std::string const& room_name)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    context_.get_room_manager().create(room_name, false);

    user->get_room()->change_user_room(user, context_.get_room_manager().get_room(room_name));
}

void user_manager::change_room(session_id const& id, std::string const& room_name)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    if(contains(id))
        change_room(get_user(id), room_name);
}

void user_manager::leave(user_ptr user, std::time_t time, std::string const& text)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    users_.erase(user->get_session_id());
    user->get_room()->leave(user, time, text);

    context_.get_ignore_manager().remove_all_user_ignores(user->get_session_id());
}

void user_manager::leave(session_id const& id, std::time_t time, std::string const& text)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    if(contains(id))
        leave(get_user(id), time, text);
}

void user_manager::update(user_ptr user, update::type update_type, std::string const& text)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    user->update(update_type, text);
}

void user_manager::update(session_id const& id, update::type update_type, std::string const& text)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    if(contains(id))
        update(get_user(id), update_type, text);
}

void user_manager::deliver_common_message(user_ptr user, std::string const& text)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    if(!context_.get_ignore_manager().is_ignored(user))
    {
        chat::message_ptr message = chat::message::create(text);

        message->set_sender(user);

        user->get_room()->deliver(message);
    }
}

void user_manager::deliver_common_message(session_id const& id, std::string const& text)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    if(contains(id))
        deliver_common_message(get_user(id), text);
}

void user_manager::deliver_personal_message(user_ptr user, std::string const& text, bool not_buffered)
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    if(!context_.get_ignore_manager().is_ignored(user))
    {
        message_ptr message = chat::message::create(text);

        message->set_recipient(user);

        user->get_room()->deliver(message, not_buffered);
    }
}

void user_manager::deliver_personal_message(session_id const& id, std::string const& text, bool not_buffered)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    if(contains(id))
        deliver_personal_message(get_user(id), text, not_buffered);
}

void user_manager::register_bot(user_ptr user)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    user->register_bot();
}

void user_manager::register_bot(session_id const& id)
{
    LOG_COMP_TRACE_FUNCTION(user_manager);

    if(contains(id))
        register_bot(get_user(id));
}

bool user_manager::contains(session_id const& id) const
{
    return users_.find(id) != users_.end();
}

user_ptr user_manager::get_user(session_id const& id) const
{
    user_map::const_iterator it = users_.find(id);

    if(it == users_.end())
        throw exception("cannot find user with session: ", id);

    return it->second;
}

std::string const& user_manager::resolve(session_id const& id) const
{
    user_map::const_iterator it = users_.find(id);

    if(it != users_.end())
        return it->second->get_nickname();

    return id.str();
}

std::size_t user_manager::get_connection_count() const
{
    std::size_t connection_count = 0;

    for(auto const& user : users_)
    {
        if(user.second->is_connected())
            ++connection_count;
    }

    return connection_count;
}

std::size_t user_manager::get_connection_count(asio::ip::address const& address) const
{
    std::size_t connection_count = 0;

    for(auto const& user : users_)
    {
        if(user.second->is_connected_from(address))
            ++connection_count;
    }

    return connection_count;
}

}   // namespace chat
