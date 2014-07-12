//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2014 Ilya Golovenko
//    This file is part of Chat.Daemon project
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
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/common.hpp>
#include <chat/forwards.hpp>
#include <chat/session_id.hpp>
#include <chat/statistics_manager.hpp>
#include <http/server_connection.hpp>
#include <http/buffer.hpp>

// BOOST headers
#include <boost/asio.hpp>

// STL headers
#include <memory>
#include <string>
#include <queue>


namespace chat
{

class connection : public std::enable_shared_from_this<connection>
{
public:
    static connection_ptr create(server_context& context, http::server_connection::pointer connection, session_id const& id);

public:
    connection(server_context& context, http::server_connection::pointer connection, session_id const& id);
    ~connection();

    connection(connection const&) = delete;
    connection& operator=(connection const&) = delete;

    session_id const& get_session_id() const;

    boost::asio::ip::address get_remote_address() const;

    void start();
    void stop();

    void stop(http::status const& status, completion_handler const& handler);
    void stop(std::string const& message, completion_handler const& handler);

    void write(http::buffer const& message, completion_handler const& handler);
    void write(std::string const& message, completion_handler const& handler);

    bool is_open() const;

private:
    server_context& context_;

    session_id session_id_;

    http::server_connection::pointer connection_;
};

}   // namespace chat

#endif  // _chat_connection_hpp
