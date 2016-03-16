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
#include <chat/ignore_manager.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <chat/exception.hpp>
#include <chat/user.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{

ignore_manager::ignore_manager(server_context& context) :
    context_(context)
{
}

void ignore_manager::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(ignore_manager);

    LOG_COMP_NOTICE(ignore_manager, "configuring");
}

void ignore_manager::start()
{
    LOG_COMP_TRACE_FUNCTION(ignore_manager);

    LOG_COMP_NOTICE(ignore_manager, "starting");

    LOG_COMP_NOTICE(ignore_manager, "started");
}

void ignore_manager::stop()
{
    LOG_COMP_TRACE_FUNCTION(ignore_manager);

    LOG_COMP_NOTICE(ignore_manager, "stopping");

    LOG_COMP_NOTICE(ignore_manager, "stopped");
}

void ignore_manager::add_total_ignore(session_id const& ignored_id)
{
    LOG_COMP_TRACE_FUNCTION(ignore_manager);

    if(ignored_id.is_any())
    {
        throw exception("cannot add total ignore for all users");
    }

    if(total_ignores_.insert(ignored_id).second)
    {
        LOG_COMP_NOTICE(ignore_manager, "added total ignore for user ", resolve_user_name(ignored_id));
    }
}

void ignore_manager::remove_total_ignore(session_id const& ignored_id)
{
    LOG_COMP_TRACE_FUNCTION(ignore_manager);

    if(total_ignores_.erase(ignored_id) > 0)
    {
        if(ignored_id.is_any())
        {
            LOG_COMP_NOTICE(ignore_manager, "removed all total ignores");
        }
        else
        {
            LOG_COMP_NOTICE(ignore_manager, "removed total ignore from user ", resolve_user_name(ignored_id));
        }
    }
}

bool ignore_manager::is_ignored(session_id const& id) const
{
    return total_ignores_.find(id) != total_ignores_.end();
}

bool ignore_manager::is_ignored(user_ptr user) const
{
    return is_ignored(user->get_session_id());
}

std::size_t ignore_manager::get_total_ignore_count() const
{
    return total_ignores_.size();
}

void ignore_manager::add_user_ignore(session_id const& id, session_id const& ignored_id)
{
    LOG_COMP_TRACE_FUNCTION(ignore_manager);

    if(id.is_any())
    {
        throw exception("invalid user session id: ", id);
    }

    if(ignored_id.is_any())
    {
        throw exception("user ", resolve_user_name(id), " cannot add ignore for all users");
    }

    if(user_ignores_.emplace(id, ignored_id).second)
    {
        LOG_COMP_NOTICE(ignore_manager, "user ", resolve_user_name(id), " added ignore for user ", resolve_user_name(ignored_id));
    }
}

void ignore_manager::remove_user_ignore(session_id const& id, session_id const& ignored_id)
{
    LOG_COMP_TRACE_FUNCTION(ignore_manager);

    if(id.is_any())
    {
        throw exception("invalid user session id: ", id);
    }

    if(user_ignores_.erase(user_ignore_pair(id, ignored_id)) > 0)
    {
        if(ignored_id.is_any())
        {
            LOG_COMP_NOTICE(ignore_manager, "removed all ignores for user ", resolve_user_name(id));
        }
        else
        {
            LOG_COMP_NOTICE(ignore_manager, "user ", resolve_user_name(id), " removed ignore from user ", resolve_user_name(ignored_id));
        }
    }
}

void ignore_manager::remove_all_user_ignores(session_id const& id)
{
    LOG_COMP_TRACE_FUNCTION(ignore_manager);

    remove_user_ignore(id, session_id::any());
}

bool ignore_manager::is_ignored(session_id const& id, session_id const& ignored_id) const
{
    return user_ignores_.find(user_ignore_pair(id, ignored_id)) != user_ignores_.end();
}

bool ignore_manager::is_ignored(user_ptr user, user_ptr ignored_user) const
{
    return is_ignored(user->get_session_id(), ignored_user->get_session_id());
}

std::size_t ignore_manager::get_user_ignore_count(session_id const& id) const
{
    return user_ignores_.count(user_ignore_pair(id, session_id::any()));
}

std::size_t ignore_manager::get_user_ignore_count() const
{
    return user_ignores_.size();
}

std::string const& ignore_manager::resolve_user_name(session_id const& id) const
{
    return context_.get_user_manager().resolve(id);
}

}   // namespace chat
