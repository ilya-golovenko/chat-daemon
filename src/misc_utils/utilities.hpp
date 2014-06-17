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
#ifndef _utilities_hpp
#define _utilities_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/functional/hash.hpp>
#include <boost/array.hpp>

// STL headers
#include <vector>
#include <string>
#include <ctime>


namespace util
{

boost::uint64_t get_milli_time();

const std::string get_os_name();
const std::string get_os_info();

struct address_hash :
    public std::unary_function<asio::ip::address, std::size_t>
{
    template <typename T, std::size_t N>
    std::size_t hash_value(const boost::array<T, N>& array) const
    {
        return boost::hash_range(array.begin(), array.end());
    }

    std::size_t operator()(const asio::ip::address& address) const
    {
        if(address.is_v4())
            return hash_value(address.to_v4().to_bytes());
        return hash_value(address.to_v6().to_bytes());
    }
};

const std::string& get_random_string(const std::vector<std::string>& strings);

const std::string errno_to_string(const std::string& function, int errnum);
const std::string file_errno_to_string(const std::string& function, const std::string& filename, int errnum);

}   // namespace util

#endif  // _utilities_hpp
