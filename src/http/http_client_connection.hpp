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
#ifndef _http_client_connection_hpp
#define _http_client_connection_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "http_request.hpp"
#include "http_response.hpp"
#include "http_response_parser.hpp"
#include "http_chunked_parser.hpp"
#include "http_buffer.hpp"
#include "http_common.hpp"
#include "tcp_connection.hpp"
#include <async_timer.hpp>
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


namespace http
{
namespace client
{

class connection :
    private boost::noncopyable,
    public log_source<connection>,
    public boost::enable_shared_from_this<connection>
{
public:
    typedef boost::shared_ptr<connection> pointer;
    static pointer create(asio::io_service& io_service);

public:
    connection(asio::io_service& io_service);
    ~connection();

    const http::response& get_response() const;
    tcp::connection::pointer get_tcp_connection() const;

    void set_data_handler(const http::data_handler& handler);
    void set_response_handler(const http::response_handler& handler);
    void set_proxy_server(const std::string& hostname, unsigned short port);

    void start(const std::string& hostname, unsigned short port,
        const http::request& request, const http::completion_handler& handler);

	//TODO: Implement!
	//
	//void connect(const std::string& hostname, unsigned short port, const http::completion_handler& handler);
	//
	//http::connection:
	//
	//void write(const http::message& message, const http::completion_handler& handler);
	//void write(const http::buffer& buffer, const http::completion_handler& handler);
	//
	//void read(const http::completion_handler& handler, long timeout);
	//void read(const http::completion_handler& handler);
	//
	//void close(bool force);
	//bool is_open() const;

	void write_request(const http::request& request, const http::completion_handler& handler);
	void write_buffer(const http::buffer& buffer, const http::completion_handler& handler);

    void read_response(const http::completion_handler& handler, long timeout);
    void read_response(const http::completion_handler& handler);

    bool is_open() const;
    void close();

    const std::string& get_log_source_name() const;

private:
    void start_connection(const std::string& hostname, unsigned short port, bool redirected);
    void stop_connection();

    bool redirect_to_response_location();

    bool is_reading_completed(const asio::error_code& error) const;
    bool is_keep_alive_connection_aborted(const asio::error_code& error) const;

    void setup_connection(const asio::error_code& error);

	//TODO: move implementation to separate functions!
	//std::pair<bool, const char*> process_message(http::message& message);
	//bool process_chunked_data(http::message& message);
	//bool process_identity_data(http::message& message);

    void stop_worker_timer();
    void start_keep_alive_timer();
    void start_read_timeout_timer();

    const http::read_handler bind_to_read_handler();
    const http::write_handler bind_to_write_handler();
    const http::timer_handler bind_to_keep_alive_timer_handler();
    const http::timer_handler bind_to_read_timeout_timer_handler();

    void call_data_handler(const char* data, std::size_t size);
    void call_response_handler(const http::response& response);

    void add_completion_handler(const http::completion_handler& handler);
    void complete(const asio::error_code& error, std::size_t bytes_transferred);

    void handle_resolve(const asio::error_code& error, asio::ip::tcp::resolver::iterator endpoint_iterator);
    void handle_connect(const asio::error_code& error, asio::ip::tcp::resolver::iterator endpoint_iterator);
    void handle_read(const asio::error_code& error, std::size_t bytes_transferred);
    void handle_write(const asio::error_code& error, std::size_t bytes_transferred);
    void handle_keep_alive_timer(const asio::error_code& error);
    void handle_read_timeout_timer(const asio::error_code& error);

private:
    std::string name_;

    unsigned short port_;
    std::string hostname_;

    unsigned short proxy_port_;
    std::string proxy_hostname_;
    bool use_proxy_server_;

    bool keep_alive_;
    bool reading_body_;
    bool chunked_encoding_;

    std::size_t chunk_size_;
    std::size_t bytes_read_;
    std::size_t redirect_count_;
    std::size_t content_length_;

    http::buffer buffer_;
    http::request request_;
    http::response response_;

    util::async_timer worker_timer_;
    asio::ip::tcp::resolver resolver_;
    tcp::connection::pointer connection_;

    http::chunked_parser chunked_parser_;
    http::response_parser response_parser_;

    http::data_handler data_handler_;
    http::response_handler response_handler_;

    std::queue<http::buffer> write_buffers_;
    std::queue<http::completion_handler> handlers_;
};

}   // namespace client
}   // namespace http

#endif  // _http_client_connection_hpp
