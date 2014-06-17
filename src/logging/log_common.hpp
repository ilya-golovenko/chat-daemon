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
#ifndef _log_common_hpp
#define _log_common_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "log_severity.hpp"
#include "log_message.hpp"
#include "log_source.hpp"


#define LOGNOT(text)        create_log_message(log_severity::notice, text)
#define LOGWRN(text)        create_log_message(log_severity::warning, text)
#define LOGERR(text)        create_log_message(log_severity::error, text)

#define DEFINE_LOG_SOURCE_NAME(name) \
public: \
    static const char* get_log_source_name() \
    { \
        return (name); \
    }

#if defined(DEBUG)
  #if !defined(ENABLE_DEBUG_LOGGING)
    #define ENABLE_DEBUG_LOGGING
  #endif
#endif

#if !defined(ENABLE_DEBUG_LOGGING)

struct null_message
{
    template <typename T>
    inline null_message& operator <<(const T&)
    {
        return *this;
    }
};

#define LOGDBG(text)        null_message()

#else

#define LOGDBG(text)        create_log_message(log_severity::debug, text)

#endif  // defined(ENABLE_DEBUG_LOGGING)

#endif  // _log_common_hpp
