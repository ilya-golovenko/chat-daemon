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
#ifndef _chat_data_reader_hpp
#define _chat_data_reader_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "chat_data_file.hpp"
#include "chat_common.hpp"
#include "chat_forwards.hpp"
#include <log_common.hpp>
#include <platform.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// STL headers
#include <string>


namespace chat
{

class data_reader :
    private boost::noncopyable,
    public log_source<data_reader>
{
DEFINE_LOG_SOURCE_NAME("data reader")

public:
    data_reader(chat::server& server);
    ~data_reader();

    void configure();
    void start();
    void stop();

private:
    void run();

    void open_data_file();
    void close_data_file();

    void check_data_file();
    void truncate_data_file();

    void stop_worker_thread();
    void start_worker_thread();

    void pass_line_to_message_parser();

private:
    chat::server& server_;

    long max_file_size_;
    std::string filename_;

    std::string line_;
    data_file data_file_;

    boost::scoped_ptr<asio::thread> work_thread_;
};

typedef boost::shared_ptr<data_reader> data_reader_ptr;

}   // namespace chat

#endif  // _chat_data_reader_hpp
