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
#ifndef _log_manager_hpp
#define _log_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "log_console_handler.hpp"
#include "log_handler.hpp"
#include "log_severity.hpp"
#include "log_message.hpp"
#include <config_common.hpp>
#include <io_thread.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>

// STL headers
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>


class log_manager :
    private boost::noncopyable
{
public:
    static log_manager& instance();

public:
    log_manager();
    ~log_manager();

    void configure();

    void enable_debug_messages();
    void disable_console_output();

    void add_message(const log_message& message);

private:
    void add_file_handler(log_severity severity, log_handler_ptr handler);
    void create_file_handler(const config_data::logging::handler& handler_config);

    void process_message(const log_message& message);

private:
    typedef std::multimap<log_severity, log_handler_ptr> handler_map_type;

private:
    bool debug_messages_;
    bool output_to_console_;

    util::io_thread io_thread_;

    handler_map_type file_handlers_;
    log_console_handler console_handler_;
};

#endif  // _log_manager_hpp
