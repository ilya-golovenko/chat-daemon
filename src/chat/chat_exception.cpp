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
#include "chat_exception.hpp"


namespace chat
{

error::error(const std::string& message) :
    message_(message)
{
}

error::error(const error& other) :
    std::exception(other)
{
    assign(other);
}

error& error::operator=(const error& other)
{
    assign(other);
    return *this;
}

error::~error() throw()
{
}

void error::assign(const error& other)
{
    if(&other != this)
        message_ = other.message_;
}

const char* error::what() const throw()
{
    return message_.c_str();
}

const std::string& error::message() const throw()
{
    return message_;
}

}   // namespace chat
