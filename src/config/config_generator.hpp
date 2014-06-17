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
#ifndef _config_generator_hpp
#define _config_generator_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "config_common.hpp"
#include "config_entry.hpp"

// BOOST headers
#include <boost/noncopyable.hpp>

// STL headers
#include <string>


class config_generator :
    private boost::noncopyable
{
public:
    config_generator();

    void generate_config(const std::string& filename);
    void save_config_file(const std::string& filename);

private:
    void add_simple_entry(const std::string& name, const std::string& value);
    void add_quoted_entry(const std::string& name, const std::string& value);

    void start_new_block(const std::string& name);
    void end_current_block();

    void indent_output();

    const std::string find_string(const std::string& name) const;
    const std::string find_number(const std::string& name) const;

    const std::string find_value(const std::string& name, const std::string& pattern) const;

private:
    std::string config_data_;
    std::ostringstream config_;
    std::size_t nesting_depth_;
};

#endif  // _config_generator_hpp
