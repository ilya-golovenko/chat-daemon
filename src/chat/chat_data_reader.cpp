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
#include "chat_data_reader.hpp"
#include "chat_server.hpp"
#include "chat_exception.hpp"
#include "chat_common.hpp"
#include <path_utils.hpp>
#include <file_utils.hpp>
#include <utilities.hpp>
#include <config_common.hpp>
#include <log_common.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/bind.hpp>

// STL headers
#include <numeric>
#include <sstream>


namespace chat
{

data_reader::data_reader(chat::server& server) :
    server_(server),
    max_file_size_(0)
{
}

data_reader::~data_reader()
{
    close_data_file();
    stop_worker_thread();
}

void data_reader::configure()
{
    LOGNOT("configuring");

    max_file_size_ = config.chat.data.max_size;

    filename_ = util::path::combine(
        config.chat.chat_path, files::chat);
}

void data_reader::start()
{
    close_data_file();
    stop_worker_thread();

    LOGNOT("starting");

    check_data_file();
    open_data_file();

    start_worker_thread();

    LOGNOT("started");
}

void data_reader::stop()
{
    LOGNOT("stopping");

    close_data_file();
    stop_worker_thread();

    LOGNOT("stopped");
}

void data_reader::run()
{
    try
    {
        while(data_file_.wait_event())
        {
            if(data_file_.read_line(line_))
            {
                do pass_line_to_message_parser();
                while(data_file_.read_line(line_));

                if(max_file_size_ > 0)
                    truncate_data_file();
            }
        }
    }
    catch(std::exception& e)
    {
        LOGERR(e.what());
        server_.stop();
    }
}

void data_reader::open_data_file()
{
    LOGNOT("opening data file");
    data_file_.open(filename_);
}

void data_reader::close_data_file()
{
    if(data_file_.is_open())
    {
        LOGNOT("closing data file");
        data_file_.close();
    }
}

void data_reader::check_data_file()
{
    if(!util::file::exists(filename_))
    {
        LOGNOT("creating new data file");
        util::file::create(filename_);
    }
    else if(config.chat.data.clean_file)
    {
        LOGNOT("truncating data file");
        util::file::truncate(filename_);
    }
}

void data_reader::truncate_data_file()
{
    if(data_file_.tell() >= max_file_size_)
    {
        LOGNOT("truncating data file");
        data_file_.truncate();
    }
}

void data_reader::pass_line_to_message_parser()
{
    server_.get_io_service().post(
        boost::bind(&message_parser::parse_message,
        server_.get_message_parser(), line_));
}

void data_reader::stop_worker_thread()
{
    if(work_thread_)
    {
        LOGNOT("stopping worker thread");

        work_thread_->join();
        work_thread_.reset();
    }
}

void data_reader::start_worker_thread()
{
    LOGNOT("starting worker thread");

    work_thread_.reset(new asio::thread(
        boost::bind(&data_reader::run, this)));
}

}   // namespace chat
