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
#ifndef _chat_skin_manager_hpp
#define _chat_skin_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/common.hpp>

// STL headers
#include <unordered_map>
#include <vector>
#include <string>


namespace chat
{

class server_context;
struct server_config;

class skin_manager
{
public:
    explicit skin_manager(server_context& context);

    skin_manager(skin_manager const&) = delete;
    skin_manager& operator=(skin_manager const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

    void load_skin(std::string const& skin);

    std::string const& get_join_message(user_ptr user) const;
    std::string const& get_leave_message(user_ptr user) const;

private:
    typedef std::vector<std::string> message_vector;
    typedef std::unordered_map<std::string, message_vector> message_map;

private:
    void load_message_files(std::string const& skin, std::string const& skin_path);
    void read_message_file(std::string const& filename, message_vector& messages);

    std::string const& get_join_message(sex::type sex, std::string const& skin) const;
    std::string const& get_leave_message(sex::type sex, std::string const& skin) const;

    std::string const& get_message(message_map const& messages, std::string const& skin) const;

    std::string const& get_random_message(message_vector const& messages) const;

private:
    static std::string replace_quotes(std::string const& str);

private:
    server_context& context_;

    std::string skin_path_;

    message_map user_join_messages_;
    message_map male_join_messages_;
    message_map female_join_messages_;

    message_map user_leave_messages_;
    message_map male_leave_messages_;
    message_map female_leave_messages_;
};

}   // namespace chat

#endif  // _chat_skin_manager_hpp
