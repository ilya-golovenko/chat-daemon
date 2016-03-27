//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 - 2016 Ilya Golovenko
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
#ifndef _http_server_connection_hpp
#define _http_server_connection_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/response.hpp>
#include <http/request.hpp>
#include <http/common.hpp>
#include <http/buffer.hpp>
#include <http/chunked_parser.hpp>
#include <http/request_parser.hpp>
#include <http/tcp_connection.hpp>

// ASIO headers
#include <asio/steady_timer.hpp>
#include <asio.hpp>

// STL headers
#include <memory>
#include <string>
#include <chrono>
#include <queue>


namespace http
{

class server_connection : public std::enable_shared_from_this<server_connection>
{
public:
    typedef std::shared_ptr<server_connection> pointer;

public:
    static pointer create(tcp::connection&& connection);

public:
    explicit server_connection(tcp::connection&& connection);
    ~server_connection();

    server_connection(server_connection const&) = delete;
    server_connection& operator=(server_connection const&) = delete;

    request const& get_request() const;

    tcp::connection& get_tcp_connection();

    bool has_custom_remote_endpoint() const;
    asio::ip::tcp::endpoint const& get_remote_endpoint() const;
    void set_remote_endpoint(asio::ip::tcp::endpoint const& endpoint);

    std::uint16_t get_remote_port() const;
    asio::ip::address get_remote_address() const;

    void read_request(completion_handler&& handler);
    void read_request(completion_handler&& handler, std::chrono::seconds timeout);

    void write_stock_response(status const& status, std::string const& server, completion_handler&& handler);

    void write_response(response const& response, completion_handler&& handler);
    void write_buffer(buffer const& buffer, completion_handler&& handler);

    void close(status const& status, std::string const& server, completion_handler&& handler);

    void close(response const& response, completion_handler&& handler);
    void close(buffer const& buffer, completion_handler&& handler);

    void close(bool force);
    bool is_open() const;

private:
    stop_handler bind_to_stop_handler();
    read_handler bind_to_read_handler();
    write_handler bind_to_write_handler();

    void close_connection(bool force);
    void write_front_connection_buffer();

    void setup_connection(asio::error_code const& error);
    bool is_reading_completed(asio::error_code const& error) const;

    void call_completion_handler(asio::error_code const& error);

    void handle_stop(asio::error_code const& error);
    void handle_read(asio::error_code const& error, std::size_t bytes_transferred);
    void handle_write(asio::error_code const& error, std::size_t bytes_transferred);

private:
    tcp::connection connection_;

    bool reading_content_;
    bool writing_response_;
    bool closing_connection_;
    bool chunked_encoding_;

    std::size_t chunk_size_;

    buffer buffer_;
    request request_;
    response response_;

    asio::ip::tcp::endpoint remote_endpoint_;

    std::queue<buffer> write_buffers_;
    std::queue<completion_handler> handlers_;

    asio::steady_timer timeout_timer_;

    chunked_parser chunked_parser_;
    request_parser request_parser_;
};

}   // namespace http

#endif  // _http_server_connection_hpp
