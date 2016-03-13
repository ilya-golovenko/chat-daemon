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
#ifndef _chat_filter_host_hpp
#define _chat_filter_host_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio.hpp>

// STL headers
#include <cstddef>
#include <chrono>


namespace chat
{

class filter_host
{
public:
    explicit filter_host(asio::ip::address const& address);

    filter_host(filter_host const&) = delete;
    filter_host& operator=(filter_host const&) = delete;

    void add_connection();

    void block(std::chrono::seconds const& duration);

    asio::ip::address const& get_address() const;

    std::chrono::seconds get_block_duration() const;

    std::size_t get_connections_per_minute() const;
    std::size_t get_connection_count() const;

    bool is_tracking_expired() const;
    bool is_block_expired() const;

private:
    typedef std::chrono::steady_clock clock;
    typedef clock::time_point clock_time_point;

private:
    clock_time_point first_conn_time_;
    clock_time_point last_conn_time_;
    clock_time_point block_end_time_;
    double connections_per_minute_;
    std::size_t connection_count_;
    asio::ip::address address_;
};

}   // namespace chat

#endif  // _chat_filter_host_hpp
