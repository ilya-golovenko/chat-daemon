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

// Application headers
#include <chat/skin_manager.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <chat/exception.hpp>
#include <chat/user.hpp>
#include <misc/path_utils.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <fstream>


namespace chat
{

skin_manager::skin_manager(server_context& context) :
    context_(context)
{
}

void skin_manager::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(skin_manager);

    LOG_COMP_NOTICE(skin_manager, "configuring");

    skin_path_ = util::path::combine(config.php_path, config.skin_path);

    load_skin(default_skin);
}

void skin_manager::start()
{
    LOG_COMP_TRACE_FUNCTION(skin_manager);

    LOG_COMP_NOTICE(skin_manager, "starting");

    LOG_COMP_NOTICE(skin_manager, "started");
}

void skin_manager::stop()
{
    LOG_COMP_TRACE_FUNCTION(skin_manager);

    LOG_COMP_NOTICE(skin_manager, "stopping");

    LOG_COMP_NOTICE(skin_manager, "stopped");
}

void skin_manager::load_skin(std::string const& skin)
{
    LOG_COMP_TRACE_FUNCTION(skin_manager);

    LOG_COMP_NOTICE(skin_manager, "loading chat skin: ", skin);

    load_message_files(skin, util::path::combine(skin_path_, skin));
}

std::string const& skin_manager::get_join_message(user_ptr user) const
{
    return get_join_message(user->get_sex(), user->get_session()->get_skin());
}

std::string const& skin_manager::get_leave_message(user_ptr user) const
{
    return get_leave_message(user->get_sex(), user->get_session()->get_skin());
}

void skin_manager::load_message_files(std::string const& skin, std::string const& skin_path)
{
    LOG_COMP_TRACE_FUNCTION(skin_manager);

    read_message_file(util::path::combine(skin_path, files::join_user), user_join_messages_[skin]);
    read_message_file(util::path::combine(skin_path, files::join_male), male_join_messages_[skin]);
    read_message_file(util::path::combine(skin_path, files::join_female), female_join_messages_[skin]);

    read_message_file(util::path::combine(skin_path, files::leave_user), user_leave_messages_[skin]);
    read_message_file(util::path::combine(skin_path, files::leave_male), male_leave_messages_[skin]);
    read_message_file(util::path::combine(skin_path, files::leave_female), female_leave_messages_[skin]);
}

void skin_manager::read_message_file(std::string const& filename, message_vector& messages)
{
    LOG_COMP_TRACE_FUNCTION(skin_manager);

    LOG_COMP_NOTICE(skin_manager, "reading message file: ", filename);

    std::string line;
    std::ifstream file;

    file.open(filename.c_str());

    if(!file.is_open())
    {
        throw exception("cannot open message file: ", filename);
    }

    messages.clear();

    while(!file.eof())
    {
        if(std::getline(file, line))
        {
            boost::algorithm::trim(line);

            if(!line.empty() && line[0] != '#')
            {
                messages.push_back(replace_quotes(line));
            }
        }
    }

    file.close();

    if(messages.empty())
    {
        throw exception("message file is empty: ", filename);
    }
}

std::string const& skin_manager::get_join_message(sex::type sex, std::string const& skin) const
{
    switch(sex)
    {
        case sex::female:
            return get_message(female_join_messages_, skin);

        case sex::male:
            return get_message(male_join_messages_, skin);

        default:
            return get_message(user_join_messages_, skin);
    }
}

std::string const& skin_manager::get_leave_message(sex::type sex, std::string const& skin) const
{
    switch(sex)
    {
        case sex::female:
            return get_message(female_leave_messages_, skin);

        case sex::male:
            return get_message(male_leave_messages_, skin);

        default:
            return get_message(user_leave_messages_, skin);
    }
}

std::string const& skin_manager::get_message(message_map const& messages, std::string const& skin) const
{
    message_map::const_iterator it = messages.find(skin);

    if(it == messages.end() || it->second.empty())
    {
        it = messages.find(default_skin);
    }

    return get_random_message(it->second);
}

std::string const& skin_manager::get_random_message(message_vector const& messages) const
{
    return messages.at(context_.generate_random_number() % messages.size());
}

std::string skin_manager::replace_quotes(std::string const& str)
{
    return boost::algorithm::replace_all_copy(str, std::string("\""), std::string("&quot;"));
}

}   // namespace chat
