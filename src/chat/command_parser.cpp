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
#include <boost/range/size.hpp>


namespace chat
{
namespace command
{

typedef int type;

type const clear_history    = 0;
type const topic_cron       = 1;
type const topic            = 2;
type const op               = 3;
type const deop             = 4;
type const unignore         = 5;
type const shutdown         = 6;
type const restart          = 7;
type const lrestart         = 8;
type const loadskin         = 9;
type const botreg           = 10;
type const join             = 11;

}   // namespace command

namespace command_codes
{

static char const* strings[] =
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

command::type to_type(std::string const& string)
{
    for(std::size_t i = 0; i < boost::size(strings); ++i)
    {
        if(string == strings[i])
        {
            return static_cast<command::type>(i);
        }
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
                deliver_unignore_command(asio::ip::address::from_string(fields[1]));
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

void command_parser::deliver_unignore_command(asio::ip::address const& address) const
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
