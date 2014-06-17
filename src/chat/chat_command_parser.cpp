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
#include "chat_command_parser.hpp"
#include "chat_exception.hpp"
#include "chat_user_manager.hpp"
#include "chat_room_manager.hpp"
#include "chat_skin_manager.hpp"
#include "chat_room.hpp"
#include "chat_user.hpp"
#include "chat_server.hpp"
#include <log_common.hpp>


namespace chat
{
namespace command
{

typedef int type;

const type clear_history    = 0;
const type topic_cron       = 1;
const type topic            = 2;
const type op               = 3;
const type deop             = 4;
const type unignore         = 5;
const type shutdown         = 6;
const type restart          = 7;
const type lrestart         = 8;
const type loadskin         = 9;
const type botreg           = 10;
const type join             = 11;

}   // namespace command

namespace command_codes
{

const std::string strings[] =
{
    "clear_history",
    "topic_cron",
    "topic",
    "op",
    "deop",
    "unignore",
    "shutdown",
    "restart",
    "lrestart",
    "loadskin",
    "botreg",
    "join"
};

enum { string_count = sizeof(strings) / sizeof(*strings) };

command::type to_type(const std::string& string)
{
    for(std::size_t i = 0; i < string_count; ++i)
    {
        if(string == strings[i])
            return static_cast<command::type>(i);
    }

    throw chat::error("cannot parse command: " + string);
}

}   // namespace command_codes

command_parser::command_parser(chat::server& server) :
    server_(server)
{
}

void command_parser::parse_command(const std::string& command) const
{
    try
    {
        std::vector<std::string> fields;
        boost::split(fields, command, boost::is_space());

        command::type command_type = command_codes::to_type(fields[0]);

        switch(command_type)
        {
            case command::clear_history:
                deliver_clear_history_command();
                break;

            case command::topic_cron:
                deliver_topic_command(false);
                break;

            case command::topic:
                deliver_topic_command(true);
                break;

            case command::op:
                deliver_op_command(fields[1], util::to_type<std::size_t>(fields[2]));
                break;

            case command::deop:
               deliver_op_command(fields[1], 0);
                break;

            case command::unignore:
                deliver_unignore_command(asio::ip::address::from_string(fields[1]));
                break;

            case command::shutdown:
                deliver_shutdown_command(fields[1]);
                break;

            case command::restart:
                deliver_restart_command(fields[1]);
                break;

            case command::lrestart:
                deliver_restart_command();
                break;

            case command::loadskin:
                deliver_loadskin_command(fields[1]);
                break;

            case command::botreg:
                deliver_botreg_command(fields[1]);
                break;

            case command::join:
                deliver_join_command(fields[1], fields[2]);
                break;

            default:
                throw chat::error("unknown command type: " + command);
        }
    }
    catch(std::exception& e)
    {
        LOGERR("cannot deliver command: %1%") << e.what();
    }
}

void command_parser::deliver_clear_history_command() const
{
    server_.get_room_manager()->clear_room_history();
}

void command_parser::deliver_topic_command(bool force) const
{
    server_.get_room_manager()->deliver_chat_topic(force);
}

void command_parser::deliver_op_command(const std::string& session_id, std::size_t access) const
{
    if(server_.get_user_manager()->contains(session_id))
        server_.get_user_manager()->user(session_id)->update(update::access, access);
}

void command_parser::deliver_unignore_command(const asio::ip::address& address) const
{
    server_.get_filter_manager()->remove_block(address);
}

void command_parser::deliver_shutdown_command(const std::string& admin_name) const
{
    LOGNOT("request to stop sent by: %1%") << admin_name;
    server_.stop();
}

void command_parser::deliver_restart_command(const std::string& admin_name) const
{
    LOGNOT("request to restart sent by: %1%") << admin_name;
    server_.restart();
}

void command_parser::deliver_restart_command() const
{
    LOGNOT("request to restart received");
    server_.restart();
}

void command_parser::deliver_loadskin_command(const std::string& skin_name) const
{
    server_.get_skin_manager()->load_skin(skin_name);
}

void command_parser::deliver_botreg_command(const std::string& session_id) const
{
    if(server_.get_user_manager()->contains(session_id))
        server_.get_user_manager()->user(session_id)->register_as_bot();
}

void command_parser::deliver_join_command(const std::string& session_id, const std::string& room_name) const
{
    server_.get_user_manager()->change_room(session_id, room_name);
}

}   // namespace chat
