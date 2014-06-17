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
#ifndef _chat_forwards_hpp
#define _chat_forwards_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/shared_ptr.hpp>


namespace chat
{

class server;

class user;
typedef boost::shared_ptr<user> user_ptr;

class room;
typedef boost::shared_ptr<room> room_ptr;

class session;
typedef boost::shared_ptr<session> session_ptr;

class message;
typedef boost::shared_ptr<message> message_ptr;

class connection;
typedef boost::shared_ptr<connection> connection_ptr;

}   // namespace chat

#endif  // _chat_forwards_hpp
