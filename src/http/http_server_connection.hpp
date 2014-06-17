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
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Applicationg headers
#include "http_response.hpp"
#include "http_request.hpp"
#include "http_common.hpp"
#include "http_buffer.hpp"
#include "http_chunked_parser.hpp"
#include "http_request_parser.hpp"
#include "tcp_connection.hpp"
#include <log_source.hpp>
#include <async_timer.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// STL headers
#include <string>
#include <queue>


namespace http
{
namespace server
{

class connection :
    private boost::noncopyable,
    public log_source<connection>,
    public boost::enable_shared_from_this<connection>
{
public:
    typedef boost::shared_ptr<connection> pointer;
    static pointer create(tcp::connection::pointer connection);

public:
    connection(tcp::connection::pointer connection);
    ~connection();

    const http::request& get_request() const;
    tcp::connection::pointer get_tcp_connection() const;

    bool has_custom_remote_endpoint() const;
    const asio::ip::tcp::endpoint& get_remote_endpoint() const;
    void set_remote_endpoint(const asio::ip::tcp::endpoint& endpoint);

    unsigned short get_remote_port() const;
    const asio::ip::address get_remote_address() const;

    void read_request(const http::completion_handler& handler);
    void read_request(const http::completion_handler& handler, long timeout);

    void write_stock_response(http::status status, const std::string& server,
        const http::completion_handler& handler);

    void write_response(const http::response& response, const http::completion_handler& handler);
    void write_buffer(const http::buffer& buffer, const http::completion_handler& handler);

    void close(http::status status, const std::string& server,
        const http::completion_handler& handler);

    void close(const http::response& response, const http::completion_handler& handler);
    void close(const http::buffer& buffer, const http::completion_handler& handler);

    void close(bool force);
    bool is_open() const;

    const std::string& get_log_source_name() const;

private:
    const http::read_handler bind_to_read_handler();
    const http::write_handler bind_to_write_handler();

    void close_connection(bool force);
    void write_front_connection_buffer();

    void setup_connection(const asio::error_code& error);
    bool is_reading_completed(const asio::error_code& error) const;

    void add_completion_handler(const http::completion_handler& handler);
    void complete(const asio::error_code& error, std::size_t bytes_transferred);

    void handle_stop(const asio::error_code& error);
    void handle_read(const asio::error_code& error, std::size_t bytes_transferred);
    void handle_write(const asio::error_code& error, std::size_t bytes_transferred);

private:
    std::string name_;

    bool reading_body_;
    bool writing_response_;
    bool closing_connection_;
    bool chunked_encoding_;

    std::size_t chunk_size_;

    http::buffer buffer_;
    http::request request_;
    http::response response_;

    tcp::connection::pointer connection_;
    asio::ip::tcp::endpoint remote_endpoint_;

    std::queue<http::buffer> write_buffers_;
    std::queue<http::completion_handler> handlers_;

    util::async_timer timeout_timer_;

    http::chunked_parser chunked_parser_;
    http::request_parser request_parser_;
};

}   // namespace server
}   // namespace http

#endif  // _http_server_connection_hpp
