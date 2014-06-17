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
#ifndef _log_message_hpp
#define _log_message_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "log_severity.hpp"

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>

// STL headers
#include <string>


class log_message
{
public:
    log_message(log_severity severity, const std::string& format);
    log_message(log_severity severity, const std::string& source, const std::string& format);

    log_message(const log_message& other);
    log_message& operator=(const log_message& other);

    ~log_message();

    bool has_source() const;

    log_severity severity() const;
    const std::string& source() const;
    boost::posix_time::ptime time() const;

    const std::string get_text() const;

    template<typename T>
    inline log_message& operator<<(const T& value);

private:
    boost::posix_time::ptime time_;
    log_severity severity_;
    boost::format formatter_;
    std::string source_;
    bool copied_;
};

template<typename T>
inline log_message& log_message::operator<<(const T& value)
{
    formatter_ % value;
    return *this;
}

inline log_message create_log_message(log_severity severity, const std::string& format)
{
    return log_message(severity, format);
}

#endif  // _log_message_hpp
