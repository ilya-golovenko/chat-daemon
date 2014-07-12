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
#include <chat/message_parser.hpp>
#include <chat/server_context.hpp>
#include <chat/exception.hpp>
#include <chat/message.hpp>
#include <chat/common.hpp>
#include <app/constants.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// BOOST headers
#include <boost/xpressive/xpressive.hpp>
#include <boost/lexical_cast.hpp>


namespace chat
{

typedef int message_type;

namespace message_types
{

const message_type common           = 0;    // Common message
const message_type personal         = 1;    // Private message
const message_type server           = 2;    // Message from server
const message_type admin            = 3;    // Message for admins
const message_type command          = 4;    // Command message
const message_type uignore_add      = 5;    // Personal ignore added
const message_type uignore_remove   = 6;    // Personal ignore removed
const message_type tignore_add      = 7;    // Total ignore added
const message_type tignore_remove   = 8;    // Total ignore removed
const message_type join             = 9;    // New user joins
const message_type leave            = 10;   // User leaves
const message_type away             = 11;   // Away message
const message_type status           = 12;   // Status changed
const message_type update           = 13;   // User config updated

}   // namespace message_types

namespace message_codes
{

const char common   = '*';
const char personal = '%';
const char server   = 's';
const char admin    = 'a';
const char command  = '#';

const char uignore  = 'i';
const char tignore  = 't';

const char add      = '+';
const char remove   = '-';

const char extended = '/';

const char join     = 'J';
const char leave    = 'P';
const char away     = 'A';
const char status   = 'S';
const char update   = 'U';

message_type to_type(std::string const& string)
{
    if(!string.empty())
    {
        switch(string[0])
        {
            case message_codes::common:
                return message_types::common;

            case message_codes::personal:
                return message_types::personal;

            case message_codes::server:
                return message_types::server;

            case message_codes::admin:
                return message_types::admin;

            case message_codes::uignore:
            {
                switch(string[1])
                {
                    case message_codes::add:
                        return message_types::uignore_add;

                    case message_codes::remove:
                        return message_types::uignore_remove;
                }
            }
            break;

            case message_codes::tignore:
            {
                switch(string[1])
                {
                    case message_codes::add:
                        return message_types::tignore_add;

                    case message_codes::remove:
                        return message_types::tignore_remove;
                }
            }
            break;

            case message_codes::command:
                return message_types::command;

            case message_codes::extended:    // Extended code
            {
                switch(string[1])
                {
                    case message_codes::join:
                        return message_types::join;

                    case message_codes::leave:
                        return message_types::leave;

                    case message_codes::away:
                        return message_types::away;

                    case message_codes::status:
                        return message_types::status;

                    case message_codes::update:
                        return message_types::update;
                }
            }
            break;
        }
    }

    throw exception("cannot parse message: ", string);
}

}   // namespace message_codes

namespace tags
{

using namespace boost::xpressive;

const mark_tag session_id(1);
const mark_tag ignored_id(2);
const mark_tag access(3);
const mark_tag text(4);
const mark_tag type(5);
const mark_tag time(6);
const mark_tag nobuf(7);

}   // namespace tags

namespace regexes
{

using namespace boost::xpressive;

const sregex common     = bos >> +~_s >> _s >> (tags::session_id= +~_s) >> _s >> (tags::text= +_) >> eos;
const sregex personal   = bos >> +~_s >> _s >> (tags::session_id= +~_s) >> _s >> (tags::nobuf= +_d) >> _s >> (tags::text= +_) >> eos;
const sregex server     = bos >> +~_s >> _s >> (tags::text= +_) >> eos;
const sregex admin      = bos >> +~_s >> _s >> (tags::access= +_d)  >> (tags::text= +_) >> eos;
const sregex command    = bos >> +~_s >> _s >> (tags::text= +_) >> eos;
const sregex uignore    = bos >> +~_s >> _s >> (tags::session_id= +~_s) >> _s >> (tags::ignored_id= +_) >> eos;
const sregex tignore    = bos >> +~_s >> _s >> (tags::ignored_id= +_) >> eos;
const sregex join       = bos >> +~_s >> _s >> (tags::session_id= +~_s) >> _s >> (tags::time= +_d) >> _s >> (tags::text= *_) >> eos;
const sregex leave      = bos >> +~_s >> _s >> (tags::session_id= +~_s) >> _s >> (tags::time= +_d) >> _s >> (tags::text= *_) >> eos;
const sregex away       = bos >> +~_s >> _s >> (tags::session_id= +~_s) >> _s >> (tags::text= *_) >> eos;
const sregex status     = bos >> +~_s >> _s >> (tags::session_id= +~_s) >> _s >> (tags::text= +_) >> eos;
const sregex update     = bos >> +~_s >> _s >> (tags::session_id= +~_s) >> _s >> (tags::type= +_d) >> _s >> +_d >> _s >> (tags::text= +_) >> eos;

sregex const& from_type(message_type message_type)
{
    switch(message_type)
    {
        case message_types::common:
            return common;

        case message_types::personal:
            return personal;

        case message_types::server:
            return server;

        case message_types::admin:
            return admin;

        case message_types::command:
            return command;

        case message_types::uignore_add:
        case message_types::uignore_remove:
            return uignore;

        case message_types::tignore_add:
        case message_types::tignore_remove:
            return tignore;

        case message_types::join:
            return join;

        case message_types::leave:
            return leave;

        case message_types::away:
            return away;

        case message_types::status:
            return status;

        case message_types::update:
            return update;
    }

    throw exception("unknown message type: ", message_type);
}

}   // namespace regexes

message_parser::message_parser(server_context& context) :
    context_(context),
    command_parser_(context)
{
}

void message_parser::parse_message(std::string const& str) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    try
    {
        context_.get_statistics_manager().add_input_message(str.size());

        LOG_COMP_TRACE(message_parser, "parsing message string: ", str);

        boost::xpressive::smatch what;

        message_type const message_type = message_codes::to_type(str);

        if(!boost::xpressive::regex_search(str, what, regexes::from_type(message_type)))
            throw exception("cannot parse message: ", str);

        switch(message_type)
        {
            case message_types::common:
                deliver_common_message(session_id(what[tags::session_id]), what[tags::text]);
                break;

            case message_types::personal:
                deliver_personal_message(session_id(what[tags::session_id]), what[tags::text], boost::lexical_cast<bool>(what[tags::nobuf]));
                break;

            case message_types::server:
                deliver_server_message(what[tags::text]);
                break;

            case message_types::admin:
                deliver_admin_message(what[tags::text], std::stoul(what[tags::access]));
                break;

            case message_types::command:
                command_parser_.parse_command(what[tags::text]);
                break;

            case message_types::uignore_add:
                deliver_uignore_add_message(session_id(what[tags::session_id]), session_id(what[tags::ignored_id]));
                break;

            case message_types::uignore_remove:
                deliver_uignore_remove_message(session_id(what[tags::session_id]), session_id(what[tags::ignored_id]));
                break;

            case message_types::tignore_add:
                deliver_tignore_add_message(session_id(what[tags::ignored_id]));
                break;

            case message_types::tignore_remove:
                deliver_tignore_remove_message(session_id(what[tags::ignored_id]));
                break;

            case message_types::join:
                deliver_join_message(session_id(what[tags::session_id]), std::stoll(what[tags::time]), what[tags::text]);
                break;

            case message_types::leave:
                deliver_leave_message(session_id(what[tags::session_id]), std::stoll(what[tags::time]), what[tags::text]);
                break;

            case message_types::away:
                deliver_update_message(session_id(what[tags::session_id]), update::away, what[tags::text]);
                break;

            case message_types::status:
                deliver_update_message(session_id(what[tags::session_id]), update::status, what[tags::text]);
                break;

            case message_types::update:
                deliver_update_message(session_id(what[tags::session_id]), std::stoi(what[tags::type]), what[tags::text]);
                break;

            default:
                throw exception("unknown message type: ", str);
        }
    }
    catch(std::exception const& e)
    {
        LOG_COMP_ERROR(message_parser, e);
    }
}

