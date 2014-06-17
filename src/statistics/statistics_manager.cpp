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
#include "statistics_manager.hpp"
#include <chat_connection.hpp>
#include <chat_server.hpp>
#include <chat_room.hpp>
#include <chat_user.hpp>
#include <file_utils.hpp>
#include <path_utils.hpp>
#include <string_utils.hpp>
#include <utilities.hpp>
#include <log_common.hpp>
#include <version.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/date_time.hpp>
#include <boost/bind.hpp>

// STL headers
#include <sstream>


const std::string field_names[] =
{
    "connect",

    "connect_chat",
    "connect_err",
    "connect_ignore",

    "out",
    "in",

    "line_out",
    "line_in"
};

const std::time_t statistics_manager::start_time_ = std::time(0);

statistics_manager::statistics_manager(chat::server& server) :
    server_(server),
    status_timer_(server_.get_io_service()),
    statistics_timer_(server_.get_io_service())
{
}

void statistics_manager::configure()
{
    LOGNOT("configuring statistics manager");

    os_name_ = util::get_os_name();
    host_name_ = asio::ip::host_name();

    simple_statistics_.assign(std::size_t(0));
    detail_statistics_.assign(simple_statistics_);

    update_run_status();
    update_all_statistics();
}

void statistics_manager::start()
{
    start_status_timer();
    start_statistics_timer();
}

void statistics_manager::stop()
{
    status_timer_.stop();
    statistics_timer_.stop();
}

void statistics_manager::add_connection()
{
    add_simple_statistics(total_connections, 1);
}

void statistics_manager::add_chat_connection()
{
    add_simple_statistics(chat_connections, 1);
}

void statistics_manager::add_ignored_connection()
{
    add_simple_statistics(ignored_connections, 1);
}

void statistics_manager::add_input_message(std::size_t size)
{
    add_simple_statistics(input_messages, 1);
    add_simple_statistics(input_bytes, size);
}

void statistics_manager::add_output_message(std::size_t size)
{
    add_simple_statistics(output_messages, 1);
    add_simple_statistics(output_bytes, size);
}

void statistics_manager::start_status_timer()
{
    status_timer_.start(boost::posix_time::seconds(config.chat.status_interval),
        boost::bind(&statistics_manager::handle_status_timer, this, _1));
}

void statistics_manager::start_statistics_timer()
{
    statistics_timer_.start(boost::posix_time::seconds(config.chat.statistics_interval),
        boost::bind(&statistics_manager::handle_statistics_timer, this, _1));
}

void statistics_manager::update_run_status()
{
    try
    {
        std::string filename = util::path::combine(
            config.chat.server_path, files::runstatus);

        util::file::write_text(filename, util::to_string(std::time(0)));
    }
    catch(std::exception& e)
    {
        LOGWRN(e.what());
    }
}

void statistics_manager::update_all_statistics()
{
    try
    {
        update_simple_statistics();
        update_detail_statistics();
    }
    catch(std::exception& e)
    {
        LOGWRN(e.what());
    }
}

void statistics_manager::update_simple_statistics()
{
    LOGDBG("updating simple statistics");

    std::string filename = util::path::combine(
        config.chat.server_path, files::statistics);

    util::file::write_text(filename, get_simple_statistics());
}

void statistics_manager::update_detail_statistics()
{
    LOGDBG("updating detail statistics");

    std::string filename = util::path::combine(
        config.chat.server_path, files::traffic);

    util::file::write_text(filename, get_detail_statistics());
}

const std::string statistics_manager::get_simple_statistics() const
{
    std::ostringstream buffer;

    std::size_t ignore_count = server_.get_user_manager()->get_ignore_count();
    std::size_t connection_count = server_.get_user_manager()->get_connection_count();

    buffer << field_names[total_connections] << ' ' << simple_statistics_[total_connections] << ' ';
    buffer << field_names[chat_connections] << ' ' << simple_statistics_[chat_connections] << ' ';
    buffer << field_names[error_connections] << ' ' << get_error_connections() << ' ';
    buffer << field_names[ignored_connections] << ' ' << simple_statistics_[ignored_connections] << ' ';
    buffer << field_names[output_bytes] << ' ' << simple_statistics_[output_bytes] << ' ';
    buffer << field_names[input_bytes] << ' ' << simple_statistics_[input_bytes] << ' ';
    buffer << field_names[output_messages] << ' ' << simple_statistics_[output_messages] << ' ';
    buffer << field_names[input_messages] << ' ' << simple_statistics_[input_messages] << std::endl;

    buffer << start_time_ << std::endl;
    buffer << connection_count << std::endl;
    buffer << ignore_count << std::endl;
    buffer << util::get_os_info() << std::endl;
    buffer << host_name_ << std::endl;
    buffer << os_name_ << std::endl;
    buffer << daemon_version_full << std::endl;
    buffer << strings::not_applicable << std::endl;
    buffer << util::path::initial() << std::endl;

    return buffer.str();
}

const std::string statistics_manager::get_detail_statistics() const
{
    std::ostringstream buffer;

    for(std::size_t field = 0; field < fields_count; ++field)
    {
        buffer << field_names[field];

        for(std::size_t point = 0; point < detail_statistics_.size(); ++point)
        {
            std::size_t count = detail_statistics_[point][field];

            if(error_connections == field)
                count = get_error_connections(point);

            buffer << ' ' << count;
        }

        buffer << std::endl;
    }

    return buffer.str();
}

std::size_t statistics_manager::get_current_detail_point() const
{
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    return static_cast<std::size_t>(now.time_of_day().total_seconds() / detail_point_duration);
}

void statistics_manager::add_simple_statistics(fields_type field, std::size_t count)
{
    simple_statistics_[field] += count;
    add_detail_statistics(field, count);
}

void statistics_manager::add_detail_statistics(fields_type field, std::size_t count)
{
    detail_statistics_[get_current_detail_point()][field] += count;
}

std::size_t statistics_manager::get_error_connections() const
{
    std::size_t connections_count = simple_statistics_[total_connections];

    connections_count -= simple_statistics_[ignored_connections];
    connections_count -= simple_statistics_[chat_connections];

    return connections_count;
}

std::size_t statistics_manager::get_error_connections(std::size_t point) const
{
    std::size_t connections_count = detail_statistics_[point][total_connections];

    connections_count -= detail_statistics_[point][ignored_connections];
    connections_count -= detail_statistics_[point][chat_connections];

    return connections_count;
}

void statistics_manager::handle_status_timer(const asio::error_code& error)
{
    if(!error)
    {
        update_run_status();
        start_status_timer();
    }
}

void statistics_manager::handle_statistics_timer(const asio::error_code& error)
{
    if(!error)
    {
        update_all_statistics();
        start_statistics_timer();
    }
}
