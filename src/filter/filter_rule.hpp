//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2014 Ilya Golovenko
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
#ifndef _chat_filter_rule_hpp
#define _chat_filter_rule_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <filter/filter_host.hpp>

// BOOST headers
#include <boost/asio.hpp>

// STL headers
#include <cstdint>
#include <string>
#include <ctime>


namespace chat
{

class filter_rule
{
public:
    filter_rule(std::string const& name,
                boost::asio::ip::address const& address,
                boost::asio::ip::address const& netmask,
                std::chrono::seconds const& block_duration,
                std::size_t connections_per_minute,
                std::size_t max_connection_count);

    filter_rule(filter_rule const&) = default;
    filter_rule& operator=(filter_rule const&) = default;

    std::string const& get_name() const;

    std::chrono::seconds const& get_block_duration() const;

    bool satisfies(filter_host const& host, std::size_t connection_count) const;

private:
    bool satisfies(boost::asio::ip::address const& address) const;
    bool satisfies(boost::asio::ip::address_v4 const& address) const;
    bool satisfies(boost::asio::ip::address_v6 const& address) const;

private:
    std::string name_;
    boost::asio::ip::address address_;
    boost::asio::ip::address netmask_;
    std::chrono::seconds block_duration_;
    std::size_t connections_per_minute_;
    std::size_t max_connection_count_;

private:
    std::uint32_t address_v4_;
    std::uint32_t netmask_v4_;
};

}   // namespace chat

#endif  // _chat_filter_rule_hpp
