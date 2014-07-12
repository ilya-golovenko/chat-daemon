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
#include <config/config_entry_set.hpp>


namespace chat
{

void config_entry_set::insert(config_entry const& entry)
{
    entries_.push_back(entry);
}

void config_entry_set::erase(const_iterator pos)
{
    entries_.erase(pos);
}

void config_entry_set::clear()
{
    entries_.clear();
}

std::size_t config_entry_set::size() const
{
    return entries_.size();
}

bool config_entry_set::empty() const
{
    return entries_.empty();
}

config_entry_set::const_iterator config_entry_set::begin() const
{
    return entries_.begin();
}

config_entry_set::const_iterator config_entry_set::end() const
{
    return entries_.end();
}

config_entry const& config_entry_set::operator[](std::size_t index) const
{
    return entries_[index];
}

}   // namespace chat
