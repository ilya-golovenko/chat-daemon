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
#ifndef _chat_config_exception_hpp
#define _chat_config_exception_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <exception>
#include <string>


namespace chat
{

class config_error : public std::exception
{
public:
    explicit config_error(std::string const& message);
    config_error(std::string const& message, int line, int column);

    config_error(config_error const&) = default;
    config_error& operator=(config_error const&) = default;

    virtual ~config_error() throw();

    virtual const char* what() const throw();
    std::string const& message() const throw();

    int line() const throw();
    int column() const throw();

private:
    std::string message_;

    int line_;
    int column_;
};

}   // namespace chat

#endif  // _chat_config_exception_hpp
