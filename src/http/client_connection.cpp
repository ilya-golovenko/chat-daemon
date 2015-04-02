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
#include <http/client_connection.hpp>
#include <http/statistics.hpp>
#include <http/url_builder.hpp>
#include <http/url.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// BOOST headers
#include <boost/logic/tribool.hpp>

// STL headers
#include <functional>
#include <algorithm>
#include <chrono>
#include <string>
#include <tuple>


namespace http
{

client_connection::pointer client_connection::create(asio::io_service& io_service, std::string const& hostname, std::uint16_t port)
{
    return std::make_shared<client_connection>(std::ref(io_service), hostname, port);
}

client_connection::client_connection(asio::io_service& io_service, std::string const& hostname, std::uint16_t port) :
    hostname_(hostname),
    port_(port),
    proxy_port_(0u),
    use_proxy_server_(false),
    keep_alive_(false),
    reading_body_(false),
    chunked_encoding_(false),
    chunk_size_(0u),
    bytes_read_(0u),
    redirect_count_(0u),
    content_length_(0u),
    buffer_(8192u),
    resolver_(io_service),
    worker_timer_(io_service)
{
    connection_ = tcp_connection::create(io_service);
}

client_connection::~client_connection()
{
    stop_connection();
}

tcp_connection::pointer client_connection::get_tcp_connection() const
{
    return connection_;
}

request const& client_connection::get_request() const
{
    return request_;
}

response const& client_connection::get_response() const
{
    return response_;
}

void client_connection::set_proxy_server(std::string const& hostname, std::uint16_t port)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    LOG_COMP_DEBUG(client_connection, "setting proxy server: ", hostname, ":", port);

    proxy_port_ = port;
    proxy_hostname_ = hostname;
    use_proxy_server_ = true;
}

void client_connection::send_request(request const& request, completion_handler const& handler)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    request_ = request;
    handler_ = handler;

    if(use_proxy_server_)
    {
        url_builder url_builder(request_.get_url());

        url_builder.set_hostname(proxy_hostname_);
        url_builder.set_port(proxy_port_);

        request_.set_url(url_builder.to_string());
    }

    start_connection(false);
}

bool client_connection::is_open() const
{
    return connection_->is_open();
}

void client_connection::close()
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    stop_connection();
}

void client_connection::start_connection(bool redirected)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    stop_worker_timer();

    response_.clear();
    response_.clear_body();

    reading_body_ = false;
    chunked_encoding_ = false;

    chunk_size_ = 0u;
    bytes_read_ = 0u;
    content_length_ = 0u;

    if(!redirected)
        redirect_count_ = 0u;

    std::string hostname = hostname_;
    std::uint16_t port = port_;

    if(use_proxy_server_)
    {
        hostname = proxy_hostname_;
        port = proxy_port_;
    }

    if(connection_->is_open() && keep_alive_ && hostname == hostname_ && port == port_)
    {
        message::buffers buffers;
        request_.to_buffers(buffers);

        LOG_COMP_DEBUG(client_connection, "writing request to http server using existing connection");
        connection_->write(buffers, bind_to_write_handler());
    }
    else
    {
        stop_connection();

        hostname_ = hostname;
        port_ = port;

        request_.set_host(hostname_, port_);

        asio::error_code error;
        asio::ip::address address;

        address = asio::ip::address::from_string(hostname_, error);

        if(!error)
        {
            asio::ip::tcp::endpoint endpoint = asio::ip::tcp::endpoint(address, port_);
            LOG_COMP_DEBUG(client_connection, "connecting to http server: ", endpoint);

            asio::ip::tcp::resolver::iterator iterator;

            connection_->connect(endpoint, /*bind_to_connect_handler()*/ std::bind(&client_connection::handle_connect, shared_from_this(), std::placeholders::_1, iterator));
        }
        else
        {
            asio::ip::tcp::resolver::query query(hostname_, std::to_string(port_));
            LOG_COMP_DEBUG(client_connection, "resolving http server host name: ", hostname_);

            resolver_.async_resolve(query, /*bind_to_resolve_handler()*/ std::bind(&client_connection::handle_resolve, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
    }
}

void client_connection::stop_connection()
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    if(connection_->is_open())
    {
        LOG_COMP_DEBUG(client_connection, "closing connection to http server");

        connection_->close();
        stop_worker_timer();

        keep_alive_ = false;
    }
}

bool client_connection::redirect_to_response_location()
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    status const& status = response_.get_status();

    if(status == status::moved_permanently ||
       status == status::moved_temporarily)
    {
        if(++redirect_count_ < max_redirect_count)
        {
            boost::optional<std::string> value = response_.get(header_names::location);

            if(value)
            {
                url const url = url::from_string(*value);
                LOG_COMP_DEBUG(client_connection, "redirecting to the new location: ", url);

                std::string hostname = hostname_;
                std::uint16_t port = port_;                

                if(!url.get_hostname().empty())
                {
                    hostname = url.get_hostname();
                    port = url.get_port();
                }

                request_.set_url(url.get_relative_url());
                request_.set_method(request_methods::get);

                if(hostname != hostname_ || port != port_)
                {
                    if(connection_->is_open())
                        connection_->close();

                    hostname_ = hostname;
                    port_ = port;
                }

                start_connection(true);

                return true;
            }
        }
    }

    return false;
}

