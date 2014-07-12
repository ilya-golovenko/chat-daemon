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
#ifndef _http_url_query_hpp
#define _http_url_query_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <http/utilities.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

// STL headers
#include <stdexcept>
#include <string>
#include <map>


namespace http
{

class url_query
{
public:
    url_query() = default;

    url_query(url_query const&) = default;
    url_query& operator=(url_query const&) = default;

    void add(std::string const& name, std::string const& value);

    template <typename T>
    void add(std::string const& name, T const& value);

    std::string const& get(std::string const& name) const;

    template <typename T>
    T get(std::string const& name) const;

    bool has(std::string const& name) const;

    void erase(std::string const& name);
    void clear();


private:
    typedef std::map<std::string, std::string, string_iless> parameter_map;

private:
    static char const true_[];
    static char const false_[];

private:
    parameter_map parameters_;
};

template <typename T>
void url_query::add(std::string const& name, T const& value)
{
    add(name, boost::lexical_cast<std::string>(value));
}

template <>
inline void url_query::add(std::string const& name, bool const& value)
{
    add(name, value ? std::string(true_) : std::string(false_));
}

template <>
inline void url_query::add(std::string const& name, std::string const& value)
{
    add(name, value);
}

template <typename T>
T url_query::get(std::string const& name) const
{
    std::string const& value = get(name);

    try
    {
        return boost::lexical_cast<T>(value);
    }
    catch(boost::bad_lexical_cast const&)
    {
        throw std::runtime_error("cannot convert parameter '" + name + "' value '" + value + "' to " + typeid(T).name());
    }
}

template <>
inline bool url_query::get(std::string const& name) const
{
    std::string const& value = get(name);

    if(boost::algorithm::iequals(value, std::string(true_)))
        return true;

    if(boost::algorithm::iequals(value, std::string(false_)))
        return false;

    return get<int>(name) ? true : false;
}

template <>
inline std::string url_query::get(std::string const& name) const
{
    return get(name);
}

}   // namespace http

#endif  // _http_url_query_hpp
