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
#ifndef _chat_server_hpp
#define _chat_server_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_message_parser.hpp"
#include "chat_data_reader.hpp"
#include "chat_accept_manager.hpp"
#include "chat_skin_manager.hpp"
#include "chat_connection_dispatcher.hpp"
#include "chat_ignore_manager.hpp"
#include "chat_user_manager.hpp"
#include "chat_room_manager.hpp"
#include "chat_forwards.hpp"
#include "chat_common.hpp"
#include <http_client_connection.hpp>
#include <http_request.hpp>
#include <statistics_manager.hpp>
#include <frontend_manager.hpp>
#include <filter_manager.hpp>
#include <config_common.hpp>
#include <async_timer.hpp>
#include <log_source.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/array.hpp>
#include <boost/ref.hpp>

// STL headers
#include <string>
#include <set>


namespace chat
{

class server :
    private boost::noncopyable,
    public log_source<server>
{
DEFINE_LOG_SOURCE_NAME("chat server")

public:
    server();
    ~server();

    void configure();

    void run();
    void stop();
    void restart();

    bool is_restarting() const;

    asio::io_service& get_io_service();

    const std::string& get_chat_topic() const;

    ignore_manager& get_ignore_manager();

    user_manager_ptr get_user_manager();
    room_manager_ptr get_room_manager();
    skin_manager_ptr get_skin_manager();
    message_parser_ptr get_message_parser();
    connection_dispatcher_ptr get_connection_dispatcher();

    filter_manager_ptr get_filter_manager();
    frontend_manager_ptr get_frontend_manager();
    statistics_manager_ptr get_statistics_manager();

    void reload_chat_topic(bool force);

private:
    void start_server();
    void stop_server();

    void create_all_managers();
    void destroy_all_managers();

    void start_all_managers();
    void stop_all_managers();

    void start_cron_timer();
    void deliver_stop_message();

    void setup_cron_connection();
    void destroy_cron_connection();

    template <typename ManagerT>
    boost::shared_ptr<ManagerT> create_manager();

    template <typename ManagerT>
    boost::shared_ptr<ManagerT> create_configured_manager();

    void handle_cron_status(const asio::error_code& error);
    void handle_cron_timer(const asio::error_code& error);
    void handle_stop_request();

private:
    bool restart_;

    std::string chat_topic_;

    unsigned short cron_port_;
    std::string cron_hostname_;

    asio::io_service io_service_;
    util::async_timer cron_timer_;

    ignore_manager ignore_manager_;

    data_reader_ptr data_reader_;
    user_manager_ptr user_manager_;
    room_manager_ptr room_manager_;
    skin_manager_ptr skin_manager_;
    accept_manager_ptr accept_manager_;
    message_parser_ptr message_parser_;
    connection_dispatcher_ptr connection_dispatcher_;

    filter_manager_ptr filter_manager_;
    frontend_manager_ptr frontend_manager_;
    statistics_manager_ptr statistics_manager_;

    http::request cron_script_request_;
    http::client::connection::pointer cron_connection_;
};

template <typename ManagerT>
boost::shared_ptr<ManagerT> server::create_manager()
{
    return boost::make_shared<ManagerT>(boost::ref(*this));
}

template <typename ManagerT>
boost::shared_ptr<ManagerT> server::create_configured_manager()
{
    boost::shared_ptr<ManagerT> manager;
    manager = create_manager<ManagerT>();
    manager->configure();
    return manager;
}

}   // namespace chat

#endif  // _chat_server_hpp
