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
#include "http_buffer.hpp"
#include "http_common.hpp"

// BOOST headers
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>


namespace http
{

buffer::buffer() :
    size_(0)
{
}

buffer::buffer(std::size_t size) :
    size_(size),
    data_(new char[size_]),
    buffer_(data_.get(), size_)
{
}

buffer::buffer(const char* data) :
    size_(strlen(data)),
    data_(new char[size_]),
    buffer_(data_.get(), size_)
{
    std::memmove(data_.get(), data, size_);
}

buffer::buffer(const std::string& data) :
    size_(data.size()),
    data_(new char[size_]),
    buffer_(data_.get(), size_)
{
    std::memmove(data_.get(), data.data(), size_);
}

buffer::buffer(const char* data, std::size_t size) :
    size_(size),
    data_(new char[size_]),
    buffer_(data_.get(), size_)
{
    std::memmove(data_.get(), data, size_);
}

buffer::buffer(const buffer& other) :
    size_(other.size_),
    data_(other.data_),
    buffer_(other.buffer_)
{
}

buffer& buffer::operator=(const buffer& other)
{
    if(&other != this)
    {
        size_ = other.size_;
        data_ = other.data_;
        buffer_ = other.buffer_;
    }

    return *this;
}

std::size_t buffer::size() const
{
    return size_;
}

const char* buffer::data() const
{
    return data_.get();
}

const buffer::value_type* buffer::begin() const
{
    return &buffer_;
}

const buffer::value_type* buffer::end() const
{
    return (begin() + 1);
}

buffer::operator asio::const_buffer() const
{
    return buffer_;
}

buffer::operator asio::mutable_buffer() const
{
    return buffer_;
}

}   // namespace http
