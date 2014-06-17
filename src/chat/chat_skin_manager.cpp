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

// Application headers
#include "chat_skin_manager.hpp"
#include "chat_exception.hpp"
#include "chat_server.hpp"
#include "chat_user.hpp"
#include <path_utils.hpp>
#include <utilities.hpp>
#include <config_common.hpp>
#include <log_common.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <fstream>


namespace chat
{

skin_manager::skin_manager(chat::server& server) :
    server_(server)
{
}

void skin_manager::configure()
{
    LOGNOT("configuring skin manager");
    load_skin(chat::default_skin);
}

void skin_manager::load_skin(const std::string& skin)
{
    LOGNOT("loading chat skin: %1%") << skin;

    std::string skin_path = util::path::combine(
        config.chat.php_path, config.chat.skin_path);

    load_message_files(skin, util::path::combine(skin_path, skin));
}

const std::string& skin_manager::get_join_message(user_ptr user) const
{
    return get_join_message(user->session()->sex(), user->session()->skin());
}

const std::string& skin_manager::get_leave_message(user_ptr user) const
{
    return get_leave_message(user->session()->sex(), user->session()->skin());
}

const std::string skin_manager::replace_quotes(const std::string& string) const
{
    return boost::replace_all_copy(string, std::string("\""), std::string("&quot;"));
}

void skin_manager::load_message_files(const std::string& skin, const std::string& skin_path)
{
    read_message_file(util::path::combine(skin_path, files::join_user), user_join_messages_[skin]);
    read_message_file(util::path::combine(skin_path, files::join_male), male_join_messages_[skin]);
    read_message_file(util::path::combine(skin_path, files::join_female), female_join_messages_[skin]);

    read_message_file(util::path::combine(skin_path, files::leave_user), user_leave_messages_[skin]);
    read_message_file(util::path::combine(skin_path, files::leave_male), male_leave_messages_[skin]);
    read_message_file(util::path::combine(skin_path, files::leave_female), female_leave_messages_[skin]);
}

void skin_manager::read_message_file(const std::string& filename, std::vector<std::string>& messages)
{
    LOGNOT("reading message file: %1%") << filename;

    std::string line;
    std::fstream file;

    file.open(filename.c_str());
    if(!file.is_open())
        throw chat::error("cannot open message file: " + filename);

    messages.clear();

    while(!file.eof())
    {
        if(std::getline(file, line))
        {
            boost::trim(line);
            if(!line.empty())
            {
                if(line[0] != '#')
                    messages.push_back(replace_quotes(line));
            }
        }
    }

    file.close();

    if(messages.empty())
        throw chat::error("message file is empty: " + filename);
}

const std::string& skin_manager::get_join_message(sex::type sex_type, const std::string& skin) const
{
    switch(sex_type)
    {
        case sex::female:
            return get_message(female_join_messages_, skin);

        case sex::male:
            return get_message(male_join_messages_, skin);

        default:
            return get_message(user_join_messages_, skin);
    }
}

const std::string& skin_manager::get_leave_message(sex::type sex_type, const std::string& skin) const
{
    switch(sex_type)
    {
        case sex::female:
            return get_message(female_leave_messages_, skin);

        case sex::male:
            return get_message(male_leave_messages_, skin);

        default:
            return get_message(user_leave_messages_, skin);
    }
}

const std::string& skin_manager::get_message(const message_map_type& messages, const std::string& skin) const
{
    message_map_type::const_iterator it = messages.find(skin);

    if(it == messages.end() || it->second.empty())
        it = messages.find(chat::default_skin);

    return util::get_random_string(it->second);
}

}   // namespace chat
