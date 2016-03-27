//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 - 2016 Ilya Golovenko
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
#ifndef _http_statistics_hpp
#define _http_statistics_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <cstdint>
#include <atomic>


namespace http
{

class statistics
{
public:
    statistics() = delete;
    ~statistics() = delete;

    static std::uint64_t get_bytes_read();
    static std::uint64_t get_bytes_written();

    static void add_bytes_read(std::uint64_t count);
    static void add_bytes_written(std::uint64_t count);

private:
    static std::atomic<std::uint64_t> bytes_read_;
    static std::atomic<std::uint64_t> bytes_written_;
};

}   // namespace http

#endif  //  _http_statistics_hpp
