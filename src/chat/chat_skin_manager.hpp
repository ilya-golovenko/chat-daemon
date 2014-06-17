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
#ifndef _chat_skin_manager_hpp
#define _chat_skin_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_forwards.hpp"
#include "chat_common.hpp"

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// STL headers
#include <string>
#include <vector>
#include <map>


namespace chat
{

class skin_manager :
    private boost::noncopyable
{
public:
    skin_manager(chat::server& server);

    void configure();
    void load_skin(const std::string& skin);

    const std::string& get_join_message(user_ptr user) const;
    const std::string& get_leave_message(user_ptr user) const;

private:
    typedef std::map<std::string, std::vector<std::string> > message_map_type;

    const std::string replace_quotes(const std::string& message) const;

    void load_message_files(const std::string& skin, const std::string& skin_path);
    void read_message_file(const std::string& filename, std::vector<std::string>& messages);

    const std::string& get_join_message(sex::type sex_type, const std::string& skin) const;
    const std::string& get_leave_message(sex::type sex_type, const std::string& skin) const;
    const std::string& get_message(const message_map_type& messages, const std::string& skin) const;

private:
    chat::server& server_;

    message_map_type user_join_messages_;
    message_map_type male_join_messages_;
    message_map_type female_join_messages_;

    message_map_type user_leave_messages_;
    message_map_type male_leave_messages_;
    message_map_type female_leave_messages_;
};

typedef boost::shared_ptr<skin_manager> skin_manager_ptr;

}   // namespace chat

#endif  // _chat_skin_manager_hpp
