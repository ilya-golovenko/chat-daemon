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
#include "config_exception.hpp"


config_error::config_error(const std::string& message) :
    message_(message),
    line_(-1),
    column_(-1)
{
}

config_error::config_error(const std::string& message, int line, int column) :
    message_(message),
    line_(line),
    column_(column)
{
}

config_error::config_error(const config_error& other) :
    std::exception(other)
{
    assign(other);
}

config_error& config_error::operator=(const config_error& other)
{
    assign(other);
    return *this;
}

config_error::~config_error() throw()
{
}

void config_error::assign(const config_error& other)
{
    if(&other != this)
    {
        message_ = other.message_;
        line_ = other.line_;
        column_ = other.column_;
    }
}

const char* config_error::what() const throw()
{
    return message_.c_str();
}

const std::string& config_error::message() const throw()
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
