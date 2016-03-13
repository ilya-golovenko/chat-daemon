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
#ifndef _chat_misc_file_utils_hpp
#define _chat_misc_file_utils_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <app/platform.hpp>

// STL headers
#include <string>

// CRT headers
#include <sys/types.h>
#include <sys/stat.h>


namespace chat
{
namespace util
{
namespace file
{

#if defined(CHAT_PLATFORM_WINDOWS)
typedef int mode_t;
mode_t const default_mode = S_IREAD | S_IWRITE;
#else
mode_t const default_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
#endif

bool exists(std::string const& filename);
std::size_t size(std::string const& filename);

void create(std::string const& filename, mode_t mode = default_mode);
void truncate(std::string const& filename, mode_t mode = default_mode);

void remove(std::string const& filename);
void rename(std::string const& old_filename, std::string const& new_filename);

std::string read_text(std::string const& filename);
void write_text(std::string const& filename, std::string const& text, mode_t mode = default_mode);

}   // namespace file
}   // namespace util
}   // namespace chat

#endif  // _chat_misc_file_utils_hpp
