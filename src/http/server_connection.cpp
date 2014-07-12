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
#include <http/server_connection.hpp>
#include <http/statistics.hpp>
#include <http/utilities.hpp>
#include <http/mime_types.hpp>
#include <http/common.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// BOOST headers
#include <boost/logic/tribool.hpp>

// STL headers
#include <functional>
#include <chrono>
#include <tuple>
#include <array>


namespace http
{

server_connection::pointer server_connection::create(tcp_connection::pointer tcp_connection)
{
    return std::make_shared<server_connection>(tcp_connection);
}

server_connection::server_connection(tcp_connection::pointer tcp_connection) :
    reading_body_(false),
    writing_response_(false),
    closing_connection_(false),
    chunked_encoding_(false),
    chunk_size_(0u),
    buffer_(4096u),
    tcp_connection_(tcp_connection),
    timeout_timer_(tcp_connection->get_io_service())
{
    set_remote_endpoint(tcp_connection->get_remote_endpoint());
}

server_connection::~server_connection()
{
    close_connection(true);
}

request const& server_connection::get_request() const
{
    return request_;
}


tcp_connection::pointer server_connection::get_tcp_connection() const
{
    return tcp_connection_;
}

bool server_connection::has_custom_remote_endpoint() const
{
    return tcp_connection_->is_open() && remote_endpoint_ != tcp_connection_->get_remote_endpoint();
}

boost::asio::ip::tcp::endpoint const& server_connection::get_remote_endpoint() const
{
    return remote_endpoint_;
}

void server_connection::set_remote_endpoint(boost::asio::ip::tcp::endpoint const& endpoint)
{
    remote_endpoint_ = endpoint;
}

std::uint16_t server_connection::get_remote_port() const
{
    return get_remote_endpoint().port();
}

boost::asio::ip::address server_connection::get_remote_address() const
{
    return get_remote_endpoint().address();
}

void server_connection::read_request(completion_handler const& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    LOG_COMP_DEBUG(server_connection, "reading request from http client");

    request_.clear();
    request_.clear_body();

    chunk_size_ = 0;
    reading_body_ = false;
    chunked_encoding_ = false;

    handlers_.push(handler);

    tcp_connection_->read(buffer_, bind_to_read_handler());
}

void server_connection::read_request(completion_handler const& handler, std::chrono::seconds const& timeout)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    read_request(handler);

    if(timeout > std::chrono::seconds::zero())
    {
        timeout_timer_.expires_from_now(timeout);
        timeout_timer_.async_wait(bind_to_stop_handler());
    }
}

void server_connection::write_stock_response(status const& status, std::string const& server, completion_handler const& handler)
{
    write_response(response::get_stock_response(status, server), handler);
}

void server_connection::write_response(response const& response, completion_handler const& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(is_open() && !closing_connection_)
    {
        response_ = response;
        writing_response_ = true;

        handlers_.push(handler);

        message::buffers buffers;
        response_.to_buffers(buffers);

        LOG_COMP_DEBUG(server_connection, "http response status: ", response_.get_status());

        LOG_COMP_DEBUG(server_connection, "writing response to http client");
        tcp_connection_->write(buffers, bind_to_write_handler());
    }
}

void server_connection::write_buffer(buffer const& buffer, completion_handler const& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(is_open() && !closing_connection_)
    {
        bool idle = write_buffers_.empty();

        write_buffers_.push(buffer);
        handlers_.push(handler);

        if(idle && !writing_response_)
            write_front_connection_buffer();
    }
}

void server_connection::close(status const& status, std::string const& server, completion_handler const& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    write_stock_response(status, server, handler);
    close_connection(false);
}

void server_connection::close(response const& response, completion_handler const& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    write_response(response, handler);
    close_connection(false);
}

void server_connection::close(buffer const& buffer, completion_handler const& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    write_buffer(buffer, handler);
    close_connection(false);
}

void server_connection::close(bool force)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    close_connection(force);
}

bool server_connection::is_open() const
{
    return tcp_connection_->is_open();
}

stop_handler server_connection::bind_to_stop_handler()
{
    return std::bind(&server_connection::handle_stop, this, std::placeholders::_1);
}

read_handler server_connection::bind_to_read_handler()
{
    return std::bind(&server_connection::handle_read, this, std::placeholders::_1, std::placeholders::_2);
}

write_handler server_connection::bind_to_write_handler()
{
    return std::bind(&server_connection::handle_write, this, std::placeholders::_1, std::placeholders::_2);
}

void server_connection::close_connection(bool force)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(tcp_connection_->is_open())
    {
        if(force || (!writing_response_ && write_buffers_.empty()))
        {
            LOG_COMP_DEBUG(server_connection, "closing connection to http client");
            timeout_timer_.cancel();
            tcp_connection_->close();
        }
        else if(!closing_connection_)
        {
            LOG_COMP_DEBUG(server_connection, "request to close connection");
            closing_connection_ = true;
        }
    }
}

