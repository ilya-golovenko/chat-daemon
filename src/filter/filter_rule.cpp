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
#include "filter_rule.hpp"
#include <log_common.hpp>

// BOOST headers
#include <boost/make_shared.hpp>


filter_rule_ptr filter_rule::create(
    const std::string& name, const std::string& address,
    const std::string& netmask, std::time_t block_duration,
    std::size_t conn_per_minute, std::size_t max_connections)
{
    return boost::make_shared<filter_rule>(name, address,
        netmask, block_duration, conn_per_minute, max_connections);
}

const std::string& filter_rule::name() const
{
    return name_;
}

std::time_t filter_rule::block_duration() const
{
    return block_duration_;
}

bool filter_rule::satisfies(filter_host_ptr host, std::size_t connections) const
{
    const asio::ip::address& address = host->address();

    if(address.is_v4() && is_address_matches(address))
    {
        if(connections >= max_connections_)
        {
            LOGWRN("too many simultaneous connections from address: %1%") << address;
            return true;
        }

        if(host->connection_count() >= conn_per_minute_ &&
            host->conn_per_minute() >= conn_per_minute_ * 1000)
        {
            LOGWRN("too many connections per minute from address: %1%") << address;
            return true;
        }
    }

    return false;
}

filter_rule::filter_rule(
    const std::string& name, const std::string& address,
    const std::string& netmask, std::time_t block_duration,
    std::size_t conn_per_minute, std::size_t max_connections) :
    name_(name),
    conn_per_minute_(conn_per_minute),
    max_connections_(max_connections),
    block_duration_(block_duration)
{
    address_ = asio::ip::address_v4::from_string(address).to_ulong();
    netmask_ = asio::ip::address_v4::from_string(netmask).to_ulong();
}

const std::string& filter_rule::get_log_source_name() const
{
    return name_;
}

bool filter_rule::is_address_matches(const asio::ip::address& address) const
{
    return (0 == ((address.to_v4().to_ulong() ^ address_) & netmask_));
}
