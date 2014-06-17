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
#include "chat_user.hpp"
#include "chat_room.hpp"
#include "chat_server.hpp"
#include "chat_connection.hpp"
#include "chat_exception.hpp"
#include "chat_message.hpp"
#include <file_utils.hpp>
#include <path_utils.hpp>
#include <utilities.hpp>
#include <config_common.hpp>
#include <log_common.hpp>

// BOOST headers
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>


namespace chat
{

const update::type update_types[] =
{
    update::connection,
    update::status,
    update::away,
    update::access,
    update::auth
};

enum { update_count = sizeof(update_types) / sizeof(*update_types) };

user_ptr user::create(chat::server& server, room_ptr room, session_ptr session)
{
    return boost::make_shared<user>(boost::ref(server), room, session);
}

user::~user()
{
    LOGDBG("terminated");
}

session_ptr user::session() const
{
    return session_;
}

const std::string& user::name() const
{
    return name_;
}

room_ptr user::room() const
{
    return room_;
}

void user::set_room(room_ptr room)
{
    room_ = room;
    session_->set(field::room, room_->name());
}

user::user(chat::server& server, room_ptr room, session_ptr session) :
    server_(server),
    timer_(server_.get_io_service()),
    bot_(false),
    active_(false),
    room_(room),
    session_(session)
{
    rebuild_user_name();
    LOGDBG("created");
}

void user::update(update::type update_type, const std::string& value)
{
    std::size_t access = 0;

    if(update_type == update::access)
        access = config.chat.admin.view_ip;

    std::string text = get_update_message(update_type, value);
    room_->deliver(chat::message::create(text, access), true);

    update_session_field(update_type, value);
}

void user::remove_session_file()
{
    try
    {
        std::string filename = util::path::combine(
            config.chat.sess_path, session_->id());

        if(util::file::exists(filename))
            util::file::remove(filename);
    }
    catch(std::exception& e)
    {
        LOGWRN(e.what());
    }
}

void user::update_session_field(update::type update_type, const std::string& value)
{
    switch(update_type)
    {
        case update::status:
            session_->set(field::status, value);
            break;

        case update::away:
            session_->set(field::away, value);
            break;

        case update::access:
            session_->set(field::access, value);
            rebuild_user_name();
            LOGNOT("access level changed");
            break;

        case update::auth:
            session_->set(field::auth, value);
            break;
    }
}

const std::string user::get_update_message(update::type update_type, const std::string& value) const
{
    std::ostringstream buffer;

    for(std::size_t index = 0; index < update_count; ++index)
    {
        if(index > 0)
            buffer << ',';

        buffer << '\"';

        if(update_types[index] == update_type)
            buffer << value;

        buffer << '\"';
    }

    boost::format formatter(formatters::user_update);

    formatter % std::time(0);
    formatter % session_->nickname();
    formatter % update_type;
    formatter % buffer.str();

    return formatter.str();
}

void user::add_ignore(const std::string& session_id)
{
    LOGNOT("adding ignore for session: %1%") << session_id;
    ignore_manager_.add_ignore(session_id);
}

void user::remove_ignore(const std::string& session_id)
{
    LOGNOT("removing ignore for session: %1%") << session_id;
    ignore_manager_.remove_ignore(session_id);
}

void user::clear_ignores()
{
    LOGNOT("clearing ignore list");
    ignore_manager_.clear_ignores();
}

bool user::is_ignored(const std::string& session_id) const
{
    return ignore_manager_.is_ignored(session_id);
}

std::size_t user::ignore_count() const
{
    return ignore_manager_.ignore_count();
}

const std::string& user::get_log_source_name() const
{
    return name_;
}

void user::deliver(const http::buffer& text)
{
    if(active_ && is_connected())
        connection_->write(text, bind_to_write_handler());
}

void user::deliver(const std::string& text)
{
    if(!text.empty())
        deliver(http::buffer(text));
}

bool user::is_connected() const
{
    return (connection_ && connection_->is_open());
}

bool user::is_connected_from(const asio::ip::address& address) const
{
    return (is_connected() && (address == connection_->get_remote_address()));
}

void user::start(connection_ptr connection)
{
    if(is_connected())
    {
        std::string message = config.chat.messages.duplicate_windows;
        connection_->stop(message, bind_to_write_handler());
    }

    active_ = true;
    start_ping_timer();

    connection_ = connection;
    connection_->start();

    rebuild_user_name();
    LOGNOT("session started");
}

void user::start()
{
    active_ = false;
    start_leave_timer();
}

void user::stop()
{
    active_ = false;
    timer_.stop();

    if(is_connected())
    {
        connection_->stop();
        connection_.reset();
    }

    remove_session_file();
    LOGNOT("session stopped");
}

bool user::is_bot() const
{
    return bot_;
}

void user::register_as_bot()
{
    bot_ = true;

    active_ = false;
    timer_.stop();

    // Set user joined status
    update(update::connection, status::joined);

    LOGNOT("registered as bot");
}

void user::rebuild_user_name()
{
    std::ostringstream buffer;

    buffer << "user " << session_->nickname();
    buffer << ", "  << session_->access();
    buffer << ", "  << session_->id();

    if(connection_)
        buffer << ", " << connection_->name();

    name_ = buffer.str();
}

void user::start_ping_timer()
{
    timer_.start(boost::posix_time::seconds(config.chat.ping_interval),
        boost::bind(&user::handle_ping, shared_from_this(), _1));
}

void user::start_leave_timer()
{
    timer_.start(boost::posix_time::seconds(config.chat.leave_timeout),
        boost::bind(&user::handle_leave, shared_from_this(), _1));
}

const chat::completion_handler user::bind_to_write_handler()
{
    return boost::bind(&user::handle_write, shared_from_this(), _1);
}

void user::handle_write(const asio::error_code& error)
{
    if(!error)
    {
        if(active_)
            start_ping_timer();
    }
    else if(error != asio::error::operation_aborted)
    {
        if(active_ && !is_connected())
        {
            LOGNOT("connection has been lost");

            active_ = false;
            start_leave_timer();

            remove_session_file();
            update(update::connection, status::lost);
        }
    }
}

void user::handle_leave(const asio::error_code& error)
{
    if(!error)
    {
        if(!is_connected())
        {
            server_.get_user_manager()->leave(shared_from_this(),
                std::time(0), config.chat.messages.lost_connection);
        }
    }
}

void user::handle_ping(const asio::error_code& error)
{
    if(!error)
        deliver(scripts::ping_update);
}

}   // namespace chat
