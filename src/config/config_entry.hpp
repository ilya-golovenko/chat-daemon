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
#ifndef _config_entry_hpp
#define _config_entry_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "config_exception.hpp"
#include <string_utils.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/shared_ptr.hpp>

// STL headers
#include <typeinfo>
#include <string>
#include <vector>
#include <map>


class config_entry;
class config_entry_impl;
class config_entry_set;

class config_entry
{
public:
    config_entry();
    explicit config_entry(const std::string& name);
    config_entry(const std::string& name, const std::string& value);

    config_entry(const config_entry& other);
    config_entry& operator=(const config_entry& other);

    template <typename T>
    inline const T as() const;

    template <typename T>
    inline const T as(const T& default_value) const;

    template <typename T>
    inline const T get(const std::string& path) const;

    template <typename T>
    inline const T get(const std::string& path, const T& default_value) const;

    const std::string& get_name() const;
    const std::string& get_value() const;

    bool has_value() const;

    void add_entry(const config_entry& entry);

    const config_entry get_entry(const std::string& path) const;
    const config_entry_set get_entries(const std::string& path) const;

private:
    boost::shared_ptr<config_entry_impl> impl_;
};

template <typename T>
inline const T config_entry::as() const
{
    const std::string& value = get_value();

    try
    {
        return util::to_type<T>(value);
    }
    catch(boost::bad_lexical_cast&)
    {
        throw config_error("cannot convert entry '" + get_name()
            + "' value '" + value + "' to " + typeid(T).name());
    }
}

template <>
inline const bool config_entry::as() const
{
    const std::string& value = get_value();

    if(value == strings::true_)
        return true;

    if(value == strings::false_)
        return false;

    try
    {
        return util::to_type<bool>(value);
    }
    catch(boost::bad_lexical_cast&)
    {
        throw config_error("cannot convert entry '" +
            get_name() + "' value '" + value + "' to bool");
    }
}

template <>
inline const std::string config_entry::as() const
{
    return get_value();
}

template <typename T>
inline const T config_entry::as(const T& default_value) const
{
    return (has_value() ? as<T>() : default_value);
}

template <typename T>
inline const T config_entry::get(const std::string& path) const
{
    return get_entry(path).as<T>();
}

template <typename T>
inline const T config_entry::get(const std::string& path, const T& default_value) const
{
    return get_entry(path).as<T>(default_value);
}

#endif  // _config_entry_hpp
