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
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "http_common.hpp"
#include "http_version.hpp"
#include "http_utilities.hpp"

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

// STL headers
#include <string>
#include <map>


namespace http
{

class message
{
public:
    typedef std::vector<asio::const_buffer> buffers_type;

public:
    message();
    message(const version& version);

    virtual ~message();

    message(const message& other);
    message& operator=(const message& other);

    void assign(const message& other);

    template <typename T>
    inline void add(const std::string& name, const T& value);
    void add(const std::string& name, const std::string& value);

    template <typename T>
    inline void set(const std::string& name, const T& value);
    void set(const std::string& name, const std::string& value);

    template <typename T>
    inline boost::optional<T> get(const std::string& name) const;
    boost::optional<std::string> get(const std::string& name) const;

    void erase(const std::string& name);
    void clear();

    const version& get_version() const;
    void set_version(const version& version);
    void set_version(std::size_t major, std::size_t minor);

    void set_body_content_length();

    void set_content_length(std::size_t length);
    boost::optional<std::size_t> get_content_length() const;

    void set_content_type(const std::string& mime_type);
    boost::optional<std::string> get_content_type() const;

    void set_transfer_encoding(const std::string& encoding);
    boost::optional<std::string> get_transfer_encoding() const;

    bool is_keep_alive() const;
    void set_keep_alive(bool keep_alive);

    bool is_body_empty() const;
    std::size_t body_size() const;
    const std::string& body() const;

    void assign_body(const std::string& data);
    void append_body(const std::string& data);

    template <typename InpIt>
    inline void assign_body(InpIt first, InpIt last);

    template <typename InpIt>
    inline void append_body(InpIt first, InpIt last);

    void clear_body();

    void to_buffers(buffers_type& buffers) const;
    void dump(std::ostream& os) const;

protected:
    typedef std::map<std::string, std::string,
        http_util::string_iless> header_map_type;

protected:
    http::version version_;
    header_map_type headers_;
    std::string body_data_;
};

template <typename T>
inline void message::add(const std::string& name, const T& value)
{
    add(name, boost::lexical_cast<std::string>(value));
}

template <typename T>
inline void message::set(const std::string& name, const T& value)
{
    set(name, boost::lexical_cast<std::string>(value));
}

template <typename T>
inline boost::optional<T> message::get(const std::string& name) const
{
    boost::optional<std::string> value = get(name);

    if(value)
        return boost::make_optional(boost::lexical_cast<T>(*value));

    return boost::optional<T>();
}

template <typename InpIt>
inline void message::assign_body(InpIt first, InpIt last)
{
    body_data_.assign(first, last);
}

template <typename InpIt>
inline void message::append_body(InpIt first, InpIt last)
{
    body_data_.append(first, last);
}

}   // namespace http

#endif  // _http_message_hpp
