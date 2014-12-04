//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2014 Ilya Golovenko
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
#include <chat/cron_connection.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <app/version.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// STL headers
#include <functional>
#include <chrono>


namespace chat
{

cron_connection::cron_connection(server_context& context) :
    context_(context),
    timer_(context_.get_io_service())
{
}

void cron_connection::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(cron_connection);

    LOG_COMP_NOTICE(cron_connection, "configuring");

    interval_ = config.cron_interval;

    http::url const url = http::url::from_string(config.http_server_endpoint);
    std::string const path = http::combine_url_path(config.url_path, files::crond);

    cron_script_request_.set_url(path);
    cron_script_request_.set_keep_alive(true);
    cron_script_request_.set_host(url.get_host_info());
    cron_script_request_.set_user_agent(build_version_text());

    cron_script_request_.set_version(http::version::http_1_1);
    cron_script_request_.set_method(http::request_methods::get);

    cron_script_request_.set(http::header_names::pragma, http::cache_tokens::no_cache);
    cron_script_request_.set(http::header_names::cache_control, http::cache_tokens::no_cache);

    connection_ = http::client_connection::create(context_.get_io_service(), url.get_hostname(), url.get_port());
}

void cron_connection::start()
{
    LOG_COMP_TRACE_FUNCTION(cron_connection);

    LOG_COMP_NOTICE(cron_connection, "configuring");

    timer_.expires_from_now(interval_);
    timer_.async_wait(bind_to_timer_hander());
}

void cron_connection::stop()
{
    LOG_COMP_TRACE_FUNCTION(cron_connection);

    LOG_COMP_NOTICE(cron_connection, "stopping");

    timer_.cancel();

    if(connection_)
    {
        connection_->close();
        connection_.reset();
    }
}

http::completion_handler cron_connection::bind_to_timer_hander()
{
    return std::bind(&cron_connection::handle_timer, this, std::placeholders::_1);
}

http::completion_handler cron_connection::bind_to_completion_hander()
{
    return std::bind(&cron_connection::handle_complete, this, std::placeholders::_1);
}

void cron_connection::handle_timer(boost::system::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(cron_connection);

    if(!error)
        connection_->send_request(cron_script_request_, bind_to_completion_hander());
}

void cron_connection::handle_complete(boost::system::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(cron_connection);

    if(!error)
    {
        http::response const& response = connection_->get_response();

        if(response.get_status() != http::status::ok)
            LOG_COMP_WARNING(cron_connection, "cannot access cron script: ", response.get_status());
    }
    else if(error != boost::asio::error::operation_aborted)
    {
        LOG_COMP_WARNING(cron_connection, "cannot access cron script: ", error);
    }

    if(error != boost::asio::error::operation_aborted)
    {
        timer_.expires_from_now(interval_);
        timer_.async_wait(bind_to_timer_hander());
    }
}

}   // namespace chat
