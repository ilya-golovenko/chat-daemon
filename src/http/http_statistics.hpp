//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 Ilya Golovenko
//    This file is part of libsphttp.
//
//    libsphttp is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    libsphttp is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with libsphttp. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------
#ifndef _http_statistics_hpp
#define _http_statistics_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <cstddef>


namespace http
{

class statistics
{
public:
    static std::size_t get_bytes_read();
    static std::size_t get_bytes_written();

    static void add_bytes_read(std::size_t count);
    static void add_bytes_written(std::size_t count);

private:
    statistics();
    ~statistics();

private:
    static volatile std::size_t bytes_read_;
    static volatile std::size_t bytes_written_;
};

}   // namespace http

#endif  //  _http_statistics_hpp
