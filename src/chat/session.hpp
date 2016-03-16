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
#ifndef _chat_session_hpp
#define _chat_session_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/session_id.hpp>
#include <chat/forwards.hpp>
#include <chat/common.hpp>
#include <app/constants.hpp>

// STL headers
#include <string>
#include <array>
#include <ctime>


namespace chat
{
namespace field
{

typedef std::size_t type;

type const nickname     = 0;
type const access       = 1;
type const address      = 2;
type const hostname     = 3;
type const sex          = 4;
type const auth         = 5;
type const skin         = 6;
type const browser      = 7;
type const status       = 8;
type const usernum      = 9;
type const away         = 10;
type const say          = 11;
type const nick_css     = 12;
type const nick_html    = 13;
type const time         = 14;
type const ignores      = 15;
type const room         = 16;

type const count        = 17;

}   // namespace field

class session
{
public:
    static session_ptr create(session_id const& id, std::string const& info);

public:
    session(session_id const& id, std::string const& info);

    session(session const&) = delete;
    session& operator=(session const&) = delete;

    session_id const& get_id() const;

    std::string const& get_nickname() const;
    std::string const& get_skin() const;
    std::string const& get_room() const;

    std::size_t get_access() const;
    sex::type get_sex() const;

    std::string const& get(field::type field_type) const;
    void set(field::type field_type, std::string const& value);

    std::string const& to_string() const;

private:
    void rebuild_session_string() const;

private:
    typedef std::array<std::string, field::count> field_array;

private:
    session_id id_;
    std::size_t access_;
    sex::type sex_;
    field_array fields_;

    mutable bool changed_;
    mutable std::string session_;
};

inline bool operator==(session const& lhs, session const& rhs)
{
    return lhs.get_id() == rhs.get_id() &&
           lhs.get_access() == rhs.get_access() &&
           lhs.get_nickname() == rhs.get_nickname();
}

inline bool operator!=(session const& lhs, session const& rhs)
{
    return !operator==(lhs, rhs);
}

}   // namespace chat

#endif  // _chat_session_hpp
