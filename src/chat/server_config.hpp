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
#ifndef _chat_server_config_hpp
#define _chat_server_config_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/filesystem/path.hpp>
#include <boost/asio.hpp>

// STL headers
#include <cstdint>
#include <vector>
#include <string>
#include <chrono>
#include <regex>


namespace chat
{

struct server_config
{
    struct endpoint
    {
        std::string hostname;
        std::uint16_t port;
    };

    std::vector<endpoint> endpoints;

    std::string url_path;
    std::string php_path;
    std::string logs_path;
    std::string skin_path;
    std::string sess_path;
    std::string chat_path;
    std::string conf_path;
    std::string server_path;

    std::chrono::seconds ping_interval;
    std::chrono::seconds cron_interval;
    std::chrono::seconds status_interval;
    std::chrono::seconds statistics_interval;

    std::chrono::seconds leave_timeout;
    std::chrono::seconds connect_timeout;

    std::size_t message_history;

    std::string http_server_endpoint;

    std::size_t max_data_file_size;
    bool clean_data_file;

    std::size_t admin_view_ip;
    std::size_t admin_ignore;

    std::string session_name;
    std::regex session_regex;

    std::string join_room_message;
    std::string change_room_message;

    std::string server_restart_message;
    std::string server_shutdown_message;
    std::string address_blocked_message;

    std::string end_of_buffer_message;
    std::string lost_connection_message;

    std::string duplicate_windows_message;

    std::string reload_irc_frame_script;
    std::string reload_this_frame_script;
    std::string document_domain_script;

    struct logging
    {
        struct console
        {
        } console;

        struct syslog
        {
        } syslog;

        struct file
        {
            bool backup_log;
            std::size_t max_size;
            std::string filename;
            std::string severity;
        };

        std::vector<file> files;
    } logging;

    struct filter
    {
        bool enabled;

        std::size_t max_list_size;

        struct rule
        {
            std::string name;

            boost::asio::ip::address address;
            boost::asio::ip::address netmask;

            std::chrono::seconds block_duration;

            std::size_t connections_per_minute;
            std::size_t max_connection_count;
        };

        std::vector<rule> rules;
    } filter;

    struct frontend
    {
        bool enabled;

        std::string http_header;

        std::vector<boost::asio::ip::address> addresses;
    } frontend;
};

//TODO: remove! temporary workaround!
extern server_config config; 

}   // namespace chat

#endif  // _chat_server_config_hpp
