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
#ifndef _chat_message_hpp
#define _chat_message_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_forwards.hpp"
#include <http_buffer.hpp>

// STL headers
#include <string>

// BOOST headers
#include <boost/noncopyable.hpp>


namespace chat
{

class message :
    private boost::noncopyable
{
public:
    static message_ptr create(const std::string& text);
    static message_ptr create(const std::string& text, std::size_t access);

    static const std::string get_user_text(const std::string& text);
    static const std::string get_admin_text(const std::string& text);

public:
    message(const std::string& text);
    message(const std::string& text, std::size_t access);

    void set_sender(user_ptr sender);
    void set_recipient(user_ptr recipient);

    std::size_t access() const;

    bool has_sender() const;
    bool has_recipient() const;

    session_ptr sender() const;
    session_ptr recipient() const;

    const http::buffer& text(std::size_t access) const;

private:
    void set_message_text(const std::string& text); 

private:
    std::size_t access_;
    session_ptr sender_;
    session_ptr recipient_;
    http::buffer user_text_;
    http::buffer admin_text_;
};

}   // namespace chat

#endif  // _chat_message_hpp
