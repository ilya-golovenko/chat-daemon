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
#ifndef _log_console_handler_hpp
#define _log_console_handler_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "log_handler.hpp"
#include <platform.hpp>

// ASIO headers
#include <asio.hpp>


class log_console_handler :
    public log_handler
{
public:
    virtual ~log_console_handler();
    virtual void add_message(const log_message& message);

private:
#if defined(_WINDOWS)
    WORD get_text_color() const;
    void set_text_color(WORD text_color);
#endif
private:
    static asio::detail::mutex mutex_;
};

#endif  // _log_console_handler_hpp
