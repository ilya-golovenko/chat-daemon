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

// Application headers
#include <http/server_connection.hpp>
#include <http/statistics.hpp>
#include <http/utilities.hpp>
#include <http/common.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// STL headers
#include <functional>
#include <chrono>
#include <tuple>
#include <array>


namespace http
{

server_connection::pointer server_connection::create(tcp::connection&& connection)
{
    return std::make_shared<server_connection>(std::forward<tcp::connection>(connection));
}

server_connection::server_connection(tcp::connection&& connection) :
    reading_content_(false),
    writing_response_(false),
    closing_connection_(false),
    chunked_encoding_(false),
    chunk_size_(0u),
    buffer_(4096u),
    connection_(std::move(connection)),
    timeout_timer_(connection_.get_io_service())
{
    set_remote_endpoint(connection_.get_remote_endpoint());
}

server_connection::~server_connection()
{
    close_connection(true);
}

request const& server_connection::get_request() const
{
    return request_;
}


tcp::connection& server_connection::get_tcp_connection()
{
    return connection_;
}

bool server_connection::has_custom_remote_endpoint() const
{
    return connection_.is_open() && remote_endpoint_ != connection_.get_remote_endpoint();
}

asio::ip::tcp::endpoint const& server_connection::get_remote_endpoint() const
{
    return remote_endpoint_;
}

void server_connection::set_remote_endpoint(asio::ip::tcp::endpoint const& endpoint)
{
    remote_endpoint_ = endpoint;
}

std::uint16_t server_connection::get_remote_port() const
{
    return get_remote_endpoint().port();
}

asio::ip::address server_connection::get_remote_address() const
{
    return get_remote_endpoint().address();
}

void server_connection::read_request(completion_handler&& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    LOG_COMP_DEBUG(server_connection, "reading request from http client");

    request_.clear();
    request_.clear_content();

    chunk_size_ = 0;
    reading_content_ = false;
    chunked_encoding_ = false;

    handlers_.push(std::forward<completion_handler>(handler));

    connection_.read(buffer_, bind_to_read_handler());
}

void server_connection::read_request(completion_handler&& handler, std::chrono::seconds timeout)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    read_request(std::forward<completion_handler>(handler));

    if(timeout > std::chrono::seconds::zero())
    {
        timeout_timer_.expires_from_now(timeout);
        timeout_timer_.async_wait(bind_to_stop_handler());
    }
}

void server_connection::write_stock_response(status const& status, std::string const& server, completion_handler&& handler)
{
    write_response(response::make_stock_response(status, server), std::forward<completion_handler>(handler));
}

void server_connection::write_response(response const& response, completion_handler&& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(is_open() && !closing_connection_)
    {
        response_ = response;
        writing_response_ = true;

        message::buffers buffers;
        response_.to_buffers(buffers);

        handlers_.push(std::forward<completion_handler>(handler));

        LOG_COMP_DEBUG(server_connection, "http response status: ", response_.get_status());

        LOG_COMP_DEBUG(server_connection, "writing response to http client");
        connection_.write(buffers, bind_to_write_handler());
    }
}

void server_connection::write_buffer(buffer const& buffer, completion_handler&& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(is_open() && !closing_connection_)
    {
        bool const idle = write_buffers_.empty();

        write_buffers_.push(buffer);

        handlers_.push(std::forward<completion_handler>(handler));

        if(idle && !writing_response_)
        {
            write_front_connection_buffer();
        }
    }
}

void server_connection::close(status const& status, std::string const& server, completion_handler&& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    write_stock_response(status, server, std::forward<completion_handler>(handler));

    close_connection(false);
}

void server_connection::close(response const& response, completion_handler&& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    write_response(response, std::forward<completion_handler>(handler));

    close_connection(false);
}

void server_connection::close(buffer const& buffer, completion_handler&& handler)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    write_buffer(buffer, std::forward<completion_handler>(handler));

    close_connection(false);
}

void server_connection::close(bool force)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    close_connection(force);
}

