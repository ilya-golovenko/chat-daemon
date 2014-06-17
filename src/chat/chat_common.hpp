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
#ifndef _chat_common_hpp
#define _chat_common_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_forwards.hpp"
#include <http_buffer.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/unordered_map.hpp>
#include <boost/function.hpp>
#include <boost/format.hpp>

// STL headers
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

const boost::format user_join("<script>_j(%1%,\"%2%\",%3%)</script>");
const boost::format user_leave("<script>_p(%1%,\"%2%\",\"%3%\",%4%,\"%5%\")</script>");
const boost::format user_update("<script>_u(%1%,[[\"%2%\",%3%,%4%]])</script>");

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

namespace parameters
{

const std::string antivirus_hack    = "antivirus_hack";

}   // namespace parameters

typedef boost::function<void (const asio::error_code&)> completion_handler;

typedef boost::unordered_map<std::string, user_ptr> user_map_type;
typedef boost::unordered_map<std::string, room_ptr> room_map_type;

}   // namespace chat

#endif  // _chat_common_hpp
