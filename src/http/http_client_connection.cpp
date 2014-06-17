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
#include "http_client_connection.hpp"
#include "http_statistics.hpp"
#include "http_url_builder.hpp"
#include "http_url.hpp"
#include <log_common.hpp>

// BOOST headers
#include <boost/logic/tribool.hpp>
#include <boost/make_shared.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/date_time.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

// STL headers
#include <algorithm>


namespace http
{
namespace client
{

connection::pointer connection::create(asio::io_service& io_service)
{
    return boost::make_shared<client::connection>(boost::ref(io_service));
}

connection::connection(asio::io_service& io_service) :
    port_(0),
    proxy_port_(0),
    use_proxy_server_(false),
    keep_alive_(false),
    reading_body_(false),
    chunked_encoding_(false),
    chunk_size_(0),
    bytes_read_(0),
    redirect_count_(0),
    content_length_(0),
    buffer_(8192),
    worker_timer_(io_service),
    resolver_(io_service)
{
    connection_ = tcp::connection::create(io_service);
}

connection::~connection()
{
    stop_connection();
}

const http::response& connection::get_response() const
{
    return response_;
}

tcp::connection::pointer connection::get_tcp_connection() const
{
    return connection_;
}

void connection::set_data_handler(const http::data_handler& handler)
{
    data_handler_ = handler;
}

void connection::set_response_handler(const http::response_handler& handler)
{
    response_handler_ = handler;
}

void connection::set_proxy_server(const std::string& hostname, unsigned short port)
{
    proxy_port_ = port;
    proxy_hostname_ = hostname;
    use_proxy_server_ = true;
}

void connection::start(const std::string& hostname, unsigned short port,
    const http::request& request, const http::completion_handler& handler)
{
    request_ = request;

    if(use_proxy_server_)
    {
        http::url_builder url_builder(request_.get_url());

        url_builder.set_hostname(hostname);
        url_builder.set_port(port);

        std::string url = url_builder.to_string(
            http::url::all_components, false);

        request_.set_url(url);
    }

    add_completion_handler(handler);
    start_connection(hostname, port, false);
}

bool connection::is_open() const
{
    return connection_->is_open();
}

void connection::close()
{
    stop_connection();
}

const std::string& connection::get_log_source_name() const
{
    return name_;
}

void connection::start_connection(const std::string& hostname, unsigned short port, bool redirected)
{
    stop_worker_timer();

    response_.clear();
    response_.clear_body();

    reading_body_ = false;
    chunked_encoding_ = false;

    chunk_size_ = 0;
    bytes_read_ = 0;
    content_length_ = 0;

    if(!redirected)
        redirect_count_ = 0;

    unsigned short new_port = port;
    std::string new_hostname = hostname;

    if(use_proxy_server_)
    {
        new_port = proxy_port_;
        new_hostname = proxy_hostname_;
    }

    if(connection_->is_open() && keep_alive_ &&
        new_hostname == hostname_ && new_port == port_)
    {
        message::buffers_type  buffers;
        request_.to_buffers(buffers);

        LOGDBG("writing request to http server using existing connection");
        connection_->write(buffers, bind_to_write_handler());
    }
    else
    {
        stop_connection();

        port_ = new_port;
        hostname_ = new_hostname;

        request_.set_host(hostname_, port_);

        name_ = boost::str(boost::format(
            "client connection %1%:%2%")
            % hostname_ % port_);

        asio::error_code error;
        asio::ip::address address;

        address = asio::ip::address::from_string(hostname_, error);

        if(!error)
        {
            asio::ip::tcp::endpoint endpoint = asio::ip::tcp::endpoint(address, port_);
            LOGDBG("connecting to http server: %1%") << endpoint;

            connection_->connect(endpoint, boost::bind(&connection::handle_connect,
                shared_from_this(), asio::placeholders::error, asio::ip::tcp::resolver::iterator()));
        }
        else
        {
            std::string port_string = boost::lexical_cast<std::string>(port_);
            asio::ip::tcp::resolver::query query(hostname_, port_string);
            LOGDBG("resolving http server hostname: %1%") << hostname_;

            resolver_.async_resolve(query, boost::bind(&connection::handle_resolve,
                shared_from_this(), asio::placeholders::error, asio::placeholders::iterator));
        }
    }
}

void connection::stop_connection()
{
    if(connection_->is_open())
    {
        LOGDBG("closing connection to http server");

        connection_->close();
        stop_worker_timer();

        keep_alive_ = false;
    }
}

bool connection::redirect_to_response_location()
{
    http::status status = response_.get_status();

    if(status == http::status::moved_permanently
        || status == http::status::moved_temporarily)
    {
        if(++redirect_count_ < max_redirect_count)
        {
            boost::optional<std::string> value =
                response_.get(header_names::location);

            if(value)
            {
                http::url url = http::url::from_string(*value);
                LOGDBG("redirecting to the new location: %1%") << url;

                unsigned short port = port_;
                std::string hostname = hostname_;

                if(!url.hostname().empty())
                {
                    port = url.port();
                    hostname = url.hostname();
                }

                request_.set_url(url.relative_url());
                request_.set_method(request_methods::get);

                start_connection(hostname, port, true);

                return true;
            }
        }
    }

    return false;
}

bool connection::is_reading_completed(const asio::error_code& error) const
{
    if(request_.get_method() != request_methods::head)
    {
        boost::optional<std::size_t> length = response_.get_content_length();
        return (length ? *length == bytes_read_ : error == asio::error::eof);
    }

    return true;
}

bool connection::is_keep_alive_connection_aborted(const asio::error_code& error) const
{
    return (keep_alive_
        && (error == asio::error::eof
        || error == asio::error::bad_descriptor
        || error == asio::error::connection_reset
        || error == asio::error::connection_aborted));
}

void connection::setup_connection(const asio::error_code& error)
{
    keep_alive_ = (!error && response_.is_keep_alive());

    boost::optional<std::size_t> content_length = response_.get_content_length();
    content_length_ = (content_length ? *content_length : static_cast<std::size_t>(-1));

    boost::optional<std::string> encoding = response_.get_transfer_encoding();
    chunked_encoding_ = encoding && boost::iequals(*encoding, encoding_tokens::chunked);
}

void connection::stop_worker_timer()
{
    LOGDBG("stopping worker timer");
    worker_timer_.stop();
}

void connection::start_keep_alive_timer()
{
    LOGDBG("starting keep alive timer");

    worker_timer_.start(
        boost::posix_time::seconds(120),
        bind_to_keep_alive_timer_handler());
}

void connection::start_read_timeout_timer()
{
    LOGDBG("starting read timeout timer");

    worker_timer_.start(
        boost::posix_time::seconds(120),
        bind_to_read_timeout_timer_handler());
}

const http::read_handler connection::bind_to_read_handler()
{
    return boost::bind(&connection::handle_read, shared_from_this(), _1, _2);
}

const http::write_handler connection::bind_to_write_handler()
{
    return  boost::bind(&connection::handle_write, shared_from_this(), _1, _2);
}

const http::timer_handler connection::bind_to_keep_alive_timer_handler()
{
    return  boost::bind(&connection::handle_keep_alive_timer, shared_from_this(), _1);
}

const http::timer_handler connection::bind_to_read_timeout_timer_handler()
{
    return  boost::bind(&connection::handle_read_timeout_timer, shared_from_this(), _1);
}

void connection::call_data_handler(const char* data, std::size_t size)
{
    if(data_handler_)
    {
        try
        {
            http::buffer buffer(data, size);
            data_handler_(buffer);
        }
        catch(std::exception& e)
        {
            LOGWRN(e.what());
        }
    }
    else
    {
        response_.append_body(data, data + size);
    }
}

void connection::call_response_handler(const http::response& response)
{
    if(response_handler_)
    {
        try
        {
            response_handler_(response);
        }
        catch(std::exception& e)
        {
            LOGWRN(e.what());
        }
    }
}

void connection::add_completion_handler(const http::completion_handler& handler)
{
    handlers_.push(handler);
}

void connection::complete(const asio::error_code& error, std::size_t bytes_transferred)
{
    if(!error && keep_alive_)
        start_keep_alive_timer();
    else
        stop_connection();

    try
    {
        (handlers_.front())(error, bytes_transferred);
    }
    catch(std::exception& e)
    {
        LOGWRN(e.what());
    }

    handlers_.pop();
}

void connection::handle_resolve(const asio::error_code& error, asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    if(!error)
    {
        LOGDBG("successfully resolved http server hostname");

        asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
        LOGDBG("connecting to http server: %1%") << endpoint;

        connection_->connect(endpoint, boost::bind(&connection::handle_connect,
            shared_from_this(), asio::placeholders::error, ++endpoint_iterator));
    }
    else
    {
        LOGDBG("cannot resolve http server hostname: %1%") << error.message();
        complete(error, 0);
    }
}

void connection::handle_connect(const asio::error_code& error, asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    if(!error)
    {
        LOGDBG("successfully connected to http server");

        message::buffers_type buffers;
        request_.to_buffers(buffers);

        LOGDBG("writing request to http server");
        connection_->write(buffers, bind_to_write_handler());
    }
    else
    {
        LOGDBG("cannot connect to http server: %1%") << error.message();

        if(endpoint_iterator != asio::ip::tcp::resolver::iterator())
        {
            if(connection_->is_open())
                connection_->close();

            asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
            LOGDBG("connecting to http server: %1%") << endpoint;

            connection_->connect(endpoint, boost::bind(&connection::handle_connect,
                shared_from_this(), asio::placeholders::error, ++endpoint_iterator));
        }
        else
        {
            complete(error, 0);
        }
    }
}

void connection::handle_write(const asio::error_code& error, std::size_t bytes_transferred)
{
    LOGDBG("%1% bytes of data has been written to http server") << bytes_transferred;

    http::statistics::add_bytes_written(bytes_transferred);

    if(is_keep_alive_connection_aborted(error))
    {
        LOGDBG("keep-alive connection has been aborted: %1%") << error.message();

        if(connection_->is_open())
            connection_->close();

        // try to restart keep-alive connection
        start_connection(hostname_, port_, false);
    }
    else if(!error)
    {
        LOGDBG("request to http server has been written successfully");

        start_read_timeout_timer();

        LOGDBG("reading response from http server");
        connection_->read(buffer_, bind_to_read_handler());
    }
    else
    {
        LOGDBG("cannot write request to http server: %1%") << error.message();
        complete(error, bytes_transferred);
    }
}

void connection::handle_read(const asio::error_code& error, std::size_t bytes_transferred)
{
    LOGDBG("%1% bytes of data has been read from http server") << bytes_transferred;

    http::statistics::add_bytes_read(bytes_transferred);

    if(is_keep_alive_connection_aborted(error))
    {
        LOGDBG("keep-alive connection has been aborted: %1%") << error.message();

        if(connection_->is_open())
            connection_->close();

        // try to restart keep-alive connection
        start_connection(hostname_, port_, false);
    }
    else if(!error || error == asio::error::eof)
    {
        boost::tribool result = true;
        const char* begin = buffer_.data();
        const char* end = begin + bytes_transferred;

        if(!reading_body_)
        {
            boost::tie(result, begin) = response_parser_.parse(response_, begin, end);

            if(boost::indeterminate(result))
            {
                if(error == asio::error::eof)
                {
                    LOGDBG("server unexpectedly closed connection: %1%") << error.message();
                    complete(error, bytes_transferred);
                }
                else
                {
                    LOGDBG("reading response from http server");
                    connection_->read(buffer_, bind_to_read_handler());
                }
            }
            else if(!result)
            {
                LOGDBG("cannot parse response from http server");
                complete(asio::error::invalid_argument, bytes_transferred);
            }
            else
            {
                chunk_size_ = 0;
                reading_body_ = true;

                stop_worker_timer();
                setup_connection(error);
                call_response_handler(response_);

                LOGDBG("http server response status: %1%") << response_.get_status();
            }
        }

       if(reading_body_)
       {
            if(chunked_encoding_)
            {
                while(result && begin < end)
                {
                    if(chunk_size_ > 0)
                    {
                        std::size_t size = end - begin;

                        if(size > chunk_size_)
                            size = chunk_size_;

                        if(size > 0)
                            call_data_handler(begin, size);

                        begin += size;
                        chunk_size_ -= size;

                        if(begin == end && chunk_size_ > 0)
                            result = boost::indeterminate;
                    }

                    if(0 == chunk_size_)
                    {
                        boost::tie(result, begin) = chunked_parser_.parse(begin, end);

                        if(!result)
                        {
                            LOGDBG("cannot parse response from http server");
                            complete(asio::error::invalid_argument, bytes_transferred);
                        }
                        else if(result)
                        {
                            chunk_size_ = chunked_parser_.get_chunk_size();

                            if(0 == chunk_size_)
                            {
                                if(!redirect_to_response_location())
                                {
                                    LOGDBG("response from http server has been read successfully");
                                    complete(asio::error_code(), bytes_transferred);
                                }
                            }
                        }
                    }
                }

                if(boost::indeterminate(result))
                {
                    if(error == asio::error::eof)
                    {
                        LOGDBG("server unexpectedly closed connection: %1%") << error.message();
                        complete(error, bytes_transferred);
                    }
                    else
                    {
                        LOGDBG("reading data from http server");
                        connection_->read(buffer_, bind_to_read_handler());
                    }
                }
            }
            else
            {
                if(begin < end)
                {
                    bytes_read_ += end - begin;
                    call_data_handler(begin, end - begin);
                }

                if(is_reading_completed(error))
                {
                    if(!redirect_to_response_location())
                    {
                        LOGDBG("response from http server has been read successfully");
                        complete(asio::error_code(), bytes_transferred);
                    }
                }
                else if(error == asio::error::eof)
                {
                    LOGDBG("server unexpectedly closed connection: %1%") << error.message();
                    complete(error, bytes_transferred);
                }
                else
                {
                    LOGDBG("reading data from http server");
                    connection_->read(buffer_, bind_to_read_handler());
                }
            }
        }
    }
    else
    {
        LOGDBG("cannot read response from http server: %1%") << error.message();
        complete(error, bytes_transferred);
    }
}

void connection::handle_keep_alive_timer(const asio::error_code& error)
{
    if(!error)
    {
        LOGDBG("closing connection because of idle timeout");
        stop_connection();
    }
}

void connection::handle_read_timeout_timer(const asio::error_code& error)
{
    if(!error)
    {
        LOGDBG("closing connection because of read timeout");
        stop_connection();
    }
}

}   // namespace client
}   // namespace http
