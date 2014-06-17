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
#include "config_loader.hpp"
#include "config_exception.hpp"
#include "config_manager.hpp"
#include "config_generator.hpp"
#include <file_utils.hpp>
#include <log_common.hpp>
#include <common.hpp>

// STL headers
#include <sstream>
#include <stdexcept>
#include <sstream>


config_data config_loader::config_;

const config_data& config_loader::get_config()
{
    return config_;
}

void config_loader::load_configuration()
{
    if(!util::file::exists(files::config))
    {
        LOGNOT("generating configuration");

        config_generator generator;

        generator.generate_config(files::old_conf);
        generator.save_config_file(files::config);
    }

    config_manager manager;

    try
    {
        LOGNOT("loading configuration");
        manager.load_configuration(files::config);
    }
    catch(config_error& e)
    {
        std::ostringstream message;

        message << "cannot parse configuration file ";
        message << files::config << ": " << e.message();

        if(e.line() > 0 && e.column() > 0)
        {
            message << " (line: " << e.line() << ", ";
            message << "column: " << e.column() << ")";
        }

        throw std::runtime_error(message.str());
    }

    // clear configuration data
    config_ = config_data();

    // chat configuration parameters

    config_entry chat = manager.get_entry("chat");

    config_entry_set listen = chat.get_entries("listen");
    config_.chat.endpoints = listen.as<std::string>();

    config_entry paths = chat.get_entry("paths");

    config_.chat.url_path = paths.get<std::string>("url", "");
    config_.chat.php_path = paths.get<std::string>("php", "../www");
    config_.chat.logs_path = paths.get<std::string>("logs", "logs");
    config_.chat.skin_path = paths.get<std::string>("skin", "skin");
    config_.chat.sess_path = paths.get<std::string>("sess", "www-sess");
    config_.chat.chat_path = paths.get<std::string>("chat", "www-chat");
    config_.chat.conf_path = paths.get<std::string>("conf", "www-conf");
    config_.chat.server_path = paths.get<std::string>("server", "www-server");

    config_entry intervals = chat.get_entry("intervals");

    config_.chat.ping_interval = intervals.get<long>("ping", 10);
    config_.chat.cron_interval = intervals.get<long>("cron", 60);
    config_.chat.status_interval = intervals.get<long>("status", 15);
    config_.chat.statistics_interval = intervals.get<long>("statistics", 300);

    config_entry timeouts = chat.get_entry("timeouts");

    config_.chat.leave_timeout = timeouts.get<long>("leave", 600);
    config_.chat.connect_timeout = timeouts.get<long>("connect", 15);

    config_.chat.antivirus_hack = chat.get<bool>("antivirus_hack", false);
    config_.chat.message_history = chat.get<std::size_t>("message_history", 50);

    config_.chat.http_server_endpoint = chat.get<std::string>("http_server", "127.0.0.1");

    config_entry data = chat.get_entry("data");

    config_.chat.data.max_size = data.get<long>("max_size", 10485760);
    config_.chat.data.clean_file = data.get<bool>("clean_file", true);

    config_entry admin = chat.get_entry("admin");

    config_.chat.admin.view_ip = admin.get<std::size_t>("view_ip", 100);
    config_.chat.admin.ignore = admin.get<std::size_t>("ignore", 300);

    config_entry session = chat.get_entry("session");

    config_.chat.session.name = session.get<std::string>("name", "PHPSESSID");
    config_.chat.session.alphabet = session.get<std::string>("alphabet", "");
    config_.chat.session.length = session.get<std::size_t>("length", 32);

    config_entry messages = chat.get_entry("messages");

    config_.chat.messages.join_room = messages.get<std::string>("join_room", "");
    config_.chat.messages.change_room = messages.get<std::string>("change_room", "");

    config_.chat.messages.server_restart = messages.get<std::string>("server_restart", "");
    config_.chat.messages.server_shutdown = messages.get<std::string>("server_shutdown", "");
    config_.chat.messages.address_blocked = messages.get<std::string>("address_blocked", "");

    config_.chat.messages.end_of_buffer = messages.get<std::string>("end_of_buffer", "");
    config_.chat.messages.lost_connection = messages.get<std::string>("lost_connection", "");
    config_.chat.messages.duplicate_windows = messages.get<std::string>("duplicate_windows", "");

    config_entry scripts = chat.get_entry("scripts");

    config_.chat.scripts.reload_irc_frame = scripts.get<std::string>("reload_irc_frame", "");
    config_.chat.scripts.reload_this_frame = scripts.get<std::string>("reload_this_frame", "");
    config_.chat.scripts.document_domain = scripts.get<std::string>("document_domain", "");

    // logging configuration parameters

    config_entry logging = manager.get_entry("logging");
    config_entry_set handlers = logging.get_entries("handler");

    for(config_entry_set::const_iterator it = handlers.begin(); it != handlers.end(); ++it)
    {
        config_data::logging::handler handler;

        handler.backup_log = it->get<bool>("backup_log");
        handler.max_size = it->get<std::size_t>("max_size");
        handler.filename = it->get<std::string>("filename");
        handler.severity = it->get<std::string>("severity");

        config_.logging.handlers.push_back(handler);
    }

    // filter configuration parameters

    config_entry filter = manager.get_entry("filter");

    config_.filter.enabled = filter.get<bool>("enabled", false);
    config_.filter.list_size = filter.get<std::size_t>("list_size", 4096);

    config_entry_set rules = filter.get_entries("rule");

    for(config_entry_set::const_iterator it = rules.begin(); it != rules.end(); ++it)
    {
        config_data::filter::rule rule;

        rule.name = it->get<std::string>("name");
        rule.address = it->get<std::string>("address");
        rule.netmask = it->get<std::string>("netmask");
        rule.block_duration = it->get<long>("block_duration");
        rule.conn_per_minute = it->get<std::size_t>("conn_per_minute");
        rule.max_connections = it->get<std::size_t>("max_connections");

        config_.filter.rules.push_back(rule);
    }

    // frontend configuration parameters

    config_entry frontend = manager.get_entry("frontend");

    config_.frontend.enabled = frontend.get<bool>("enabled", false);
    config_.frontend.http_header = frontend.get<std::string>("http_header", "X-Real-IP");

    config_entry_set addresses = frontend.get_entries("address");
    config_.frontend.addresses = addresses.as<std::string>("127.0.0.1");
}
