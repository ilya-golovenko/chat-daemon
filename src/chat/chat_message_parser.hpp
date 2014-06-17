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
#ifndef _chat_message_parser_hpp
#define _chat_message_parser_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_common.hpp"
#include "chat_forwards.hpp"
#include "chat_command_parser.hpp"

// BOOST headers
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// STL headers
#include <string>
#include <ctime>


namespace chat
{

class message_parser :
    private boost::noncopyable,
    public boost::enable_shared_from_this<message_parser>
{
public:
    message_parser(chat::server& server);
    void parse_message(const std::string& string) const;

    void deliver_common_message(const std::string& session_id, const std::string& text) const;
    void deliver_personal_message(const std::string& session_id, const std::string& text, bool not_buffered) const;
    void deliver_server_message(const std::string& text) const;
    void deliver_admin_message(const std::string& text, std::size_t access) const;
    void deliver_command_message(const std::string& text) const;
    void deliver_uignore_add_message(const std::string& session_id, const std::string& ignored_id) const;
    void deliver_uignore_remove_message(const std::string& session_id, const std::string& ignored_id) const;
    void deliver_tignore_add_message(const std::string& ignored_id) const;
    void deliver_tignore_remove_message(const std::string& ignored_id) const;
    void deliver_join_message(const std::string& session_id, std::time_t time, const std::string& session_info) const;
    void deliver_leave_message(const std::string& session_id, std::time_t time, const std::string& text) const;
    void deliver_update_message(const std::string& session_id, update::type update_type, const std::string& text) const;

private:
    chat::server& server_;
    command_parser command_parser_;
};

typedef boost::shared_ptr<message_parser> message_parser_ptr;

}   // namespace chat

#endif  // _chat_message_parser_hpp
