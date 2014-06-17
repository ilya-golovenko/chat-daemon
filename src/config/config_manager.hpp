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
#ifndef _config_manager_hpp
#define _config_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "config_preprocessor.hpp"
#include "config_parser.hpp"
#include "config_entry_set.hpp"
#include "config_entry.hpp"

// BOOST headers
#include <boost/noncopyable.hpp>

// STL headers
#include <string>


class config_manager :
    private boost::noncopyable
{
public:
    static config_manager& instance();

public:
    void load_configuration(const std::string& filename);

    template <typename T>
    const T get(const std::string& path) const;

    template <typename T>
    const T get(const std::string& path, const T& default_value) const;

    template <typename T>
    const std::vector<T> get_vector(const std::string& path) const;

    template <typename T>
    const std::vector<T> get_vector(const std::string& path, const T& default_value) const;

    const config_entry get_entry(const std::string& path) const;
    const config_entry_set get_entries(const std::string& path) const;

private:
    config_entry root_;
};

template <typename T>
const T config_manager::get(const std::string& path) const
{
    config_entry entry = get_entry(path);
    return entry.as<T>();
}

template <typename T>
const T config_manager::get(const std::string& path, const T& default_value) const
{
    config_entry entry = get_entry(path);
    return entry.as<T>(default_value);
}

template <typename T>
const std::vector<T> config_manager::get_vector(const std::string& path) const
{
    config_entry_set entries = get_entries(path);
    return entries.as<T>();
}

template <typename T>
const std::vector<T> config_manager::get_vector(const std::string& path, const T& default_value) const
{
    config_entry_set entries = get_entries(path);
    return entries.as<T>(default_value);
}

#endif  // _config_manager_hpp
