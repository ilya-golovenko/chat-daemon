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
#include <http/buffer.hpp>
#include <http/common.hpp>


namespace http
{

buffer::buffer() :
    size_(0)
{
}

buffer::buffer(std::size_t size) :
    size_(size),
    data_(new char[size_], std::default_delete<char[]>()),
    buffer_(data_.get(), size_)
{
}

buffer::buffer(char const* data) :
    size_(std::strlen(data)),
    data_(new char[size_], std::default_delete<char[]>()),
    buffer_(data_.get(), size_)
{
    std::memmove(data_.get(), data, size_);
}

buffer::buffer(std::string const& data) :
    size_(data.size()),
    data_(new char[size_], std::default_delete<char[]>()),
    buffer_(data_.get(), size_)
{
    std::memmove(data_.get(), data.data(), size_);
}

buffer::buffer(char const* data, std::size_t size) :
    size_(size),
    data_(new char[size_], std::default_delete<char[]>()),
    buffer_(data_.get(), size_)
{
    std::memmove(data_.get(), data, size_);
}

char const* buffer::data() const
{
    return data_.get();
}

std::size_t buffer::size() const
{
    return size_;
}

bool buffer::empty() const
{
    return 0u == size_;
}

buffer::const_iterator buffer::begin() const
{
    return &buffer_;
}

buffer::const_iterator buffer::end() const
{
    return std::next(begin());
}

buffer::operator boost::asio::const_buffer() const
{
    return buffer_;
}

buffer::operator boost::asio::mutable_buffer() const
{
    return buffer_;
}

}   // namespace http
