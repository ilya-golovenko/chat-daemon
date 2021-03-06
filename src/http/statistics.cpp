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
#include <http/statistics.hpp>


namespace http
{

std::atomic<std::uint64_t> statistics::bytes_read_ = ATOMIC_VAR_INIT(0ULL);
std::atomic<std::uint64_t> statistics::bytes_written_ = ATOMIC_VAR_INIT(0ULL);

std::uint64_t statistics::get_bytes_read()
{
    return bytes_read_.load(std::memory_order_relaxed);
}

std::uint64_t statistics::get_bytes_written()
{
    return bytes_written_.load(std::memory_order_relaxed);
}

void statistics::add_bytes_read(std::uint64_t count)
{
    bytes_read_.fetch_add(count, std::memory_order_relaxed);
}

void statistics::add_bytes_written(std::uint64_t count)
{
    bytes_written_.fetch_add(count, std::memory_order_relaxed);
}

}   // namespace http
