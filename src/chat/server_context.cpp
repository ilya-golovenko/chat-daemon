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
#include <chat/server_context.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{

server_context::server_context(asio::io_service& io_service) :
    io_service_(io_service),
    skin_manager_(*this),
    user_manager_(*this),
    room_manager_(*this),
    ignore_manager_(*this),
    accept_manager_(*this),
    filter_manager_(*this),
    frontend_manager_(*this),
    connection_manager_(*this),
    statistics_manager_(*this),
    message_parser_(*this),
    data_file_reader_(*this)
{
}

void server_context::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(server_context);

    LOG_COMP_NOTICE(server_context, "configuring");

    LOG_COMP_DEBUG(server_context, "initializing random generator");
    random_engine_.seed(random_device_());

    skin_manager_.configure(config);
    user_manager_.configure(config);
    room_manager_.configure(config);
    ignore_manager_.configure(config);
    accept_manager_.configure(config);
    filter_manager_.configure(config);
    frontend_manager_.configure(config);
    connection_manager_.configure(config);
    statistics_manager_.configure(config);

    //message_parser_.configure(config);
    data_file_reader_.configure(config);
}

void server_context::start()
{
    LOG_COMP_TRACE_FUNCTION(server_context);

    LOG_COMP_NOTICE(server_context, "starting");

    skin_manager_.start();
    user_manager_.start();
    room_manager_.start();
    ignore_manager_.start();
    accept_manager_.start();
    filter_manager_.start();
    frontend_manager_.start();
    connection_manager_.start();
    statistics_manager_.start();

    //message_parser_.start();
    data_file_reader_.start();

    LOG_COMP_NOTICE(server_context, "started");    
}

void server_context::stop()
{
    LOG_COMP_TRACE_FUNCTION(server_context);

    LOG_COMP_NOTICE(server_context, "stopping");

    skin_manager_.stop();
    user_manager_.stop();
    room_manager_.stop();
    ignore_manager_.stop();
    accept_manager_.stop();
    filter_manager_.stop();
    frontend_manager_.stop();
    connection_manager_.stop();
    statistics_manager_.stop();

    //message_parser_.stop();
    data_file_reader_.stop();

    LOG_COMP_NOTICE(server_context, "stopped");
}

asio::io_service& server_context::get_io_service()
{
    return io_service_;
}

std::uint32_t server_context::generate_random_number()
{
    return std::uniform_int_distribution<std::uint32_t>()(random_engine_);
}

skin_manager& server_context::get_skin_manager()
{
    return skin_manager_;
}

user_manager& server_context::get_user_manager()
{
    return user_manager_;
}

room_manager& server_context::get_room_manager()
{
    return room_manager_;
}

ignore_manager& server_context::get_ignore_manager()
{
    return ignore_manager_;
}

accept_manager& server_context::get_accept_manager()
{
    return accept_manager_;
}

filter_manager& server_context::get_filter_manager()
{
    return filter_manager_;
}

frontend_manager& server_context::get_frontend_manager()
{
    return frontend_manager_;
}

connection_manager& server_context::get_connection_manager()
{
    return connection_manager_;
}

statistics_manager& server_context::get_statistics_manager()
{
    return statistics_manager_;
}

message_parser& server_context::get_message_parser()
{
    return message_parser_;
}

data_file_reader& server_context::get_data_file_reader()
{
    return data_file_reader_;
}

}   // namespace chat
