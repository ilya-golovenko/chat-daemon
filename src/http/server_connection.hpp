//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 Ilya Golovenko
//    This file is part of libsphttp.
//
//    libsphttp is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    libsphttp is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with libsphttp. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------
#ifndef _http_server_connection_hpp
#define _http_server_connection_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Applicationg headers
#include <http/response.hpp>
#include <http/request.hpp>
#include <http/common.hpp>
#include <http/buffer.hpp>
#include <http/chunked_parser.hpp>
#include <http/request_parser.hpp>
#include <http/tcp_connection.hpp>

// BOOST headers
#include <boost/asio/steady_timer.hpp>
#include <boost/asio.hpp>

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
    static pointer create(tcp_connection::pointer tcp_connection);

public:
    explicit server_connection(tcp_connection::pointer tcp_connection);
    ~server_connection();

    server_connection(server_connection const&) = delete;
    server_connection& operator=(server_connection const&) = delete;

    request const& get_request() const;

    tcp_connection::pointer get_tcp_connection() const;

    bool has_custom_remote_endpoint() const;
    boost::asio::ip::tcp::endpoint const& get_remote_endpoint() const;
    void set_remote_endpoint(boost::asio::ip::tcp::endpoint const& endpoint);

    std::uint16_t get_remote_port() const;
    boost::asio::ip::address get_remote_address() const;

    void read_request(completion_handler const& handler);
    void read_request(completion_handler const& handler, std::chrono::seconds const& timeout);

    void write_stock_response(status const& status, std::string const& server, completion_handler const& handler);

    void write_response(response const& response, completion_handler const& handler);
    void write_buffer(buffer const& buffer, completion_handler const& handler);

    void close(status const& status, std::string const& server, completion_handler const& handler);

    void close(response const& response, completion_handler const& handler);
    void close(buffer const& buffer, completion_handler const& handler);

    void close(bool force);
    bool is_open() const;

private:
    stop_handler bind_to_stop_handler();
    read_handler bind_to_read_handler();
    write_handler bind_to_write_handler();

    void close_connection(bool force);
    void write_front_connection_buffer();

    void setup_connection(boost::system::error_code const& error);
    bool is_reading_completed(boost::system::error_code const& error) const;

    void call_completion_handler(boost::system::error_code const& error);

    void handle_stop(boost::system::error_code const& error);
    void handle_read(boost::system::error_code const& error, std::size_t bytes_transferred);
    void handle_write(boost::system::error_code const& error, std::size_t bytes_transferred);

private:
    bool reading_body_;
    bool writing_response_;
    bool closing_connection_;
    bool chunked_encoding_;

    std::size_t chunk_size_;

    buffer buffer_;
    request request_;
    response response_;

    tcp_connection::pointer tcp_connection_;
    boost::asio::ip::tcp::endpoint remote_endpoint_;

    std::queue<buffer> write_buffers_;
    std::queue<completion_handler> handlers_;

    boost::asio::steady_timer timeout_timer_;

    chunked_parser chunked_parser_;
    request_parser request_parser_;
};

}   // namespace http

#endif  // _http_server_connection_hpp