void server_connection::write_front_connection_buffer()
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    std::array<boost::asio::const_buffer, 2> buffers =
    {{
        write_buffers_.front(),
        boost::asio::buffer(strings::crlf)
    }};

    LOG_COMP_TRACE(server_connection, "writing data buffer to http client");
    tcp_connection_->write(buffers, bind_to_write_handler());
}

void server_connection::setup_connection(boost::system::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    boost::optional<std::string> const encoding = request_.get_transfer_encoding();
    chunked_encoding_ = encoding && boost::algorithm::iequals(*encoding, encoding_tokens::chunked);
}

bool server_connection::is_reading_completed(boost::system::error_code const& error) const
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(request_.get_method() == request_methods::post)
    {
        boost::optional<std::size_t> const length = request_.get_content_length();
        return length ? *length == request_.get_body_size() : error == boost::asio::error::eof;
    }

    return true;
}

void server_connection::call_completion_handler(boost::system::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(error)
        close_connection(true);

    try
    {
        handlers_.front()(error);
    }
    catch(std::exception const& e)
    {
        LOG_COMP_WARNING(server_connection, e);
    }

    handlers_.pop();
}

void server_connection::handle_stop(boost::system::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(!error)
    {
        LOG_COMP_DEBUG(server_connection, "aborting connection because of timeout");
        close_connection(true);
    }
}

void server_connection::handle_read(boost::system::error_code const& error, std::size_t bytes_transferred)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    LOG_COMP_TRACE(server_connection, bytes_transferred, " bytes of data has been read from http client");

    statistics::add_bytes_read(bytes_transferred);

    if(!error || error == boost::asio::error::eof)
    {
        boost::tribool result = true;
        const char* begin = buffer_.data();
        const char* end = begin + bytes_transferred;

        if(!reading_body_)
        {
            std::tie(result, begin) = request_parser_.parse(request_, begin, end);

            if(boost::indeterminate(result))
            {
                if(error == boost::asio::error::eof)
                {
                    LOG_COMP_DEBUG(server_connection, "client unexpectedly closed connection: ", error);
                    call_completion_handler(error);
                }
                else
                {
                    LOG_COMP_DEBUG(server_connection, "reading request from http client");
                    tcp_connection_->read(buffer_, bind_to_read_handler());
                }
            }
            else if(!result)
            {
                LOG_COMP_DEBUG(server_connection, "cannot parse request from http client");
                call_completion_handler(boost::asio::error::invalid_argument);
            }
            else
            {
                chunk_size_ = 0;
                reading_body_ = true;

                timeout_timer_.cancel();
                setup_connection(error);
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
                        std::tie(result, begin) = chunked_parser_.parse(begin, end);

                        if(!result)
                        {
                            LOG_COMP_DEBUG(server_connection, "cannot parse request from http client");
                            call_completion_handler(boost::asio::error::invalid_argument);
                        }
                        else if(result)
                        {
                            chunk_size_ = chunked_parser_.get_chunk_size();

                            if(0 == chunk_size_)
                            {
                                LOG_COMP_DEBUG(server_connection, "request from http client has been read successfully");
                                call_completion_handler(boost::system::error_code());
                            }
                        }
                    }
                }

                if(boost::indeterminate(result))
                {
                    if(error == boost::asio::error::eof)
                    {
                        LOG_COMP_DEBUG(server_connection, "client unexpectedly closed connection: ", error);
                        call_completion_handler(error);
                    }
                    else
                    {
                        LOG_COMP_TRACE(server_connection, "reading data from http client");
                        tcp_connection_->read(buffer_, bind_to_read_handler());
                    }
                }
            }
            else
            {
                if(begin < end)
                    request_.append_body(begin, end);

                if(is_reading_completed(error))
                {
                    LOG_COMP_DEBUG(server_connection, "request from http client has been read successfully");
                    call_completion_handler(boost::system::error_code());
                }
                else if(error == boost::asio::error::eof)
                {
                    LOG_COMP_DEBUG(server_connection, "client unexpectedly closed connection: ", error);
                    call_completion_handler(error);
                }
                else
                {
                    LOG_COMP_TRACE(server_connection, "reading data from http client");
                    tcp_connection_->read(buffer_, bind_to_read_handler());
                }
            }
        }
    }
    else
    {
        LOG_COMP_DEBUG(server_connection, "cannot read request from http client: ", error);
        call_completion_handler(error);
    }
}

void server_connection::handle_write(boost::system::error_code const& error, std::size_t bytes_transferred)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    LOG_COMP_TRACE(server_connection, bytes_transferred, " bytes of data has been written to http client");

    statistics::add_bytes_read(bytes_transferred);

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

        call_completion_handler(boost::system::error_code());
    }
    else
    {
        LOG_COMP_DEBUG(server_connection, "cannot write data to http client: ", error);
        call_completion_handler(error);
    }
}

}   // namespace http
