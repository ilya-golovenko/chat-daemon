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
#include <chat/accept_manager.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <chat/exception.hpp>
#include <http/utilities.hpp>
#include <http/url.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{

accept_manager::accept_manager(server_context& context) :
    context_(context)
{
}

void accept_manager::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(accept_manager);

    LOG_COMP_NOTICE(accept_manager, "configuring");

    acceptors_.clear();
    endpoints_.clear();

    if(config.endpoints.empty())
        throw exception("no listen endpoints specified");

    for(auto const& endpoint : config.endpoints)
    {
        resolve_endpoint(endpoint.hostname, endpoint.port);
    }
}

void accept_manager::start()
{
    LOG_COMP_TRACE_FUNCTION(accept_manager);

    LOG_COMP_NOTICE(accept_manager, "starting");

    for(auto const& endpoint : endpoints_)
    {
        create_acceptor(endpoint);
    }

    LOG_COMP_NOTICE(accept_manager, "started");
}

void accept_manager::stop()
{
    LOG_COMP_TRACE_FUNCTION(accept_manager);

    LOG_COMP_NOTICE(accept_manager, "stopping");

    for(auto& acceptor : acceptors_)
    {
        acceptor.close();
    }

    acceptors_.clear();

    LOG_COMP_NOTICE(accept_manager, "stopped");
}

void accept_manager::resolve_endpoint(std::string const& hostname, std::uint16_t port)
{
    LOG_COMP_TRACE_FUNCTION(accept_manager);

    asio::error_code error;
    asio::ip::address address;

    address = asio::ip::address::from_string(hostname, error);

    if(!error)
    {
        add_endpoint(asio::ip::tcp::endpoint(address, port));
    }
    else
    {
        LOG_COMP_NOTICE(accept_manager, "resolving host name: ", hostname);

        asio::ip::tcp::resolver resolver(context_.get_io_service());
        asio::ip::tcp::resolver::query query(hostname, std::to_string(port));

        asio::ip::tcp::resolver::iterator it;
        asio::ip::tcp::resolver::iterator end;

        it = resolver.resolve(query, error);

        if(!error)
        {
            while(it != end)
                add_endpoint(*it++);
        }
        else
        {
            LOG_COMP_WARNING(accept_manager, "cannot resolve host name: ", hostname);
        }
    }
}

void accept_manager::add_endpoint(asio::ip::tcp::endpoint const& endpoint)
{
    LOG_COMP_TRACE_FUNCTION(accept_manager);

    if(endpoints_.insert(endpoint).second)
    {
        LOG_COMP_NOTICE(accept_manager, "added listen endpoint: ", endpoint);
    }
    else
    {
        LOG_COMP_WARNING(accept_manager, "endpoint already exists, skipping: ", endpoint);
    }
}

void accept_manager::create_acceptor(asio::ip::tcp::endpoint const& endpoint)
{
    LOG_COMP_TRACE_FUNCTION(accept_manager);

    LOG_COMP_INFO(accept_manager, "creating acceptor: ", endpoint);

    acceptors_.emplace_back(context_.get_io_service(), endpoint);

    accept_connection(acceptors_.back());
}

void accept_manager::accept_connection(asio::ip::tcp::acceptor& acceptor)
{
    LOG_COMP_TRACE_FUNCTION(accept_manager);

    http::tcp_connection::pointer connection = http::tcp_connection::create(context_.get_io_service());

    acceptor.async_accept(connection->get_socket(), std::bind(&accept_manager::handle_accept, this, std::ref(acceptor), connection, std::placeholders::_1));
}

void accept_manager::pass_connection_to_connection_manager(http::server_connection::pointer connection)
{
    LOG_COMP_TRACE_FUNCTION(accept_manager);

    context_.get_connection_manager().process_connection(connection);
}

void accept_manager::handle_accept(asio::ip::tcp::acceptor& acceptor, http::tcp_connection::pointer connection, asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(accept_manager);

    if(!error)
    {
        LOG_COMP_TRACE(accept_manager, "accepted connection from ", connection->get_remote_endpoint());

        pass_connection_to_connection_manager(http::server_connection::create(connection));

        accept_connection(acceptor);
    }
}

}   // namespace chat
