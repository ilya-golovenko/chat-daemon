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

// Application heeaders
#include "chat_ignore_manager.hpp"

// BOOST headers
#include <boost/format.hpp>


namespace chat
{

void ignore_manager::add_ignore(const std::string& session_id)
{
    check_create_set();
    ignores_->insert(session_id);
}

void ignore_manager::remove_ignore(const std::string& session_id)
{
    if(ignores_)
    {
        ignores_->erase(session_id);

        if(ignores_->empty())
            ignores_.reset();
    }
}

void ignore_manager::clear_ignores()
{
    if(ignores_)
    {
        ignores_->clear();
        ignores_.reset();
    }
}

bool ignore_manager::is_ignored(const std::string& session_id) const
{
    if(!ignores_)
        return false;

    return (ignores_->find(session_id) != ignores_->end());
}

std::size_t ignore_manager::ignore_count() const
{
    return (ignores_ ? ignores_->size() : 0);
}

void ignore_manager::check_create_set()
{
    if(!ignores_)
        ignores_.reset(new ignore_set_type());
}

}   // namespace chat
