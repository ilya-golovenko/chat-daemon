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
#ifndef _chat_data_file_hpp
#define _chat_data_file_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <file/file_handle.hpp>
#include <file/file_event.hpp>
#include <app/platform.hpp>

// STL headers
#include <vector>
#include <string>


namespace chat
{

class data_file
{
public:
    data_file();
    ~data_file();

    data_file(data_file const&) = delete;
    data_file& operator=(data_file const&) = delete;

    void open(std::string const& filename);
    void close();

    bool is_open() const;

    bool wait_event();
    bool read_line(std::string& line);

    std::size_t tell();
    std::size_t length();

    void truncate();

private:
    util::file_handle file_;
    util::file_event event_;
    std::string line_;
};

}   // namespace chat

#endif  // _chat_data_file_hpp
