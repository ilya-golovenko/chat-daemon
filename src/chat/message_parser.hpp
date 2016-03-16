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
#ifndef _chat_message_parser_hpp
#define _chat_message_parser_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/command_parser.hpp>
#include <chat/session_id.hpp>
#include <chat/common.hpp>

// STL headers
#include <string>
#include <ctime>


namespace chat
{

class message_parser
{
public:
    explicit message_parser(server_context& context);

    message_parser(message_parser const&) = delete;
    message_parser& operator=(message_parser const&) = delete;

    void parse_message(std::string const& str) const;

    void deliver_common_message(session_id const& id, std::string const& text) const;
    void deliver_personal_message(session_id const& id, std::string const& text, bool not_buffered) const;
    void deliver_server_message(std::string const& text) const;
    void deliver_admin_message(std::string const& text, std::size_t access) const;
    void deliver_command_message(std::string const& text) const;
    void deliver_uignore_add_message(session_id const& id, session_id const& ignored_id) const;
    void deliver_uignore_remove_message(session_id const& id, session_id const& ignored_id) const;
    void deliver_tignore_add_message(session_id const& ignored_id) const;
    void deliver_tignore_remove_message(session_id const& ignored_id) const;
    void deliver_join_message(session_id const& id, std::time_t time, std::string const& session_info) const;
    void deliver_leave_message(session_id const& id, std::time_t time, std::string const& text) const;
    void deliver_update_message(session_id const& id, update::type update_type, std::string const& text) const;

private:
    server_context& context_;
    command_parser command_parser_;
};

}   // namespace chat

#endif  // _chat_message_parser_hpp
