//---------------------------------------------------------------------------
//
//    Copyright (C) 2008 - 2016 Ilya Golovenko
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
#include <filter/host.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{
namespace filter
{

host::host(asio::ip::address const& address) :
    first_conn_time_(clock::now()),
    connections_per_minute_(0),
    connection_count_(1u),
    address_(address)
{
}

void host::add_connection()
{
    LOG_COMP_TRACE_FUNCTION(filter::host);

    ++connection_count_;

    last_conn_time_ = clock::now();

    if(last_conn_time_ > first_conn_time_)
    {
        std::chrono::duration<double, std::chrono::minutes::period> interval(last_conn_time_ - first_conn_time_);
        connections_per_minute_ = 0.1 * connection_count_ / interval.count() + 0.9 * connections_per_minute_;
    }
}

void host::block(std::chrono::seconds duration)
{
    LOG_COMP_TRACE_FUNCTION(filter::host);

    block_end_time_ = clock::now() + duration;
}

asio::ip::address const& host::get_address() const
{
    return address_;
}

std::chrono::seconds host::get_block_duration() const
{
    return std::chrono::duration_cast<std::chrono::seconds>(clock::now() - block_end_time_);
}

std::size_t host::get_connections_per_minute() const
{
    return static_cast<std::size_t>(connections_per_minute_);
}

std::size_t host::get_connection_count() const
{
    return connection_count_;
}

bool host::is_tracking_expired() const
{
    return clock::now() > last_conn_time_ + std::chrono::hours(6);
}

bool host::is_block_expired() const
{
    return clock::now() >= block_end_time_;
}

}   // namespace filter
}   // namespace chat
