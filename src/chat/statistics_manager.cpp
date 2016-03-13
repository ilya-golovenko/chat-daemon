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
#include <chat/statistics_manager.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <chat/connection.hpp>
#include <chat/server.hpp>
#include <chat/room.hpp>
#include <chat/user.hpp>
#include <misc/file_utils.hpp>
#include <misc/path_utils.hpp>
#include <app/version.hpp>
#include <app/constants.hpp>
#include <app/operating_system_info.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// STL headers
#include <functional>
#include <sstream>
#include <chrono>


namespace chat
{

static char const* field_names[] =
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

std::time_t const start_time = std::time(nullptr);

statistics_manager::statistics_manager(server_context& context) :
    context_(context),
    status_timer_(context_.get_io_service()),
    statistics_timer_(context_.get_io_service())
{
}

void statistics_manager::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    LOG_COMP_NOTICE(statistics_manager, "configuring");

    server_path_ = config.server_path;

    status_interval_ = config.status_interval;
    statistics_interval_ = config.statistics_interval;

    simple_statistics_.fill(std::size_t(0u));
    detail_statistics_.fill(simple_statistics_);

    update_run_status();
    update_all_statistics();
}

void statistics_manager::start()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    LOG_COMP_NOTICE(statistics_manager, "starting");

    start_status_timer();
    start_statistics_timer();

    LOG_COMP_NOTICE(statistics_manager, "stopping");
}

void statistics_manager::stop()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    LOG_COMP_NOTICE(statistics_manager, "stopping");

    status_timer_.cancel();
    statistics_timer_.cancel();

    LOG_COMP_NOTICE(statistics_manager, "stopped"); 
}

void statistics_manager::add_connection()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    add_simple_statistics(total_connections, 1);
}

void statistics_manager::add_chat_connection()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    add_simple_statistics(chat_connections, 1);
}

void statistics_manager::add_ignored_connection()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    add_simple_statistics(ignored_connections, 1);
}

void statistics_manager::add_input_message(std::size_t size)
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    add_simple_statistics(input_messages, 1);
    add_simple_statistics(input_bytes, size);
}

void statistics_manager::add_output_message(std::size_t size)
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    add_simple_statistics(output_messages, 1);
    add_simple_statistics(output_bytes, size);
}

void statistics_manager::start_status_timer()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    status_timer_.expires_from_now(status_interval_);
    status_timer_.async_wait(/*TODO: bind_to_status_timer_handler()*/ std::bind(&statistics_manager::handle_status_timer, this, std::placeholders::_1));
}

void statistics_manager::start_statistics_timer()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    statistics_timer_.expires_from_now(statistics_interval_);
    statistics_timer_.async_wait(/*TODO: bind_to_statistics_timer_handler()*/ std::bind(&statistics_manager::handle_statistics_timer, this, std::placeholders::_1));
}

void statistics_manager::update_run_status()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    try
    {
        std::string filename = util::path::combine(server_path_, ::files::runstatus);

        util::file::write_text(filename, std::to_string(std::time(nullptr)));
    }
    catch(std::exception const& e)
    {
        LOG_COMP_WARNING(statistics_manager, e);
    }
}

void statistics_manager::update_all_statistics()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    try
    {
        update_simple_statistics();
        update_detail_statistics();
    }
    catch(std::exception const& e)
    {
        LOG_COMP_WARNING(statistics_manager, e);
    }
}

void statistics_manager::update_simple_statistics()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    LOG_COMP_DEBUG(statistics_manager, "updating simple statistics");

    std::string filename = util::path::combine(server_path_, ::files::statistics);

    util::file::write_text(filename, get_simple_statistics());
}

void statistics_manager::update_detail_statistics()
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    LOG_COMP_DEBUG(statistics_manager, "updating detail statistics");

    std::string filename = util::path::combine(server_path_, ::files::traffic);

    util::file::write_text(filename, get_detail_statistics());
}

std::string statistics_manager::get_simple_statistics() const
{
    std::ostringstream buffer;

    std::size_t const ignore_count = context_.get_ignore_manager().get_user_ignore_count();
    std::size_t const connection_count = context_.get_user_manager().get_connection_count();

    buffer << field_names[total_connections] << ' ' << simple_statistics_[total_connections] << ' ';
    buffer << field_names[chat_connections] << ' ' << simple_statistics_[chat_connections] << ' ';
    buffer << field_names[error_connections] << ' ' << get_error_connections() << ' ';
    buffer << field_names[ignored_connections] << ' ' << simple_statistics_[ignored_connections] << ' ';
    buffer << field_names[output_bytes] << ' ' << simple_statistics_[output_bytes] << ' ';
    buffer << field_names[input_bytes] << ' ' << simple_statistics_[input_bytes] << ' ';
    buffer << field_names[output_messages] << ' ' << simple_statistics_[output_messages] << ' ';
    buffer << field_names[input_messages] << ' ' << simple_statistics_[input_messages] << std::endl;

    buffer << start_time << std::endl;
    buffer << connection_count << std::endl;
    buffer << ignore_count << std::endl;
    buffer << get_operating_system_info() << std::endl;
    buffer << asio::ip::host_name() << std::endl;
    buffer << operating_system_info::get_name() << std::endl;
    buffer << build_version_text() << std::endl;
    buffer << strings::not_applicable << std::endl;
    buffer << util::path::initial() << std::endl;

    return buffer.str();
}

std::string statistics_manager::get_detail_statistics() const
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
    boost::posix_time::ptime const now = boost::posix_time::second_clock::local_time();
    return static_cast<std::size_t>(now.time_of_day().total_seconds() / detail_point_duration);
}

void statistics_manager::add_simple_statistics(fields_type field, std::size_t count)
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    simple_statistics_[field] += count;
    add_detail_statistics(field, count);
}

void statistics_manager::add_detail_statistics(fields_type field, std::size_t count)
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

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

void statistics_manager::handle_status_timer(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    if(!error)
    {
        update_run_status();
        start_status_timer();
    }
}

void statistics_manager::handle_statistics_timer(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(statistics_manager);

    if(!error)
    {
        update_all_statistics();
        start_statistics_timer();
    }
}

std::string statistics_manager::get_operating_system_info()
{
    std::ostringstream buffer;

    std::time_t const seconds_per_minute = 60;
    std::time_t const seconds_per_hour = 3600;
    std::time_t const seconds_per_day = 86400;

    std::time_t const uptime = operating_system_info::get_uptime();

    if(uptime < seconds_per_hour)
    {
        // uptime in minutes
        buffer << "uptime: " << uptime / seconds_per_minute << " minutes";
    }
    else if(uptime < seconds_per_day)
    {
        // uptime in hours
        buffer << "uptime: " << uptime / seconds_per_hour << " hours";
    }
    else
    {
        // uptime in days
        buffer << "uptime: " << uptime / seconds_per_day << " days";
    }

#if defined(CHAT_OSINFO_HAS_LOADAVG)

    std::array<double, 3> const loadavg = operating_system_info::get_loadavg();

    buffer << "; load average: " << std::fixed << std::setprecision(2);
    buffer << loadavg[0] << ", " << loadavg[1] << ", " << loadavg[2]; 

#endif    // defined(CHAT_OSINFO_HAS_LOADAVG)

    return buffer.str();
}

}   // namespace chat
