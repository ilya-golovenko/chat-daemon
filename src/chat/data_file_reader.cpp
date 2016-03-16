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

// Application headers
#include <chat/data_file_reader.hpp>
#include <chat/server_context.hpp>
#include <chat/server_config.hpp>
#include <chat/exception.hpp>
#include <chat/common.hpp>
#include <misc/file_utils.hpp>
#include <misc/path_utils.hpp>
#include <app/constants.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{

data_file_reader::data_file_reader(server_context& context) :
    context_(context),
    max_file_size_(0u),
    clean_data_file_(true)
{
}

void data_file_reader::configure(server_config const& config)
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    LOG_COMP_NOTICE(data_file_reader, "configuring");

    max_file_size_ = config.max_data_file_size;

    clean_data_file_ = config.clean_data_file;

    filename_ = util::path::combine(config.chat_path, files::chat);
}

void data_file_reader::start()
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    LOG_COMP_NOTICE(data_file_reader, "starting");

    check_data_file();
    open_data_file();

    start_worker_thread();

    LOG_COMP_NOTICE(data_file_reader, "started");
}

void data_file_reader::stop()
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    LOG_COMP_NOTICE(data_file_reader, "stopping");

    close_data_file();
    stop_worker_thread();

    LOG_COMP_NOTICE(data_file_reader, "stopped");
}

void data_file_reader::run()
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    try
    {
        std::string line;

        while(data_file_.wait_event())
        {
            while(data_file_.read_line(line))
            {
                dispatch_line(line);
            }

            if(max_file_size_)
                truncate_data_file();
        }
    }
    catch(std::exception const& e)
    {
        LOG_COMP_ERROR(data_file_reader, e);
        context_.stop();
    }
}

void data_file_reader::open_data_file()
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    LOG_COMP_INFO(data_file_reader, "opening data file");
    data_file_.open(filename_);
}

void data_file_reader::close_data_file()
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    if(data_file_.is_open())
    {
        LOG_COMP_INFO(data_file_reader, "closing data file");
        data_file_.close();
    }
}

void data_file_reader::check_data_file()
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    if(!util::file::exists(filename_))
    {
        LOG_COMP_INFO(data_file_reader, "creating new data file");
        util::file::create(filename_);
    }
    else if(clean_data_file_)
    {
        LOG_COMP_INFO(data_file_reader, "truncating data file");
        util::file::truncate(filename_);
    }
}

void data_file_reader::truncate_data_file()
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    if(data_file_.tell() >= max_file_size_)
    {
        LOG_COMP_INFO(data_file_reader, "truncating data file");
        data_file_.truncate();
    }
}

void data_file_reader::start_worker_thread()
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    LOG_COMP_DEBUG(data_file_reader, "starting worker thread");
    worker_thread_ = std::thread(&data_file_reader::run, this);
}

void data_file_reader::stop_worker_thread()
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    LOG_COMP_DEBUG(data_file_reader, "stopping worker thread");
    worker_thread_.join();
}

void data_file_reader::dispatch_line(std::string const& line)
{
    LOG_COMP_TRACE_FUNCTION(data_file_reader);

    LOG_COMP_TRACE(data_file_reader, "dispatching line: ", line);
    context_.dispatch([=]{ context_.get_message_parser().parse_message(line); });
}

}   // namespace chat
