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
#ifndef _tcp_connection_hpp
#define _tcp_connection_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <handler_alloc.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


namespace tcp
{

class connection :
    private boost::noncopyable
{
public:
    typedef boost::shared_ptr<connection> pointer;
    static pointer create(asio::io_service& io_service);

public:
    connection(asio::io_service& io_service);

    asio::ip::tcp::socket& socket();
    asio::io_service& get_io_service();

    const asio::ip::tcp::endpoint get_remote_endpoint() const;
    const asio::ip::address get_remote_address() const;
    unsigned short get_remote_port() const;

    template <typename SettableSocketOption>
    void set_option(const SettableSocketOption& option);

    template <typename GettableSocketOption>
    void get_option(GettableSocketOption& option) const;

    template <typename ConnectHandler>
    void connect(const asio::ip::tcp::endpoint& endpoint, const ConnectHandler& handler);

    template <typename MutableBufferSequence, typename ReadHandler>
    void read(const MutableBufferSequence& buffers, const ReadHandler& handler);

    template <typename ConstBufferSequence, typename WriteHandler>
    void write(const ConstBufferSequence& buffers, const WriteHandler& handler);

    bool is_open() const;
    void close();

private:
    typedef util::handler_allocator<1, 512> allocator_type;

private:
    asio::ip::tcp::socket socket_;
    allocator_type allocator_;
};

template <typename SettableSocketOption>
void connection::set_option(const SettableSocketOption& option)
{
    socket_.set_option(option);
}

template <typename GettableSocketOption>
void connection::get_option(GettableSocketOption& option) const
{
    socket_.get_option(option);
}

template <typename ConnectHandler>
void connection::connect(const asio::ip::tcp::endpoint& endpoint, const ConnectHandler& handler)
{
    socket_.async_connect(endpoint, util::make_custom_alloc_handler(allocator_, handler));
}

template <typename MutableBufferSequence, typename ReadHandler>
void connection::read(const MutableBufferSequence& buffers, const ReadHandler& handler)
{
    socket_.async_read_some(buffers, util::make_custom_alloc_handler(allocator_, handler));
}

template <typename ConstBufferSequence, typename WriteHandler>
void connection::write(const ConstBufferSequence& buffers, const WriteHandler& handler)
{
    socket_.async_write_some(buffers, util::make_custom_alloc_handler(allocator_, handler));
}

}   // namespace tcp

#endif  // _tcp_connection_hpp
