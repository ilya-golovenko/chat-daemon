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
#include "http_message.hpp"
#include "http_common.hpp"

// BOOST headers
#include <boost/lexical_cast.hpp>


namespace http
{

message::message()
{
}

message::message(const version& version) :
    version_(version)
{
}

message::~message()
{
}

message::message(const message& other)
{
    assign(other);
}

message& message::operator=(const message& other)
{
    assign(other);
    return *this;
}

void message::assign(const message& other)
{
    if(&other != this)
    {
        version_ = other.version_;
        headers_ = other.headers_;
        body_data_ = other.body_data_;
    }
}

void message::add(const std::string& name, const std::string& value)
{
    headers_.insert(std::make_pair(name, value));
}

void message::set(const std::string& name, const std::string& value)
{
    erase(name);
    add(name, value);
}

boost::optional<std::string> message::get(const std::string& name) const
{
    header_map_type::const_iterator it = headers_.find(name);

    if(it != headers_.end())
        return boost::make_optional(it->second);

    return boost::optional<std::string>();
}

void message::erase(const std::string& name)
{
    headers_.erase(name);
}

void message::clear()
{
    headers_.clear();
}

const version& message::get_version() const
{
    return version_;
}

void message::set_version(const version& version)
{
    version_ = version;
}

void message::set_version(std::size_t major, std::size_t minor)
{
    version_ = version(major, minor);
}

void message::set_body_content_length()
{
    set_content_length(body_size());
}

void message::set_content_length(std::size_t length)
{
    set(header_names::content_length, length);
}

boost::optional<std::size_t> message::get_content_length() const
{
    boost::optional<std::string> value = get(header_names::content_length);

    if(value)
        return boost::make_optional(boost::lexical_cast<std::size_t>(*value));

    return boost::optional<std::size_t>();
}

void message::set_content_type(const std::string& mime_type)
{
    set(header_names::content_type, mime_type);
}

boost::optional<std::string> message::get_content_type() const
{
    return get(header_names::content_type);
}

void message::set_transfer_encoding(const std::string& encoding)
{
    set(header_names::transfer_encoding, encoding);
}

boost::optional<std::string> message::get_transfer_encoding() const
{
    return get(header_names::transfer_encoding);
}

bool message::is_keep_alive() const
{
    boost::optional<std::string> value = get(header_names::content_length);

    if(!value)
    {
        value = get(header_names::transfer_encoding);

        if(value)
        {
            if(!boost::iequals(*value, encoding_tokens::chunked))
                value = boost::optional<std::string>();
        }
    }

    if(value)
    {
        value = get(header_names::proxy_connection);

        if(!value)
            value = get(header_names::connection);

        if(value)
            return boost::iequals(*value, connection_tokens::keep_alive);

        return (http::version::_1_1 == version_);
    }

    return false;
}

void message::set_keep_alive(bool keep_alive)
{
    if(keep_alive)
        set(header_names::connection, connection_tokens::keep_alive);
    else
        set(header_names::connection, connection_tokens::close);
}

bool message::is_body_empty() const
{
    return body_data_.empty();
}

std::size_t message::body_size() const
{
    return body_data_.size();
}

const std::string& message::body() const
{
    return body_data_;
}

void message::assign_body(const std::string& data)
{
    body_data_.assign(data);
}

void message::append_body(const std::string& data)
{
    body_data_.append(data);
}

void message::clear_body()
{
    body_data_.clear();
}

void message::to_buffers(buffers_type& buffers) const
{
    header_map_type::const_iterator begin = headers_.begin();
    header_map_type::const_iterator end = headers_.end();

    for(header_map_type::const_iterator it = begin; it != end; ++it)
    {
        buffers.push_back(asio::buffer(it->first));
        buffers.push_back(asio::buffer(strings::separator));
        buffers.push_back(asio::buffer(it->second));
        buffers.push_back(asio::buffer(strings::crlf));
    }

    buffers.push_back(asio::buffer(strings::crlf));
    buffers.push_back(asio::buffer(body_data_));
}

void message::dump(std::ostream& os) const
{
    header_map_type::const_iterator begin = headers_.begin();
    header_map_type::const_iterator end = headers_.end();

    for(header_map_type::const_iterator it = begin; it != end; ++it)
    {
        os << it->first;
        os <<  strings::separator;
        os << it->second;
        os << std::endl;
    }

    os << std::endl;
    os << body_data_;
}

}   // namespace http
