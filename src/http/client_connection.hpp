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
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/request.hpp>
#include <http/response.hpp>
#include <http/response_parser.hpp>
#include <http/chunked_parser.hpp>
#include <http/tcp_connection.hpp>
#include <http/buffer.hpp>
#include <http/common.hpp>

// BOOST headers
#include <boost/asio/steady_timer.hpp>
#include <boost/asio.hpp>

// STL headers
#include <cstdint>
#include <memory>
#include <string>
#include <queue>


namespace http
{

class client_connection : public std::enable_shared_from_this<client_connection>
{
public:
    typedef std::shared_ptr<client_connection> pointer;

public:
    static pointer create(boost::asio::io_service& io_service, std::string const& hostname, std::uint16_t port);

public:
    client_connection(boost::asio::io_service& io_service, std::string const& hostname, std::uint16_t port);
    ~client_connection();

    client_connection(client_connection const&) = delete;
    client_connection& operator=(client_connection const&) = delete;

    tcp_connection::pointer get_tcp_connection() const;

    request const& get_request() const;
    response const& get_response() const;

    void set_proxy_server(std::string const& hostname, std::uint16_t port);

    void send_request(request const& request, completion_handler const& handler);

    bool is_open() const;
    void close();

private:
    void start_connection(bool redirected);
    void stop_connection();

    bool redirect_to_response_location();

    bool is_reading_completed(boost::system::error_code const& error) const;
    bool is_keep_alive_connection_aborted(boost::system::error_code const& error) const;

    void setup_connection(boost::system::error_code const& error);

    void stop_worker_timer();
    void start_keep_alive_timer();
    void start_read_timeout_timer();

    read_handler bind_to_read_handler();
    write_handler bind_to_write_handler();
    timer_handler bind_to_keep_alive_timer_handler();
    timer_handler bind_to_read_timeout_timer_handler();

    void call_completion_handler(boost::system::error_code const& error);

    void handle_resolve(boost::system::error_code const& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    void handle_connect(boost::system::error_code const& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    void handle_read(boost::system::error_code const& error, std::size_t bytes_transferred);
    void handle_write(boost::system::error_code const& error, std::size_t bytes_transferred);
    void handle_keep_alive_timer(boost::system::error_code const& error);
    void handle_read_timeout_timer(boost::system::error_code const& error);

private:
    std::string hostname_;
    std::uint16_t port_;

    std::string proxy_hostname_;
    std::uint16_t proxy_port_;
    bool use_proxy_server_;

    bool keep_alive_;
    bool reading_body_;
    bool chunked_encoding_;

    std::size_t chunk_size_;
    std::size_t bytes_read_;
    std::size_t redirect_count_;
    std::size_t content_length_;

    buffer buffer_;
    request request_;
    response response_;

    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::steady_timer worker_timer_;

    tcp_connection::pointer connection_;

    chunked_parser chunked_parser_;
    response_parser response_parser_;

    completion_handler handler_;
};

}   // namespace http

#endif  // _http_client_connection_hpp
