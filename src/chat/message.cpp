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
#include <chat/message.hpp>
#include <chat/user.hpp>
#include <app/constants.hpp>

// BOOST headers
#include <boost/xpressive/xpressive.hpp>


namespace chat
{
namespace regexes
{

using namespace boost::xpressive;

sregex const user   = as_xpr("<\x07") >> -*~as_xpr('\x07') >> "\x07>";
sregex const admin  = as_xpr("<\x07") | as_xpr("\x07>");

}   // namespace regexes

message_ptr message::create(std::string const& text)
{
    return std::make_shared<message>(text);
}

message_ptr message::create(std::string const& text, std::size_t access)
{
    return std::make_shared<message>(text, access);
}

std::string message::get_user_text(std::string const& text)
{
    return boost::xpressive::regex_replace(text, regexes::user, strings::empty);
}

std::string message::get_admin_text(std::string const& text)
{
    return boost::xpressive::regex_replace(text, regexes::admin, strings::empty);
}

void message::set_sender(user_ptr sender)
{
    sender_ = sender->get_session();
}

void message::set_recipient(user_ptr recipient)
{
    recipient_ = recipient->get_session();
}

std::size_t message::get_access() const
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

session_ptr message::get_sender() const
{
    return sender_;
}

session_ptr message::get_recipient() const
{
    return recipient_;
}

http::buffer const& message::get_text(std::size_t access) const
{
    return access >= 100 /*config.admin_view_ip*/ ? admin_text_ : user_text_;
}

message::message(std::string const& text) :
    access_(0u)
{
    set_message_text(text);
}

message::message(std::string const& text, std::size_t access) :
    access_(access)
{
    set_message_text(text);
}

void message::set_message_text(std::string const& text)
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
