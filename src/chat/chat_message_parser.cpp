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
#include "chat_message_parser.hpp"
#include "chat_exception.hpp"
#include "chat_message.hpp"
#include "chat_common.hpp"
#include "chat_server.hpp"
#include "chat_room.hpp"
#include "chat_user.hpp"
#include <log_common.hpp>
#include <utilities.hpp>
#include <string_utils.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>


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

message_type to_type(const std::string& string)
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

    throw chat::error("cannot parse message: " + string);
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

using namespace tags;
using namespace boost::xpressive;

const sregex common     = bos >> +~_s >> _s >> (session_id= +~_s) >> _s >> (text= +_) >> eos;
const sregex personal   = bos >> +~_s >> _s >> (session_id= +~_s) >> _s >> (nobuf= +_d) >> _s >> (text= +_) >> eos;
const sregex server     = bos >> +~_s >> _s >> (text= +_) >> eos;
const sregex admin      = bos >> +~_s >> _s >> (access= +_d)  >> (text= +_) >> eos;
const sregex command    = bos >> +~_s >> _s >> (text= +_) >> eos;
const sregex uignore    = bos >> +~_s >> _s >> (session_id= +~_s) >> _s >> (ignored_id= +_) >> eos;
const sregex tignore    = bos >> +~_s >> _s >> (ignored_id= +_) >> eos;
const sregex join       = bos >> +~_s >> _s >> (session_id= +~_s) >> _s >> (time= +_d) >> _s >> (text= *_) >> eos;
const sregex leave      = bos >> +~_s >> _s >> (session_id= +~_s) >> _s >> (time= +_d) >> _s >> (text= *_) >> eos;
const sregex away       = bos >> +~_s >> _s >> (session_id= +~_s) >> _s >> (text= *_) >> eos;
const sregex status     = bos >> +~_s >> _s >> (session_id= +~_s) >> _s >> (text= +_) >> eos;
const sregex update     = bos >> +~_s >> _s >> (session_id= +~_s) >> _s >> (type= +_d) >> _s >> +_d >> _s >> (text= +_) >> eos;

const sregex& from_type(message_type message_type)
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

    throw chat::error("unknown message type: " + util::to_string(message_type));
}

}   // namespace regexes

message_parser::message_parser(chat::server& server) :
    server_(server),
    command_parser_(server)
{
}

void message_parser::parse_message(const std::string& string) const
{
    try
    {
        server_.get_statistics_manager()->add_input_message(string.size());

        LOGDBG("parsing message string: %1%") << string;

        message_type message_type;
        boost::xpressive::smatch what;

        message_type = message_codes::to_type(string);

        if(!boost::xpressive::regex_search(string, what, regexes::from_type(message_type)))
            throw chat::error("cannot parse message: " + string);

        switch(message_type)
        {
            case message_types::common:
                deliver_common_message(what[tags::session_id], what[tags::text]);
                break;

            case message_types::personal:
                deliver_personal_message(what[tags::session_id], what[tags::text], util::to_type<bool>(what[tags::nobuf]));
                break;

            case message_types::server:
                deliver_server_message(what[tags::text]);
                break;

            case message_types::admin:
                deliver_admin_message(what[tags::text], util::to_type<std::size_t>(what[tags::access]));
                break;

            case message_types::command:
                command_parser_.parse_command(what[tags::text]);
                break;

            case message_types::uignore_add:
                deliver_uignore_add_message(what[tags::session_id], what[tags::ignored_id]);
                break;

            case message_types::uignore_remove:
                deliver_uignore_remove_message(what[tags::session_id], what[tags::ignored_id]);
                break;

            case message_types::tignore_add:
                deliver_tignore_add_message(what[tags::ignored_id]);
                break;

            case message_types::tignore_remove:
                deliver_tignore_remove_message(what[tags::ignored_id]);
                break;

            case message_types::join:
                deliver_join_message(what[tags::session_id], util::to_type<std::time_t>(what[tags::time]), what[tags::text]);
                break;

            case message_types::leave:
                deliver_leave_message(what[tags::session_id], util::to_type<std::time_t>(what[tags::time]), what[tags::text]);
                break;

            case message_types::away:
                deliver_update_message(what[tags::session_id], update::away, what[tags::text]);
                break;

            case message_types::status:
                deliver_update_message(what[tags::session_id], update::status, what[tags::text]);
                break;

            case message_types::update:
                deliver_update_message(what[tags::session_id], util::to_type<update::type>(what[tags::type]), what[tags::text]);
                break;

            default:
                throw chat::error("unknown message type: " + string);
        }
    }
    catch(std::exception& e)
    {
        LOGERR("cannot deliver message: %1%") << e.what();
    }
}

