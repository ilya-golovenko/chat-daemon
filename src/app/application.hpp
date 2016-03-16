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
#ifndef _chat_app_application_hpp
#define _chat_app_application_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/server.hpp>

// BOOST headers
#include <boost/filesystem/path.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <string>


namespace chat
{

class application
{
public:
    application();

    application(application const&) = delete;
    application& operator=(application const&) = delete;

    int run(int argc, char* argv[]);
    void stop();

private:
    int run_application(int argc, char* argv[]);
    void stop_application();

    bool parse_command_line(int argc, char* argv[]);
    void parse_config_file(std::string const& filename);

    void configure_logging_library();
    void configure_signal_handler(); 

    void run_io_service();

    void save_pid_file();
    void delete_pid_file();

private:
    void handle_signal(asio::error_code const& error, int signal);

private:
    /// Run as UNIX daemon
    bool daemonize_;

    // Server instance
    chat::server server_;

    /// Server configuration
    chat::server_config config_;

    /// I/O service
    asio::io_service io_service_;

    /// Processes signals from operating system
    asio::signal_set signals_;

    /// Application file name
    boost::filesystem::path app_filename_;

    /// PID file name
    boost::filesystem::path pid_filename_;

    /// configuration file name
    boost::filesystem::path config_filename_;
};

}   // namespace chat

#endif  // _chat_app_application_hpp
