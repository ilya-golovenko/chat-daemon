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
#include "chat_connection.hpp"
#include "chat_exception.hpp"
#include "chat_server.hpp"
#include "chat_common.hpp"
#include <http_response.hpp>
#include <http_utilities.hpp>
#include <http_mime_types.hpp>
#include <http_common.hpp>
#include <log_common.hpp>
#include <utilities.hpp>
#include <string_utils.hpp>
#include <banner.hpp>

// BOOST headers
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>


namespace chat
{

connection_ptr connection::create(chat::server& server,
    http::server::connection::pointer connection, const std::string& session_id)
{
    return boost::make_shared<chat::connection>(boost::ref(server), connection, session_id);
}

connection::~connection()
{
    stop();

    LOGDBG("terminated");
}

const std::string& connection::name() const
{
    return name_;
}

const std::string& connection::session_id() const
{
    return session_id_;
}

const asio::ip::address connection::get_remote_address() const
{
    return connection_->get_remote_address();
}

void connection::start()
{
    LOGDBG("starting");
    server_.get_statistics_manager()->add_chat_connection();
}

void connection::stop()
{
    if(is_open())
    {
        LOGDBG("stopping");
        connection_->close(false);
    }
}

void connection::stop(http::status status, const chat::completion_handler& handler)
{
    add_completion_handler(handler);

    connection_->write_stock_response(status,
        daemon_version, bind_to_write_handler());

    stop();
}

void connection::stop(const std::string& message, const chat::completion_handler& handler)
{
    write(message, handler);
    stop();
}

void connection::write(const http::buffer& message, const chat::completion_handler& handler)
{
    add_completion_handler(handler);
    connection_->write_buffer(message, bind_to_write_handler());
}

void connection::write(const std::string& message, const chat::completion_handler& handler)
{
    write(http::buffer(message), handler);
}

bool connection::is_open() const
{
    return connection_->is_open();
}

const std::string& connection::get_log_source_name() const
{
    return name_;
}

connection::connection(chat::server& server,
    http::server::connection::pointer connection,
    const std::string& session_id) :
    server_(server),
    session_id_(session_id),
    connection_(connection)
{
    name_ = util::to_string(connection->get_remote_endpoint());

    LOGDBG("created");
}

const http::write_handler connection::bind_to_write_handler()
{
    return boost::bind(&connection::handle_write, shared_from_this(), _1, _2);
}

void connection::complete(const asio::error_code& error)
{
    try
    {
        (handlers_.front())(error);
    }
    catch(std::exception& e)
    {
        LOGWRN(e.what());
    }

    handlers_.pop();
}

void connection::add_completion_handler(const chat::completion_handler& handler)
{
    handlers_.push(handler);
}

void connection::handle_write(const asio::error_code& error, std::size_t bytes_transferred)
{
    if(!error)
        server_.get_statistics_manager()->add_output_message(bytes_transferred);

    complete(error);
}

}   // namespace chat
