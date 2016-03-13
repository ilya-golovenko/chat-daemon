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
#include <chat/config_manager.hpp>
#include <chat/server_config.hpp>
#include <chat/exception.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{

void config_manager::load_configuration(std::string const& filename, server_config& config) const
{
    LOG_COMP_TRACE_FUNCTION(config_manager);

    LOG_COMP_NOTICE(config_manager, "loading configuration");

    config.endpoints.push_back({ "127.0.0.1", 8888 });

    config.url_path     = "";
    config.php_path     = "../www";
    config.logs_path    = "logs";
    config.skin_path    = "skin";
    config.sess_path    = "www-sess";
    config.chat_path    = "www-chat";
    config.conf_path    = "www-conf";
    config.server_path  = "www-server";

    config.ping_interval = std::chrono::seconds(10);
    config.cron_interval = std::chrono::seconds(60);
    config.status_interval = std::chrono::seconds(15);
    config.statistics_interval = std::chrono::seconds(300);

    config.leave_timeout = std::chrono::seconds(600);
    config.connect_timeout = std::chrono::seconds(15);

    config.message_history = 50;

    config.http_server_endpoint = "127.0.0.1";

    config.max_data_file_size = 10485760;
    config.clean_data_file = true;

    config.admin_view_ip = 100;
    config.admin_ignore = 300;

    config.session_name = "PHPSESSID";
    config.session_regex = "^[a-zA-Z0-9,-]{21,40}$";

    config.join_room_message = "<font color=red><b><center>You have come into the room {@room}</center></b></font><br>";
    config.change_room_message = "user has passed into the room {@room}";

    config.server_restart_message = "<hr size=1 noshade><h2>Chat server is being restarted. Please wait...</h2><script>s2()</script>";
    config.server_shutdown_message = "<hr size=1 noshade><h2>Chat server is shutting down...</h2><br><br><br><script>s2()</script>";
    config.address_blocked_message = "<script>_i(\"q_notice\",{@time},'','','','',\"Address <b>{@address}</b> has been blocked by IP filter\")</script>";

    config.end_of_buffer_message = "<hr size=1 noshade>";
    config.lost_connection_message = "connection with the user has been lost";

    config.duplicate_windows_message = "<hr size=1 noshade><font color=red>"
                                       "<h3>Connection has been closed by chat server.<br>"
                                       "Please, do not open two or more chat windows simultaneously!</h3>";

    config.reload_irc_frame_script = "<script>"
                                     "var irc_frame_url='http://'+top.http_host+'/'+top.wwp+top.url_irc+'?'+top.asess;"
                                     "parent.frames.irc.location.href=irc_frame_url;"
                                     "</script>";

    config.reload_this_frame_script = "<script>"
                                      "location.reload(true);"
                                      "</script>";

    config.document_domain_script = "<script>"
                                    "if(navigator.appName==\"Netscape\" || navigator.appName==\"Opera\") document.domain=\"{@domain}\";"
                                    "</script>";

    config.filter.enabled = false;
    config.filter.max_list_size = 4096;
/*
    struct rule
    {
        std::string name;
        std::string address;

        std::size_t max_connection_count;
        std::size_t connections_per_minute;

        std::chrono::seconds block_duration;
    };

    std::vector<rule> rules;
*/

    config.frontend.enabled = false;
    config.frontend.http_header = "X-Real-IP";

    //config.frontend.addresses;
}

}   // namespace chat
