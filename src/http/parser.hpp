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
#ifndef _http_parser_hpp
#define _http_parser_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/logic/tribool.hpp>

// STL headers
#include <cstdint>
#include <utility>


namespace http
{

class parser_base
{
public:
    parser_base() = default;
    ~parser_base() = default;

    parser_base(parser_base const&) = delete;
    parser_base& operator=(parser_base const&) = delete;

public:
    static bool is_char(std::uint8_t c);
    static bool is_digit(std::uint8_t c);
    static bool is_xdigit(std::uint8_t c);
    static bool is_control(std::uint8_t c);

public:
    static const std::size_t max_message_size        = 32768u;
    static const std::size_t max_header_name_length  = 1024u;
    static const std::size_t max_header_value_length = 8192u;
    static const std::size_t max_chunk_size_length   = 1024u;
};

template <typename Parser>
class basic_parser : public parser_base
{
public:
    basic_parser() = default;
    ~basic_parser() = default;

    basic_parser(basic_parser const&) = delete;
    basic_parser& operator=(basic_parser const&) = delete;

    template <typename Iterator>
    std::pair<boost::tribool, Iterator> parse(Iterator begin, Iterator end);

    template <typename Message, typename Iterator>
    std::pair<boost::tribool, Iterator> parse(Message& message, Iterator begin, Iterator end);
};

template <typename Parser>
template <typename Iterator>
std::pair<boost::tribool, Iterator> basic_parser<Parser>::parse(Iterator begin, Iterator end)
{
    boost::tribool result = boost::indeterminate;
    Parser& impl = static_cast<Parser&>(*this);

    while(begin != end)
    {
        result = impl.consume(*begin++);

        if(result || !result)
        {
            impl.reset();
            break;
        }
    }

    return std::make_pair(result, begin);
}

template <typename Parser>
template <typename Message, typename Iterator>
std::pair<boost::tribool, Iterator> basic_parser<Parser>::parse(Message& message, Iterator begin, Iterator end)
{
    boost::tribool result = boost::indeterminate;
    Parser& impl = static_cast<Parser&>(*this);

    while(begin != end)
    {
        result = impl.consume(message, *begin++);

        if(result || !result)
        {
            impl.reset();
            break;
        }
    }

    return std::make_pair(result, begin);
}

}   // namespace http

#endif  // _http_parser_hpp
