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
#include "chat_room_manager.hpp"
#include "chat_message.hpp"
#include "chat_room.hpp"
#include "chat_server.hpp"
#include "chat_exception.hpp"
#include "chat_common.hpp"
#include <config_common.hpp>
#include <path_utils.hpp>
#include <log_common.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

// STL headers
#include <fstream>


namespace chat
{

room_manager::room_manager(chat::server& server) :
    server_(server)
{
}

void room_manager::configure()
{
    LOGNOT("configuring room manager");

    std::string filename = util::path::combine(
        config.chat.php_path, files::rooms);

    std::vector<std::string> rooms;
    read_rooms_file(filename, rooms);

    std::for_each(rooms.begin(), rooms.end(), boost::bind(
        &chat::room_manager::create, this, _1, true));
}

void room_manager::create(const std::string& room_name, bool persistent)
{
    LOGNOT("creating chat room: %1%") << room_name;

    rooms_.insert(std::make_pair(room_name,
        chat::room::create(server_, room_name, persistent)));
}

void room_manager::remove(const std::string& room_name)
{
    room_map_type::iterator it = rooms_.find(room_name);
    if(it != rooms_.end())
    {
        LOGNOT("removing chat room: %1%") << room_name;
        rooms_.erase(it);
    }
}

bool room_manager::contains(const std::string& room_name) const
{
    return (rooms_.find(room_name) != rooms_.end());
}

room_ptr room_manager::room(const std::string& room_name) const
{
    room_map_type::const_iterator it = rooms_.find(room_name);
    if(it != rooms_.end()) return it->second;

    throw chat::error("chat room does not exist: " + room_name);
}

void room_manager::deliver_message(chat::message_ptr message, bool not_buffered)
{
    std::for_each(rooms_.begin(), rooms_.end(), boost::bind(&chat::room::deliver,
        boost::bind(&room_map_type::value_type::second, _1), message, not_buffered));
}

void room_manager::deliver_message(const std::string& message, bool not_buffered)
{
    if(!message.empty())
        deliver_message(chat::message::create(message), not_buffered);
}

void room_manager::clear_room_history()
{
    std::for_each(rooms_.begin(), rooms_.end(), boost::bind(
        &room::clear_history, boost::bind(&room_map_type::value_type::second, _1)));
}

void room_manager::deliver_chat_topic(bool force)
{
    server_.reload_chat_topic(force);
    deliver_message(server_.get_chat_topic(), true);
}

void room_manager::read_rooms_file(const std::string& filename, std::vector<std::string>& rooms)
{
    LOGNOT("reading rooms file: %1%") << filename;

    rooms.clear();

    std::size_t pos;
    std::string line;
    std::fstream file;

    file.open(filename.c_str());
    if(file.is_open())
    {
        while(!file.eof())
        {
            if(std::getline(file, line))
            {
                boost::trim_left(line);
                if(!line.empty())
                {
                    if(line[0] != '#')
                    {
                        pos = line.find_first_of('|');

                        if(pos != std::string::npos)
                            line.erase(pos, line.length());

                        rooms.push_back(line);
                    }
                }
            }
        }

        file.close();

        if(rooms.empty())
            throw chat::error("rooms file is empty: " + filename);
    }
    else
    {
        LOGWRN("no chat rooms specified, default one will be used");
        rooms.push_back(default_room);
    }
}

}   // namespace chat

