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
#include "win_utils.hpp"
#include <common.hpp>

// BOOST headers
#include <boost/scoped_array.hpp>

// STL headers
#include <sstream>


namespace util
{

#if defined(_WINDOWS)

const std::string win32_ctrl_name(DWORD ctrl_type)
{
    switch(ctrl_type)
    {
        case CTRL_C_EVENT:
            return std::string("Ctrl+C");

        case CTRL_BREAK_EVENT:
            return std::string("Ctrl+Break");

        case CTRL_CLOSE_EVENT:
            return std::string("Close");
    }

    return strings::unknown;
}

const std::string win32_char_to_oem(const std::string& text)
{
    enum { buffer_size = 256 };
    char static_buffer[buffer_size];
    boost::scoped_array<char> buffer;
    char* pbuffer = static_buffer;

    if(text.size() >= buffer_size)
    {
        buffer.reset(new char[text.size() + 1]);
        pbuffer = buffer.get();
    }

    if(!CharToOem(text.c_str(), pbuffer))
        throw std::runtime_error(win32_error_to_string("CharToOem", GetLastError()));

    return std::string(pbuffer);
}

const std::string win32_error_to_string(const std::string& function, DWORD error_code)
{ 
    std::ostringstream buffer;
    LPSTR message_buffer = 0;

    buffer << function << " failed";

    if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM, 0, error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&message_buffer), 0, 0))
    {
        if(message_buffer)
        {
            buffer << ": " << message_buffer;
            LocalFree(message_buffer);
        }
    }

    return buffer.str();
}

#endif  // defined(_WINDOWS)

}   // namespace util
