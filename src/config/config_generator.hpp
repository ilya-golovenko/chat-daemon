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
#ifndef _chat_config_generator_hpp
#define _chat_config_generator_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <config/config_common.hpp>
#include <config/config_entry.hpp>

// STL headers
#include <string>


namespace chat
{

class config_generator
{
public:
    config_generator();

    config_generator(config_generator const&) = delete;
    config_generator& operator=(config_generator const&) = delete;

    void generate_config(std::string const& filename);
    void save_config_file(std::string const& filename);

private:
    void add_simple_entry(std::string const& name, std::string const& value);
    void add_quoted_entry(std::string const& name, std::string const& value);

    void start_new_block(std::string const& name);
    void end_current_block();

    void indent_output();

    std::string find_string(std::string const& name) const;
    std::string find_number(std::string const& name) const;

    std::string find_value(std::string const& name, std::string const& pattern) const;

private:
    std::string config_data_;
    std::ostringstream config_;
    std::size_t nesting_depth_;
};

}   // namespace chat

#endif  // _chat_config_generator_hpp
