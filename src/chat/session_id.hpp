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
#ifndef _chat_session_id_hpp
#define _chat_session_id_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <functional>
#include <utility>
#include <string>
#include <iosfwd>


namespace chat
{

class session_id
{
public:
    static session_id const& any();

public:
    explicit session_id(std::string const& str);

    session_id(session_id const&) = default;
    session_id& operator=(session_id const&) = default;

    std::string const& str() const;

    bool is_any() const;

private:
    std::string str_;
};

bool operator<(session_id const& lhs, session_id const& rhs);
bool operator==(session_id const& lhs, session_id const& rhs);

std::ostream& operator<<(std::ostream& os, session_id const& id);

}   // namespace chat

namespace std
{

template <>
struct hash<chat::session_id> : std::unary_function<chat::session_id, std::size_t>
{
    std::size_t operator()(chat::session_id const& id) const
    {
        return std::hash<std::string>()(id.str());
    }
};

template <>
struct hash<std::pair<chat::session_id, chat::session_id>> : std::unary_function<std::pair<chat::session_id, chat::session_id>, std::size_t>
{
    std::size_t operator()(std::pair<chat::session_id, chat::session_id> const& value) const
    {
        return std::hash<chat::session_id>()(value.first) ^ std::hash<chat::session_id>()(value.second);
    }
};

}   // namespace std

#endif  // _chat_session_id_hpp
