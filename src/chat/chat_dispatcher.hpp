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
#ifndef _chat_dispatcher_hpp
#define _chat_dispatcher_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio.hpp>

// STL headers
#include <string>


namespace chat
{

class dispatcher :
    private boost::noncopyable
{
public:
    dispatcher(asio::io_service& io_service);
    ~dispatcher();

    void process_http_connection(http::server_connection_ptr connection);

    void process_chat_connection(chat::connection_ptr connection);

    void process_data_line(const std::string& data_line);

    void process_chat_command(const std::string& command);

private:
    asio::io_service& io_service_;
};

class basic_processor :
    private boost::noncopyable
{
public:
    basic_processor(dispatcher& dispatcher);
    virtual ~basic_processor();

    dispatcher& get_dispathcer();

    virtual void configure();

    virtual void start();
    virtual void stop();

    virtual bool is_async() const;
    virtual bool is_threaded() const;

private:
    dispatcher& dispatcher_;
};

class http_connection_processor :
    public basic_processor
{
public:
    http_connection_processor(dispatcher& dispatcher);
    virtual ~http_connection_processor();

    virtual void process(http::server_connection_ptr connection);
};

}   // namespace chat

#endif  // _chat_dispatcher_hpp
