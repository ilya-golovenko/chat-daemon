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
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// BOOST headers
#include <boost/logic/tribool.hpp>
#include <boost/noncopyable.hpp>


namespace http
{
namespace detail
{

static const char char_table[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0
};

}   // namespace detail

template <typename Parser>
class parser :
    private boost::noncopyable
{
public:
    template <typename InpIt>
    std::pair<boost::tribool, InpIt> parse(InpIt begin, InpIt end);

    template <typename Message, typename InpIt>
    std::pair<boost::tribool, InpIt> parse(Message& message, InpIt begin, InpIt end);

public:
    static inline bool is_char(unsigned char c);
    static inline bool is_digit(unsigned char c);
    static inline bool is_xdigit(unsigned char c);
    static inline bool is_control(unsigned char c);

public:
    static const std::size_t max_message_size           = 32768;
    static const std::size_t max_header_name_length     = 1024;
    static const std::size_t max_header_value_length    = 8192;
    static const std::size_t max_chunk_size_length      = 1024;
};

template <typename Parser>
template <typename InpIt>
std::pair<boost::tribool, InpIt> parser<Parser>::parse(InpIt begin, InpIt end)
{
    boost::tribool result = boost::indeterminate;
    Parser* impl = static_cast<Parser*>(this);

    while(begin != end)
    {
        result = impl->consume(*begin++);

        if(result || !result)
        {
            impl->reset();
            break;
        }
    }

    return std::make_pair(result, begin);
}

template <typename Parser>
template <typename Message, typename InpIt>
std::pair<boost::tribool, InpIt> parser<Parser>::parse(Message& message, InpIt begin, InpIt end)
{
    boost::tribool result = boost::indeterminate;
    Parser* impl = static_cast<Parser*>(this);

    while(begin != end)
    {
        result = impl->consume(message, *begin++);

        if(result || !result)
        {
            impl->reset();
            break;
        }
    }

    return std::make_pair(result, begin);
}

template <typename Parser>
inline bool parser<Parser>::is_char(unsigned char c)
{
    return (c <= 127 && !!detail::char_table[c]);
}

template <typename Parser>
inline bool parser<Parser>::is_digit(unsigned char c)
{
    return (c >= '0' && c <= '9');
}

template <typename Parser>
inline bool parser<Parser>::is_xdigit(unsigned char c)
{
    return ((c >= '0' && c <= '9')
        || (c >= 'a' && c <= 'f')
        || (c >= 'A' && c <= 'F'));
}

template <typename Parser>
inline bool parser<Parser>::is_control(unsigned char c)
{
    return (c <= 31 || c == 127);
}

}   // namespace http

#endif  // _http_parser_hpp
