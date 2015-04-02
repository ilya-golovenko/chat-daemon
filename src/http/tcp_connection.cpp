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
#include <http/tcp_connection.hpp>

// STL headers
#include <functional>


namespace http
{

tcp_connection::pointer tcp_connection::create(asio::io_service& io_service)
{
    return std::make_shared<tcp_connection>(std::ref(io_service));
}

asio::ip::tcp::socket& tcp_connection::get_socket()
{
    return socket_;
}

asio::io_service& tcp_connection::get_io_service()
{
    return socket_.get_io_service();
}

bool tcp_connection::is_open() const
{
    return socket_.is_open();
}

void tcp_connection::close()
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

asio::ip::tcp::endpoint tcp_connection::get_remote_endpoint() const
{
    asio::error_code ignored_error;
    return socket_.remote_endpoint(ignored_error);
}

asio::ip::address tcp_connection::get_remote_address() const
{
    return get_remote_endpoint().address();
}

std::uint16_t tcp_connection::get_remote_port() const
{
    return get_remote_endpoint().port();
}

tcp_connection::tcp_connection(asio::io_service& io_service) :
    socket_(io_service)
{
}

}   // namespace http
