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
#include <chat/server.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// STL headers
#include <exception>


namespace chat
{

server::server() :
    context_(io_service_),
    cron_connection_(context_)
{
}

void server::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(server);

    LOG_COMP_NOTICE(server, "configuring");

    context_.configure(config);

    cron_connection_.configure(config);
}

void server::start()
{
    LOG_COMP_TRACE_FUNCTION(server);

    LOG_COMP_NOTICE(server, "starting");

    context_.start();

    cron_connection_.start();

    LOG_COMP_DEBUG(server, "starting worker thread");
    worker_thread_ = std::thread(&server::run, this);

    LOG_COMP_NOTICE(server, "started");
}

void server::stop()
{
    LOG_COMP_TRACE_FUNCTION(server);

    LOG_COMP_NOTICE(server, "stopping");

    cron_connection_.stop();

    deliver_stop_message();

    context_.stop();

    LOG_COMP_DEBUG(server, "stopping worker thread");
    worker_thread_.join();

    io_service_.reset();

    LOG_COMP_NOTICE(server, "stopped");
}

asio::io_service& server::get_io_service()
{
    return io_service_;
}

void server::run()
{
    LOG_COMP_TRACE_FUNCTION(server);

    for(;;)
    {
        try
        {
            io_service_.run();
            break;
        }
        catch(std::exception const& e)
        {
            LOG_COMP_ERROR(server, e);
        }
    }
}

void server::deliver_stop_message()
{
    LOG_COMP_TRACE_FUNCTION(server);

    LOG_COMP_DEBUG(server, "delivering stop message");

    //TODO: context_.get_room_manager().deliver_message(config.server_shutdown_message);
}

}   // namespace chat
