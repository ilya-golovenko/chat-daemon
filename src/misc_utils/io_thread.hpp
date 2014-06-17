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
#ifndef _io_thread_hpp
#define _io_thread_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>


namespace util
{

class io_thread :
    private boost::noncopyable
{
public:
    typedef boost::function<void ()> handler;

public:
    io_thread();
    ~io_thread();

    void start();
    void stop();

    void post(const handler& handler);
    asio::io_service& operator&();

private:
    const handler bind_to_run();
    const handler bind_to_stop();

private:
    asio::io_service io_service_;
    boost::scoped_ptr<asio::thread> thread_;
    boost::scoped_ptr<asio::io_service::work> work_;
};

}   // namespace util

#endif  // _io_thread_hpp
