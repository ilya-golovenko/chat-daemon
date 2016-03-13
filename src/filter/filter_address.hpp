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
#ifndef _chat_filter_address_hpp
#define _chat_filter_address_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/optional.hpp>

// ASIO headers
#include <asio.hpp>

// STL headers
#include <cstdint>
#include <string>


namespace chat
{

class filter_address
{
public:
    static filter_address from_string(std::string const& str);

public:
    explicit filter_address(asio::ip::address const& address);

    filter_address(filter_address const&) = default;
    filter_address& operator=(filter_address const&) = default;

    filter_address(filter_address&&) = default;
    filter_address& operator=(filter_address&&) = default;

    void set_netmask(asio::ip::address const& netmask);
    void set_prefix_length(std::size_t prefix_length);

    bool satisfies(asio::ip::address const& address) const;

private:
    asio::ip::address address_;
    boost::optional<std::size_t> prefix_length_;
    boost::optional<asio::ip::address_v4> netmask_;
};

}   // namespace chat

#endif  // _chat_filter_address_hpp
