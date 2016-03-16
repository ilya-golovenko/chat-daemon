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
#include <chat/user.hpp>
#include <chat/room.hpp>
#include <chat/server.hpp>
#include <chat/connection.hpp>
#include <chat/exception.hpp>
#include <chat/message.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>
#include <missio/format/print.hpp>

// BOOST headers
#include <boost/range/size.hpp>

// STL headers
#include <functional>
#include <chrono>


namespace chat
{

update::type const update_types[] =
{
    update::connection,
    update::status,
    update::away,
    update::access,
    update::auth
};

user_ptr user::create(server_context& context, room_ptr room, session_ptr session)
{
    return std::make_shared<user>(std::ref(context), room, session);
}

user::user(server_context& context, room_ptr room, session_ptr session) :
    context_(context),
    timer_(context_.get_io_service()),
    bot_(false),
    active_(false),
    room_(room),
    session_(session)
{
    LOG_COMP_DEBUG(user, "created");
}

user::~user()
{
    LOG_COMP_DEBUG(user, "terminated");
}

session_ptr user::get_session() const
{
    return session_;
}

session_id const& user::get_session_id() const
{
    return session_->get_id();
}

std::string const& user::get_nickname() const
{
    return session_->get_nickname();
}

std::size_t user::get_access() const
{
    return session_->get_access();
}

sex::type user::get_sex() const
{
    return session_->get_sex();
}

room_ptr user::get_room() const
{
    return room_;
}

void user::set_room(room_ptr room)
{
    LOG_COMP_TRACE_FUNCTION(user);

    room_ = room;

    session_->set(field::room, room_->get_name());
}

void user::update(update::type update_type, std::string const& value)
{
    LOG_COMP_TRACE_FUNCTION(user);

    std::size_t access = 0;

    if(update_type == update::access)
        access = 100; //TODO: config.admin_view_ip;

    std::string text = get_update_message(update_type, value);
    room_->deliver(chat::message::create(text, access), true);

    update_session_field(update_type, value);
}

void user::update_session_field(update::type update_type, std::string const& value)
{
    LOG_COMP_TRACE_FUNCTION(user);

    switch(update_type)
    {
        case update::status:
            session_->set(field::status, value);
            break;

        case update::away:
            session_->set(field::away, value);
            break;

        case update::access:
            LOG_COMP_NOTICE(user, "setting access level ", value, " for user ", get_nickname());
            session_->set(field::access, value);
            break;

        case update::auth:
            session_->set(field::auth, value);
            break;
    }
}

std::string user::get_update_message(update::type update_type, std::string const& value) const
{
    LOG_COMP_TRACE_FUNCTION(user);

    std::ostringstream buffer;

    for(std::size_t index = 0; index < boost::size(update_types); ++index)
    {
        if(index > 0)
        {
            buffer << ',';
        }

        buffer << '\"';

        if(update_types[index] == update_type)
        {
            buffer << value;
        }

        buffer << '\"';
    }

    std::string message_text;

    std::string const& nickname = session_->get_nickname();

    missio::format::print(message_text, formatters::user_update, std::time(nullptr), nickname, update_type, buffer.str());

    return message_text;
}

void user::deliver(http::buffer const& text)
{
    LOG_COMP_TRACE_FUNCTION(user);

    if(active_ && is_connected())
    {
        connection_->write(text, bind_to_write_handler());
    }
}

void user::deliver(std::string const& text)
{
    if(!text.empty())
    {
        deliver(http::buffer(text));
    }
}

bool user::is_connected() const
{
    return connection_ && connection_->is_open();
}

bool user::is_connected_from(asio::ip::address const& address) const
{
    return is_connected() && address == connection_->get_remote_address();
}

void user::start(connection_ptr connection)
{
    LOG_COMP_TRACE_FUNCTION(user);

    if(is_connected())
    {
        //TODO: context_.get_user_manager().stop()
        //std::string message = config.duplicate_windows_message;
        //connection_->stop(message, bind_to_write_handler());
    }

    active_ = true;
    start_ping_timer();

    connection_ = connection;
    connection_->start();

    LOG_COMP_DEBUG(user, "started session for user ", get_nickname());
}

void user::start()
{
    LOG_COMP_TRACE_FUNCTION(user);

    active_ = false;
    start_leave_timer();
}

void user::stop()
{
    LOG_COMP_TRACE_FUNCTION(user);

    active_ = false;
    timer_.cancel();

    if(is_connected())
    {
        connection_->stop();
        connection_.reset();
    }

    context_.get_user_manager().remove_session_file(get_session_id());

    LOG_COMP_DEBUG(user, "stopped session for user ", get_nickname());
}

bool user::is_bot() const
{
    return bot_;
}

void user::register_bot()
{
    LOG_COMP_TRACE_FUNCTION(user);

    bot_ = true;

    active_ = false;
    timer_.cancel();

    // Set user joined status
    update(update::connection, status::joined);

    LOG_COMP_NOTICE(user, "user ", get_nickname(), " registered as bot");
}

void user::start_ping_timer()
{
    timer_.expires_from_now(/*TODO: config.ping_interval*/ std::chrono::seconds(10));
    timer_.async_wait(/*TODO: bind_to_ping_handler()*/ std::bind(&user::handle_ping, shared_from_this(), std::placeholders::_1));
}

void user::start_leave_timer()
{
    timer_.expires_from_now(/*TODO: config.leave_timeout*/ std::chrono::seconds(600));
    timer_.async_wait(/*TODO: bind_to_leave_handler()*/ std::bind(&user::handle_leave, shared_from_this(), std::placeholders::_1));
}

chat::completion_handler user::bind_to_write_handler()
{
    return std::bind(&user::handle_write, shared_from_this(), std::placeholders::_1);
}

void user::handle_write(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(user);

    if(!error)
    {
        if(active_)
        {
            start_ping_timer();
        }
    }
    else if(error != asio::error::operation_aborted)
    {
        if(active_ && !is_connected())
        {
            active_ = false;
            start_leave_timer();

            update(update::connection, status::lost);

            context_.get_user_manager().remove_session_file(get_session_id());

            LOG_COMP_NOTICE(user, "lost connection with user ", get_nickname());
        }
    }
}

void user::handle_leave(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(user);

    if(!error)
    {
        //if(!is_connected()) //TODO: context_.get_user_manager().lost(shared_from_this());
        //    context_.get_user_manager().leave(shared_from_this(), std::time(nullptr), config.lost_connection_message);
    }
}

void user::handle_ping(asio::error_code const& error)
{
    LOG_COMP_TRACE_FUNCTION(user);

    if(!error)
    {
        deliver(scripts::ping_update);
    }
}

}   // namespace chat
