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
#include <chat/frontend_manager.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <http/utilities.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// STL headers
#include <functional>
#include <tuple>


namespace chat
{

frontend_manager::frontend_manager(server_context& context) :
    context_(context),
    enabled_(false)
{
}

void frontend_manager::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(frontend_manager);

    LOG_COMP_NOTICE(frontend_manager, "configuring");

    addresses_.clear();

    enabled_ = config.frontend.enabled;
    if(enabled_)
    {
        if(!config.frontend.addresses.empty())
        {
            http_header_ = config.frontend.http_header;
            LOG_COMP_INFO(frontend_manager, "http header: ", http_header_);

            for(auto const& address : config.frontend.addresses)
            {
                LOG_COMP_INFO(frontend_manager, "adding frontend address: ", address);
                addresses_.insert(address);
            }
        }
        else
        {
            LOG_COMP_WARNING(frontend_manager, "no frontend addresses specified");
            enabled_ = false;
        }
    }

    if(!enabled_)
        LOG_COMP_NOTICE(frontend_manager, "disabled");
}

void frontend_manager::start()
{
    LOG_COMP_TRACE_FUNCTION(frontend_manager);

    LOG_COMP_NOTICE(frontend_manager, "starting");

    LOG_COMP_NOTICE(frontend_manager, "started");
}

void frontend_manager::stop()
{
    LOG_COMP_TRACE_FUNCTION(frontend_manager);

    LOG_COMP_NOTICE(frontend_manager, "stopping");

    LOG_COMP_NOTICE(frontend_manager, "stopped");
}

void frontend_manager::process_connection(http::server_connection::pointer connection)
{
    LOG_COMP_TRACE_FUNCTION(frontend_manager);

    if(is_connected_via_frontend(connection))
    {
        http::request const& request = connection->get_request();
        boost::optional<std::string> value = request.get(http_header_);

        if(value)
        {
            LOG_COMP_TRACE(frontend_manager, "connected via frontend: ", connection->get_remote_endpoint());

            asio::ip::address address = asio::ip::address::from_string(*value);
            std::uint16_t port = connection->get_remote_port();

            asio::ip::tcp::endpoint endpoint(address, port);
            connection->set_remote_endpoint(endpoint); //TODO: this functionality should not be in server_connection

            LOG_COMP_TRACE(frontend_manager, "connection endpoint: ", endpoint);
        }
    }
}

bool frontend_manager::is_connected_via_frontend(http::server_connection::pointer connection) const
{
    return enabled_ && addresses_.count(connection->get_remote_address()) > 0;
}

}   // namespace chat
