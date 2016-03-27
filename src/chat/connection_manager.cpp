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
#include <chat/connection_manager.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <chat/connection.hpp>
#include <chat/exception.hpp>
#include <chat/common.hpp>
#include <http/utilities.hpp>
#include <http/mime_types.hpp>
#include <http/url_builder.hpp>
#include <http/url_query_parser.hpp>
#include <http/url_query.hpp>
#include <http/url.hpp>
#include <misc/file_utils.hpp>
#include <misc/path_utils.hpp>
#include <app/version.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <functional>
#include <regex>
#include <tuple>


namespace chat
{

connection_manager::connection_manager(server_context& context) :
    context_(context)
{
}

void connection_manager::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(connection_manager);

    LOG_COMP_NOTICE(connection_manager, "configuring");

    std::string filename = util::path::combine(config.php_path, files::z_channel);

    LOG_COMP_DEBUG(connection_manager, "reading chat header message");
    chat_header_ = http::buffer(util::file::read_text(filename));

    if(!config.endpoints.empty())
    {
        std::string document_domain = config.document_domain_script;
        std::string hostname = config.endpoints.front().hostname;

        boost::algorithm::replace_first(document_domain, domain_name_placeholder, hostname);

        document_domain_fix_ = http::buffer(document_domain);
    }

    daemon_banner_ = http::buffer("<!--" + build_banner_text() + "-->");

    reload_irc_frame_ = http::buffer(config.reload_irc_frame_script);

    connect_timeout_ = config.connect_timeout;

    session_name_ = config.session_name;

    session_path_ = config.sess_path;

    create_client_http_response();
}

void connection_manager::start()
{
    LOG_COMP_TRACE_FUNCTION(connection_manager);

    LOG_COMP_NOTICE(connection_manager, "starting");

    LOG_COMP_NOTICE(connection_manager, "started");
}

void connection_manager::stop()
{
    LOG_COMP_TRACE_FUNCTION(connection_manager);

    LOG_COMP_NOTICE(connection_manager, "stopping");

    LOG_COMP_NOTICE(connection_manager, "stopped");
}

void connection_manager::process_connection(http::server_connection::pointer connection)
{
    LOG_COMP_TRACE_FUNCTION(connection_manager);

    try
    {
        context_.get_statistics_manager().add_connection();

        LOG_COMP_TRACE(connection_manager, "processing connection: ", connection->get_remote_endpoint());

        bool blocked = false;

        if(!context_.get_frontend_manager().is_connected_via_frontend(connection))
        {
            blocked = context_.get_filter_manager().check_blocked(connection->get_remote_address());
        }

        if(!blocked)
        {
            connection->get_tcp_connection().set_option(asio::ip::tcp::no_delay(true));
            connection->read_request(bind_to_read_handler(connection), connect_timeout_);
        }
        else
        {
            context_.get_statistics_manager().add_ignored_connection();
            connection->close(true);
        }
    }
    catch(std::exception const& e)
    {
        LOG_COMP_WARNING(connection_manager, e);
        connection->close(http::status::bad_request, build_version_text(), bind_to_write_handler());
    }
}

void connection_manager::create_client_http_response()
{
    LOG_COMP_TRACE_FUNCTION(connection_manager);

    client_response_.set_keep_alive(false);
    client_response_.set_status(http::status::ok);
    client_response_.set_server(build_version_text());
    client_response_.set_content_type(http::mime::text_html);
    client_response_.set(http::header_names::pragma, http::cache_tokens::no_cache);
    client_response_.set(http::header_names::cache_control, http::cache_tokens::no_cache);
}

session_id connection_manager::extract_session_id(http::request const& request)
{
    LOG_COMP_TRACE_FUNCTION(connection_manager);

    http::url_query_parser url_query_parser;

    http::url const url = http::url::from_string(request.get_url());
    http::url_query const url_query = url_query_parser.parse(url);

    if(!url_query.has(session_name_))
    {
        throw exception("request url does not contain session id: ", url);
    }

    std::string const& value = url_query.get(session_name_);

    if(value.empty())
    {
        throw exception("request url contains empty session id: ", url);
    }

    std::regex const session_id_regex("^[a-zA-Z0-9,-]{21,40}$");

    if(!std::regex_match(value, session_id_regex))
    {
        throw exception("request url contains bad session id: ", url);
    }

    return session_id(value);
}

http::completion_handler connection_manager::bind_to_write_handler()
{
    return std::bind(&connection_manager::handle_write, this, std::placeholders::_1);
}

http::completion_handler connection_manager::bind_to_read_handler(http::server_connection::pointer connection)
{
    return std::bind(&connection_manager::handle_read, this, connection, std::placeholders::_1);
}

void connection_manager::handle_write(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(connection_manager);

    //TODO:
    //if(!error)
    //    context_.get_statistics_manager().add_output_message(bytes_transferred);
}

void connection_manager::handle_read(http::server_connection::pointer connection, asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(connection_manager);

    if(!error)
    {
        try
        {
            bool blocked = false;

            context_.get_frontend_manager().process_connection(connection);

            if(connection->has_custom_remote_endpoint())
            {
                blocked = context_.get_filter_manager().check_blocked(connection->get_remote_address());
            }

            if(!blocked)
            {
                http::request const& request = connection->get_request();

                if(request.get_method() != http::request_methods::get)
                {
                    throw exception("bad request method: ", request.get_method());
                }

                session_id const id = extract_session_id(request);

                connection->write_response(client_response_, bind_to_write_handler());
                connection->write_buffer(daemon_banner_, bind_to_write_handler());
                connection->write_buffer(chat_header_, bind_to_write_handler());

                if(!document_domain_fix_.empty())
                {
                    if(!connection->has_custom_remote_endpoint())
                    {
                        connection->write_buffer(document_domain_fix_, bind_to_write_handler());
                    }
                }

                if(!context_.get_user_manager().contains(id))
                {
                    std::string const filename = util::path::combine(session_path_, id.str());

                    if(util::file::exists(filename))
                    {
                        context_.get_user_manager().join(id, std::time(nullptr), util::file::read_text(filename));
                    }
                }

                if(!context_.get_user_manager().contains(id))
                {
                    connection->close(reload_irc_frame_, bind_to_write_handler());
                    throw exception("session file does not exist: ", id.str());
                }

                context_.get_user_manager().accept(chat::connection::create(context_, connection, id));
            }
            else
            {
                LOG_COMP_TRACE(connection_manager, "address is ignored: ", connection->get_remote_address());
                context_.get_statistics_manager().add_ignored_connection();
                connection->close(true);
            }
        }
        catch(std::exception const& e)
        {
            LOG_COMP_WARNING(connection_manager, e);
            connection->close(http::status::bad_request, build_version_text(), bind_to_write_handler());
        }
    }
}

}   // namespace chat
