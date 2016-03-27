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
#include <chat/room_manager.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <chat/message.hpp>
#include <chat/room.hpp>
#include <chat/exception.hpp>
#include <chat/common.hpp>
#include <misc/path_utils.hpp>
#include <misc/file_utils.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <functional>
#include <fstream>


namespace chat
{

room_manager::room_manager(server_context& context) :
    context_(context)
{
}

void room_manager::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    LOG_COMP_NOTICE(room_manager, "configuring");

    reload_chat_topic(true);

    config_path_ = config.conf_path;

    std::string filename = util::path::combine(config_path_, files::rooms);

    std::vector<std::string> rooms;
    read_rooms_file(filename, rooms);

    std::for_each(rooms.begin(), rooms.end(), std::bind(&chat::room_manager::create, this, std::placeholders::_1, true));
}

void room_manager::start()
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    LOG_COMP_NOTICE(room_manager, "starting");

    LOG_COMP_NOTICE(room_manager, "started");
}

void room_manager::stop()
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    LOG_COMP_NOTICE(room_manager, "stopping");

    LOG_COMP_NOTICE(room_manager, "stopped");
}

void room_manager::create(std::string const& room_name, bool persistent)
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    if(rooms_.emplace(room_name, chat::room::create(context_, room_name, persistent)).second)
    {
        LOG_COMP_NOTICE(room_manager, "created ", persistent ? "persistent" : "temporary", " chat room: ", room_name);
    }
}

void room_manager::remove(std::string const& room_name)
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    if(rooms_.erase(room_name) > 0)
    {
        LOG_COMP_NOTICE(room_manager, "removed chat room: ", room_name);
    }
}

bool room_manager::contains(std::string const& room_name) const
{
    return rooms_.find(room_name) != rooms_.end();
}

room_ptr room_manager::get_room(std::string const& room_name) const
{
    room_map::const_iterator it = rooms_.find(room_name);

    if(it == rooms_.end())
    {
        throw exception("chat room does not exist: ", room_name);
    }

    return it->second;
}

void room_manager::deliver_message(message_ptr message, bool not_buffered)
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    std::for_each(rooms_.begin(), rooms_.end(), std::bind(&chat::room::deliver,
        std::bind(&room_map::value_type::second, std::placeholders::_1), message, not_buffered));
}

void room_manager::deliver_message(std::string const& message, bool not_buffered)
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    if(!message.empty())
    {
        deliver_message(chat::message::create(message), not_buffered);
    }
}

void room_manager::clear_room_history()
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    std::for_each(rooms_.begin(), rooms_.end(), std::bind(&room::clear_history,
        std::bind(&room_map::value_type::second, std::placeholders::_1)));
}

void room_manager::reload_chat_topic(bool force)
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    if(force || chat_topic_.empty())
    {
        LOG_COMP_DEBUG(room_manager, "reloading chat topic");

        try
        {
            std::string filename = util::path::combine(config_path_, files::topic);

            if(util::file::exists(filename))
            {
                chat_topic_ = util::file::read_text(filename);
            }
        }
        catch(std::exception const& e)
        {
            LOG_COMP_WARNING(room_manager, e);
        }
    }
}

void room_manager::deliver_chat_topic(bool force)
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    reload_chat_topic(force);

    deliver_message(chat_topic_, true);
}

std::string const& room_manager::get_chat_topic() const
{
    return chat_topic_;
}

void room_manager::read_rooms_file(std::string const& filename, std::vector<std::string>& rooms)
{
    LOG_COMP_TRACE_FUNCTION(room_manager);

    LOG_COMP_NOTICE(room_manager, "reading rooms file: ", filename);

    rooms.clear();

    std::size_t pos;
    std::string line;
    std::ifstream file;

    file.open(filename.c_str());

    if(!file.is_open())
    {
        LOG_COMP_WARNING(room_manager, "no chat rooms specified, default one will be used");
        rooms.push_back(default_room);
        return;
    }

    while(!file.eof())
    {
        if(std::getline(file, line))
        {
            boost::algorithm::trim_left(line);

            if(!line.empty() && line[0] != '#')
            {
                pos = line.find_first_of('|');

                if(pos != std::string::npos)
                {
                    line.erase(pos, line.size());
                }

                rooms.push_back(line);
            }
        }
    }

    file.close();

    if(rooms.empty())
    {
        throw exception("rooms file is empty: ", filename);
    }
}

}   // namespace chat
