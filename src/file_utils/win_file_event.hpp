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
#ifndef _util_win_file_event_hpp
#define _util_win_file_event_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <file_utils.hpp>
#include <platform.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>


namespace util
{

#if defined(_WINDOWS)

class win_file_event :
    private boost::noncopyable
{
public:
    win_file_event(file& file);
    ~win_file_event();

    void initialize();
    void shutdown();

    bool wait();

private:
    static bool is_valid_handle(HANDLE handle);

private:
    file& file_;
    HANDLE change_handle_;
    volatile bool terminated_;
};

#endif  // defined(_WINDOWS)

}   // namespace util

#endif  // _util_win_file_event_hpp