void message_parser::deliver_common_message(session_id const& id, std::string const& text) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_user_manager().deliver_common_message(id, text);
}

void message_parser::deliver_personal_message(session_id const& id, std::string const& text, bool not_buffered) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_user_manager().deliver_personal_message(id, text, not_buffered);
}

void message_parser::deliver_server_message(std::string const& text) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_room_manager().deliver_message(text);
}

void message_parser::deliver_admin_message(std::string const& text, std::size_t access) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_room_manager().deliver_message(chat::message::create(text, access));
}

void message_parser::deliver_uignore_add_message(session_id const& id, session_id const& ignored_id) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_ignore_manager().add_user_ignore(id, ignored_id);
}

void message_parser::deliver_uignore_remove_message(session_id const& id, session_id const& ignored_id) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_ignore_manager().remove_user_ignore(id, ignored_id);
}

void message_parser::deliver_tignore_add_message(session_id const& ignored_id) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_ignore_manager().add_total_ignore(ignored_id);
}

void message_parser::deliver_tignore_remove_message(session_id const& ignored_id) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_ignore_manager().remove_total_ignore(ignored_id);
}

void message_parser::deliver_join_message(session_id const& id, std::time_t time, std::string const& session_info) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_user_manager().join(id, time, session_info);
}

void message_parser::deliver_leave_message(session_id const& id, std::time_t time, std::string const& text) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_user_manager().leave(id, time, text);
}

void message_parser::deliver_update_message(session_id const& id, update::type update_type, std::string const& text) const
{
    LOG_COMP_TRACE_FUNCTION(message_parser);

    context_.get_user_manager().update(id, update_type, text);
}

}   // namespace chat
