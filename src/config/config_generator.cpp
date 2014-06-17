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
#include "config_generator.hpp"
#include <file_utils.hpp>
#include <string_utils.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string.hpp>

// STL headers
#include <stdexcept>


config_generator::config_generator() : nesting_depth_(0)
{
}

void config_generator::generate_config(const std::string& filename)
{
    nesting_depth_ = 0;
    config_.str(strings::empty);

    config_data_ = util::file::read_text(filename);

    start_new_block("chat");

    add_simple_entry("listen", find_string(
        "wwwhost") + ":" + find_number("wwwport"));

    start_new_block("paths");

    add_quoted_entry("url", find_string("wwp"));
    add_quoted_entry("php", find_string("phppath"));
    add_quoted_entry("logs", find_string("logdir"));
    add_quoted_entry("skin", find_string("skindir"));
    add_quoted_entry("sess", find_string("sesspath"));
    add_quoted_entry("chat", find_string("chatpath"));
    add_quoted_entry("conf", find_string("confpath"));
    add_quoted_entry("server", find_string("serverpath"));

    end_current_block();    // paths
    start_new_block("intervals");

    std::string ping_string = find_number("idle1");

    std::size_t ping_value = util::to_type<std::size_t>(ping_string);
    add_simple_entry("ping", util::to_string(ping_value / 1000));

    add_simple_entry("cron", find_number("cron_php_run"));
    add_simple_entry("status", find_number("cron_live"));
    add_simple_entry("statistics", "300");  // no original value

    end_current_block();    // intervals
    start_new_block("timeouts");

    add_simple_entry("leave", find_number("idle7"));
    add_simple_entry("connect", find_number("time_http"));
    add_simple_entry("shutdown", find_number("time_shutdown"));

    end_current_block();

    add_simple_entry("antivirus_hack", "false");
    add_simple_entry("message_history", find_number("history"));

    const std::string hostname = find_string("wwwhost");
    const std::string port = find_number("http_port");

    add_simple_entry("http_server", hostname + ":" + port);

    start_new_block("data");

    add_simple_entry("clean_file", find_number("clean_old_chat"));
    add_simple_entry("max_size", find_number("max_chat_size"));

    end_current_block();    // data
    start_new_block("admin");

    add_simple_entry("view_ip", find_number("admcmd_ip"));
    add_simple_entry("ignore", find_number("admcmd_ignore"));

    end_current_block();    // admin
    start_new_block("session");

    add_quoted_entry("name", "PHPSESSID");
    add_quoted_entry("alphabet", "0123456789abcdefABCDEF");
    add_simple_entry("length", "32");

    end_current_block();    // session
    start_new_block("messages");

    add_quoted_entry("join_room", "<font color=red><b><center>You have come into the room {@room}</center></b></font><br>");
    add_quoted_entry("change_room", "user has passed into the room {@room}");

    add_quoted_entry("server_restart", "<hr><h2>Chat server is being restarted. Please wait...</h2><script>s2()</script>");
    add_quoted_entry("server_shutdown", "<hr><h2>Chat server is shutting down...</h2><br><br><br><script>s2()</script>");
    add_quoted_entry("address_blocked", "<script>_i(\"q_notice\",{@time},'','','','',\"Address <b>{@address}</b> has been blocked by IP filter\")</script>");

    add_quoted_entry("lost_connection", "connection has been lost");
    add_quoted_entry("end_of_buffer", "<hr>");

    add_quoted_entry("duplicate_windows", "<hr><font color=red><h3>Connection has been closed "
        "by chat server.<br>Please, do not open two or more chat windows simultaneously!</h3>");

    end_current_block();    // messages
    start_new_block("scripts");

    add_quoted_entry("reload_irc_frame", "<script>var irc_frame_url='http://'+top.http_host+'/'+"
        "top.wwp+top.url_irc+'?'+top.asess;parent.frames.irc.location.href=irc_frame_url;</script>");

    add_quoted_entry("reload_this_frame", "<script>location.reload(true)</script>");

    add_quoted_entry("document_domain", "<script>if(navigator.appName==\"Netscape\""
        " || navigator.appName==\"Opera\") document.domain=\"{@domain}\";</script>");

    end_current_block();    // scripts
    end_current_block();    // chat

    start_new_block("logging");
    start_new_block("handler");

    add_simple_entry("backup_log", "true");
    add_simple_entry("max_size", "1048576");
    add_quoted_entry("filename", "spdaemon.log");
    add_simple_entry("severity", "notice, warning, error");

    end_current_block();
    start_new_block("handler");

    add_simple_entry("backup_log", "true");
    add_simple_entry("max_size", "1048576");
    add_quoted_entry("filename", "error.log");
    add_simple_entry("severity", "error");

    end_current_block();
    start_new_block("handler");

    add_simple_entry("backup_log", "true");
    add_simple_entry("max_size", "1048576");
    add_quoted_entry("filename", "warning.log");
    add_simple_entry("severity", "error, warning");

    end_current_block();
    start_new_block("handler");

    add_simple_entry("backup_log", "false");
    add_simple_entry("max_size", "4194304");
    add_quoted_entry("filename", "debug.log");
    add_simple_entry("severity", "all");

    end_current_block();    // handler
    end_current_block();    // logging

    start_new_block("filter");

    add_simple_entry("enabled", "true");
    add_simple_entry("list_size", "4096");

    start_new_block("rule");

    add_quoted_entry("name", "localhost");
    add_simple_entry("address", "127.0.0.1");
    add_simple_entry("netmask", "255.0.0.0");
    add_simple_entry("block_duration", "600");
    add_simple_entry("conn_per_minute", "50");
    add_simple_entry("max_connections", "20");

    end_current_block();
    start_new_block("rule");

    add_quoted_entry("name", "intranet");
    add_simple_entry("address", "192.168.0.0");
    add_simple_entry("netmask", "255.255.0.0");
    add_simple_entry("block_duration", "600");
    add_simple_entry("conn_per_minute", "50");
    add_simple_entry("max_connections", "20");

    end_current_block();
    start_new_block("rule");

    add_quoted_entry("name", "intranet");
    add_simple_entry("address", "10.0.0.0");
    add_simple_entry("netmask", "255.0.0.0");
    add_simple_entry("block_duration", "600");
    add_simple_entry("conn_per_minute", "50");
    add_simple_entry("max_connections", "20");

    end_current_block();
    start_new_block("rule");

    add_quoted_entry("name", "default");
    add_simple_entry("address", "0.0.0.0");
    add_simple_entry("netmask", "0.0.0.0");
    add_simple_entry("block_duration", "600");
    add_simple_entry("conn_per_minute", "20");
    add_simple_entry("max_connections", "10");

    end_current_block();    // rule
    end_current_block();    // filter

    start_new_block("frontend");

    add_simple_entry("enabled", "false");
    add_simple_entry("http_header", "X-Real-IP");
    add_simple_entry("address", "127.0.0.1");

    end_current_block();    // frontend
}

