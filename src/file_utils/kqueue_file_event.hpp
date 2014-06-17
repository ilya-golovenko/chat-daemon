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
#ifndef _util_kqueue_file_event_hpp
#define _util_kqueue_file_event_hpp

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

#if defined(_FREEBSD)

class kqueue_file_event :
    private boost::noncopyable
{
public:
    kqueue_file_event(file& file);
    ~kqueue_file_event();

    void initialize();
    void shutdown();

    bool wait();

private:
    file& file_;
    int kqueue_fd_;
    struct kevent event_;
};

#endif  // defined(_FREEBSD)

}   // namespace util

#endif  // _util_kqueue_file_event_hpp