bool client_connection::is_reading_completed(asio::error_code const& error) const
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    if(request_.get_method() != request_methods::head)
    {
        boost::optional<std::size_t> length = response_.get_content_length();
        return length ? *length == bytes_read_ : error == asio::error::eof;
    }

    return true;
}

bool client_connection::is_keep_alive_connection_aborted(asio::error_code const& error) const
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    if(keep_alive_)
    {
        return error == asio::error::eof || 
               error == asio::error::bad_descriptor ||
               error == asio::error::connection_reset ||
               error == asio::error::connection_aborted;
    }

    return false;
}

void client_connection::setup_connection(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    keep_alive_ = !error && response_.is_keep_alive();

    boost::optional<std::size_t> content_length = response_.get_content_length();
    content_length_ = content_length ? *content_length : static_cast<std::size_t>(-1);

    boost::optional<std::string> encoding = response_.get_transfer_encoding();
    chunked_encoding_ = encoding && boost::algorithm::iequals(*encoding, encoding_tokens::chunked);
}

void client_connection::stop_worker_timer()
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    LOG_COMP_DEBUG(client_connection, "stopping worker timer");
    worker_timer_.cancel();
}

void client_connection::start_keep_alive_timer()
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    LOG_COMP_TRACE(client_connection, "starting keep alive timer");

    worker_timer_.expires_from_now(std::chrono::seconds(120));
    worker_timer_.async_wait(bind_to_keep_alive_timer_handler());
}

void client_connection::start_read_timeout_timer()
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    LOG_COMP_TRACE(client_connection, "starting read timeout timer");

    worker_timer_.expires_from_now(std::chrono::seconds(120));
    worker_timer_.async_wait(bind_to_read_timeout_timer_handler());
}

read_handler client_connection::bind_to_read_handler()
{
    return std::bind(&client_connection::handle_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2);
}

write_handler client_connection::bind_to_write_handler()
{
    return std::bind(&client_connection::handle_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2);
}

timer_handler client_connection::bind_to_keep_alive_timer_handler()
{
    return std::bind(&client_connection::handle_keep_alive_timer, shared_from_this(), std::placeholders::_1);
}

timer_handler client_connection::bind_to_read_timeout_timer_handler()
{
    return std::bind(&client_connection::handle_read_timeout_timer, shared_from_this(), std::placeholders::_1);
}

void client_connection::call_completion_handler(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    if(!error && keep_alive_)
    {
        start_keep_alive_timer();
    }
    else
    {
        stop_connection();
    }

    try
    {
        handler_(error);
    }
    catch(std::exception const& e)
    {
        LOG_COMP_WARNING(client_connection, e);
    }
}

void client_connection::handle_resolve(asio::error_code const& error, asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    if(!error)
    {
        LOG_COMP_DEBUG(client_connection, "successfully resolved http server host name");

        asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
        LOG_COMP_DEBUG(client_connection, "connecting to http server: ", endpoint);

        connection_->connect(endpoint, /*bind_to_connect_handler()*/ std::bind(&client_connection::handle_connect, shared_from_this(), std::placeholders::_1, ++endpoint_iterator));
    }
    else
    {
        LOG_COMP_DEBUG(client_connection, "cannot resolve http server host name: ", error);
        call_completion_handler(error);
    }
}

void client_connection::handle_connect(asio::error_code const& error, asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    if(!error)
    {
        LOG_COMP_DEBUG(client_connection, "successfully connected to http server");

        message::buffers buffers;
        request_.to_buffers(buffers);

        LOG_COMP_DEBUG(client_connection, "writing request to http server");
        connection_->write(buffers, bind_to_write_handler());
    }
    else
    {
        LOG_COMP_DEBUG(client_connection, "cannot connect to http server: ", error);

        if(endpoint_iterator != asio::ip::tcp::resolver::iterator())
        {
            if(connection_->is_open())
                connection_->close();

            asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
            LOG_COMP_DEBUG(client_connection, "connecting to http server: ", endpoint);

            connection_->connect(endpoint, /*bind_to_connect_handler()*/ std::bind(&client_connection::handle_connect, shared_from_this(), std::placeholders::_1, ++endpoint_iterator));
        }
        else
        {
            call_completion_handler(error);
        }
    }
}

