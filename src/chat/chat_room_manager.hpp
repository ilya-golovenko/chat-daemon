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
#ifndef _chat_room_manager_hpp
#define _chat_room_manager_hpp

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

// STL headers
#include <vector>
#include <string>


namespace chat
{

class room_manager :
    private boost::noncopyable
{
public:
    room_manager(chat::server& server);
    void configure();

    void create(const std::string& room_name, bool persistent = true);
    void remove(const std::string& room_name);

    bool contains(const std::string& room_name) const;
    room_ptr room(const std::string& room_name) const;

    void deliver_message(chat::message_ptr message, bool not_buffered = false);
    void deliver_message(const std::string& message, bool not_buffered = false);

    void clear_room_history();
    void deliver_chat_topic(bool force);

private:
    void read_rooms_file(const std::string& filename, std::vector<std::string>& rooms);

private:
    chat::server& server_;
    room_map_type rooms_;
};

typedef boost::shared_ptr<room_manager> room_manager_ptr;

}   // namespace chat

#endif  // _chat_room_manager_hpp
