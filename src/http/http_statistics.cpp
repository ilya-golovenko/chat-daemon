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
#include "http_statistics.hpp"


namespace http
{

volatile std::size_t statistics::bytes_read_ = 0;
volatile std::size_t statistics::bytes_written_ = 0;

std::size_t statistics::get_bytes_read()
{
    return bytes_read_;
}

std::size_t statistics::get_bytes_written()
{
    return bytes_written_;
}

void statistics::add_bytes_read(std::size_t count)
{
    bytes_read_ += count;
}

void statistics::add_bytes_written(std::size_t count)
{
    bytes_written_ += count;
}

}   // namespace http