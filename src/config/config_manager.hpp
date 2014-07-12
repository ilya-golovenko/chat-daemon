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
#ifndef _chat_config_manager_hpp
#define _chat_config_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <config/config_preprocessor.hpp>
#include <config/config_parser.hpp>
#include <config/config_entry_set.hpp>
#include <config/config_entry.hpp>

// STL headers
#include <string>


namespace chat
{

class config_manager
{
public:
    static config_manager& instance();

public:
    config_manager() = default;
    ~config_manager() = default;

    config_manager(config_manager const&) = delete;
    config_manager& operator=(config_manager const&) = delete;

    void load_configuration(std::string const& filename);

    template <typename T>
    T get(std::string const& path) const;

    template <typename T>
    T get(std::string const& path, T const& default_value) const;

    template <typename T>
    std::vector<T> get_vector(std::string const& path) const;

    template <typename T>
    std::vector<T> get_vector(std::string const& path, T const& default_value) const;

    config_entry get_entry(std::string const& path) const;
    config_entry_set get_entries(std::string const& path) const;

private:
    config_entry root_;
};

template <typename T>
T config_manager::get(std::string const& path) const
{
    return get_entry(path).as<T>();
}

template <typename T>
T config_manager::get(std::string const& path, T const& default_value) const
{
    return get_entry(path).as<T>(default_value);
}

template <typename T>
std::vector<T> config_manager::get_vector(std::string const& path) const
{
    return get_entries(path).as<T>();
}

template <typename T>
std::vector<T> config_manager::get_vector(std::string const& path, T const& default_value) const
{
    return get_entries(path).as<T>(default_value);
}

}   // namespace chat

#endif  // _chat_config_manager_hpp
