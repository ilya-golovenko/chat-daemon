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
#ifndef _async_timer_hpp
#define _async_timer_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "handler_alloc.hpp"

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>


namespace util
{

class async_timer :
    private boost::noncopyable
{
public:
    explicit async_timer(asio::io_service& io_service) :
        timer_(io_service)
    {
    }

    template <typename Duration, typename Handler>
    void start(const Duration& duration, const Handler& handler)
    {
        timer_.expires_from_now(duration);
        timer_.async_wait(make_custom_alloc_handler(allocator_, handler));
    }

    void stop()
    {
        timer_.cancel();
    }

private:
    typedef handler_allocator<2, 64> allocator_type;

private:
    asio::deadline_timer timer_;
    allocator_type allocator_;
};

}   // namespace util

#endif  // _async_timer_hpp
