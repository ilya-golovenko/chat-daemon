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
#include <config/config_entry.hpp>
#include <config/config_entry_set.hpp>
#include <config/config_entry_impl.hpp>


namespace chat
{

config_entry::config_entry() :
    impl_(std::make_shared<config_entry_impl>())
{
}

config_entry::config_entry(std::string const& name) :
    impl_(std::make_shared<config_entry_impl>(name))
{
}

config_entry::config_entry(std::string const& name, std::string const& value) :
    impl_(std::make_shared<config_entry_impl>(name, value))
{
}

std::string const& config_entry::get_name() const
{
    return impl_->get_name();
}

std::string const& config_entry::get_value() const
{
    return impl_->get_value();
}

bool config_entry::has_value() const
{
    return impl_->has_value();
}

void config_entry::add_entry(config_entry const& entry)
{
    impl_->add_entry(entry);
}

config_entry config_entry::get_entry(std::string const& path) const
{
    return impl_->get_entry(path);
}

config_entry_set config_entry::get_entries(std::string const& path) const
{
    return impl_->get_entries(path);
}

}   // namespace chat
