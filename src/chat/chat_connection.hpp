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
#ifndef _chat_connection_hpp
#define _chat_connection_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_common.hpp"
#include "chat_forwards.hpp"
#include <statistics_manager.hpp>
#include <http_buffer.hpp>
#include <http_server_connection.hpp>
#include <log_common.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// STL headers
#include <string>
#include <queue>


namespace chat
{

class connection :
    private boost::noncopyable,
    public log_source<connection>,
    public boost::enable_shared_from_this<connection>
{
public:
    static connection_ptr create(chat::server& server,
        http::server::connection::pointer connection,
        const std::string& session_id);

public:
    connection(chat::server& server,
        http::server::connection::pointer connection,
        const std::string& session_id);

    ~connection();

    const std::string& name() const;
    const std::string& session_id() const;

    const asio::ip::address get_remote_address() const;

    void start();
    void stop();

    void stop(http::status status, const chat::completion_handler& handler);
    void stop(const std::string& message, const chat::completion_handler& handler);

    void write(const http::buffer& message, const chat::completion_handler& handler);
    void write(const std::string& message, const chat::completion_handler& handler);

    bool is_open() const;

    const std::string& get_log_source_name() const;

private:
    void complete(const asio::error_code& error);
    void add_completion_handler(const chat::completion_handler& handler);

    const http::write_handler bind_to_write_handler();
    void handle_write(const asio::error_code& error, std::size_t bytes_transferred);

private:
    chat::server& server_;

    std::string name_;
    std::string session_id_;

    std::queue<completion_handler> handlers_;
    http::server::connection::pointer connection_;
};

}   // namespace chat

#endif  // _chat_connection_hpp
