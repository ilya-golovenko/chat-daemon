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
#ifndef _chat_config_entry_impl_hpp
#define _chat_config_entry_impl_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <config/config_entry.hpp>

// STL headers
#include <string>
#include <map>


namespace chat
{

class config_entry_impl
{
public:
    typedef std::multimap<std::string, config_entry> entry_map;

public:
    config_entry_impl();
    explicit config_entry_impl(std::string const& name);
    config_entry_impl(std::string const& name, std::string const& value);

    config_entry_impl(config_entry_impl const&) = delete;
    config_entry_impl& operator=(config_entry_impl const&) = delete;

    bool has_value() const;

    std::string const& get_name() const;
    std::string const& get_value() const;

    void add_entry(config_entry const& entry);

    config_entry get_entry(std::string const& path) const;
    config_entry_set get_entries(std::string const& path) const;

private:
    void throw_if_empty() const;

private:
    std::string name_;
    std::string value_;
    bool has_value_;
    entry_map entries_;
};

}   // namespace chat

#endif  // _chat_config_entry_impl_hpp
