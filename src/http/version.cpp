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
#include <http/version.hpp>

// MISSIO headers
#include <missio/format/print.hpp>


namespace http
{

version const version::http_1_0(1u, 0u);
version const version::http_1_1(1u, 1u);

version::version() :
    major_(1u),
    minor_(1u),
    str_("HTTP/1.1")
{
}

version::version(std::uint8_t major, std::uint8_t minor) :
    major_(major),
    minor_(minor)
{
    missio::format::print(str_, "HTTP/{0}.{1}", major_, minor_);
}

std::uint8_t version::get_major() const
{
    return major_;
}

std::uint8_t version::get_minor() const
{
    return minor_;
}

std::string const& version::to_string() const
{
    return str_;
}

bool operator==(version const& lhs, version const& rhs)
{
    return lhs.major_ == rhs.major_ && lhs.minor_ == rhs.minor_;
}

bool operator<(version const& lhs, version const& rhs)
{
    return lhs.major_ == rhs.major_ ? lhs.minor_ < rhs.minor_ : lhs.major_ < rhs.major_;
}

}   // namespace http
