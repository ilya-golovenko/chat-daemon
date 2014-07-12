//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2014 Ilya Golovenko
//    This file is part of Chat.Daemon project
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
#ifndef _file_file_handle_hpp
#define _file_file_handle_hpp

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

#if defined(CHAT_PLATFORM_WINDOWS)
typedef int mode_t;
mode_t const default_mode = S_IREAD | S_IWRITE;
#else
mode_t const default_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
#endif

class file_handle
{
public:
    file_handle();
    ~file_handle();

    file_handle(file_handle const&) = delete;
    file_handle& operator=(file_handle const&) = delete;

    explicit file_handle(int filedesc);
    explicit file_handle(std::string const& filename);

    void open(std::string const& filename);
    std::string const& filename() const;

    operator int() const;

    void close();
    bool is_open() const;

    bool eof();

    std::size_t tell();
    std::size_t length();

    void truncate();

    std::size_t seek(long offset, int origin);

    bool read_char(char& ch);
    std::size_t read(void* buffer, std::size_t size);

    bool write_char(char ch);
    std::size_t write(void const* buffer, std::size_t size);

private:
    void invalidate_read_buffer();
    long seek_impl(long offset, int origin);

private:
    int filedesc_;
    std::string filename_;

    char buffer_[4096];
    long buffer_begin_;
    long buffer_end_;
};

}   // namespace util
}   // namespace chat

#endif  // _file_file_handle_hpp
