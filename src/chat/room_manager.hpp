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
#ifndef _chat_room_manager_hpp
#define _chat_room_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/forwards.hpp>
#include <chat/common.hpp>

// STL headers
#include <vector>
#include <string>


namespace chat
{

class server_context;
struct server_config;

class room_manager
{
public:
    explicit room_manager(server_context& context);

    room_manager(room_manager const&) = delete;
    room_manager& operator=(room_manager const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

    void create(std::string const& room_name, bool persistent = true);
    void remove(std::string const& room_name);

    bool contains(std::string const& room_name) const;
    room_ptr get_room(std::string const& room_name) const;

    void deliver_message(message_ptr message, bool not_buffered = false);
    void deliver_message(std::string const& message, bool not_buffered = false);

    void clear_room_history();

    void reload_chat_topic(bool force);
    void deliver_chat_topic(bool force);

    std::string const& get_chat_topic() const;

private:
    void read_rooms_file(std::string const& filename, std::vector<std::string>& rooms);

private:
    server_context& context_;

    std::string chat_topic_;

    std::string config_path_;

    room_map rooms_;
};

}   // namespace chat

#endif  // _chat_room_manager_hpp
