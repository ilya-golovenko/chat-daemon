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
#include "chat_connection_dispatcher.hpp"
#include "chat_connection.hpp"
#include "chat_exception.hpp"
#include "chat_server.hpp"
#include "chat_common.hpp"
#include <http_utilities.hpp>
#include <http_mime_types.hpp>
#include <http_url_builder.hpp>
#include <http_url.hpp>
#include <http_url_query_parser.hpp>
#include <http_url_query.hpp>
#include <config_common.hpp>
#include <log_common.hpp>
#include <path_utils.hpp>
#include <file_utils.hpp>
#include <utilities.hpp>
#include <banner.hpp>

// BOOST headers
#include <boost/tuple/tuple.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>


namespace chat
{

connection_dispatcher::connection_dispatcher(chat::server& server) :
    server_(server)
{
}

void connection_dispatcher::configure()
{
    LOGNOT("configuring connection dispather");

    create_chat_header();
    create_client_http_response();
    create_document_domain_fix_script();

    reload_irc_frame_ = http::buffer(
        config.chat.scripts.reload_irc_frame);

    daemon_banner_ = http::buffer(daemon_html_banner);
}

void connection_dispatcher::process_connection(http::server::connection::pointer connection)
{
    try
    {
        LOGDBG("processing connection: %1%") << connection->get_remote_endpoint();
        server_.get_statistics_manager()->add_connection();

        bool blocked = false;

        if(!server_.get_frontend_manager()->is_connected_via_frontend(connection))
            blocked = server_.get_filter_manager()->check_blocked(connection);

        if(!blocked)
        {
            connection->get_tcp_connection()->set_option(asio::ip::tcp::no_delay(true));
            connection->read_request(bind_to_read_handler(connection), config.chat.connect_timeout);
        }
        else
        {
            server_.get_statistics_manager()->add_ignored_connection();
            connection->close(true);
        }
    }
    catch(std::exception& e)
    {
        LOGWRN("error while processing connection: %1%") << e.what();
        connection->close(http::status::bad_request, daemon_version, bind_to_write_handler());
    }
}

void connection_dispatcher::create_chat_header()
{
    LOGDBG("reading chat header message");

    std::string filename = util::path::combine(
        config.chat.php_path, files::z_channel);

    chat_header_ = http::buffer(
        util::file::read_text(filename));
}

void connection_dispatcher::create_client_http_response()
{
    client_response_.set_keep_alive(false);
    client_response_.set_server(daemon_version);
    client_response_.set_status(http::status::ok);
    client_response_.set_content_type(http::mime_types::text_html);
    client_response_.set(http::header_names::pragma, http::cache_tokens::no_cache);
    client_response_.set(http::header_names::cache_control, http::cache_tokens::no_cache);
}

void connection_dispatcher::create_document_domain_fix_script()
{
    if(config.chat.endpoints.empty())
        throw chat::error("no listen endpoints specified");

    std::string document_domain = config.chat.scripts.document_domain;
    http::url url = http::url::from_string(config.chat.endpoints.front());
    boost::replace_first(document_domain, domain_name_placeholder, url.hostname());

    document_domain_fix_ = http::buffer(document_domain);
}

std::pair<std::string, bool> connection_dispatcher::process_request(const http::request& request)
{
    http::url_query_parser url_query_parser;

    if(request.get_method() != http::request_methods::get)
        throw chat::error("bad request method: " + request.get_method());

    http::url url = http::url::from_string(request.get_url());
    http::url_query url_query = url_query_parser.parse(url);

    if(!url_query.has(config.chat.session.name))
        throw chat::error("request url does not contain session id: " + request.get_url());

    std::string session_id = url_query.get<std::string>(config.chat.session.name);

    if(session_id.empty() || config.chat.session.length != session_id.size())
        throw chat::error("request url contains bad session id: " + request.get_url());

    if(!config.chat.session.alphabet.empty())
    {
        if(!boost::all(session_id, boost::is_any_of(config.chat.session.alphabet)))
            throw chat::error("request url contains bad session id: " + request.get_url());
    }

    bool antivirus_hack = config.chat.antivirus_hack;

    if(url_query.has(parameters::antivirus_hack))
        antivirus_hack = url_query.get<bool>(parameters::antivirus_hack);

    return std::make_pair(session_id, antivirus_hack);
}

const http::write_handler connection_dispatcher::bind_to_write_handler()
{
    return boost::bind(&connection_dispatcher::handle_write, shared_from_this(), _1, _2);
}

const http::read_handler connection_dispatcher::bind_to_read_handler(http::server::connection::pointer connection)
{
    return boost::bind(&connection_dispatcher::handle_read, shared_from_this(), connection, _1, _2);
}

void connection_dispatcher::handle_write(const asio::error_code& error, std::size_t bytes_transferred)
{
    if(!error)
        server_.get_statistics_manager()->add_output_message(bytes_transferred);
}

void connection_dispatcher::handle_read(http::server::connection::pointer connection,
    const asio::error_code& error, std::size_t /*bytes_transferred*/)
{
    if(!error)
    {
        try
        {
            bool blocked = false;

            server_.get_frontend_manager()->process_connection(connection);

            if(connection->has_custom_remote_endpoint())
                blocked = server_.get_filter_manager()->check_blocked(connection);

            if(!blocked)
            {
                std::string session_id;
                bool antivirus_hack;

                boost::tie(session_id, antivirus_hack) = process_request(connection->get_request());

                if(!antivirus_hack)
                    connection->write_response(client_response_, bind_to_write_handler());

                connection->write_buffer(daemon_banner_, bind_to_write_handler());
                connection->write_buffer(chat_header_, bind_to_write_handler());

                if(!connection->has_custom_remote_endpoint())
                    connection->write_buffer(document_domain_fix_, bind_to_write_handler());

                if(!server_.get_user_manager()->contains(session_id))
                {
                    std::string filename = util::path::combine(
                        config.chat.sess_path, session_id);

                    if(util::file::exists(filename))
                    {
                        server_.get_user_manager()->join(session_id,
                            std::time(0), util::file::read_text(filename));
                    }
                }

                if(server_.get_user_manager()->contains(session_id))
                {
                    server_.get_user_manager()->accept(
                        chat::connection::create(server_, connection, session_id));
                }
                else
                {
                    connection->close(reload_irc_frame_, bind_to_write_handler());
                    throw chat::error("session file does not exist: " + session_id);
                }
            }
            else
            {
                LOGDBG("address is ignored: %1%") << connection->get_remote_address();
                server_.get_statistics_manager()->add_ignored_connection();
                connection->close(true);
            }
        }
        catch(std::exception& e)
        {
            LOGWRN("%1%: %2%") << connection->get_log_source_name() << e.what();
            connection->close(http::status::bad_request, daemon_version, bind_to_write_handler());
        }
    }
}

}   // namespace chat
