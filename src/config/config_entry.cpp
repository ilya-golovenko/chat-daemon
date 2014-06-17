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
#include "config_entry.hpp"
#include "config_entry_set.hpp"
#include "config_entry_impl.hpp"

// BOOST headers
#include <boost/make_shared.hpp>


config_entry::config_entry() :
    impl_(boost::make_shared<config_entry_impl>())
{
}

config_entry::config_entry(const std::string& name) :
    impl_(boost::make_shared<config_entry_impl>(name))
{
}

config_entry::config_entry(const std::string& name, const std::string& value) :
    impl_(boost::make_shared<config_entry_impl>(name, value))
{
}

config_entry::config_entry(const config_entry& other) :
    impl_(other.impl_)
{
}

config_entry& config_entry::operator=(const config_entry& other)
{
    if(&other != this)
        impl_ = other.impl_;

    return *this;
}

const std::string& config_entry::get_name() const
{
    return impl_->get_name();
}

const std::string& config_entry::get_value() const
{
    return impl_->get_value();
}

bool config_entry::has_value() const
{
    return impl_->has_value();
}

void config_entry::add_entry(const config_entry& entry)
{
    impl_->add_entry(entry);
}

const config_entry config_entry::get_entry(const std::string& path) const
{
    return impl_->get_entry(path);
}

const config_entry_set config_entry::get_entries(const std::string& path) const
{
    return impl_->get_entries(path);
}