void message_parser::deliver_common_message(const std::string& session_id, const std::string& text) const
{
    if(server_.get_user_manager()->contains(session_id))
    {
        if(!server_.get_ignore_manager().is_ignored(session_id))
        {
            chat::message_ptr message = chat::message::create(text);
            message->set_sender(server_.get_user_manager()->user(session_id));
            server_.get_user_manager()->user(session_id)->room()->deliver(message);
        }
    }
}

void message_parser::deliver_personal_message(const std::string& session_id, const std::string& text, bool not_buffered) const
{
    if(server_.get_user_manager()->contains(session_id))
    {
        if(!server_.get_ignore_manager().is_ignored(session_id))
        {
            chat::message_ptr message = chat::message::create(text);
            message->set_recipient(server_.get_user_manager()->user(session_id));
            server_.get_user_manager()->user(session_id)->room()->deliver(message, not_buffered);
        }
    }
}

void message_parser::deliver_server_message(const std::string& text) const
{
    server_.get_room_manager()->deliver_message(text);
}

void message_parser::deliver_admin_message(const std::string& text, std::size_t access) const
{
    server_.get_room_manager()->deliver_message(chat::message::create(text, access));
}

void message_parser::deliver_uignore_add_message(const std::string& session_id, const std::string& ignored_id) const
{
    if(server_.get_user_manager()->contains(session_id))
        server_.get_user_manager()->user(session_id)->add_ignore(ignored_id);
}

void message_parser::deliver_uignore_remove_message(const std::string& session_id, const std::string& ignored_id) const
{
    if(server_.get_user_manager()->contains(session_id))
    {
        if(ignored_id[0] == '*')
            server_.get_user_manager()->user(session_id)->clear_ignores();
        else
            server_.get_user_manager()->user(session_id)->remove_ignore(ignored_id);
    }
}

void message_parser::deliver_tignore_add_message(const std::string& ignored_id) const
{
    LOGNOT("adding total ignore for session: %1%") << ignored_id;
    server_.get_ignore_manager().add_ignore(ignored_id);
}

void message_parser::deliver_tignore_remove_message(const std::string& ignored_id) const
{
    if(ignored_id[0] == '*')
    {
        LOGNOT("clearing total ignore list");
        server_.get_ignore_manager().clear_ignores();
    }
    else
    {
        LOGNOT("removing total ignore for session: %1%") << ignored_id;
        server_.get_ignore_manager().remove_ignore(ignored_id);
    }
}

void message_parser::deliver_join_message(const std::string& session_id, std::time_t time, const std::string& session_info) const
{
    server_.get_user_manager()->join(session_id, time, session_info);
}

void message_parser::deliver_leave_message(const std::string& session_id, std::time_t time, const std::string& text) const
{
    server_.get_user_manager()->leave(session_id, time, text);
}

void message_parser::deliver_update_message(const std::string& session_id, update::type update_type, const std::string& text) const
{
    if(server_.get_user_manager()->contains(session_id))
        server_.get_user_manager()->user(session_id)->update(update_type, text);
}

}   // namespace chat