void config_generator::save_config_file(const std::string& filename)
{
    util::file::write_text(filename, config_.str());
}

void config_generator::add_simple_entry(const std::string& name, const std::string& value)
{
    indent_output();

    config_ << name << "\t\t" << value << ";\n";
}

void config_generator::add_quoted_entry(const std::string& name, const std::string& value)
{
    indent_output();

    std::string escaped_value = value;
    boost::replace_all(escaped_value, "\"", "\\\"");

    config_ << name << "\t\t\"" << escaped_value << "\";\n";
}

void config_generator::start_new_block(const std::string& name)
{
    config_ << '\n';

    indent_output();
    ++nesting_depth_;

    config_ << name << " {\n";
}

void config_generator::end_current_block()
{
    if(nesting_depth_ > 0)
    {
        --nesting_depth_;
        indent_output();

        config_ << "}\n";
    }
}

void config_generator::indent_output()
{
    std::size_t count = nesting_depth_;
    while(count--) config_ << '\t';
}

const std::string config_generator::find_string(const std::string& name) const
{
    return find_value(name, "^\\s*set\\s+" + name + "\\s+\"([^\"]*)\"");
}

const std::string config_generator::find_number(const std::string& name) const
{
    return find_value(name, "^\\s*set\\s+" + name + "\\s+(\\d+)");
}

const std::string config_generator::find_value(const std::string& name, const std::string& pattern) const
{
    boost::xpressive::smatch what;
    boost::xpressive::sregex_compiler compiler;

    boost::xpressive::sregex regex = compiler.compile(pattern);

    if(!boost::xpressive::regex_search(config_data_, what, regex))
        throw std::runtime_error("cannot find required config value: " + name);

    return boost::trim_copy(what.str(1));
}
