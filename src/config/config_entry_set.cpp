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

// Application headers
#include "config_entry_set.hpp"


config_entry_set::config_entry_set()
{
}

void config_entry_set::insert(const config_entry& entry)
{
    entries_.push_back(entry);
}

void config_entry_set::erase(iterator where)
{
    entries_.erase(where);
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
