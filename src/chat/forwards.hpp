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
#ifndef _chat_forwards_hpp
#define _chat_forwards_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <memory>


namespace chat
{

class server;

class user;
class room;
class session;
class message;
class connection;

typedef std::shared_ptr<user> user_ptr;
typedef std::shared_ptr<room> room_ptr;
typedef std::shared_ptr<session> session_ptr;
typedef std::shared_ptr<message> message_ptr;
typedef std::shared_ptr<connection> connection_ptr;

}   // namespace chat

#endif  // _chat_forwards_hpp
