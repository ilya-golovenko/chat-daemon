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
#ifndef _chat_config_entry_hpp
#define _chat_config_entry_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <config/config_exception.hpp>
#include <app/constants.hpp>

// BOOST headers
#include <boost/lexical_cast.hpp>

// STL headers
#include <typeinfo>
#include <memory>
#include <string>
#include <vector>
#include <map>


namespace chat
{

class config_entry;
class config_entry_impl;
class config_entry_set;

class config_entry
{
public:
    config_entry();
    explicit config_entry(std::string const& name);
    config_entry(std::string const& name, std::string const& value);

    config_entry(config_entry const&) = default;
    config_entry& operator=(config_entry const&) = default;

    template <typename T>
    T as() const;

    template <typename T>
    T as(T const& default_value) const;

    template <typename T>
    T get(std::string const& path) const;

    template <typename T>
    T get(std::string const& path, T const& default_value) const;

    std::string const& get_name() const;
    std::string const& get_value() const;

    bool has_value() const;

    void add_entry(config_entry const& entry);

    config_entry get_entry(std::string const& path) const;
    config_entry_set get_entries(std::string const& path) const;

private:
    std::shared_ptr<config_entry_impl> impl_;
};

template <typename T>
T config_entry::as() const
{
    std::string const& value = get_value();

    try
    {
        return boost::lexical_cast<T>(value);
    }
    catch(boost::bad_lexical_cast const&)
    {
        throw config_error("cannot convert entry '" + get_name() + "' value '" + value + "' to " + typeid(T).name());
    }
}

template <>
inline bool config_entry::as() const
{
    std::string const& value = get_value();

    if(value == strings::true_)
        return true;

    if(value == strings::false_)
        return false;

    try
    {
        return boost::lexical_cast<bool>(value);
    }
    catch(boost::bad_lexical_cast const&)
    {
        throw config_error("cannot convert entry '" + get_name() + "' value '" + value + "' to bool");
    }
}

template <>
inline std::string config_entry::as() const
{
    return get_value();
}

template <typename T>
T config_entry::as(T const& default_value) const
{
    return has_value() ? as<T>() : default_value;
}

template <typename T>
T config_entry::get(std::string const& path) const
{
    return get_entry(path).as<T>();
}

template <typename T>
T config_entry::get(std::string const& path, T const& default_value) const
{
    return get_entry(path).as<T>(default_value);
}

}   // namespace chat

#endif  // _chat_config_entry_hpp
