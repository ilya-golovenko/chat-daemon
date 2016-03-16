//---------------------------------------------------------------------------
//
//    Copyright (C) 2008 - 2016 Ilya Golovenko
//    This file is part of Chat.Daemon project
//
//    spchatd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spchatd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spchatd. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include <misc/win_utils.hpp>

#if defined(CHAT_PLATFORM_WINDOWS)

// MISSIO headers
#include <missio/format/write.hpp>

// STL headers
#include <sstream>


namespace chat
{
namespace util
{
namespace win
{

std::string char_to_oem(std::string const& text)
{
    std::vector<char> buffer(text.size());

    if(!::CharToOemBuffA(text.c_str(), &buffer.front(), static_cast<DWORD>(buffer.size())))
    {
        throw std::runtime_error(error_to_string("CharToOem", ::GetLastError()));
    }

    return std::string(buffer.begin(), buffer.end());
}

std::string error_to_string(char const* function, DWORD error_code)
{ 
    std::string str;

    missio::format::write(str, function, " failed");

    LPSTR buffer_pointer = nullptr;

    if(::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                        nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        reinterpret_cast<LPSTR>(&buffer_pointer), 0u, nullptr))
    {
        std::unique_ptr<char, decltype(&::LocalFree)> buffer(buffer_pointer, ::LocalFree);

        //TODO: missio::format::write(str, ": ", buffer_pointer);
    }

    missio::format::write(str, " (", error_code, ")");

    return str;
}

}   // namespace win
}   // namespace util
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_WINDOWS)
