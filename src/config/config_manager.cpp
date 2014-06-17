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

// Application headers
#include "config_manager.hpp"
#include <file_utils.hpp>
#include <singleton.hpp>


config_manager& config_manager::instance()
{
    return util::singleton<config_manager>::instance();
}

void config_manager::load_configuration(const std::string& filename)
{
    std::string data = util::file::read_text(filename);
    data = config_preprocessor().process(data);
    root_ = config_parser().parse(data);
}

const config_entry config_manager::get_entry(const std::string& path) const
{
    return root_.get_entry(path);
}

const config_entry_set config_manager::get_entries(const std::string& path) const
{
    return root_.get_entries(path);
}
