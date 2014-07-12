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
#ifndef _chat_command_parser_hpp
#define _chat_command_parser_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/session_id.hpp>

// BOOST headers
#include <boost/asio.hpp>

// STL headers
#include <string>
#include <ctime>


namespace chat
{

class server_context;

class command_parser
{
public:
    explicit command_parser(server_context& context);

    command_parser(command_parser const&) = delete;
    command_parser& operator=(command_parser const&) = delete;

    void parse_command(std::string const& command) const;

private:
    void deliver_clear_history_command() const;
    void deliver_topic_command(bool force) const;
    void deliver_op_command(session_id const& id, std::size_t access) const;
    void deliver_unignore_command(boost::asio::ip::address const& address) const;
    void deliver_shutdown_command(std::string const& admin_name) const;
    void deliver_restart_command(std::string const& admin_name) const;
    void deliver_reconfigure_command() const;
    void deliver_loadskin_command(std::string const& skin_name) const;
    void deliver_botreg_command(session_id const& id) const;
    void deliver_join_command(session_id const& id, std::string const& room_name) const;

private:
    server_context& context_;
};

}   // namespace chat

#endif  // _chat_command_parser_hpp
