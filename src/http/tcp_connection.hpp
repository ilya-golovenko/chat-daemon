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
#ifndef _http_tcp_connection_hpp
#define _http_tcp_connection_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio.hpp>

// STL headers
#include <utility>


namespace tcp
{

class connection
{
public:
    explicit connection(asio::io_service& io_service);

    connection(connection const&) = delete;
    connection& operator=(connection const&) = delete;

    connection(connection&&) = default;
    connection& operator=(connection&&) = default;

    asio::ip::tcp::socket& get_socket();
    asio::io_service& get_io_service();

    asio::ip::tcp::endpoint get_remote_endpoint() const;
    asio::ip::address get_remote_address() const;
    std::uint16_t get_remote_port() const;

    template <typename SettableSocketOption>
    void set_option(SettableSocketOption const& option);

    template <typename GettableSocketOption>
    void get_option(GettableSocketOption& option) const;

    template <typename ConnectHandler>
    void connect(asio::ip::tcp::endpoint const& endpoint, ConnectHandler&& handler);

    template <typename MutableBufferSequence, typename ReadHandler>
    void read(MutableBufferSequence const& buffers, ReadHandler&& handler);

    template <typename ConstBufferSequence, typename WriteHandler>
    void write(ConstBufferSequence const& buffers, WriteHandler&& handler);

    bool is_open() const;
    void close();

private:
    asio::ip::tcp::socket socket_;
};

template <typename SettableSocketOption>
void connection::set_option(SettableSocketOption const& option)
{
    socket_.set_option(option);
}

template <typename GettableSocketOption>
void connection::get_option(GettableSocketOption& option) const
{
    socket_.get_option(option);
}

template <typename ConnectHandler>
void connection::connect(asio::ip::tcp::endpoint const& endpoint, ConnectHandler&& handler)
{
    socket_.async_connect(endpoint, std::forward<ConnectHandler>(handler));
}

template <typename MutableBufferSequence, typename ReadHandler>
void connection::read(MutableBufferSequence const& buffers, ReadHandler&& handler)
{
    socket_.async_read_some(buffers, std::forward<ReadHandler>(handler));
}

template <typename ConstBufferSequence, typename WriteHandler>
void connection::write(ConstBufferSequence const& buffers, WriteHandler&& handler)
{
    socket_.async_write_some(buffers, std::forward<WriteHandler>(handler));
}

}   // namespace tcp

#endif  // _http_tcp_connection_hpp
