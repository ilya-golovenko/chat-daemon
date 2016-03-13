//---------------------------------------------------------------------------
//
//    Copyright (C) 2008 - 2016 Ilya Golovenko
//    This file is part of Chat.Daemon project
//
//    spchatd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spchatd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spchatd. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include <chat/message_dispatcher.hpp>
#include <chat/server_context.hpp>
#include <chat/session_id.hpp>
#include <chat/message.hpp>
#include <chat/session.hpp>
#include <chat/user.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{

message_dispatcher::message_dispatcher(server_context& context, user_map& users) :
    context_(context),
    users_(users),
    recent_messages_(50) //TODO: config.message_history
{
}

void message_dispatcher::deliver_message(message_ptr message, bool not_buffered)
{
    LOG_COMP_TRACE_FUNCTION(message_dispatcher);

    if(!not_buffered)
        buffer_message(message);

    if(message->has_recipient())
    {
        session_id const& id = message->get_recipient()->get_id();

        user_map::const_iterator it = users_.find(id);

        if(it != users_.end())
            process_message(it->second, message);
    }
    else
    {
        for(auto const& user : users_)
        {
            process_message(user.second, message);
        }
    }
}

void message_dispatcher::buffer_message(message_ptr message)
{
    LOG_COMP_TRACE_FUNCTION(message_dispatcher);

    recent_messages_.push_back(message);
}

void message_dispatcher::deliver_recent_messages(user_ptr user)
{
    LOG_COMP_TRACE_FUNCTION(message_dispatcher);

    for(auto const& message : recent_messages_)
    {
        process_message(user, message);
    }
}

void message_dispatcher::clear_recent_messages()
{
    LOG_COMP_TRACE_FUNCTION(message_dispatcher);

    recent_messages_.clear();
}

void message_dispatcher::process_message(user_ptr user, message_ptr message)
{
    LOG_COMP_TRACE_FUNCTION(message_dispatcher);

    if(is_message_intended_for_user(user, message))
    {
        if(!is_message_ignored_by_user(user, message))
            user->deliver(message->get_text(user->get_access()));
    }
}

bool message_dispatcher::is_message_intended_for_user(user_ptr user, message_ptr message) const
{
    if(message->has_recipient())
        return *message->get_recipient() == *user->get_session();

    return user->get_access() >= message->get_access();
}

bool message_dispatcher::is_message_ignored_by_user(user_ptr user, message_ptr message) const
{
    if(message->has_sender())
    {
        if(message->get_sender()->get_access() < 300) //TODO: config.admin_ignore
            return is_user_ignored_by_user(user, message->get_sender()->get_id());
    }

    return false;
}

bool message_dispatcher::is_user_ignored_by_user(user_ptr user, session_id const& id) const
{
    return context_.get_ignore_manager().is_ignored(user->get_session_id(), id);
}

}   // namespace chat
