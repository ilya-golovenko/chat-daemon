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
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "http_utilities.hpp"

// BOOST headers
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
    url_query();

    url_query(const url_query& other);
    url_query& operator=(const url_query& other);

    void assign(const url_query& other);

    const std::string& get_value(const std::string& name) const;
    void add(const std::string& name, const std::string& value);

    void erase(const std::string& name);
    void clear();

    bool has(const std::string& name) const;

    template <typename T>
    inline const T get(const std::string& name) const;

private:
    typedef std::map<std::string, std::string,
        http_util::string_iless> parameter_map_type;

private:
    static const std::string true_;
    static const std::string false_;

private:
    parameter_map_type parameters_;
};

template <typename T>
inline const T url_query::get(const std::string& name) const
{
    const std::string& value = get_value(name);

    try
    {
        return boost::lexical_cast<T>(value);
    }
    catch(boost::bad_lexical_cast&)
    {
        throw std::runtime_error("cannot convert parameter '" + name
            + "' value '" + value + "' to " + typeid(T).name());
    }
}

template <>
inline const bool url_query::get(const std::string& name) const
{
    const std::string& value = get_value(name);

    if(value == true_)
        return true;

    if(value == false_)
        return false;

    return (get<int>(name) ? true : false);
}

template <>
inline const std::string url_query::get(const std::string& name) const
{
    return get_value(name);
}

}   // namespace http

#endif  // _http_url_query_hpp
