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
#ifndef _http_tcp_connection_hpp
#define _http_tcp_connection_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/asio.hpp>

// STL headers
#include <memory>


namespace http
{

class tcp_connection
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;

public:
    static pointer create(boost::asio::io_service& io_service);

public:
    explicit tcp_connection(boost::asio::io_service& io_service);

    tcp_connection(tcp_connection const&) = delete;
    tcp_connection& operator=(tcp_connection const&) = delete;

    boost::asio::ip::tcp::socket& get_socket();
    boost::asio::io_service& get_io_service();

    boost::asio::ip::tcp::endpoint get_remote_endpoint() const;
    boost::asio::ip::address get_remote_address() const;
    std::uint16_t get_remote_port() const;

    template <typename SettableSocketOption>
    void set_option(SettableSocketOption const& option);

    template <typename GettableSocketOption>
    void get_option(GettableSocketOption& option) const;

    template <typename ConnectHandler>
    void connect(boost::asio::ip::tcp::endpoint const& endpoint, ConnectHandler const& handler);

    template <typename MutableBufferSequence, typename ReadHandler>
    void read(MutableBufferSequence const& buffers, ReadHandler const& handler);

    template <typename ConstBufferSequence, typename WriteHandler>
    void write(ConstBufferSequence const& buffers, WriteHandler const& handler);

    bool is_open() const;
    void close();

private:
    boost::asio::ip::tcp::socket socket_;
};

template <typename SettableSocketOption>
void tcp_connection::set_option(SettableSocketOption const& option)
{
    socket_.set_option(option);
}

template <typename GettableSocketOption>
void tcp_connection::get_option(GettableSocketOption& option) const
{
    socket_.get_option(option);
}

template <typename ConnectHandler>
void tcp_connection::connect(boost::asio::ip::tcp::endpoint const& endpoint, ConnectHandler const& handler)
{
    socket_.async_connect(endpoint, handler);
}

template <typename MutableBufferSequence, typename ReadHandler>
void tcp_connection::read(MutableBufferSequence const& buffers, ReadHandler const& handler)
{
    socket_.async_read_some(buffers, handler);
}

template <typename ConstBufferSequence, typename WriteHandler>
void tcp_connection::write(ConstBufferSequence const& buffers, WriteHandler const& handler)
{
    socket_.async_write_some(buffers, handler);
}

}   // namespace http

#endif  // _http_tcp_connection_hpp
