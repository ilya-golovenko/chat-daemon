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
#include "chat_server.hpp"
#include "chat_user.hpp"
#include "chat_room.hpp"
#include "chat_common.hpp"
#include "chat_message.hpp"
#include "chat_exception.hpp"
#include "chat_data_reader.hpp"
#include "chat_connection.hpp"
#include <file_utils.hpp>
#include <path_utils.hpp>
#include <utilities.hpp>
#include <http_utilities.hpp>
#include <http_response.hpp>
#include <http_url.hpp>
#include <http_url_builder.hpp>
#include <config_common.hpp>
#include <log_common.hpp>
#include <version.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>


namespace chat
{

server::server() :
    restart_(false),
    cron_port_(0),
    io_service_(),
    cron_timer_(io_service_)
{
}

server::~server()
{
}

void server::configure()
{
    LOGNOT("configuring");

    LOGNOT("initializing random generator");
    std::srand(static_cast<unsigned int>(std::time(0)));

    LOGNOT("reading chat topic");
    reload_chat_topic(true);

    create_all_managers();
    setup_cron_connection();
}

void server::run()
{
    start_server();

    for(;;)
    {
        try
        {
            io_service_.run();
            io_service_.reset();

            break;
        }
        catch(std::exception& e)
        {
            LOGERR("unhandled exception: %1%") << e.what();
            LOGWRN("restarting because of the error");

            io_service_.reset();
        }
    }

    destroy_all_managers();
    LOGNOT("stopped");
}

void server::stop()
{
    io_service_.post(boost::bind(
        &server::handle_stop_request, this));
}

void server::restart()
{
    restart_ = true;
    stop();
}

bool server::is_restarting() const
{
    return restart_;
}

asio::io_service& server::get_io_service()
{
    return io_service_;
}

const std::string& server::get_chat_topic() const
{
    return chat_topic_;
}

ignore_manager& server::get_ignore_manager()
{
    return ignore_manager_;
}

user_manager_ptr server::get_user_manager()
{
    return user_manager_;
}

room_manager_ptr server::get_room_manager()
{
    return room_manager_;
}

skin_manager_ptr server::get_skin_manager()
{
    return skin_manager_;
}

message_parser_ptr server::get_message_parser()
{
    return message_parser_;
}

connection_dispatcher_ptr server::get_connection_dispatcher()
{
    return connection_dispatcher_;
}

filter_manager_ptr server::get_filter_manager()
{
    return filter_manager_;
}

frontend_manager_ptr server::get_frontend_manager()
{
    return frontend_manager_;
}

statistics_manager_ptr server::get_statistics_manager()
{
    return statistics_manager_;
}

void server::reload_chat_topic(bool force)
{
    if(force || chat_topic_.empty())
    {
        LOGDBG("reloading chat topic");

        try
        {
            std::string filename = util::path::combine(
                config.chat.conf_path, files::topic);

            if(util::file::exists(filename))
                chat_topic_ = util::file::read_text(filename);
        }
        catch(std::exception& e)
        {
            LOGWRN(e.what());
        }
    }
}

void server::start_server()
{
    LOGNOT("starting");

    restart_ = false;

    start_cron_timer();
    start_all_managers();

    LOGNOT("started");
}

void server::stop_server()
{
   if(restart_)
       LOGNOT("restarting");
    else
       LOGNOT("stopping");

    deliver_stop_message();
    destroy_cron_connection();

    LOGNOT("stopping user sessions");
    user_manager_->stop_all_user_sessions();

    stop_all_managers();
}

void server::create_all_managers()
{
    LOGNOT("creating all managers");

    skin_manager_ = create_configured_manager<skin_manager>();
    room_manager_ = create_configured_manager<room_manager>();
    user_manager_ = create_manager<user_manager>();

    statistics_manager_ = create_configured_manager<statistics_manager>();
    frontend_manager_ = create_configured_manager<frontend_manager>();
    filter_manager_ = create_configured_manager<filter_manager>();

    connection_dispatcher_ = create_configured_manager<connection_dispatcher>();
    accept_manager_ = create_configured_manager<accept_manager>();

    message_parser_ = create_manager<message_parser>();
    data_reader_ = create_configured_manager<data_reader>();
}

void server::destroy_all_managers()
{
    LOGNOT("destroying all managers");

    data_reader_.reset();
    message_parser_.reset();

    accept_manager_.reset();
    connection_dispatcher_.reset();

    filter_manager_.reset();
    frontend_manager_.reset();
    statistics_manager_.reset();

    user_manager_.reset();
    room_manager_.reset();
    skin_manager_.reset();
}

void server::start_all_managers()
{
    LOGNOT("starting all managers");

    statistics_manager_->start();
    filter_manager_->start();
    accept_manager_->start();
    data_reader_->start();
}

void server::stop_all_managers()
{
    LOGNOT("stopping all managers");

    data_reader_->stop();
    accept_manager_->stop();
    filter_manager_->stop();
    statistics_manager_->stop();
}

void server::start_cron_timer()
{
    cron_timer_.start(boost::posix_time::seconds(
        config.chat.cron_interval), boost::bind(
        &server::handle_cron_timer, this, _1));
}

void server::deliver_stop_message()
{
    LOGDBG("delivering stop message");

    if(restart_)
    {
        room_manager_->deliver_message(
            config.chat.messages.server_restart);
    }
    else
    {
        room_manager_->deliver_message(
            config.chat.messages.server_shutdown);
    }
}

void server::setup_cron_connection()
{
    LOGNOT("setting up cron script connection");

    http::url url = http::url::from_string(
        config.chat.http_server_endpoint);

    std::string path = http_util::combine_url_path(
        config.chat.url_path, files::crond);

    cron_port_ = url.port();
    cron_hostname_ = url.hostname();

    cron_script_request_.set_url(path);
    cron_script_request_.set_keep_alive(true);
    cron_script_request_.set_host(url.host_info());
    cron_script_request_.set_user_agent(daemon_version);

    cron_script_request_.set_version(http::version::_1_1);
    cron_script_request_.set_method(http::request_methods::get);

    cron_script_request_.set(http::header_names::pragma, http::cache_tokens::no_cache);
    cron_script_request_.set(http::header_names::cache_control, http::cache_tokens::no_cache);

    cron_connection_ = http::client::connection::create(io_service_);
}

void server::destroy_cron_connection()
{
    LOGNOT("destroying cron script connection");

    cron_timer_.stop();

    if(cron_connection_)
    {
        cron_connection_->close();
        cron_connection_.reset();
    }
}


void server::handle_cron_status(const asio::error_code& error)
{
    if(!error)
    {
        const http::response& response = cron_connection_->get_response();

        if(response.get_status() != http::status::ok)
            LOGWRN("error while accessing cron script: %1% ") << response.get_status();
    }
    else if(error != asio::error::operation_aborted)
    {
        LOGWRN("cannot access cron script: %1%") << error.message();
    }

    if(error != asio::error::operation_aborted)
        start_cron_timer();
}

void server::handle_cron_timer(const asio::error_code& error)
{
    if(!error)
    {
        cron_connection_->start(
            cron_hostname_, cron_port_, cron_script_request_,
            boost::bind(&server::handle_cron_status, this, _1));
    }
}

void server::handle_stop_request()
{
    stop_server();
}

}   // namespace chat
