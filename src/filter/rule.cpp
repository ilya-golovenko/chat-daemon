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
#include <filter/rule.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{
namespace filter
{

rule::rule(std::string const& name,
           address const& address,
           std::size_t max_connection_count,
           std::size_t connections_per_minute,
           std::chrono::seconds block_duration) :
    name_(name),
    address_(address),
    max_connection_count_(max_connection_count),
    connections_per_minute_(connections_per_minute),
    block_duration_(block_duration)
{
}

std::string const& rule::get_name() const
{
    return name_;
}

std::chrono::seconds rule::get_block_duration() const
{
    return block_duration_;
}

bool rule::satisfies(host const& host, std::size_t connection_count) const
{
    LOG_COMP_TRACE_FUNCTION(filter::rule);

    asio::ip::address const& address = host.get_address();

    if(address_.satisfies(address))
    {
        if(connection_count >= max_connection_count_)
        {
            LOG_COMP_WARNING(filter::rule, "too many simultaneous connections from host: ", address);
            return true;
        }

        if(host.get_connection_count() >= connections_per_minute_ &&
           host.get_connections_per_minute() >= connections_per_minute_)
        {
            LOG_COMP_WARNING(filter::rule, "too many connections per minute from host: ", address);
            return true;
        }
    }

    return false;
}

}   // namespace filter
}   // namespace chat