bool server_connection::is_open() const
{
    return connection_.is_open();
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

    if(connection_.is_open())
    {
        if(force || (!writing_response_ && write_buffers_.empty()))
        {
            LOG_COMP_DEBUG(server_connection, "closing connection to http client");
            timeout_timer_.cancel();
            connection_.close();
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

    std::array<asio::const_buffer, 2> buffers =
    {{
        write_buffers_.front(),
        asio::buffer(strings::crlf)
    }};

    LOG_COMP_TRACE(server_connection, "writing data buffer to http client");
    connection_.write(buffers, bind_to_write_handler());
}

void server_connection::setup_connection(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    boost::optional<std::string> const encoding = request_.get_transfer_encoding();
    chunked_encoding_ = encoding && boost::algorithm::iequals(*encoding, encoding_tokens::chunked);
}

bool server_connection::is_reading_completed(asio::error_code const& error) const
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(request_.get_method() == request_methods::post)
    {
        boost::optional<std::size_t> const length = request_.get_content_length();
        return length ? *length == request_.content_size() : error == asio::error::eof;
    }

    return true;
}

void server_connection::call_completion_handler(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(error)
    {
        close_connection(true);
    }

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

void server_connection::handle_stop(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    if(!error)
    {
        LOG_COMP_DEBUG(server_connection, "aborting connection because of timeout");
        close_connection(true);
    }
}

void server_connection::handle_read(asio::error_code const& error, std::size_t bytes_transferred)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    LOG_COMP_TRACE(server_connection, bytes_transferred, " bytes of data has been read from http client");

    statistics::add_bytes_read(bytes_transferred);

    if(!error || error == asio::error::eof)
    {
        parse_result result = parse_result::ok;

        char const* begin = buffer_.data();
        char const* end = begin + bytes_transferred;

        if(!reading_content_)
        {
            std::tie(result, begin) = request_parser_.parse(request_, begin, end);

            if(result == parse_result::more)
            {
                if(error == asio::error::eof)
                {
                    LOG_COMP_DEBUG(server_connection, "client unexpectedly closed connection: ", error);
                    call_completion_handler(error);
                }
                else
                {
                    LOG_COMP_DEBUG(server_connection, "reading request from http client");
                    connection_.read(buffer_, bind_to_read_handler());
                }
            }
            else if(result == parse_result::error)
            {
                LOG_COMP_DEBUG(server_connection, "cannot parse request from http client");
                call_completion_handler(asio::error::invalid_argument);
            }
            else
            {
                chunk_size_ = 0;
                reading_content_ = true;

                timeout_timer_.cancel();
                setup_connection(error);
            }
        }

        if(reading_content_)
        {
            if(chunked_encoding_)
            {
                while(result == parse_result::ok && begin != end)
                {
                    if(chunk_size_ > 0)
                    {
                        std::size_t size = end - begin;

                        if(size > chunk_size_)
                        {
                            size = chunk_size_;
                        }

                        if(size > 0)
                        {
                            char const* end = begin + size;
                            request_.add_content(begin, end);
                        }

                        begin += size;
                        chunk_size_ -= size;

                        if(begin == end && chunk_size_ > 0)
                        {
                            result = parse_result::more;
                        }
                    }

                    if(0 == chunk_size_)
                    {
                        std::tie(result, begin) = chunked_parser_.parse(begin, end);

                        if(result == parse_result::error)
                        {
                            LOG_COMP_DEBUG(server_connection, "cannot parse request from http client");
                            call_completion_handler(asio::error::invalid_argument);
                        }
                        else if(result == parse_result::ok)
                        {
                            chunk_size_ = chunked_parser_.get_chunk_size();

                            if(0 == chunk_size_)
                            {
                                LOG_COMP_DEBUG(server_connection, "request from http client has been read successfully");
                                call_completion_handler(asio::error_code());
                            }
                        }
                    }
                }

                if(result == parse_result::more)
                {
                    if(error == asio::error::eof)
                    {
                        LOG_COMP_DEBUG(server_connection, "client unexpectedly closed connection: ", error);
                        call_completion_handler(error);
                    }
                    else
                    {
                        LOG_COMP_TRACE(server_connection, "reading data from http client");
                        connection_.read(buffer_, bind_to_read_handler());
                    }
                }
            }
            else
            {
                if(begin < end)
                {
                    request_.add_content(begin, end);
                }

                if(is_reading_completed(error))
                {
                    LOG_COMP_DEBUG(server_connection, "request from http client has been read successfully");
                    call_completion_handler(asio::error_code());
                }
                else if(error == asio::error::eof)
                {
                    LOG_COMP_DEBUG(server_connection, "client unexpectedly closed connection: ", error);
                    call_completion_handler(error);
                }
                else
                {
                    LOG_COMP_TRACE(server_connection, "reading data from http client");
                    connection_.read(buffer_, bind_to_read_handler());
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

void server_connection::handle_write(asio::error_code const& error, std::size_t bytes_transferred)
{
    LOG_COMP_TRACE_FUNCTION(server_connection);

    LOG_COMP_TRACE(server_connection, bytes_transferred, " bytes of data has been written to http client");

    statistics::add_bytes_read(bytes_transferred);

    if(!error)
    {
        if(!writing_response_)
        {
            write_buffers_.pop();
        }

        writing_response_ = false;

        if(write_buffers_.empty())
        {
            if(closing_connection_)
            {
                close_connection(true);
            }
        }
        else
        {
            write_front_connection_buffer();
        }

        call_completion_handler(asio::error_code());
    }
    else
    {
        LOG_COMP_DEBUG(server_connection, "cannot write data to http client: ", error);
        call_completion_handler(error);
    }
}

}   // namespace http
