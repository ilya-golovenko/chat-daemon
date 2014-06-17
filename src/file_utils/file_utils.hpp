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
#ifndef _file_utils_hpp
#define _file_utils_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <platform.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>

// STL headers
#include <string>


namespace util
{

#if defined(_WINDOWS)
typedef int mode_t;
const mode_t default_mode = S_IREAD | S_IWRITE;
#else
const mode_t default_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
#endif

class file :
    private boost::noncopyable
{
public:
    file();
    ~file();

    explicit file(int filedesc);
    explicit file(const std::string& filename);

    void open(const std::string& filename);
    const std::string& filename() const;

    operator int() const;

    void close();
    bool is_open() const;

    bool eof();
    long tell();
    long length();
    void truncate();

    long seek(long offset, int origin);

    bool read_char(char& ch);
    std::size_t read(void* buffer, std::size_t size);

public:
    static bool exists(const std::string& filename);
    static std::size_t size(const std::string& filename);

    static void create(const std::string& filename, mode_t mode = default_mode);
    static void truncate(const std::string& filename, mode_t mode = default_mode);

    static void remove(const std::string& filename);
    static void rename(const std::string& old_filename, const std::string& new_filename);

    static const std::string read_text(const std::string& filename);
    static void write_text(const std::string& filename, const std::string& text, mode_t mode = default_mode);

private:
    void invalidate_read_buffer();
    long _seek(long offset, int origin);

private:
    typedef char buffer_type[4096];

private:
    int filedesc_;
    std::string filename_;

    buffer_type buffer_;
    int buffer_begin_;
    int buffer_end_;
};

}   // namespace util

#endif  // _file_utils_hpp
