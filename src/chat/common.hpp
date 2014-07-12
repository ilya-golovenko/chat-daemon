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

const std::string default_skin = "def";
const std::string default_room = "main";

const std::string room_name_placeholder   = "{@room}";
const std::string domain_name_placeholder = "{@domain}";

namespace update
{

typedef int type;

const type connection   = 1;
const type status       = 2;
const type away         = 4;
const type access       = 8;
const type auth         = 16;

}   // namespace update

namespace status
{

typedef int type;

const type joining  = 0;
const type joined   = 1;
const type lost     = 2;

}   // namespace status

namespace sex
{

typedef int type;

const type user     = 0;
const type male     = 1;
const type female   = 2;

}   // namespace sex

namespace formatters
{

const missio::format::string user_join("<script>_j({0},\"{1}\",{2})</script>");
const missio::format::string user_leave("<script>_p({0},\"{1}\",\"{2}\",{3},\"{4}\")</script>");
const missio::format::string user_update("<script>_u({0},[[\"{1}\",{2},{3}]])</script>");

}   // namespace formatters

namespace scripts
{

const std::string userlist_begin    = "<script>top.userlist_init([";
const std::string userlist_end      = "])</script>";

const http::buffer start_events("<script>top.startevent=1</script>");
const http::buffer ping_update("<script>s()</script>");

}   // namespace scripts

namespace files
{

const std::string pid           = "dimachat.pid";

const std::string chat          = "chat.txt";
const std::string rooms         = "rooms.txt";
const std::string crond         = "crond.php";
const std::string topic         = "topic.html";
const std::string z_channel     = "z-channel.html";

const std::string join_user     = "msg_join_user.txt";
const std::string join_male     = "msg_join_male.txt";
const std::string join_female   = "msg_join_female.txt";
const std::string leave_user    = "msg_part_user.txt";
const std::string leave_male    = "msg_part_male.txt";
const std::string leave_female  = "msg_part_female.txt";

}   // namespace files

typedef std::function<void (boost::system::error_code const&)> completion_handler;

typedef std::unordered_map<session_id, user_ptr> user_map;
typedef std::unordered_map<std::string, room_ptr> room_map;

}   // namespace chat

#endif  // _chat_common_hpp
