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
#include "chat_message.hpp"
#include "chat_user.hpp"
#include <config_common.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/xpressive/xpressive.hpp>
#include <boost/make_shared.hpp>


namespace chat
{
namespace regexes
{

using namespace boost::xpressive;

const sregex user   = as_xpr("<\x07") >> -*~as_xpr('\x07') >> "\x07>";
const sregex admin  = as_xpr("<\x07") | as_xpr("\x07>");

}   // namespace regexes

message_ptr message::create(const std::string& text)
{
    return boost::make_shared<message>(text);
}

message_ptr message::create(const std::string& text, std::size_t access)
{
    return boost::make_shared<message>(text, access);
}

const std::string message::get_user_text(const std::string& text)
{
    return boost::xpressive::regex_replace(text, regexes::user, strings::empty);
}

const std::string message::get_admin_text(const std::string& text)
{
    return boost::xpressive::regex_replace(text, regexes::admin, strings::empty);
}

void message::set_sender(user_ptr sender)
{
    sender_ = sender->session();
}

void message::set_recipient(user_ptr recipient)
{
    recipient_ = recipient->session();
}

std::size_t message::access() const
{
    return access_;
}

bool message::has_sender() const
{
    return !!sender_;
}

bool message::has_recipient() const
{
    return !!recipient_;
}

session_ptr message::sender() const
{
    return sender_;
}

session_ptr message::recipient() const
{
    return recipient_;
}

const http::buffer& message::text(std::size_t access) const
{
    return (access >= config.chat.admin.view_ip ? admin_text_ : user_text_);
}

message::message(const std::string& text) :
    access_(0)
{
    set_message_text(text);
}

message::message(const std::string& text, std::size_t access) :
    access_(access)
{
    set_message_text(text);
}

void message::set_message_text(const std::string& text)
{
    if(text.find('\x07') == std::string::npos)
    {
        user_text_ = admin_text_ = http::buffer(text);
    }
    else
    {
        user_text_ = http::buffer(get_user_text(text));
        admin_text_ = http::buffer(get_admin_text(text));
    }
}

}   // namespace chat
