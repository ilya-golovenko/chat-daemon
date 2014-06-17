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

// Application headers
#include "io_thread.hpp"

// BOOST headers
#include <boost/bind.hpp>


namespace util
{

io_thread::io_thread()
{
}

io_thread::~io_thread()
{
    stop();
}

void io_thread::start()
{
    if(!thread_)
    {
        work_.reset(new asio::io_service::work(io_service_));
        thread_.reset(new asio::thread(bind_to_run()));
    }
}

void io_thread::stop()
{
    if(thread_)
    {
        work_.reset();
        post(bind_to_stop());

        thread_->join();
        thread_.reset();

        io_service_.reset();
    }
}

void io_thread::post(const handler& handler)
{
    if(thread_)
        io_service_.post(handler);
}

asio::io_service& io_thread::operator&()
{
    return io_service_;
}

const io_thread::handler io_thread::bind_to_run()
{
    return boost::bind(&asio::io_service::run, &io_service_);
}

const io_thread::handler io_thread::bind_to_stop()
{
    return boost::bind(&asio::io_service::stop, &io_service_);
}

}   // namespace util
