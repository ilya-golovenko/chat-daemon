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
#ifndef _chat_ignore_manager_hpp
#define _chat_ignore_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/session_id.hpp>
#include <chat/user_fwd.hpp>

// STL headers
#include <unordered_set>
#include <utility>
#include <string>


namespace chat
{

class server_context;
struct server_config;

class ignore_manager
{
public:
    explicit ignore_manager(server_context& context);

    ignore_manager(ignore_manager const&) = delete;
    ignore_manager& operator=(ignore_manager const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

    void add_total_ignore(session_id const& ignored_id);
    void remove_total_ignore(session_id const& ignored_id);

    bool is_ignored(session_id const& id) const;
    bool is_ignored(user_ptr user) const;

    std::size_t get_total_ignore_count() const;

    void add_user_ignore(session_id const& id, session_id const& ignored_id);
    void remove_user_ignore(session_id const& id, session_id const& ignored_id);
    void remove_all_user_ignores(session_id const& id);

    bool is_ignored(session_id const& id, session_id const& ignored_id) const;
    bool is_ignored(user_ptr user, user_ptr ignored_user) const;

    std::size_t get_user_ignore_count(session_id const& id) const;
    std::size_t get_user_ignore_count() const;

private:
    std::string const& resolve_user_name(session_id const& id) const;

private:
    typedef std::unordered_set<session_id> total_ignore_set;
    typedef std::pair<session_id, session_id> user_ignore_pair;
    typedef std::unordered_set<user_ignore_pair> user_ignore_set;

private:
    server_context& context_;

    total_ignore_set total_ignores_;
    user_ignore_set user_ignores_;
};

}   // namespace chat

#endif  // _chat_ignore_manager_hpp
