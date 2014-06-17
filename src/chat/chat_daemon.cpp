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
#include "chat_daemon.hpp"
#include "chat_exception.hpp"
#include "chat_server.hpp"
#include "chat_common.hpp"
#include <file_utils.hpp>
#include <path_utils.hpp>
#include <utilities.hpp>
#include <singleton.hpp>
#include <log_common.hpp>
#include <log_manager.hpp>
#include <config_loader.hpp>
#include <singleton.hpp>
#include <banner.hpp>

// BOOST headers
#include <boost/bind.hpp>

// STL headers
#include <iostream>
#include <sstream>


namespace chat
{

daemon& daemon::instance()
{
    return util::singleton<chat::daemon>::instance();
}

void daemon::print_usage() const
{
    std::cout << "usage: spdaemon [arguments]\n\n";
    std::cout << "arguments:\n\n";
    std::cout << "    -h    show help information\n";
    std::cout << "    -v    show daemon version\n";
    std::cout << "    -l    show license information\n";
#if defined(ENABLE_DEBUG_LOGGING)
    std::cout << "    -g    enable debug logging\n";
#endif  // defined(ENABLE_DEBUG_LOGGING)
#if defined(_UNIX)
    std::cout << "    -d    run as daemon\n";
#endif  // defined(_UNIX)
    std::cout << std::endl;
}

void daemon::print_version() const
{
    std::cout << daemon_text_banner << std::endl;
}

void daemon::print_license() const
{
    std::cout << daemon_license_text << std::endl;
}

bool daemon::parse_args(int argc, char* argv[])
{
    for(int i = 1; i < argc; ++i)
    {
        if(argv[i][0] == '-' || argv[i][0] == '/')
        {
            switch(argv[i][1])
            {
                case 'h':
                case '?':
                    print_usage();
                    return false;

                case 'v':
                    print_version();
                    return false;

                case 'l':
                    print_license();
                    return false;

                case 'g':
#if defined(ENABLE_DEBUG_LOGGING)
                    log_manager::instance().enable_debug_messages();
#endif  // defined(ENABLE_DEBUG_LOGGING)
                    break;

                case 'd':
#if defined(_UNIX)
                    daemonize_ = true;
#endif  // defined(_UNIX)
                    break;

                default:
                    std::cout << "unknown command line argument: " << argv[i] << std::endl;
                    return false;
            }
        }
    }

    return true;
}

#if defined(_UNIX)
void daemon::daemonize_self()
{
    pid_t pid = fork();

    if(pid < 0)    // fork failed
        throw chat::error(util::errno_to_string("fork", errno));

    if(pid > 0)    // parent process exits
    {
        std::cout << "daemon started: pid = " << pid << std::endl;
        std::cout << "check log files for more details" << std::endl;
        exit(EXIT_SUCCESS);
    }

    // Daemon cannot write to console
    log_manager::instance().disable_console_output();

    // Change file mode mask
    umask(0);

    // Create new session
    if(setsid() < 0)
        throw chat::error(util::errno_to_string("setsid", errno));

    // Close standard descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}
#endif  // defined(_UNIX)

void daemon::save_daemon_pid()
{
    std::string filename = get_pid_filename();
    LOGNOT("saving daemon pid to file: %1%") << filename;

    if(util::file::exists(filename))
        LOGWRN("daemon has been terminated unexpectedly during the previous start");

    std::ofstream pidfile(filename.c_str(), std::ios_base::trunc);
    if(!pidfile.is_open())
        throw chat::error("cannot save daemon pid");

    pidfile << getpid();
    pidfile.close();
}

void daemon::delete_daemon_pid()
{
    util::file::remove(get_pid_filename());
}

const std::string daemon::get_pid_filename() const
{
    return util::path::combine(config.chat.server_path, files::pid);
}

void daemon::start_server()
{
    server_.run();
}

void daemon::configure_server()
{
    LOGNOT("configuring daemon");

    // Init global logger
    log_manager::instance().configure();

    // Configure chat server
    server_.configure();
}

void daemon::reload_configuration()
{
    config_loader loader;
    loader.load_configuration();
}

daemon::daemon() :
    daemonize_(false)
{
}

void daemon::run(int argc, char* argv[])
{
    // Parse command line
    if(parse_args(argc, argv))
    {
        // Show version info
        print_version();

        LOGNOT("starting daemon");

        // Load configuration data
        reload_configuration();

#if defined(_UNIX)
        if(daemonize_)
            daemonize_self();
#endif  // defined(_UNIX)

        save_daemon_pid();

        do
        {
            configure_server();
            start_server();

            if(server_.is_restarting())
                reload_configuration();
        }
        while(server_.is_restarting());

        delete_daemon_pid();
        LOGNOT("daemon terminated");
    }
}

void daemon::restart()
{
    server_.restart();
}

void daemon::stop()
{
    server_.stop();
}

}   // namespace chat
