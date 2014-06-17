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
#include "log_message.hpp"
#include "log_manager.hpp"


log_message::log_message(log_severity severity, const std::string& format) :
    severity_(severity),
    formatter_(format),
    copied_(false)
{
    formatter_.exceptions(boost::io::no_error_bits);
    time_ = boost::posix_time::second_clock::local_time();
}

log_message::log_message(log_severity severity, const std::string& source, const std::string& format) :
    severity_(severity),
    formatter_(format),
    source_(source),
    copied_(false)
{
    formatter_.exceptions(boost::io::no_error_bits);
    time_ = boost::posix_time::second_clock::local_time();
}

log_message::log_message(const log_message& other) :
    time_(other.time_),
    severity_(other.severity_),
    formatter_(other.formatter_),
    source_(other.source_),
    copied_(true)
{
}

log_message& log_message::operator=(const log_message& other)
{
    if(&other != this)
    {
        time_ = other.time_;
        severity_ = other.severity_;
        formatter_ = other.formatter_;
        source_ = other.source_;
        copied_ = true;
    }

    return *this;
}

log_message::~log_message()
{
    if(!copied_)
        log_manager::instance().add_message(*this);
}

bool log_message::has_source() const
{
    return !source_.empty();
}

log_severity log_message::severity() const
{
    return severity_;
}

const std::string& log_message::source() const
{
    return source_;
}

boost::posix_time::ptime log_message::time() const
{
    return time_;
}

const std::string log_message::get_text() const
{
    return formatter_.str();
}
