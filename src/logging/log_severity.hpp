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
#ifndef _log_severity_hpp
#define _log_severity_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <string>


class log_severity
{
public:
    typedef enum severities_type
    {
        none        = 0,
        notice      = 1,
        warning     = 2,
        error       = 4,
        debug       = 8,
        all         = notice | warning | error | debug
    };

public:
    static log_severity from_string(const std::string& string);

public:
    log_severity();

    log_severity(const log_severity& other);
    log_severity(severities_type severity_value);

    log_severity& operator=(const log_severity& other);
    log_severity& operator=(severities_type severity_value);

    severities_type value() const;
    operator severities_type() const;
    const std::string to_string() const;

    log_severity operator+(const log_severity& other);
    log_severity operator-(const log_severity& other);

    log_severity& operator+=(const log_severity& other);
    log_severity& operator-=(const log_severity& other);

private:
    severities_type value_;
};

inline std::ostream& operator<<(std::ostream& os, const log_severity& severity)
{
    os << severity.to_string();
    return os;
}

#endif  // _log_severity_hpp
