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
#include "chat_accept_manager.hpp"
#include "chat_exception.hpp"
#include "chat_server.hpp"
#include <http_utilities.hpp>
#include <http_url.hpp>
#include <utilities.hpp>
#include <string_utils.hpp>
#include <log_common.hpp>

// BOOST headers
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>


namespace chat
{

accept_manager::accept_manager(chat::server& server) :
    server_(server)
{
}

accept_manager::~accept_manager()
{
    io_thread_.stop();
}

void accept_manager::configure()
{
    LOGNOT("configuring");

    endpoints_.clear();

    if(config.chat.endpoints.empty())
        throw chat::error("no listen endpoints specified");

    std::for_each(config.chat.endpoints.begin(), config.chat.endpoints.end(),
        boost::bind(&accept_manager::resolve_endpoint, shared_from_this(), _1));
}

void accept_manager::start()
{
    io_thread_.stop();
    acceptors_.clear();

    LOGNOT("starting");

    std::for_each(endpoints_.begin(), endpoints_.end(), boost::bind(
        &accept_manager::create_acceptor, shared_from_this(), _1));

    io_thread_.start();
    LOGNOT("started");
}

void accept_manager::stop()
{
    LOGNOT("stopping");

    std::for_each(acceptors_.begin(), acceptors_.end(),
        boost::bind(&asio::ip::tcp::acceptor::close, _1));

    acceptors_.clear();
    io_thread_.stop();

    LOGNOT("stopped");
}

void accept_manager::resolve_endpoint(const std::string& endpoint)
{
    http::url url = http::url::from_string(endpoint);

    asio::error_code error;
    asio::ip::address address;

    unsigned short port = url.port();
    std::string hostname = url.hostname();

    address = asio::ip::address::from_string(hostname, error);

    if(!error)
    {
        add_endpoint(asio::ip::tcp::endpoint(address, port));
    }
    else
    {
        LOGNOT("resolving endpoint: %1%") << endpoint;

        asio::ip::tcp::resolver resolver(&io_thread_);
        const std::string port_string = util::to_string(port);
        asio::ip::tcp::resolver::query query(hostname, port_string);

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
            LOGWRN("cannot resolve endpoint: %1%") << endpoint;
        }
    }
}

void accept_manager::add_endpoint(const asio::ip::tcp::endpoint& endpoint)
{
    if(!endpoints_.count(endpoint))
    {
        LOGNOT("adding listen endpoint: %1%") << endpoint;
        endpoints_.insert(endpoint);
    }
    else
    {
        LOGNOT("endpoint already exists, skipping: %1%") << endpoint;
    }
}

accept_manager::acceptor_ptr accept_manager::create_new_acceptor(const asio::ip::tcp::endpoint& endpoint)
{
    LOGNOT("creating acceptor: %1%") << endpoint;
    return boost::make_shared<asio::ip::tcp::acceptor>(boost::ref(&io_thread_), endpoint);
}

void accept_manager::create_acceptor(const asio::ip::tcp::endpoint& endpoint)
{
    acceptors_.push_back(create_new_acceptor(endpoint));
    accept_connection(acceptors_.back());
}

void accept_manager::accept_connection(acceptor_ptr acceptor)
{
    tcp::connection::pointer connection = tcp::connection::create(server_.get_io_service());

    acceptor->async_accept(connection->socket(), boost::bind(&accept_manager::handle_accept,
        shared_from_this(), acceptor, connection, asio::placeholders::error));
}

void accept_manager::pass_connection_to_connection_dispatcher(http::server::connection::pointer connection)
{
    server_.get_io_service().post(boost::bind(
        &connection_dispatcher::process_connection,
        server_.get_connection_dispatcher(), connection));
}

void accept_manager::handle_accept(acceptor_ptr acceptor, tcp::connection::pointer connection, const asio::error_code& error)
{
    if(!error)
    {
        LOGDBG("accepted connection: %1%") << connection->get_remote_endpoint();

        pass_connection_to_connection_dispatcher(
            http::server::connection::create(connection));

        accept_connection(acceptor);
    }
}

}   // namespace chat
