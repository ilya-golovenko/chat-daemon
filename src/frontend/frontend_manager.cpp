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
#include "frontend_manager.hpp"
#include <http_utilities.hpp>
#include <config_common.hpp>
#include <utilities.hpp>
#include <log_common.hpp>

// BOOST headers
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>


frontend_manager::frontend_manager(chat::server& server) :
    server_(server),
    enabled_(false)
{
}

void frontend_manager::configure()
{
    LOGNOT("configuring");

    addresses_.clear();

    enabled_ = config.frontend.enabled;
    if(enabled_)
    {
        if(!config.frontend.addresses.empty())
        {
            http_header_ = config.frontend.http_header;
            LOGNOT("http header: %1%") << http_header_;

            std::for_each(config.frontend.addresses.begin(),
                config.frontend.addresses.end(), boost::bind(
                &frontend_manager::add_frontend_address, this, _1));
        }
        else
        {
            LOGWRN("no frontend addresses specified");
            enabled_ = false;
        }
    }

    if(!enabled_)
        LOGNOT("disabled");
}

void frontend_manager::process_connection(http::server::connection::pointer connection)
{
    if(is_connected_via_frontend(connection))
    {
        const http::request& request = connection->get_request();
        boost::optional<std::string> value = request.get(http_header_);

        if(value)
        {
            LOGDBG("connected via frontend: %1%") << connection->get_remote_endpoint();

            asio::ip::address address = asio::ip::address::from_string(*value);
            unsigned short port = connection->get_remote_port();

            asio::ip::tcp::endpoint endpoint(address, port);
            connection->set_remote_endpoint(endpoint);

            LOGDBG("connection endpoint: %1%") << endpoint;
        }
    }
}

bool frontend_manager::is_connected_via_frontend(http::server::connection::pointer connection) const
{
    return (enabled_ && addresses_.count(connection->get_remote_address()));
}

void frontend_manager::add_frontend_address(const std::string& address)
{
    LOGNOT("adding frontend address: %1%") << address;
    addresses_.insert(asio::ip::address::from_string(address));
}
