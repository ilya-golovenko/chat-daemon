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
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/shared_array.hpp>

// STL headers
#include <string>


namespace http
{

class buffer
{
public:
    buffer();
    explicit buffer(std::size_t size);
    explicit buffer(const char* data);
    explicit buffer(const std::string& data);
    buffer(const char* data, std::size_t size);

    buffer(const buffer& other);
    buffer& operator=(const buffer& other);

    std::size_t size() const;
    const char* data() const;

public:
    // implement MutableBufferSequence requirements

    typedef asio::mutable_buffer value_type;
    typedef const value_type* const_iterator;

    const value_type* begin() const;
    const value_type* end() const;

    operator asio::const_buffer() const;
    operator asio::mutable_buffer() const;

private:
    typedef boost::shared_array<char> data_type;

private:
    std::size_t size_;
    data_type data_;
    value_type buffer_;
};

}   // namespace http

#endif  // #endif _http_buffer_hpp
