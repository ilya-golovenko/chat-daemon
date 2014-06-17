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
#ifndef _chat_session_hpp
#define _chat_session_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_common.hpp"
#include "chat_forwards.hpp"
#include <utilities.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>

// STL headers
#include <string>
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

class session :
    private boost::noncopyable
{
public:
    static session_ptr create(const std::string& id, const std::string& info);

public:
    session(const std::string& id, const std::string& info);

    const std::string& id() const;

    const std::string& nickname() const;
    const std::string& skin() const;
    const std::string& room() const;

    std::size_t access() const;
    sex::type sex() const;

    const std::string& get(field::type field_type) const;
    void set(field::type field_type, const std::string& value);

    const std::string& to_string();

private:
    void rebuild_session_string();

private:
    typedef boost::array<std::string, field::count> field_array_type;

private:
    std::string id_;
    std::size_t access_;
    sex::type sex_;
    field_array_type fields_;
    bool changed_;
    std::string session_;
};

inline bool operator==(const session& left, const session& right)
{
    return (left.id() == right.id()
        && left.access() == right.access()
        && left.nickname() == right.nickname());
}

inline bool operator!=(const session& left, const session& right)
{
    return !(left == right);
}

}   // namespace chat

#endif  // _chat_session_hpp
