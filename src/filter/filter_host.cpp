//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009 Ilya Golovenko
//    This file is part of spdaemon.
//
//    spdaemon is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spdaemon is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spdaemon. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include "filter_host.hpp"
#include <utilities.hpp>

// BOOST headers
#include <boost/make_shared.hpp>


filter_host_ptr filter_host::create(const asio::ip::address& address)
{
    return boost::make_shared<filter_host>(address);
}

const asio::ip::address& filter_host::address() const
{
    return address_;
}

boost::uint64_t filter_host::last_conn_time() const
{
    return last_conn_time_;
}

void filter_host::add_connection()
{
    last_conn_time_ = util::get_milli_time();

    if(0ULL == first_conn_time_)
        first_conn_time_= last_conn_time_;

    ++connection_count_;

    if(last_conn_time_ > first_conn_time_)
    {
        boost::int64_t interval = last_conn_time_ - first_conn_time_;
        std::size_t value = 6000000ULL * connection_count_ / interval;
        conn_per_minute_ = (100 * value + 900 * conn_per_minute_) / 1000;
    }
}

void filter_host::block(std::time_t duration)
{
    block_end_time_ = std::time(0) + duration;
}

std::size_t filter_host::conn_per_minute() const
{
    return conn_per_minute_;
}

std::size_t filter_host::connection_count() const
{
    return connection_count_;
}

bool filter_host::is_block_expired() const
{
    return (get_block_duration() <= 0);
}

std::time_t filter_host::get_block_duration() const
{
    return (block_end_time_ - std::time(0));
}

filter_host::filter_host(const asio::ip::address& address) :
    address_(address),
    block_end_time_(0),
    first_conn_time_(0ULL),
    last_conn_time_(0ULL),
    connection_count_(0),
    conn_per_minute_(0)
{
}
