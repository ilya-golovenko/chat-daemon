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
#ifndef _chat_config_common_hpp
#define _chat_config_common_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <vector>
#include <string>


namespace chat
{

struct config_data
{
    struct chat
    {
        std::vector<std::string> endpoints;

        std::string url_path;
        std::string php_path;
        std::string logs_path;
        std::string skin_path;
        std::string sess_path;
        std::string chat_path;
        std::string conf_path;
        std::string server_path;

        long ping_interval;
        long cron_interval;
        long status_interval;
        long statistics_interval;

        long leave_timeout;
        long connect_timeout;

        std::size_t message_history;

        std::string http_server_endpoint;

        struct data
        {
            long max_size;
            bool clean_file;
        } data;

        struct admin
        {
            std::size_t view_ip;
            std::size_t ignore;
        } admin;

        struct session
        {
            std::string name;
            std::string alphabet;
            std::size_t length;
        } session;

        struct messages
        {
            std::string join_room;
            std::string change_room;

            std::string server_restart;
            std::string server_shutdown;
            std::string address_blocked;

            std::string end_of_buffer;
            std::string lost_connection;

            std::string duplicate_windows;
        } messages;

        struct scripts
        {
            std::string reload_irc_frame;
            std::string reload_this_frame;
            std::string document_domain;
        } scripts;
    } chat;

    struct logging
    {
        struct handler
        {
            bool backup_log;
            std::size_t max_size;
            std::string filename;
            std::string severity;
        };

        std::vector<handler> handlers;
    } logging;

    struct filter
    {
        bool enabled;
        std::size_t list_size;

        struct rule
        {
            std::string name;

            std::string address;
            std::string netmask;

            long block_duration;

            std::size_t conn_per_minute;
            std::size_t max_connections;
        };

        std::vector<rule> rules;
    } filter;

    struct frontend
    {
        bool enabled;
        std::string http_header;
        std::vector<std::string> addresses;
    } frontend;
};

}   // namespace chat

#endif  // _chat_config_common_hpp
