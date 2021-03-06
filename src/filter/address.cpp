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

// Application headers
#include <filter/address.hpp>

// BOOST headers
#include <boost/lexical_cast.hpp>

// STL headers
#include <stdexcept>


namespace
{

asio::ip::address parse_address_string(std::string const& str)
{
    asio::error_code error_code;

    asio::ip::address const address = asio::ip::address::from_string(str, error_code);

    if(error_code)
    {
        throw std::runtime_error("invalid IP address: " + str);
    }

    return address;
}

asio::ip::address_v4 parse_address_v4_string(std::string const& str)
{
    asio::error_code error_code;

    asio::ip::address_v4 const address = asio::ip::address_v4::from_string(str, error_code);

    if(error_code)
    {
        throw std::runtime_error("invalid IPv4 address: " + str);
    }

    return address;
}

bool is_same_subnet(asio::ip::address_v4 const& address1, asio::ip::address_v4 const& address2, asio::ip::address_v4 const& netmask)
{
    return (address1.to_ulong() & netmask.to_ulong()) == (address2.to_ulong() & netmask.to_ulong());
}

bool is_same_subnet(asio::ip::address_v6 const& address1, asio::ip::address_v6 const& address2, std::size_t prefix_length)
{
    asio::ip::address_v6::bytes_type const& address1_bytes = address1.to_bytes();
    asio::ip::address_v6::bytes_type const& address2_bytes = address2.to_bytes();

    std::size_t const prefix_bytes_size = prefix_length / 8;
    std::size_t const prefix_bits_size = prefix_length % 8;

    for(std::size_t i = 0; i < prefix_bytes_size; ++i)
    {
        if(address1_bytes[i] != address2_bytes[i])
        {
            return false;
        }
    }

    unsigned char const mask = ~static_cast<unsigned char>((1 << (8 - prefix_bits_size)) - 1);

    return (address1_bytes[prefix_bytes_size] & mask) == (address2_bytes[prefix_bytes_size] & mask);
}

}   // namespace

namespace chat
{
namespace filter
{

address address::from_string(std::string const& str)
{
    std::size_t const pos = str.find('/');

    std::string const address_string = str.substr(0, pos);

    address address(::parse_address_string(address_string));

    if(pos != std::string::npos)
    {
        std::size_t prefix_length;

        std::string const prefix_string = str.substr(pos + 1);

        if(!boost::conversion::try_lexical_convert(prefix_string, prefix_length))
        {
            address.set_netmask(::parse_address_v4_string(prefix_string));
        }
        else
        {
            address.set_prefix_length(prefix_length);
        }
    }

    return address;
}

address::address(asio::ip::address const& address):
    address_(address)
{
}

void address::set_netmask(asio::ip::address_v4 const& netmask)
{
    if(address_.is_v6())
    {
        throw std::runtime_error("cannot set netmask for IPv6 address");
    }

    netmask_ = netmask;
    prefix_length_ = boost::none;
}

void address::set_prefix_length(std::size_t prefix_length)
{
    if(address_.is_v4())
    {
        if(prefix_length > 32u)
        {
            throw std::runtime_error("invalid IPv4 prefix length: " + std::to_string(prefix_length));
        }

        std::size_t const shift = prefix_length < 32 ? 32 - prefix_length : 0;
        unsigned long const netmask = shift < 32 ? ~((1UL << shift) - 1) : 0UL;

        netmask_ = asio::ip::address_v4( netmask );
        prefix_length_ = boost::none;
    }
    else
    {
        if(prefix_length > 128u)
        {
            throw std::runtime_error("invalid IPv6 prefix length: " + std::to_string(prefix_length));
        }

        prefix_length_ = prefix_length;
        netmask_ = boost::none;
    }
}

bool address::satisfies(asio::ip::address const& address) const
{
    if(address_.is_v4())
    {
        if(netmask_)
        {
            if(address.is_v4())
            {
                return ::is_same_subnet(address_.to_v4(), address.to_v4(), *netmask_);
            }
            else
            {
                asio::ip::address_v6 const address_v6 = address.to_v6();

                if(address_v6.is_v4_mapped() || address_v6.is_v4_compatible())
                {
                    return ::is_same_subnet(address_.to_v4(), address_v6.to_v4(), *netmask_);
                }
            }
        }
    }
    else
    {
        if(prefix_length_)
        {
            if(address.is_v6())
            {
                return ::is_same_subnet(address_.to_v6(), address.to_v6(), *prefix_length_);
            }
        }
    }

    return address_ == address;
}

}   // namespace filter
}   // namespace chat
