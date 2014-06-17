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
#ifndef _filter_host_hpp
#define _filter_host_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <cstddef>
#include <ctime>


class filter_host;
typedef boost::shared_ptr<filter_host> filter_host_ptr;

class filter_host :
    private boost::noncopyable
{
public:
    static filter_host_ptr create(const asio::ip::address& address);

public:
    filter_host(const asio::ip::address& address);

    const asio::ip::address& address() const;
    boost::uint64_t last_conn_time() const;

    void add_connection();
    void block(std::time_t duration);

    std::size_t conn_per_minute() const;
    std::size_t connection_count() const;

    bool is_block_expired() const;
    std::time_t get_block_duration() const;

private:
    asio::ip::address address_;
    std::time_t block_end_time_;
    boost::uint64_t first_conn_time_;
    boost::uint64_t last_conn_time_;
    std::size_t connection_count_;
    std::size_t conn_per_minute_;  // 1 corresponds to 0.001 conn/min
};

#endif  // _filter_host_hpp
