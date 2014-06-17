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
#ifndef _config_entry_impl_hpp
#define _config_entry_impl_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "config_entry.hpp"

// BOOST headers
#include <boost/noncopyable.hpp>

// STL headers
#include <string>


class config_entry_impl :
    private boost::noncopyable
{
public:
    typedef std::multimap<std::string, config_entry> entry_map_type;

public:
    config_entry_impl();
    explicit config_entry_impl(const std::string& name);
    config_entry_impl(const std::string& name, const std::string& value);

    bool has_value() const;

    const std::string& get_name() const;
    const std::string& get_value() const;

    void add_entry(const config_entry& entry);

    const config_entry get_entry(const std::string& path) const;
    const config_entry_set get_entries(const std::string& path) const;

private:
    void throw_if_empty() const;

private:
    std::string name_;
    std::string value_;
    bool has_value_;
    entry_map_type entries_;
};

#endif  // _config_entry_impl_hpp
