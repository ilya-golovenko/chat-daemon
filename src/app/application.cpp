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

// Application headers
#include <app/application.hpp>
#include <app/version.hpp>
#include <chat/server.hpp>
#include <chat/server_context.hpp>
#include <misc/file_utils.hpp>
#include <misc/path_utils.hpp>

// MISSIO headers
#include <missio/logging/all_writers.hpp>
#include <missio/logging/common.hpp>

// BOOST headers
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>

// STL headers
#include <functional>
#include <iostream>
#include <sstream>

// CRT headers
#include <signal.h>


namespace chat
{

application::application() :
    daemonize_(false),
    signals_(io_service_)
{
}

int application::run(int argc, char* argv[])
{
    //TODO: temporary, for debugging purposes
    std::cin.get();

    try
    {
        return run_application(argc, argv);
    }
    catch(std::exception const& e)
    {
        std::cout << e.what() << std::endl;
    }

    return EXIT_FAILURE;
}

void application::stop()
{
    LOG_COMP_INFO(application, "terminating");

    try
    {
        server_.stop();
        delete_pid_file();
    }
    catch(std::exception const& e)
    {
        LOG_COMP_FAILURE(application, e);
    }
}

int application::run_application(int argc, char* argv[])
{
    missio::logging::initialize();

    if(parse_command_line(argc, argv))
    {
#if defined(CHAT_PLATFORM_POSIX)
        if(daemonize_)
        {
            if(detail::posix_daemon_helper::daemonize())
            {
                std::cout << "daemon started, pid = " << ::getpid() << std::endl;
                std::cout << "check log files for details" << std::endl;

                return EXIT_SUCCESS;
            }
        }
#endif  // defined(CHAT_PLATFORM_POSIX)

        try
        {
            LOG_COMP_INFO(application, "starting");

            save_pid_file();

            configure_logging_library();
            configure_signal_handler();

            server_.configure(config_);
            server_.start();

            LOG_COMP_INFO(application, "started");

            run_io_service();

            LOG_COMP_INFO(application, "stopped");
        }
        catch(std::exception const& e)
        {
            LOG_COMP_FAILURE(application, e);

            stop();
        }

        LOG_COMP_INFO(application, "terminated");
    }

    missio::logging::shutdown();

    return EXIT_SUCCESS;
}

bool application::parse_command_line(int argc, char* argv[])
{
    namespace po = boost::program_options;

    po::options_description description("options");

    description.add_options()
        ("help,h", "show usage information")
        ("version,v", "show version information")
        ("license,l", "show license information")
        ("config,c", po::value(&config_filename_)->default_value("spdaemon.conf"), "daemon configuration file");

#if defined(CHAT_PLATFORM_POSIX)
    description.add_options()
        ("daemonize,d", po::value(&daemonize_)->zero_tokens()->implicit_value(true), "run as UNIX daemon");
#endif  // defined(CHAT_PLATFORM_POSIX)

    po::variables_map options;

    po::store(po::parse_command_line(argc, argv, description), options);

    if(options.count("help"))
    {
        std::cout << "usage: spdaemon [options]" << std::endl;
        std::cout << description << std::endl;
        return false;
    }

    if(options.count("version"))
    {
        std::cout << build_version_text() << std::endl;
        return false;
    }

    if(options.count("license"))
    {
        std::cout << build_license_text() << std::endl;
        return false;
    }

    try
    {
        po::notify(options);
    }
    catch(po::required_option const& e)
    {
        std::cout << "required option missing: " + e.get_option_name() << std::endl;
        std::cout << "usage: spdaemon [options]" << std::endl;
        std::cout << description << std::endl;
        return false;
    }

    if(!boost::filesystem::exists(config_filename_))
    {
        std::cout << "configuration file does not exist: " << config_filename_ << std::endl;
        return false;
    }

    try
    {
        parse_config_file(config_filename_.string());
    }
    catch(config_error const& e)
    {
        std::cout << "cannot parse configuration file: " << e.what() << std::endl;

        return false;
    }

    app_filename_ = boost::filesystem::absolute(argv[0]);

    pid_filename_ = app_filename_;
    pid_filename_.replace_extension("pid");

    return true;
}

void application::parse_config_file(std::string const& filename)
{
    config_manager_.load_configuration(filename);
}

void application::configure_logging_library()
{
    LOG_COMP_INFO(application, "configuring logger");

    if(!daemonize_)
    {
        missio::logging::console_options(missio::logging::debug)
            .format("%TimeStamp%\t%Severity%\t[%Component%]\t%Message%\n")
            .add_writer();
    }

    missio::logging::file_options(missio::logging::info)
        .filename("spdaemon.log")
        .format("%MessageID%\t%TimeStamp%\t%Severity%\t%ProcessID%\t%ThreadID%\t[%Component%]\t[%Function% @ %File%:%Line%] %Message%\n")
        .max_file_size(1000000)
        .max_file_index(5)
        .add_writer();

#if defined(DEBUG) || defined(_DEBUG)
    missio::logging::file_options(missio::logging::trace)
        .filename("spdaemon_trace.log")
        .format("%MessageID%\t%TimeStamp%\t%Severity%\t%ProcessID%\t%ThreadID%\t[%Component%]\t[%Function% @ %File%:%Line%] %Message%\n")
        .max_file_size(1000000)
        .max_file_index(5)
        .add_writer();
#endif  // defined(DEBUG) || defined(_DEBUG)

#if defined(CHAT_PLATFORM_POSIX)
    missio::logging::syslog_options(missio::logging::error)
        .ident(short_name)
        .format("[%Component%] %Message%")
        .add_writer();
    );
#endif  // defined(CHAT_PLATFORM_POSIX)

    missio::logging::start();
}

void application::configure_signal_handler()
{
    signals_.add(SIGINT);
    signals_.add(SIGTERM);

#if defined(SIGQUIT)
    signals_.add(SIGQUIT);
#endif

#if defined(SIGHUP)
    signals_.add(SIGHUP);
#endif

#if defined(SIGBREAK)
    signals_.add(SIGBREAK);
#endif

    signals_.async_wait(std::bind(&application::handle_signal, this, std::placeholders::_1, std::placeholders::_2));
}

void application::run_io_service()
{
    for(;;)
    {
        try
        {
            io_service_.run();
            return;
        }
        catch(std::exception const& e)
        {
            LOG_COMP_ERROR(application, e);
        }
    }
}

void application::save_pid_file()
{
    LOG_COMP_INFO(application, "saving application pid to file: ", pid_filename_);

    if(boost::filesystem::exists(pid_filename_))
        LOG_COMP_WARNING(application, "application terminated unexpectedly during the previous start");

    boost::filesystem::ofstream pid_file(pid_filename_, std::ios::trunc);

    if(!pid_file.is_open())
        throw std::runtime_error("cannot save application pid");

    pid_file << ::getpid();
}

void application::delete_pid_file()
{
    LOG_COMP_INFO("removing application pid file: ", pid_filename_);

    boost::filesystem::remove(pid_filename_);
}

void application::handle_signal(boost::system::error_code const& error, int signal)
{
    if(!error)
    {
        LOG_COMP_INFO("received signal from operating system: ", signal);

        stop();
    }
}

}   // namespace chat
