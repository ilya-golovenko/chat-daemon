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
#include "log_console_handler.hpp"
#include <win_utils.hpp>

// STL headers
#include <iostream>


asio::detail::mutex log_console_handler::mutex_;

log_console_handler::~log_console_handler()
{
}

#if defined(_WINDOWS)

namespace colors
{

const WORD green    = FOREGROUND_GREEN;
const WORD red      = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD yellow   = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

}   // namespace colors

void log_console_handler::add_message(const log_message& message)
{
    asio::detail::mutex::scoped_lock lock(mutex_);
    log_severity severity = message.severity();

    WORD old_color = get_text_color();
    bool has_custom_color = false;

    if(severity == log_severity::error)
    {
        set_text_color(colors::red);
        has_custom_color = true;
    }
    else if(severity == log_severity::warning)
    {
        set_text_color(colors::yellow);
        has_custom_color = true;
    }

    std::cout << util::win32_char_to_oem(
        message.severity().to_string()) << '\t';

    if(message.has_source())
    {
        if(!has_custom_color)
            set_text_color(colors::green);

        std::cout << util::win32_char_to_oem(message.source()) << ": ";

        if(!has_custom_color)
            set_text_color(old_color);
    }

    std::cout << util::win32_char_to_oem(message.get_text()) << std::endl;

    if(has_custom_color)
        set_text_color(old_color);
}

WORD log_console_handler::get_text_color() const
{
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

    HANDLE std_out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(std_out_handle, &screen_buffer_info);

    return screen_buffer_info.wAttributes;
}

void log_console_handler::set_text_color(WORD text_color)
{
    HANDLE std_out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(std_out_handle, text_color);
}

#else // #if !defined(_WINDOWS)

void log_console_handler::add_message(const log_message& message)
{
    asio::detail::mutex::scoped_lock lock(mutex_);

    std::cout << message.severity() << '\t';

    if(message.has_source())
        std::cout << message.source() << ": ";

    std::cout << message.get_text() << std::endl;
}

#endif  // defined(_WINDOWS)
