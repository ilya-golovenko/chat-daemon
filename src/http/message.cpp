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
#include <http/message.hpp>
#include <http/common.hpp>


namespace http
{

message::message(version const& version) :
    version_(version)
{
}

message::~message()
{
}

void message::add(std::string const& name, std::string const& value)
{
    headers_.emplace(name, value);
}

void message::set(std::string const& name, std::string const& value)
{
    erase(name);
    add(name, value);
}

boost::optional<std::string> message::get(std::string const& name) const
{
    header_map::const_iterator it = headers_.find(name);

    if(it != headers_.end())
    {
        return boost::make_optional(it->second);
    }

    return boost::optional<std::string>();
}

void message::erase(std::string const& name)
{
    headers_.erase(name);
}

void message::clear()
{
    headers_.clear();
}

version const& message::get_version() const
{
    return version_;
}

void message::set_version(version const& version)
{
    version_ = version;
}

void message::set_version(std::uint8_t major, std::uint8_t minor)
{
    version_ = version(major, minor);
}

void message::set_body_content_length()
{
    set_content_length(get_body_size());
}

void message::set_content_length(std::size_t length)
{
    set(header_names::content_length, length);
}

boost::optional<std::size_t> message::get_content_length() const
{
    boost::optional<std::string> value = get(header_names::content_length);

    if(value)
    {
        return boost::optional<std::size_t>(std::stoul(*value));
    }

    return boost::optional<std::size_t>();
}

void message::set_content_type(std::string const& mime_type)
{
    set(header_names::content_type, mime_type);
}

boost::optional<std::string> message::get_content_type() const
{
    return get(header_names::content_type);
}

void message::set_transfer_encoding(std::string const& encoding)
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
            if(!boost::algorithm::iequals(*value, encoding_tokens::chunked))
            {
                value = boost::optional<std::string>();
            }
        }
    }

    if(value)
    {
        value = get(header_names::proxy_connection);

        if(!value)
        {
            value = get(header_names::connection);
        }

        if(value)
        {
            return boost::iequals(*value, connection_tokens::keep_alive);
        }

        return version::http_1_1 == version_;
    }

    return false;
}

void message::set_keep_alive(bool keep_alive)
{
    set(header_names::connection, keep_alive ? connection_tokens::keep_alive : connection_tokens::close);
}

bool message::is_body_empty() const
{
    return body_data_.empty();
}

std::size_t message::get_body_size() const
{
    return body_data_.size();
}

std::string const& message::get_body() const
{
    return body_data_;
}

void message::assign_body(std::string const& data)
{
    body_data_.assign(data);
}

void message::append_body(std::string const& data)
{
    body_data_.append(data);
}

void message::clear_body()
{
    body_data_.clear();
}

void message::to_buffers(buffers& buffers) const
{
    for(header_map::value_type const& header : headers_)
    {
        buffers.push_back(asio::buffer(header.first));
        buffers.push_back(asio::buffer(strings::separator));
        buffers.push_back(asio::buffer(header.second));
        buffers.push_back(asio::buffer(strings::crlf));
    }

    buffers.push_back(asio::buffer(strings::crlf));
    buffers.push_back(asio::buffer(body_data_));
}

void message::dump(std::ostream& os) const
{
    for(header_map::value_type const& header : headers_)
    {
        os << header.first;
        os << strings::separator;
        os << header.second;
        os << std::endl;
    }

    os << std::endl;
    os << body_data_;
}

}   // namespace http
