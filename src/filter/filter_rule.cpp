//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2015 Ilya Golovenko
//    This file is part of Chat.Daemon project
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
#include <filter/filter_rule.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{

filter_rule::filter_rule(std::string const& name,
                         filter_address const& address,
                         std::size_t max_connection_count,
                         std::size_t connections_per_minute,
                         std::chrono::seconds const& block_duration) :
    name_(name),
    address_(address),
    max_connection_count_(max_connection_count),
    connections_per_minute_(connections_per_minute),
    block_duration_(block_duration)
{
}

std::string const& filter_rule::get_name() const
{
    return name_;
}

std::chrono::seconds const& filter_rule::get_block_duration() const
{
    return block_duration_;
}

bool filter_rule::satisfies(filter_host const& host, std::size_t connection_count) const
{
    LOG_COMP_TRACE_FUNCTION(filter_rule);

    asio::ip::address const& address = host.get_address();

    if(address_.satisfies(address))
    {
        if(connection_count >= max_connection_count_)
        {
            LOG_COMP_WARNING(filter_rule, "too many simultaneous connections from host: ", address);
            return true;
        }

        if(host.get_connection_count() >= connections_per_minute_ &&
           host.get_connections_per_minute() >= connections_per_minute_)
        {
            LOG_COMP_WARNING(filter_rule, "too many connections per minute from host: ", address);
            return true;
        }
    }

    return false;
}

}   // namespace chat
