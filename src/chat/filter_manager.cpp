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
#include <chat/filter_manager.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <misc/path_utils.hpp>
#include <misc/file_utils.hpp>
#include <app/constants.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>
#include <missio/format/print.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <functional>


namespace chat
{

static char const time_placeholder[]    = "{@time}";
static char const address_placeholder[] = "{@address}";

filter_manager::filter_manager(server_context& context) :
    context_(context),
    max_list_size_(4096u),
    timer_(context_.get_io_service())
{
}

void filter_manager::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    LOG_COMP_NOTICE(filter_manager, "configuring");

    for(server_config::filter::rule const& rule : config.filter.rules)
    {
        LOG_COMP_INFO(filter_manager, "creating filter rule: ", rule.name);

        filter_address const address = filter_address::from_string(rule.address);

        rules_.emplace_back(rule.name, address, rule.max_connection_count, rule.connections_per_minute, rule.block_duration);
    }

    max_list_size_ = config.filter.max_list_size;

    address_blocked_message_ = config.address_blocked_message;

    if(!address_blocked_message_.empty())
    {
        boost::algorithm::replace_first(address_blocked_message_, std::string(address_placeholder), std::string("{0}"));
        boost::algorithm::replace_first(address_blocked_message_, std::string(time_placeholder), std::string("{1}"));
    }

    update_ignorelist_file(true);
}

void filter_manager::start()
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    LOG_COMP_NOTICE(filter_manager, "starting");

    start_update_timer();

    LOG_COMP_NOTICE(filter_manager, "started");
}

void filter_manager::stop()
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    LOG_COMP_NOTICE(filter_manager, "stopping");

    timer_.cancel();

    tracked_hosts_.clear();
    blocked_hosts_.clear();

    hosts_.clear();
    rules_.clear();

    LOG_COMP_NOTICE(filter_manager, "stopped");
}

bool filter_manager::check_blocked(asio::ip::address const& address)
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    host_map::iterator blocked_host = find_blocked_host(address);

    if(blocked_host != blocked_hosts_.end())
    {
        add_host_connection(blocked_host);
        return true;
    }

    host_map::iterator tracked_host = tracked_hosts_.find(address);

    if(tracked_host != tracked_hosts_.end())
    {
        add_host_connection(tracked_host);

        rule_vector::iterator rule = find_rule(tracked_host);

        if(rule != rules_.end())
        {
            block_host(tracked_host, *rule);
            return true;
        }

        return false;
    }

    update_tracked_hosts(false);
    add_tracked_host(address);

    return false;
}

void filter_manager::remove_block(asio::ip::address const& address)
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    host_map::iterator host = blocked_hosts_.find(address);

    if(host != blocked_hosts_.end())
    {
        LOG_COMP_NOTICE(filter_manager, "removing block from host: ", address);

        blocked_hosts_.erase(host);
        tracked_hosts_.erase(address);

        hosts_.erase(host->second);
    }

    update_ignorelist_file(true);
    start_update_timer();
}

void filter_manager::start_update_timer()
{
    timer_.expires_from_now(std::chrono::seconds(60));
    timer_.async_wait(std::bind(&filter_manager::handle_timer, this, std::placeholders::_1));
}

void filter_manager::handle_timer(asio::error_code const& error)
{
    if(!error)
    {
        update_ignorelist_file(false);
        update_tracked_hosts(true);
        start_update_timer();
    }
}

void filter_manager::update_tracked_hosts(bool full)
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    while((full && !hosts_.empty()) || tracked_hosts_.size() >= max_list_size_)
    {
        if(hosts_.front().is_tracking_expired())
        {
            asio::ip::address const& address = hosts_.front().get_address();

            LOG_COMP_DEBUG(filter_manager, "removing tracked host: ", address);

            blocked_hosts_.erase(address);
            tracked_hosts_.erase(address);

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
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    if(force || !blocked_hosts_.empty())
    {
        std::ostringstream buffer;

        host_map::iterator host = blocked_hosts_.begin();
        host_map::iterator end = blocked_hosts_.end();

        while(host != end)
        {
            if(host->second->is_block_expired())
            {
                LOG_COMP_NOTICE(filter_manager, "host block has expired: ", host->first);
                blocked_hosts_.erase(host++);
            }
            else
            {
                buffer << host->second->get_block_duration().count() << '=' << host->first << std::endl;
                ++host;
            }
        }

        try
        {
            std::string filename = util::path::combine(config.server_path, ::files::ignorelist);

            util::file::write_text(filename, buffer.str());
        }
        catch(std::exception const& e)
        {
            LOG_COMP_WARNING(filter_manager, e);
        }
    }
}

void filter_manager::add_host_connection(host_map::iterator host)
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    LOG_COMP_TRACE(filter_manager, "adding host connection: ", host->first);

    hosts_.splice(hosts_.end(), hosts_, host->second);

    host->second->add_connection();
}

void filter_manager::add_tracked_host(asio::ip::address const& address)
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    LOG_COMP_DEBUG(filter_manager, "adding tracked host: ", address);

    tracked_hosts_.emplace(address, hosts_.emplace(hosts_.end(), address));
}

void filter_manager::block_host(host_map::iterator host, filter_rule const& rule)
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    LOG_COMP_WARNING(filter_manager, "host ", host->first, " was blocked by ", rule.get_name(), " rule");

    host->second->block(rule.get_block_duration());

    blocked_hosts_.insert(*host);

    update_ignorelist_file(true);
    start_update_timer();

    deliver_address_blocked_message(host->first);
}

filter_manager::rule_vector::iterator filter_manager::find_rule(host_map::iterator host)
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    std::size_t const connection_count = context_.get_user_manager().get_connection_count(host->first);

    return std::find_if(rules_.begin(), rules_.end(), [=](filter_rule const& rule){ return rule.satisfies(*host->second, connection_count); });
}

filter_manager::host_map::iterator filter_manager::find_blocked_host(asio::ip::address const& address)
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    host_map::iterator host = blocked_hosts_.find(address);

    if(host != blocked_hosts_.end())
    {
        if(host->second->is_block_expired())
        {
            LOG_COMP_NOTICE(filter_manager, "host block has expired: ", host->first);

            blocked_hosts_.erase(host);
            host = blocked_hosts_.end();
        }
    }

    return host;
}

void filter_manager::deliver_address_blocked_message(asio::ip::address const& address)
{
    LOG_COMP_TRACE_FUNCTION(filter_manager);

    if(!address_blocked_message_.empty())
    {
        std::string message;

        missio::format::print(message, address_blocked_message_, address, std::time(nullptr));

        context_.get_message_parser().deliver_admin_message(message, config.admin_view_ip);
    }
}

}   // namespace chat
