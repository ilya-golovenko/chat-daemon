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
#include <http/url_query.hpp>


namespace http
{

char const url_query::true_[]   = "true";
char const url_query::false_[]  = "false";

void url_query::add(std::string const& name, std::string const& value)
{
    parameters_.emplace(name, value);
}

bool url_query::has(std::string const& name) const
{
    return parameters_.find(name) != parameters_.end();
}

std::string const& url_query::get(std::string const& name) const
{
    parameter_map::const_iterator it = parameters_.find(name);

    if(it == parameters_.end())
        throw std::runtime_error("cannot find parameter: " + name);

    return it->second;
}

void url_query::erase(std::string const& name)
{
    parameters_.erase(name);
}

void url_query::clear()
{
    parameters_.clear();
}

}   // namespace http
