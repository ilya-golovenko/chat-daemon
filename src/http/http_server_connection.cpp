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
#include "http_server_connection.hpp"
#include "http_utilities.hpp"
#include "http_mime_types.hpp"
#include "http_common.hpp"
#include <log_common.hpp>

// BOOST headers
#include <boost/lexical_cast.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/make_shared.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/date_time.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>


namespace http
{
namespace server
{

connection::pointer connection::create(tcp::connection::pointer connection)
{
    return boost::make_shared<server::connection>(connection);
}

connection::connection(tcp::connection::pointer connection) :
    reading_body_(false),
    writing_response_(false),
    closing_connection_(false),
    chunked_encoding_(false),
    chunk_size_(0),
    buffer_(4096),
    connection_(connection),
    timeout_timer_(connection->get_io_service())
{
    set_remote_endpoint(connection->get_remote_endpoint());
}

connection::~connection()
{
    close_connection(true);
}

const http::request& connection::get_request() const
{
    return request_;
}


tcp::connection::pointer connection::get_tcp_connection() const
{
    return connection_;
}

bool connection::has_custom_remote_endpoint() const
{
    if(!connection_->is_open())
        return false;

    return (remote_endpoint_ != connection_->get_remote_endpoint());
}

const asio::ip::tcp::endpoint& connection::get_remote_endpoint() const
{
    return remote_endpoint_;
}

void connection::set_remote_endpoint(const asio::ip::tcp::endpoint& endpoint)
{
    remote_endpoint_ = endpoint;

    name_ = "server connection " +
        boost::lexical_cast<std::string>(endpoint);
}

unsigned short connection::get_remote_port() const
{
    return get_remote_endpoint().port();
}

const asio::ip::address connection::get_remote_address() const
{
    return get_remote_endpoint().address();
}

void connection::read_request(const http::completion_handler& handler)
{
    LOGDBG("reading request from http client");

    request_.clear();
    request_.clear_body();

    chunk_size_ = 0;
    reading_body_ = false;
    chunked_encoding_ = false;

    add_completion_handler(handler);
    connection_->read(buffer_, bind_to_read_handler());
}

void connection::read_request(const http::completion_handler& handler, long timeout)
{
    read_request(handler);

    if(timeout > 0)
    {
        timeout_timer_.start(boost::posix_time::seconds(timeout),
            boost::bind(&connection::handle_stop, shared_from_this(), _1));
    }
}

void connection::write_stock_response(http::status status, const std::string& server, const http::completion_handler& handler)
{
    write_response(http::response::stock_response(status, server), handler);
}

void connection::write_response(const http::response& response, const http::completion_handler& handler)
{
    if(is_open() && !closing_connection_)
    {
        response_ = response;
        writing_response_ = true;

        add_completion_handler(handler);

        message::buffers_type buffers;
        response_.to_buffers(buffers);

        LOGDBG("http response status: %1% ") << response_.get_status();

        LOGDBG("writing response to http client");
        connection_->write(buffers, bind_to_write_handler());
    }
}

void connection::write_buffer(const http::buffer& buffer, const http::completion_handler& handler)
{
    if(is_open() && !closing_connection_)
    {
        bool idle = write_buffers_.empty();
        write_buffers_.push(buffer);

        add_completion_handler(handler);

        if(idle && !writing_response_)
            write_front_connection_buffer();
    }
}

void connection::close(http::status status, const std::string& server, const http::completion_handler& handler)
{
    write_stock_response(status, server, handler);
    close_connection(false);
}

void connection::close(const http::response& response, const http::completion_handler& handler)
{
    write_response(response, handler);
    close_connection(false);
}

void connection::close(const http::buffer& buffer, const http::completion_handler& handler)
{
    write_buffer(buffer, handler);
    close_connection(false);
}

void connection::close(bool force)
{
    close_connection(force);
}

bool connection::is_open() const
{
    return connection_->is_open();
}

const std::string& connection::get_log_source_name() const
{
    return name_;
}

const http::read_handler connection::bind_to_read_handler()
{
    return boost::bind(&connection::handle_read, shared_from_this(), _1, _2);
}

const http::write_handler connection::bind_to_write_handler()
{
    return boost::bind(&connection::handle_write, shared_from_this(), _1, _2);
}

void connection::close_connection(bool force)
{
    if(connection_->is_open())
    {
        if(force || (!writing_response_ && write_buffers_.empty()))
        {
            LOGDBG("closing connection to http client");
            timeout_timer_.stop();
            connection_->close();
        }
        else if(!closing_connection_)
        {
            LOGDBG("request to close connection");
            closing_connection_ = true;
        }
    }
}

void connection::write_front_connection_buffer()
{
    boost::array<asio::const_buffer, 2> buffers =
    {{
        write_buffers_.front(),
        asio::buffer(strings::crlf)
    }};

    LOGDBG("writing data buffer to http client");
    connection_->write(buffers, bind_to_write_handler());
}

void connection::setup_connection(const asio::error_code& error)
{
    boost::optional<std::string> encoding = request_.get_transfer_encoding();
    chunked_encoding_ = encoding && boost::iequals(*encoding, encoding_tokens::chunked);
}

bool connection::is_reading_completed(const asio::error_code& error) const
{
    if(request_.get_method() == request_methods::post)
    {
        boost::optional<std::size_t> length = request_.get_content_length();
        return (length ? *length == request_.body_size() : error == asio::error::eof);
    }

    return true;
}

void connection::add_completion_handler(const http::completion_handler& handler)
{
    handlers_.push(handler);
}

void connection::complete(const asio::error_code& error, std::size_t bytes_transferred)
{
    if(error)
        close_connection(true);

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

void connection::handle_stop(const asio::error_code& error)
{
    if(!error)
    {
        LOGDBG("aborting connection because of timeout");
        close_connection(true);
    }
}

void connection::handle_read(const asio::error_code& error, std::size_t bytes_transferred)
{
    LOGDBG("%1% bytes of data has been read from http client") << bytes_transferred;

    if(!error || error == asio::error::eof)
    {
        boost::tribool result = true;
        const char* begin = buffer_.data();
        const char* end = begin + bytes_transferred;

        if(!reading_body_)
        {
            boost::tie(result, begin) = request_parser_.parse(request_, begin, end);

            if(boost::indeterminate(result))
            {
                if(error == asio::error::eof)
                {
                    LOGDBG("client unexpectedly closed connection: %1%") << error.message();
                    complete(error, bytes_transferred);
                }
                else
                {
                    LOGDBG("reading request from http client");
                    connection_->read(buffer_, bind_to_read_handler());
                }
            }
            else if(!result)
            {
                LOGDBG("cannot parse request from http client");
                complete(asio::error::invalid_argument, bytes_transferred);
            }
            else
            {
                chunk_size_ = 0;
                reading_body_ = true;

                timeout_timer_.stop();
                setup_connection(error);
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
                        {
                            const char* end = begin + size;
                            request_.append_body(begin, end);
                        }

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
                            LOGDBG("cannot parse request from http client");
                            complete(asio::error::invalid_argument, bytes_transferred);
                        }
                        else if(result)
                        {
                            chunk_size_ = chunked_parser_.get_chunk_size();

                            if(0 == chunk_size_)
                            {
                                LOGDBG("request from http client has been read successfully");
                                complete(asio::error_code(), bytes_transferred);
                            }
                        }
                    }
                }

                if(boost::indeterminate(result))
                {
                    if(error == asio::error::eof)
                    {
                        LOGDBG("client unexpectedly closed connection: %1%") << error.message();
                        complete(error, bytes_transferred);
                    }
                    else
                    {
                        LOGDBG("reading data from http client");
                        connection_->read(buffer_, bind_to_read_handler());
                    }
                }
            }
            else
            {
                if(begin < end)
                    request_.append_body(begin, end);

                if(is_reading_completed(error))
                {
                    LOGDBG("request from http client has been read successfully");
                    complete(asio::error_code(), bytes_transferred);
                }
                else if(error == asio::error::eof)
                {
                    LOGDBG("client unexpectedly closed connection: %1%") << error.message();
                    complete(error, bytes_transferred);
                }
                else
                {
                    LOGDBG("reading data from http client");
                    connection_->read(buffer_, bind_to_read_handler());
                }
            }
        }
    }
    else
    {
        LOGDBG("cannot read request from http client: %1%") << error.message();
        complete(error, bytes_transferred);
    }
}

void connection::handle_write(const asio::error_code& error, std::size_t bytes_transferred)
{
    LOGDBG("%1% bytes of data has been written to http client") << bytes_transferred;

    if(!error)
    {
        if(!writing_response_)
            write_buffers_.pop();

        writing_response_ = false;

        if(write_buffers_.empty())
        {
            if(closing_connection_)
                close_connection(true);
        }
        else
        {
            write_front_connection_buffer();
        }

        complete(asio::error_code(), bytes_transferred);
    }
    else
    {
        LOGDBG("cannot write data to http client: %1%") << error.message();
        complete(error, bytes_transferred);
    }
}

}   // namespace server
}   // namespace http