void client_connection::handle_write(asio::error_code const& error, std::size_t bytes_transferred)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    LOG_COMP_TRACE(client_connection, bytes_transferred, " bytes of data has been written to http server");

    statistics::add_bytes_written(bytes_transferred);

    if(is_keep_alive_connection_aborted(error))
    {
        LOG_COMP_DEBUG(client_connection, "keep-alive connection has been aborted: ", error);

        if(connection_->is_open())
            connection_->close();

        // try to restart keep-alive connection
        start_connection(false);
    }
    else if(!error)
    {
        LOG_COMP_DEBUG(client_connection, "request to http server has been written successfully");

        start_read_timeout_timer();

        LOG_COMP_DEBUG(client_connection, "reading response from http server");
        connection_->read(buffer_, bind_to_read_handler());
    }
    else
    {
        LOG_COMP_DEBUG(client_connection, "cannot write request to http server: ", error);
        call_completion_handler(error);
    }
}

void client_connection::handle_read(asio::error_code const& error, std::size_t bytes_transferred)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    LOG_COMP_TRACE(client_connection, bytes_transferred, " bytes of data has been read from http server");

    statistics::add_bytes_read(bytes_transferred);

    if(is_keep_alive_connection_aborted(error))
    {
        LOG_COMP_DEBUG(client_connection, "keep-alive connection has been aborted: ", error);

        if(connection_->is_open())
            connection_->close();

        // try to restart keep-alive connection
        start_connection(false);
    }
    else if(!error || error == asio::error::eof)
    {
        boost::tribool result = true;
        char const* begin = buffer_.data();
        char const* end = begin + bytes_transferred;

        if(!reading_body_)
        {
            std::tie(result, begin) = response_parser_.parse(response_, begin, end);

            if(boost::indeterminate(result))
            {
                if(error == asio::error::eof)
                {
                    LOG_COMP_DEBUG(client_connection, "server unexpectedly closed connection: ", error);
                    call_completion_handler(error);
                }
                else
                {
                    LOG_COMP_DEBUG(client_connection, "reading response from http server");
                    connection_->read(buffer_, bind_to_read_handler());
                }
            }
            else if(!result)
            {
                LOG_COMP_DEBUG(client_connection, "cannot parse response from http server");
                call_completion_handler(asio::error::invalid_argument);
            }
            else
            {
                chunk_size_ = 0u;
                reading_body_ = true;

                stop_worker_timer();
                setup_connection(error);

                LOG_COMP_DEBUG(client_connection, "http server response status: ", response_.get_status());
                call_completion_handler(asio::error_code());
            }
        }

        if(reading_body_)
        {
            if(chunked_encoding_)
            {
                while(result && begin != end)
                {
                    if(chunk_size_ > 0)
                    {
                        std::size_t size = end - begin;

                        if(size > chunk_size_)
                            size = chunk_size_;

                        if(size > 0)
                            response_.append_body(begin, begin + size);

                        begin += size;
                        chunk_size_ -= size;

                        if(begin == end && chunk_size_ > 0)
                            result = boost::indeterminate;
                    }

                    if(0u == chunk_size_)
                    {
                        std::tie(result, begin) = chunked_parser_.parse(begin, end);

                        if(!result)
                        {
                            LOG_COMP_DEBUG(client_connection, "cannot parse response from http server");
                            call_completion_handler(asio::error::invalid_argument);
                        }
                        else if(result)
                        {
                            chunk_size_ = chunked_parser_.get_chunk_size();

                            if(0u == chunk_size_)
                            {
                                if(!redirect_to_response_location())
                                {
                                    LOG_COMP_DEBUG(client_connection, "response from http server has been read successfully");
                                    call_completion_handler(asio::error_code());
                                }
                            }
                        }
                    }
                }

                if(boost::indeterminate(result))
                {
                    if(error == asio::error::eof)
                    {
                        LOG_COMP_DEBUG(client_connection, "server unexpectedly closed connection: ", error);
                        call_completion_handler(error);
                    }
                    else
                    {
                        LOG_COMP_TRACE(client_connection, "reading data from http server");
                        connection_->read(buffer_, bind_to_read_handler());
                    }
                }
            }
            else
            {
                if(begin != end)
                {
                    bytes_read_ += end - begin;
                    response_.append_body(begin, end);
                }

                if(is_reading_completed(error))
                {
                    if(!redirect_to_response_location())
                    {
                        LOG_COMP_DEBUG(client_connection, "response from http server has been read successfully");
                        call_completion_handler(asio::error_code());
                    }
                }
                else if(error == asio::error::eof)
                {
                    LOG_COMP_DEBUG(client_connection, "server unexpectedly closed connection: ", error);
                    call_completion_handler(error);
                }
                else
                {
                    LOG_COMP_TRACE(client_connection, "reading data from http server");
                    connection_->read(buffer_, bind_to_read_handler());
                }
            }
        }
    }
    else
    {
        LOG_COMP_DEBUG(client_connection, "cannot read response from http server: ", error);
        call_completion_handler(error);
    }
}

void client_connection::handle_keep_alive_timer(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    if(!error)
    {
        LOG_COMP_DEBUG(client_connection, "closing connection because of idle timeout");
        stop_connection();
    }
}

void client_connection::handle_read_timeout_timer(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(client_connection);

    if(!error)
    {
        LOG_COMP_DEBUG(client_connection, "closing connection because of read timeout");
        stop_connection();
    }
}

}   // namespace http
