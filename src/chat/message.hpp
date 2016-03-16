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
#ifndef _chat_message_hpp
#define _chat_message_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/forwards.hpp>
#include <http/buffer.hpp>

// STL headers
#include <string>


namespace chat
{

class message
{
public:
    static message_ptr create(std::string const& text);
    static message_ptr create(std::string const& text, std::size_t access);

    static std::string get_user_text(std::string const& text);
    static std::string get_admin_text(std::string const& text);

public:
    message(std::string const& text);
    message(std::string const& text, std::size_t access);

    message(message const&) = delete;
    message& operator=(message const&) = delete;

    void set_sender(user_ptr sender);
    void set_recipient(user_ptr recipient);

    std::size_t get_access() const;

    bool has_sender() const;
    bool has_recipient() const;

    session_ptr get_sender() const;
    session_ptr get_recipient() const;

    http::buffer const& get_text(std::size_t access) const;

private:
    void set_message_text(std::string const& text); 

private:
    std::size_t access_;
    session_ptr sender_;
    session_ptr recipient_;
    http::buffer user_text_;
    http::buffer admin_text_;
};

}   // namespace chat

#endif  // _chat_message_hpp
