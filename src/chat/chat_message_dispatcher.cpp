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
#include "chat_message_dispatcher.hpp"
#include "chat_message.hpp"
#include "chat_user.hpp"
#include <config_common.hpp>

// BOOST headers
#include <boost/bind.hpp>


namespace chat
{

message_dispatcher::message_dispatcher(user_map_type& users) :
    users_(users),
    recent_messages_(config.chat.message_history)
{
}

void message_dispatcher::deliver_message(message_ptr message, bool not_buffered)
{
    if(!not_buffered)
        buffer_message(message);

    if(message->has_recipient())
    {
        const std::string& session_id = message->recipient()->id();
        user_map_type::const_iterator it = users_.find(session_id);

        if(it != users_.end())
            process_message(it->second, message);
    }
    else
    {
        std::for_each(users_.begin(), users_.end(),
            boost::bind(&message_dispatcher::process_message, this,
            boost::bind(&user_map_type::value_type::second, _1), message));
    }
}

void message_dispatcher::buffer_message(message_ptr message)
{
    recent_messages_.push_back(message);
}

void message_dispatcher::deliver_recent_messages(user_ptr user)
{
    std::for_each(recent_messages_.begin(), recent_messages_.end(),
        boost::bind(&message_dispatcher::process_message, this, user, _1));
}

void message_dispatcher::clear_recent_messages()
{
    recent_messages_.clear();
}

void message_dispatcher::process_message(user_ptr user, message_ptr message)
{
    if(is_message_intended_for_user(user, message))
    {
        if(!is_message_ignored_by_user(user, message))
            user->deliver(message->text(user->session()->access()));
    }
}

bool message_dispatcher::is_message_intended_for_user(user_ptr user, message_ptr message) const
{
    if(message->has_recipient())
        return (*message->recipient() == *user->session());
    return (user->session()->access() >= message->access());
}

bool message_dispatcher::is_message_ignored_by_user(user_ptr user, message_ptr message) const
{
    if(message->has_sender())
    {
        if(message->sender()->access() < config.chat.admin.ignore)
            return user->is_ignored(message->sender()->id());
    }

    return false;
}

}   // namespace chat
