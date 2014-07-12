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

const type nickname     = 0;
const type access       = 1;
const type address      = 2;
const type hostname     = 3;
const type sex          = 4;
const type auth         = 5;
const type skin         = 6;
const type browser      = 7;
const type status       = 8;
const type usernum      = 9;
const type away         = 10;
const type say          = 11;
const type nick_css     = 12;
const type nick_html    = 13;
const type time         = 14;
const type ignores      = 15;
const type room         = 16;

const type count        = 17;

}   // namespace field

class session
{
public:
    static session_ptr create(const session_id& id, std::string const& info);

public:
    session(const session_id& id, std::string const& info);

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

inline bool operator==(const session& lhs, const session& rhs)
{
    return lhs.get_id() == rhs.get_id() &&
           lhs.get_access() == rhs.get_access() &&
           lhs.get_nickname() == rhs.get_nickname();
}

inline bool operator!=(const session& lhs, const session& rhs)
{
    return !operator==(lhs, rhs);
}

}   // namespace chat

#endif  // _chat_session_hpp
