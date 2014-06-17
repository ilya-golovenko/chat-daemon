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
#include "http_version.hpp"

// BOOST headers
#include <boost/format.hpp>

// STL headers
#include <stdexcept>


namespace http
{

version version::_1_0(1, 0);
version version::_1_1(1, 1);

version::version() :
    major_(1),
    minor_(1),
    string_("HTTP/1.1")
{
}

version::version(std::size_t major, std::size_t minor) :
    major_(major),
    minor_(minor)
{
    string_ = boost::str(boost::format("HTTP/%1%.%2%") % major_ % minor_);
}

version::version(const version& other) :
    major_(other.major_),
    minor_(other.minor_),
    string_(other.string_)
{
}

version& version::operator=(const version& other)
{
    if(&other != this)
    {
        major_ = other.major_;
        minor_ = other.minor_;
        string_ = other.string_;
    }

    return *this;
}

std::size_t version::get_major() const
{
    return major_;
}

std::size_t version::get_minor() const
{
    return minor_;
}

const std::string& version::to_string() const
{
    return string_;
}

bool operator==(const version& lhs, const version& rhs)
{
    return (lhs.get_major() == rhs.get_major()
        && lhs.get_minor() == rhs.get_minor());
}

bool operator<(const version& lhs, const version& rhs)
{
    return (lhs.get_major() == rhs.get_major()
        ? lhs.get_minor() < rhs.get_minor()
        : lhs.get_major() < rhs.get_major());
}

}   // namespace http
