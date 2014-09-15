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
#ifndef _chat_config_entry_set_hpp
#define _chat_config_entry_set_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <config/config_entry.hpp>

// STL headers
#include <string>
#include <vector>


namespace chat
{

class config_entry_set
{
public:
    typedef std::vector<config_entry> entry_vector;
    typedef entry_vector::reference reference;
    typedef entry_vector::const_reference const_reference;
    typedef entry_vector::iterator iterator;
    typedef entry_vector::const_iterator const_iterator;

public:
    config_entry_set() = default;
    ~config_entry_set() = default;

    config_entry_set(config_entry_set const&) = default;
    config_entry_set& operator=(config_entry_set const&) = default;

    config_entry_set(config_entry_set&&) = default;
    config_entry_set& operator=(config_entry_set&&) = default;

    void insert(config_entry const& entry);
    void erase(iterator pos);
    void clear();

    std::size_t size() const;
    bool empty() const;

    template <typename T>
    std::vector<T> as() const;

    template <typename T>
    std::vector<T> as(T const& default_value) const;

    const_iterator begin() const;
    const_iterator end() const;

    config_entry const& operator[](std::size_t index) const;

private:
    entry_vector entries_;
};

template <typename T>
std::vector<T> config_entry_set::as() const
{
    if(entries_.empty())
        throw config_error("config entry set is empty");

    std::vector<T> values;

    values.reserve(entries_.size());

    for(config_entry const& entry : entries_)
    {
        values.push_back(entry.as<T>());
    }

    return values;
}

template <typename T>
std::vector<T> config_entry_set::as(T const& default_value) const
{
    return entries_.empty() ? std::vector<T>(1, default_value) : as<T>();
}

}   // namespace chat

#endif  // _chat_config_entry_set_hpp
