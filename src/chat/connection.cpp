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
#include <chat/connection.hpp>
#include <chat/server_context.hpp>
#include <chat/exception.hpp>
#include <chat/common.hpp>
#include <http/response.hpp>
#include <http/utilities.hpp>
#include <http/mime_types.hpp>
#include <http/common.hpp>
#include <app/version.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// BOOST headers
#include <boost/lexical_cast.hpp>

// STL headers
#include <functional>


namespace chat
{

connection_ptr connection::create(server_context& context, http::server_connection::pointer connection, session_id const& id)
{
    return std::make_shared<chat::connection>(std::ref(context), connection, id);
}

connection::connection(server_context& context, http::server_connection::pointer connection, session_id const& id) :
    context_(context),
    session_id_(id),
    connection_(connection)
{
    LOG_COMP_TRACE(connection, "created");
}

connection::~connection()
{
    stop();

    LOG_COMP_TRACE(connection, "terminated");
}

session_id const& connection::get_session_id() const
{
    return session_id_;
}

asio::ip::address connection::get_remote_address() const
{
    return connection_->get_remote_address();
}

void connection::start()
{
    LOG_COMP_TRACE_FUNCTION(connection);

    LOG_COMP_TRACE(connection, "starting");

    context_.get_statistics_manager().add_chat_connection();
}

void connection::stop()
{
    LOG_COMP_TRACE_FUNCTION(connection);

    if(is_open())
    {
        LOG_COMP_TRACE(connection, "stopping");
        connection_->close(false);
    }
}

void connection::stop(http::status const& status, completion_handler const& handler)
{
    LOG_COMP_TRACE_FUNCTION(connection);

    connection_->write_stock_response(status, build_version_text(), handler);

    stop();
}

void connection::stop(std::string const& message, completion_handler const& handler)
{
    LOG_COMP_TRACE_FUNCTION(connection);

    write(message, handler);

    stop();
}

void connection::write(http::buffer const& message, completion_handler const& handler)
{
    LOG_COMP_TRACE_FUNCTION(connection);

    connection_->write_buffer(message, handler);

    context_.get_statistics_manager().add_output_message(message.size());
}

void connection::write(std::string const& message, const chat::completion_handler& handler)
{
    LOG_COMP_TRACE_FUNCTION(connection);

    write(http::buffer(message), handler);
}

bool connection::is_open() const
{
    return connection_->is_open();
}

}   // namespace chat
