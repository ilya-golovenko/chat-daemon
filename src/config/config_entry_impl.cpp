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
#include <config/config_entry_impl.hpp>
#include <config/config_entry_set.hpp>

// STL headers
#include <tuple>


namespace chat
{

void config_entry_impl::add_entry(config_entry const& entry)
{
    entries_.emplace(entry.get_name(), entry);
}

std::string const& config_entry_impl::get_name() const
{
    return name_;
}

std::string const& config_entry_impl::get_value() const
{
    throw_if_empty();
    return value_;
}

bool config_entry_impl::has_value() const
{
    return has_value_;
}

config_entry config_entry_impl::get_entry(std::string const& path) const
{
    if(path.empty())
        throw config_error("config entry path cannot be empty");

    std::size_t pos = path.find_first_of('.');

    if(pos == std::string::npos)
    {
        entry_map::const_iterator it = entries_.find(path);

        if(it != entries_.end())
            return it->second;
    }
    else
    {
        entry_map::const_iterator it = entries_.find(path.substr(0, pos));

        if(it != entries_.end())
            return it->second.get_entry(path.substr(pos + 1));
    }

    return config_entry();
}

config_entry_set config_entry_impl::get_entries(std::string const& path) const
{
    if(path.empty())
        throw config_error("config entry path cannot be empty");

    std::size_t pos = path.find_first_of('.');

    if(pos == std::string::npos)
    {
        entry_map::const_iterator it;
        entry_map::const_iterator end;

        std::tie(it, end) = entries_.equal_range(path);

        if(it != entries_.end())
        {
            config_entry_set entries;

            for(; it != end; ++it)
                entries.insert(it->second);

            return entries;
        }
    }
    else
    {
        entry_map::const_iterator it = entries_.find(path.substr(0, pos));

        if(it != entries_.end())
            return it->second.get_entries(path.substr(pos + 1));
    }

    return config_entry_set();
}

config_entry_impl::config_entry_impl() :
    has_value_(false)
{
}

config_entry_impl::config_entry_impl(std::string const& name) :
    name_(name),
    has_value_(false)
{
}

config_entry_impl::config_entry_impl(std::string const& name, std::string const& value) :
    name_(name),
    value_(value),
    has_value_(true)
{
}

void config_entry_impl::throw_if_empty() const
{
    if(!has_value_)
        throw config_error("config entry has no value: " + name_);
}

}   // namespace chat
