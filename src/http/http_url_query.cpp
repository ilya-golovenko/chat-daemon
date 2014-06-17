//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 Ilya Golovenko
//    This file is part of libsphttp.
//
//    libsphttp is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    libsphttp is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with libsphttp. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include "http_url_query.hpp"


namespace http
{

const std::string url_query::true_   = "true";
const std::string url_query::false_  = "false";

url_query::url_query()
{
}

url_query::url_query(const url_query& other)
{
    assign(other);
}

url_query& url_query::operator=(const url_query& other)
{
    assign(other);
    return *this;
}

void url_query::assign(const url_query& other)
{
    if(&other != this)
        parameters_ = other.parameters_;
}

const std::string& url_query::get_value(const std::string& name) const
{
    parameter_map_type::const_iterator it = parameters_.find(name);

    if(it == parameters_.end())
        throw std::runtime_error("cannot find parameter: " + name);

    return it->second;
}

void url_query::add(const std::string& name, const std::string& value)
{
    parameters_.insert(std::make_pair(name, value));
}

void url_query::erase(const std::string& name)
{
    parameters_.erase(name);
}

void url_query::clear()
{
    parameters_.clear();
}

bool url_query::has(const std::string& name) const
{
    return (parameters_.find(name) != parameters_.end());
}

}   // namespace http
