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
#include <config/config_exception.hpp>


namespace chat
{

config_error::config_error(std::string const& message) :
    message_(message),
    line_(-1),
    column_(-1)
{
}

config_error::config_error(std::string const& message, int line, int column) :
    message_(message),
    line_(line),
    column_(column)
{
}

config_error::~config_error() throw()
{
}

char const* config_error::what() const throw()
{
    return message_.c_str();
}

std::string const& config_error::message() const throw()
{
    return message_;
}

int config_error::line() const throw()
{
    return line_;
}

int config_error::column() const throw()
{
    return column_;
}

}   // namespace chat
