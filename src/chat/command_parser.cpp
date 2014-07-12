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
#include <chat/command_parser.hpp>
#include <chat/exception.hpp>
#include <chat/user_manager.hpp>
#include <chat/room_manager.hpp>
#include <chat/skin_manager.hpp>
#include <chat/room.hpp>
#include <chat/user.hpp>
#include <chat/server.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>


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

command::type to_type(std::string const& string)
{
    for(std::size_t i = 0; i < string_count; ++i)
    {
        if(string == strings[i])
            return static_cast<command::type>(i);
    }

    throw exception("cannot parse command: ", string);
}

}   // namespace command_codes

command_parser::command_parser(server_context& context) :
    context_(context)
{
}

void command_parser::parse_command(std::string const& command) const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    try
    {
        std::vector<std::string> fields;

        boost::algorithm::split(fields, command, boost::algorithm::is_space());

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
                deliver_op_command(session_id(fields[1]), std::stoul(fields[2]));
                break;

            case command::deop:
               deliver_op_command(session_id(fields[1]), 0);
                break;

            case command::unignore:
                deliver_unignore_command(boost::asio::ip::address::from_string(fields[1]));
                break;

            case command::shutdown:
                deliver_shutdown_command(fields[1]);
                break;

            case command::restart:
                deliver_restart_command(fields[1]);
                break;

            case command::lrestart:
                deliver_reconfigure_command();
                break;

            case command::loadskin:
                deliver_loadskin_command(fields[1]);
                break;

            case command::botreg:
                deliver_botreg_command(session_id(fields[1]));
                break;

            case command::join:
                deliver_join_command(session_id(fields[1]), fields[2]);
                break;

            default:
                throw exception("unknown command type: ", command);
        }
    }
    catch(std::exception const& e)
    {
        LOG_COMP_WARNING(command_parser, e);
    }
}

void command_parser::deliver_clear_history_command() const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    context_.get_room_manager().clear_room_history();
}

void command_parser::deliver_topic_command(bool force) const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    context_.get_room_manager().deliver_chat_topic(force);
}

void command_parser::deliver_op_command(session_id const& id, std::size_t access) const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    //TODO: fix!
    //context_.get_user_manager().update(id, update::access, access);
}

void command_parser::deliver_unignore_command(boost::asio::ip::address const& address) const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    context_.get_filter_manager().remove_block(address);
}

void command_parser::deliver_shutdown_command(std::string const& admin_name) const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    LOG_COMP_NOTICE(command_parser, "received request to stop sent by user ", admin_name);
    context_.stop();
}

void command_parser::deliver_restart_command(std::string const& admin_name) const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    LOG_COMP_NOTICE(command_parser, "received request to restart sent by user ", admin_name);
    //context_.restart();
}

void command_parser::deliver_reconfigure_command() const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    LOG_COMP_NOTICE(command_parser, "received request to reconfigure");
    //context_.reconfigure();
}

void command_parser::deliver_loadskin_command(std::string const& skin_name) const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    context_.get_skin_manager().load_skin(skin_name);
}

void command_parser::deliver_botreg_command(session_id const& id) const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    context_.get_user_manager().register_bot(id);
}

void command_parser::deliver_join_command(session_id const& id, std::string const& room_name) const
{
    LOG_COMP_TRACE_FUNCTION(command_parser);

    context_.get_user_manager().change_room(id, room_name);
}

}   // namespace chat
