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

// Application headers
#include "tcp_connection.hpp"

// BOOST headers
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>


namespace tcp
{

connection::pointer connection::create(asio::io_service& io_service)
{
    return boost::make_shared<connection>(boost::ref(io_service));
}

asio::ip::tcp::socket& connection::socket()
{
    return socket_;
}

asio::io_service& connection::get_io_service()
{
    return socket_.get_io_service();
}

bool connection::is_open() const
{
    return socket_.is_open();
}

void connection::close()
{
    if(socket_.is_open())
    {
        asio::ip::tcp::socket::shutdown_type shutdown;
        shutdown = asio::ip::tcp::socket::shutdown_both;

        asio::error_code ignored_ec;
        socket_.shutdown(shutdown, ignored_ec);

        socket_.close();
    }
}

const asio::ip::tcp::endpoint connection::get_remote_endpoint() const
{
    asio::error_code ignored_error;
    return socket_.remote_endpoint(ignored_error);
}

const asio::ip::address connection::get_remote_address() const
{
    return get_remote_endpoint().address();
}

unsigned short connection::get_remote_port() const
{
    return get_remote_endpoint().port();
}

connection::connection(asio::io_service& io_service) :
    socket_(io_service)
{
}

}   // namespace tcp
