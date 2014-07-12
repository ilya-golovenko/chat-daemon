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
#include <config/config_manager.hpp>
#include <misc/file_utils.hpp>


namespace chat
{

config_manager& config_manager::instance()
{
    static config_manager instance;
    return instance;
}

void config_manager::load_configuration(std::string const& filename)
{
    std::string data = util::file::read_text(filename);
    data = config_preprocessor().process(data);
    root_ = config_parser().parse(data);
}

config_entry config_manager::get_entry(std::string const& path) const
{
    return root_.get_entry(path);
}

config_entry_set config_manager::get_entries(std::string const& path) const
{
    return root_.get_entries(path);
}

}   // namespace chat
