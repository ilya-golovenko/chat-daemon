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
#ifndef _chat_misc_hash_utils_hpp
#define _chat_misc_hash_utils_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio.hpp>

// STL headers
#include <functional>
#include <string>


namespace std
{

template <>
struct hash<asio::ip::address> : std::unary_function<asio::ip::address, std::size_t>
{
    std::size_t operator()(asio::ip::address const& address) const
    {
        return std::hash<std::string>()(address.to_string());
    }
};

}   // namespace std

#endif  // _chat_misc_hash_utils_hpp
