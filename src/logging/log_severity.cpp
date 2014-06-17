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

// Application headers
#include "log_severity.hpp"

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <stdexcept>
#include <vector>


log_severity log_severity::from_string(const std::string& string)
{
    int severity_value = none;
    std::vector<std::string> strings;

    boost::split(strings, string, boost::is_space() ||
        boost::is_any_of(","), boost::token_compress_on);

    for(std::size_t i = 0; i < strings.size(); ++i)
    {
        if(strings[i] == "all")
            severity_value = all;
        else if(strings[i] == "none")
            severity_value = none;
        else if(strings[i] == "notice")
            severity_value |= notice;
        else if(strings[i] == "warning")
            severity_value |= warning;
        else if(strings[i] == "error")
            severity_value |= error;
        else if(strings[i] == "debug")
            severity_value |= debug;
    }

    return static_cast<severities_type>(severity_value);
}

log_severity::log_severity() :
    value_(none)
{
}

log_severity::log_severity(const log_severity& other) :
    value_(other.value_)
{
}

log_severity::log_severity(severities_type severity_value) :
    value_(severity_value)
{
}

log_severity& log_severity::operator=(const log_severity& other)
{
    if(&other != this)
        value_ = other.value_;
    return *this;
}

log_severity& log_severity::operator=(severities_type severity_value)
{
    value_ = severity_value;
    return *this;
}

log_severity::severities_type log_severity::value() const
{
    return value_;
}

log_severity::operator log_severity::severities_type() const
{
    return value_;
}

const std::string log_severity::to_string() const
{
    switch(value_)
    {
        case none:
            return "none";

        case notice:
            return "notice";

        case warning:
            return "warning";

        case error:
            return "error";

        case debug:
            return "debug";

        case all:
            return "all";
    }

    return "none";
}

log_severity log_severity::operator+(const log_severity& other)
{
    int severity_value = static_cast<int>(value_) | other.value();
    return static_cast<severities_type>(severity_value);
}

log_severity log_severity::operator-(const log_severity& other)
{
    int severity_value = static_cast<int>(value_) & ~other.value();
    return static_cast<severities_type>(severity_value);
}

log_severity& log_severity::operator+=(const log_severity& other)
{
    int severity_value = static_cast<int>(value_) | other.value();
    value_ = static_cast<severities_type>(severity_value);
    return *this;
}

log_severity& log_severity::operator-=(const log_severity& other)
{
    int severity_value = static_cast<int>(value_) & ~other.value();
    value_ = static_cast<severities_type>(severity_value);
    return *this;
}
