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
#ifndef _chat_message_dispatcher_hpp
#define _chat_message_dispatcher_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_forwards.hpp"
#include "chat_common.hpp"

// STL headers
#include <string>

// BOOST headers
#include <boost/circular_buffer.hpp>
#include <boost/noncopyable.hpp>


namespace chat
{

class message_dispatcher :
    private boost::noncopyable
{
public:
    message_dispatcher(user_map_type& users);

    void deliver_message(message_ptr message, bool not_buffered);
    void deliver_recent_messages(user_ptr user);
    void buffer_message(message_ptr message);
    void clear_recent_messages();

private:
    void process_message(user_ptr user, message_ptr message);

    bool is_message_intended_for_user(user_ptr user, message_ptr message) const;
    bool is_message_ignored_by_user(user_ptr user, message_ptr message) const;

private:
    typedef boost::circular_buffer<message_ptr> message_buffer_type;

private:
    user_map_type& users_;
    message_buffer_type recent_messages_;
};

}   // namespace chat

#endif  // _chat_message_dispatcher_hpp
