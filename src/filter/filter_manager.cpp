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
#include "filter_manager.hpp"
#include <chat_server.hpp>
#include <chat_room.hpp>
#include <chat_user.hpp>
#include <chat_message_parser.hpp>
#include <config_common.hpp>
#include <log_common.hpp>
#include <path_utils.hpp>
#include <file_utils.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>


const std::string filter_manager::time_placeholder    = "{@time}";
const std::string filter_manager::address_placeholder = "{@address}";

filter_manager::filter_manager(chat::server& server) :
    server_(server),
    enabled_(true),
    list_size_(4096),
    timer_(server_.get_io_service())
{
}

void filter_manager::configure()
{
    LOGNOT("configuring");

    hosts_.clear();
    rules_.clear();

    tracked_hosts_.clear();
    blocked_hosts_.clear();

    enabled_ = config.filter.enabled;
    if(enabled_)
    {
        if(!config.filter.rules.empty())
        {
            list_size_ = config.filter.list_size;
            LOGNOT("address list size: %1%") << list_size_;

            std::for_each(config.filter.rules.begin(),
                config.filter.rules.end(), boost::bind(
                &filter_manager::create_rule, this, _1));
        }
        else
        {
            LOGWRN("no rules specified");
            enabled_ = false;
        }
    }

    if(!enabled_)
        LOGNOT("disabled");

    update_ignorelist_file(true);
}

void filter_manager::start()
{
    if(enabled_)
        start_update_timer();
}

void filter_manager::stop()
{
    timer_.stop();
}

bool filter_manager::check_blocked(http::server::connection::pointer connection)
{
    if(enabled_)
    {
        asio::ip::address address;
        host_map_type::iterator host;
        rule_vector_type::iterator rule;

        address = connection->get_remote_address();
        host = find_blocked_host(address);

        if(host != blocked_hosts_.end())
        {
            add_host_connection(host);
            return true;
        }

        host = get_tracked_host(address);
        add_host_connection(host);

        rule = find_rule(host);

        if(rule != rules_.end())
        {
            block_host(host, *rule);
            return true;
        }
    }

    return false;
}

void filter_manager::remove_block(const asio::ip::address& address)
{
    if(enabled_)
    {
        host_map_type::iterator host = blocked_hosts_.find(address);

        if(host != blocked_hosts_.end())
        {
            LOGNOT("address block has been removed: %1%") << address;

            blocked_hosts_.erase(host);
            tracked_hosts_.erase(address);

            hosts_.erase(host->second);
        }

        update_ignorelist_file(true);
        start_update_timer();
    }
}

void filter_manager::start_update_timer()
{
    timer_.start(boost::posix_time::seconds(60), boost::bind(
        &filter_manager::handle_timer, shared_from_this(), _1));
}

void filter_manager::handle_timer(const asio::error_code& error)
{
    if(!error)
    {
        update_ignorelist_file(false);
        update_tracked_hosts();
        start_update_timer();
    }
}

void filter_manager::update_tracked_hosts()
{
    boost::uint64_t now = util::get_milli_time();

    while(!hosts_.empty())
    {
        if(hosts_.front()->last_conn_time() + 21600000ULL < now)
        {
            blocked_hosts_.erase(hosts_.front()->address());
            tracked_hosts_.erase(hosts_.front()->address());

            hosts_.erase(hosts_.begin());
        }
        else
        {
            break;
        }
    }
}

void filter_manager::update_ignorelist_file(bool force)
{
    std::ostringstream buffer;

    host_map_type::iterator host = blocked_hosts_.begin();
    host_map_type::iterator end = blocked_hosts_.end();

    if(force || host != end)
    {
        while(host != end)
        {
            if((*host->second)->is_block_expired())
            {
                LOGNOT("address block has been expired: %1%") << host->first;
                blocked_hosts_.erase(host++);
            }
            else
            {
                buffer << (*host->second)->get_block_duration();
                buffer << '=' << host->first << std::endl;

                ++host;
            }
        }

        try
        {
            std::string filename = util::path::combine(
                config.chat.server_path, files::ignorelist);

            util::file::write_text(filename, buffer.str());
        }
        catch(std::exception& e)
        {
            LOGWRN(e.what());
        }
    }
}

void filter_manager::create_rule(const config_data::filter::rule& rule_config)
{
    create_rule(rule_config.name, rule_config.address,
        rule_config.netmask, rule_config.block_duration,
        rule_config.conn_per_minute, rule_config.max_connections);
}

void filter_manager::create_rule(
    const std::string& name, const std::string& address,
    const std::string& netmask, std::time_t block_duration,
    std::size_t conn_per_minute, std::size_t max_connections)
{
    LOGNOT("creating %1% rule") << name;

    rules_.push_back(filter_rule::create(name, address,
        netmask, block_duration, conn_per_minute, max_connections));
}

void filter_manager::add_host_connection(host_map_type::iterator host)
{
    (*host->second)->add_connection();
    hosts_.splice(hosts_.end(), hosts_, host->second);
}

void filter_manager::block_host(host_map_type::iterator host, filter_rule_ptr rule)
{
    LOGWRN("address %1% has been blocked by %2% rule")
        << host->first << rule->name();

    (*host->second)->block(rule->block_duration());
    blocked_hosts_.insert(*host);

    update_ignorelist_file(true);
    start_update_timer();

    deliver_address_blocked_message(host->first);
}

std::size_t filter_manager::get_connection_count(const asio::ip::address& address)
{
    return server_.get_user_manager()->get_connection_count(address);
}

filter_manager::rule_vector_type::iterator filter_manager::find_rule(host_map_type::iterator host)
{
    std::size_t connection_count = get_connection_count(host->first);

    return std::find_if(rules_.begin(), rules_.end(), boost::bind(
        &filter_rule::satisfies, _1, *host->second, connection_count));
}

filter_manager::host_map_type::iterator filter_manager::find_blocked_host(const asio::ip::address& address)
{
    host_map_type::iterator host = blocked_hosts_.find(address);

    if(host != blocked_hosts_.end())
    {
        if((*host->second)->is_block_expired())
        {
            LOGNOT("address block has been expired: %1%") << host->first;

            blocked_hosts_.erase(host);
            host = blocked_hosts_.end();
        }
    }

    return host;
}

filter_manager::host_map_type::iterator filter_manager::get_tracked_host(const asio::ip::address& address)
{
    host_map_type::iterator host = tracked_hosts_.find(address);

    if(host == tracked_hosts_.end())
    {
        while(tracked_hosts_.size() >= list_size_)
        {
            blocked_hosts_.erase(hosts_.front()->address());
            tracked_hosts_.erase(hosts_.front()->address());

            hosts_.erase(hosts_.begin());
        }

        hosts_.push_back(filter_host::create(address));
        host_map_type::value_type value(address, --hosts_.end());
        boost::tie(host, boost::tuples::ignore) = tracked_hosts_.insert(value);
    }

    return host;
}

void filter_manager::deliver_address_blocked_message(const asio::ip::address& address)
{
    if(!config.chat.messages.address_blocked.empty())
    {
        std::string text = config.chat.messages.address_blocked;
        boost::replace_first(text, address_placeholder, address.to_string());
        boost::replace_first(text, time_placeholder, util::to_string(std::time(0)));
        server_.get_message_parser()->deliver_admin_message(text, config.chat.admin.view_ip);
    }
}
