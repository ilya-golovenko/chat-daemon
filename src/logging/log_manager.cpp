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
#include "log_manager.hpp"
#include "log_common.hpp"
#include "log_file_handler.hpp"
#include "log_console_handler.hpp"
#include <path_utils.hpp>
#include <utilities.hpp>
#include <config_common.hpp>
#include <singleton.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>

// STL headers
#include <stdexcept>


log_manager& log_manager::instance()
{
    return util::singleton<log_manager>::instance();
}

log_manager::log_manager() :
    debug_messages_(false),
    output_to_console_(true)
{
}

log_manager::~log_manager()
{
    io_thread_.stop();
}

void log_manager::configure()
{
    io_thread_.stop();
    file_handlers_.clear();

    std::for_each(config.logging.handlers.begin(),
        config.logging.handlers.end(), boost::bind(
        &log_manager::create_file_handler, this, _1));

    io_thread_.start();
}

void log_manager::enable_debug_messages()
{
    debug_messages_ = true;
}

void log_manager::disable_console_output()
{
    output_to_console_ = false;
}

void log_manager::add_message(const log_message& message)
{
    bool is_debug = (message.severity() == log_severity::debug);

    if(debug_messages_ || !is_debug)
    {
        if(output_to_console_ && !is_debug)
            console_handler_.add_message(message);

        io_thread_.post(boost::bind(
            &log_manager::process_message, this, message));
    }
}

void log_manager::create_file_handler(const config_data::logging::handler& handler_config)
{
    log_severity severity = log_severity::from_string(handler_config.severity);

#if defined(ENABLE_DEBUG_LOGGING)
    if(!debug_messages_)
        severity -= log_severity::debug;
#else
    if(severity & log_severity::debug)
        severity = log_severity::none;
#endif  // defined(ENABLE_DEBUG_LOGGING)

    if(severity != log_severity::none)
    {
        std::string filename = util::path::combine(
            config.chat.logs_path, handler_config.filename);

        std::ostream::pos_type max_size = static_cast<
            std::ostream::pos_type>(handler_config.max_size);

        log_handler_ptr file_handler = log_file_handler::create(
            filename, max_size, handler_config.backup_log);

        add_file_handler(severity, file_handler);
    }
}

void log_manager::add_file_handler(log_severity severity, log_handler_ptr handler)
{
    if(severity & log_severity::notice)
        file_handlers_.insert(std::make_pair(log_severity::notice, handler));

    if(severity & log_severity::warning)
        file_handlers_.insert(std::make_pair(log_severity::warning, handler));

    if(severity & log_severity::error)
        file_handlers_.insert(std::make_pair(log_severity::error, handler));

    if(severity & log_severity::debug)
        file_handlers_.insert(std::make_pair(log_severity::debug, handler));
}

void log_manager::process_message(const log_message& message)
{
    handler_map_type::const_iterator first;
    handler_map_type::const_iterator last;

    boost::tie(first, last) = file_handlers_.equal_range(message.severity());

    if(first != file_handlers_.end())
    {
        for(; first != last; ++first)
            first->second->add_message(message);
    }
}
