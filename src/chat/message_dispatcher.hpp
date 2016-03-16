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
#ifndef _chat_message_dispatcher_hpp
#define _chat_message_dispatcher_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/session_id.hpp>
#include <chat/forwards.hpp>
#include <chat/common.hpp>

// BOOST headers
#include <boost/circular_buffer.hpp>

// STL headers
#include <string>


namespace chat
{

class server_context;

class message_dispatcher
{
public:
    message_dispatcher(server_context& context, user_map& users);

    message_dispatcher(message_dispatcher const&) = delete;
    message_dispatcher& operator=(message_dispatcher const&) = delete;

    void deliver_message(message_ptr message, bool not_buffered);
    void deliver_recent_messages(user_ptr user);
    void buffer_message(message_ptr message);
    void clear_recent_messages();

private:
    void process_message(user_ptr user, message_ptr message);

    bool is_message_intended_for_user(user_ptr user, message_ptr message) const;
    bool is_message_ignored_by_user(user_ptr user, message_ptr message) const;
    bool is_user_ignored_by_user(user_ptr user, session_id const& id) const;

private:
    //TODO: probably replace with std::deque?
    typedef boost::circular_buffer<message_ptr> message_buffer;

private:
    server_context& context_;
    user_map& users_;
    message_buffer recent_messages_;
};

}   // namespace chat

#endif  // _chat_message_dispatcher_hpp
