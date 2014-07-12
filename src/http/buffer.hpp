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
#ifndef _http_buffer_hpp
#define _http_buffer_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/asio.hpp>

// STL headers
#include <memory>
#include <string>


namespace http
{

class buffer
{
public:
    buffer();

    explicit buffer(std::size_t size);
    explicit buffer(char const* data);
    explicit buffer(std::string const& data);
    buffer(char const* data, std::size_t size);

    buffer(buffer const&) = default;
    buffer& operator=(buffer const&) = default;

    char const* data() const;

    std::size_t size() const;
    bool empty() const;

public:
    // implement MutableBufferSequence requirements

    typedef boost::asio::mutable_buffer value_type;
    typedef value_type const* const_iterator;

    const_iterator begin() const;
    const_iterator end() const;

    operator boost::asio::const_buffer() const;
    operator boost::asio::mutable_buffer() const;

private:
    typedef std::shared_ptr<char> data_type;

private:
    std::size_t size_;
    data_type data_;
    value_type buffer_;
};

}   // namespace http

#endif  // #endif _http_buffer_hpp
