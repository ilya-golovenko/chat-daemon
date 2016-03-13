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
#ifndef _chat_filter_manager_hpp
#define _chat_filter_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <filter/filter_host.hpp>
#include <filter/filter_rule.hpp>
#include <misc/hash_utils.hpp>

// ASIO headers
#include <asio/steady_timer.hpp>
#include <asio.hpp>

// STL headers
#include <unordered_map>
#include <string>
#include <vector>
#include <list>


namespace chat
{

class server_context;
struct server_config;

class filter_manager
{
public:
    explicit filter_manager(server_context& context);

    filter_manager(filter_manager const&) = delete;
    filter_manager& operator=(filter_manager const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

    bool check_blocked(asio::ip::address const& address);
    void remove_block(asio::ip::address const& address);

private:
    typedef std::list<filter_host> host_list;
    typedef std::vector<filter_rule> rule_vector;

    typedef std::unordered_map<asio::ip::address, host_list::iterator> host_map;

private:
    void start_update_timer();
    void handle_timer(asio::error_code const& error);

    void update_tracked_hosts(bool full);
    void update_ignorelist_file(bool force);

    void add_host_connection(host_map::iterator host);
    void add_tracked_host(asio::ip::address const& address);
    void block_host(host_map::iterator host, filter_rule const& rule);

    rule_vector::iterator find_rule(host_map::iterator host);

    host_map::iterator find_blocked_host(asio::ip::address const& address);

    void deliver_address_blocked_message(asio::ip::address const& address);

private:
    server_context& context_;

    std::size_t max_list_size_;

    std::size_t admin_view_ip_;

    std::string server_path_;

    host_list hosts_;
    rule_vector rules_;

    host_map tracked_hosts_;
    host_map blocked_hosts_;

    asio::steady_timer timer_;

    std::string address_blocked_message_;
};

}   // namespace chat

#endif  // _chat_filter_manager_hpp
