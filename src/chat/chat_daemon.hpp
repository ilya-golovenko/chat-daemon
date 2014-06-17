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
#ifndef _chat_daemon_hpp
#define _chat_daemon_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_server.hpp"
#include <platform.hpp>

// STL headers
#include <string>

// BOOST headers
#include <boost/noncopyable.hpp>


namespace chat
{

class daemon :
    private boost::noncopyable
{
public:
    static daemon& instance();

public:
    daemon();

    void run(int argc, char* argv[]);
    void restart();
    void stop();

private:
    void print_usage() const;
    void print_version() const;
    void print_license() const;

    bool parse_args(int argc, char* argv[]);

#if defined(_UNIX)
    void daemonize_self();
#endif  // defined(_UNIX)

    void start_server();
    void configure_server();
    void reload_configuration();

    void save_daemon_pid();
    void delete_daemon_pid();

    const std::string get_pid_filename() const;

private:
    bool daemonize_;
    chat::server server_;
};

}   // namespace chat

#endif  // _chat_daemon_hpp
