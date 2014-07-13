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
#ifndef _chat_common_hpp
#define _chat_common_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/session_id.hpp>
#include <chat/forwards.hpp>
#include <http/buffer.hpp>

// MISSIO headers
#include <missio/format/string.hpp>

// BOOST headers
#include <boost/system/error_code.hpp>

// STL headers
#include <unordered_map>
#include <functional>
#include <string>


namespace chat
{

std::string const default_skin = "def";
std::string const default_room = "main";

std::string const room_name_placeholder   = "{@room}";
std::string const domain_name_placeholder = "{@domain}";

namespace update
{

typedef int type;

type const connection   = 1;
type const status       = 2;
type const away         = 4;
type const access       = 8;
type const auth         = 16;

}   // namespace update

namespace status
{

typedef int type;

type const joining  = 0;
type const joined   = 1;
type const lost     = 2;

}   // namespace status

namespace sex
{

typedef int type;

type const user     = 0;
type const male     = 1;
type const female   = 2;

}   // namespace sex

namespace formatters
{

missio::format::string const user_join("<script>_j({0},\"{1}\",{2})</script>");
missio::format::string const user_leave("<script>_p({0},\"{1}\",\"{2}\",{3},\"{4}\")</script>");
missio::format::string const user_update("<script>_u({0},[[\"{1}\",{2},{3}]])</script>");

}   // namespace formatters

namespace scripts
{

std::string const userlist_begin    = "<script>top.userlist_init([";
std::string const userlist_end      = "])</script>";

http::buffer const start_events("<script>top.startevent=1</script>");
http::buffer const ping_update("<script>s()</script>");

}   // namespace scripts

namespace files
{

std::string const pid           = "dimachat.pid";

std::string const chat          = "chat.txt";
std::string const rooms         = "rooms.txt";
std::string const crond         = "crond.php";
std::string const topic         = "topic.html";
std::string const z_channel     = "z-channel.html";

std::string const join_user     = "msg_join_user.txt";
std::string const join_male     = "msg_join_male.txt";
std::string const join_female   = "msg_join_female.txt";
std::string const leave_user    = "msg_part_user.txt";
std::string const leave_male    = "msg_part_male.txt";
std::string const leave_female  = "msg_part_female.txt";

}   // namespace files

typedef std::function<void (boost::system::error_code const&)> completion_handler;

typedef std::unordered_map<session_id, user_ptr> user_map;
typedef std::unordered_map<std::string, room_ptr> room_map;

}   // namespace chat

#endif  // _chat_common_hpp
