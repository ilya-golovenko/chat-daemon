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
#ifndef _chat_command_parser_hpp
#define _chat_command_parser_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_common.hpp"
#include "chat_forwards.hpp"

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>

// STL headers
#include <string>
#include <ctime>


namespace chat
{

class command_parser :
    private boost::noncopyable
{
public:
    command_parser(chat::server& server);
    void parse_command(const std::string& command) const;

private:
    void deliver_clear_history_command() const;
    void deliver_topic_command(bool force) const;
    void deliver_op_command(const std::string& session_id, std::size_t access) const;
    void deliver_unignore_command(const asio::ip::address& address) const;
    void deliver_shutdown_command(const std::string& admin_name) const;
    void deliver_restart_command(const std::string& admin_name) const;
    void deliver_restart_command() const;
    void deliver_loadskin_command(const std::string& skin_name) const;
    void deliver_botreg_command(const std::string& session_id) const;
    void deliver_join_command(const std::string& session_id, const std::string& room_name) const;

private:
    chat::server& server_;
};

}   // namespace chat

#endif  // _chat_command_parser_hpp
