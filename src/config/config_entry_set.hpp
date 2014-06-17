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
#ifndef _config_entry_set_hpp
#define _config_entry_set_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "config_entry.hpp"

// STL headers
#include <string>
#include <vector>


class config_entry_set
{
public:
    typedef std::vector<config_entry> entry_vector_type;

    typedef entry_vector_type::iterator iterator;
    typedef entry_vector_type::const_iterator const_iterator;

public:
    config_entry_set();

    void insert(const config_entry& entry);
    void erase(iterator where);
    void clear();

    std::size_t size() const;
    bool empty() const;

    template <typename T>
    const std::vector<T> as() const;

    template <typename T>
    const std::vector<T> as(const T& default_value) const;

    inline iterator begin()
    {
        return entries_.begin();
    }

    inline iterator end()
    {
        return entries_.end();
    }

    inline const const_iterator begin() const
    {
        return entries_.begin();
    }

    inline const const_iterator end() const
    {
        return entries_.end();
    }

    inline const config_entry& operator[](std::size_t index) const
    {
        return entries_[index];
    }

private:
    entry_vector_type entries_;
};

template <typename T>
const std::vector<T> config_entry_set::as() const
{
    if(entries_.empty())
        throw config_error("config entry set is empty");

    std::vector<T> values;
    values.reserve(entries_.size());

    entry_vector_type::const_iterator it = entries_.begin();
    entry_vector_type::const_iterator end = entries_.end();

    for(; it != end; ++it)
        values.push_back(it->as<T>());

    return values;
}

template <typename T>
const std::vector<T> config_entry_set::as(const T& default_value) const
{
    return (!entries_.empty() ? as<T>() : std::vector<T>(1, default_value));
}

#endif  // _config_entry_set_hpp
