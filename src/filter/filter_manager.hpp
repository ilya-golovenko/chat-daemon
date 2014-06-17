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
#ifndef _filter_manager_hpp
#define _filter_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "filter_host.hpp"
#include "filter_rule.hpp"
#include <http_server_connection.hpp>
#include <chat_forwards.hpp>
#include <config_common.hpp>
#include <async_timer.hpp>
#include <utilities.hpp>
#include <log_common.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

// STL headers
#include <string>
#include <vector>
#include <list>


class filter_manager :
    private boost::noncopyable,
    public log_source<filter_manager>,
    public boost::enable_shared_from_this<filter_manager>
{
DEFINE_LOG_SOURCE_NAME("filter manager")

public:
    filter_manager(chat::server& server);

    void configure();
    void start();
    void stop();

    bool check_blocked(http::server::connection::pointer connection);
    void remove_block(const asio::ip::address& address);

private:
    typedef std::list<filter_host_ptr> host_list_type;

    typedef boost::unordered_map<asio::ip::address,
        host_list_type::iterator, util::address_hash> host_map_type;

    typedef std::vector<filter_rule_ptr> rule_vector_type;

private:
    void start_update_timer();
    void handle_timer(const asio::error_code& error);

    void update_tracked_hosts();
    void update_ignorelist_file(bool force);

    void create_rule(const config_data::filter::rule& rule_config);

    void create_rule(
        const std::string& name, const std::string& address,
        const std::string& netmask, std::time_t block_duration,
        std::size_t conn_per_minute, std::size_t max_connections);

    void add_host_connection(host_map_type::iterator host);
    void block_host(host_map_type::iterator host, filter_rule_ptr rule);

    std::size_t get_connection_count(const asio::ip::address& address);
    rule_vector_type::iterator find_rule(host_map_type::iterator host);

    host_map_type::iterator find_blocked_host(const asio::ip::address& address);
    host_map_type::iterator get_tracked_host(const asio::ip::address& address);

    void deliver_address_blocked_message(const asio::ip::address& address);

private:
    static const std::string time_placeholder;
    static const std::string address_placeholder;

private:
    chat::server& server_;

    bool enabled_;
    std::size_t list_size_;

    host_list_type hosts_;
    rule_vector_type rules_;

    host_map_type tracked_hosts_;
    host_map_type blocked_hosts_;

    util::async_timer timer_;
};

typedef boost::shared_ptr<filter_manager> filter_manager_ptr;

#endif  // _filter_manager_hpp
