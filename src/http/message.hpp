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
#ifndef _http_message_hpp
#define _http_message_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/common.hpp>
#include <http/version.hpp>
#include <http/utilities.hpp>

// BOOST headers
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/asio.hpp>

// STL headers
#include <string>
#include <map>


namespace http
{

class message
{
public:
    typedef std::vector<boost::asio::const_buffer> buffers;

public:
    message() = default;

    explicit message(version const& version);

    virtual ~message();

    message(message const&) = default;
    message& operator=(message const&) = default;

    void add(std::string const& name, std::string const& value);

    template <typename T>
    void add(std::string const& name, T const& value);

    void set(std::string const& name, std::string const& value);

    template <typename T>
    void set(std::string const& name, T const& value);

    boost::optional<std::string> get(std::string const& name) const;

    template <typename T>
    boost::optional<T> get(std::string const& name) const;

    void erase(std::string const& name);
    void clear();

    version const& get_version() const;
    void set_version(version const& version);
    void set_version(std::uint8_t major, std::uint8_t minor);

    void set_body_content_length();

    void set_content_length(std::size_t length);
    boost::optional<std::size_t> get_content_length() const;

    void set_content_type(std::string const& mime_type);
    boost::optional<std::string> get_content_type() const;

    void set_transfer_encoding(std::string const& encoding);
    boost::optional<std::string> get_transfer_encoding() const;

    bool is_keep_alive() const;
    void set_keep_alive(bool keep_alive);

    bool is_body_empty() const;
    std::size_t get_body_size() const;
    std::string const& get_body() const;

    void assign_body(std::string const& data);
    void append_body(std::string const& data);

    template <typename Iterator>
    void assign_body(Iterator first, Iterator last);

    template <typename Iterator>
    void append_body(Iterator first, Iterator last);

    void clear_body();

    void to_buffers(buffers& buffers) const;
    void dump(std::ostream& os) const;

protected:
    typedef std::map<std::string, std::string, string_iless> header_map;

protected:
    version version_;
    header_map headers_;
    std::string body_data_;
};

template <typename T>
void message::add(std::string const& name, T const& value)
{
    add(name, boost::lexical_cast<std::string>(value));
}

template <typename T>
void message::set(std::string const& name, T const& value)
{
    set(name, boost::lexical_cast<std::string>(value));
}

template <typename T>
boost::optional<T> message::get(std::string const& name) const
{
    boost::optional<std::string> value = get(name);

    if(value)
        return boost::make_optional(boost::lexical_cast<T>(*value));

    return boost::optional<T>();
}

template <typename Iterator>
void message::assign_body(Iterator first, Iterator last)
{
    body_data_.assign(first, last);
}

template <typename Iterator>
void message::append_body(Iterator first, Iterator last)
{
    body_data_.append(first, last);
}

}   // namespace http

#endif  // _http_message_hpp
