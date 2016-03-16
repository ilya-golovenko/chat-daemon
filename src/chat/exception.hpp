//---------------------------------------------------------------------------
//
//    Copyright (C) 2008 - 2016 Ilya Golovenko
//    This file is part of Chat.Daemon project
//
//    spchatd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spchatd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spchatd. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------
#ifndef _chat_exception_hpp
#define _chat_exception_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// MISSIO headers
#include <missio/format/write.hpp>

// STL headers
#include <exception>
#include <string>


namespace chat
{

class exception : public std::exception
{
public:
    explicit exception(std::string const& message);

    template <typename ...Args>
    explicit exception(Args const& ...args);

    virtual ~exception() throw();

    exception(exception const&) = default;
    exception& operator=(exception const&) = default;

    virtual char const* what() const throw();

private:
    std::string message_;
};

template <typename ...Args>
exception::exception(Args const& ...args)
{
    missio::format::write(message_, args...);
}

}   // namespace chat

#endif  // _chat_exception_hpp
