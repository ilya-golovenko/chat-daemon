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
#ifndef _filter_rule_hpp
#define _filter_rule_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "filter_host.hpp"
#include <log_source.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// STL headers
#include <string>


class filter_rule;
typedef boost::shared_ptr<filter_rule> filter_rule_ptr;

class filter_rule :
    private boost::noncopyable,
    public log_source<filter_rule>
{
public:
    static filter_rule_ptr create(
        const std::string& name, const std::string& address,
        const std::string& netmask, std::time_t block_duration,
        std::size_t conn_per_minute, std::size_t max_connections);

public:
    filter_rule(
        const std::string& name, const std::string& address,
        const std::string& netmask, std::time_t block_duration,
        std::size_t conn_per_minute, std::size_t max_connections);

    const std::string& name() const;
    std::time_t block_duration() const;

    bool satisfies(filter_host_ptr host, std::size_t connections) const;

public:
    const std::string& get_log_source_name() const;

private:
    bool is_address_matches(const asio::ip::address& address) const;

private:
    std::string name_;
    unsigned long address_;
    unsigned long netmask_;
    std::size_t conn_per_minute_;
    std::size_t max_connections_;
    std::time_t block_duration_;
};

#endif  // _filter_rule_hpp
