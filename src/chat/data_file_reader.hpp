//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2015 Ilya Golovenko
//    This file is part of Chat.Daemon project
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
#ifndef _chat_data_file_reader_hpp
#define _chat_data_file_reader_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <chat/data_file.hpp>
#include <chat/common.hpp>

// STL headers
#include <string>
#include <thread>


namespace chat
{

class server_context;
struct server_config;

class data_file_reader
{
public:
    explicit data_file_reader(server_context& context);

    data_file_reader(data_file_reader const&) = delete;
    data_file_reader& operator=(data_file_reader const&) = delete;

    void configure(server_config const& config);

    void start();
    void stop();

private:
    void run();

    void open_data_file();
    void close_data_file();

    void check_data_file();
    void truncate_data_file();

    void start_worker_thread();
    void stop_worker_thread();

    void dispatch_line(std::string const& line);

private:
    server_context& context_;

    data_file data_file_;

    std::string filename_;
    std::size_t max_file_size_;
    bool clean_data_file_;

    std::thread worker_thread_;
};

}   // namespace chat

#endif  // _chat_data_file_reader_hpp
