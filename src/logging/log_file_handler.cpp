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
#include "log_file_handler.hpp"
#include <file_utils.hpp>
#include <common.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>

// STL headers
#include <stdexcept>
#include <memory>


log_handler_ptr log_file_handler::create(const std::string& filename, std::ostream::pos_type max_size, bool backup_log)
{
    return boost::make_shared<log_file_handler>(filename, max_size, backup_log);
}

log_file_handler::log_file_handler(const std::string& filename, std::ostream::pos_type max_size, bool backup_log) :
    backup_log_(backup_log),
    filename_(filename),
    max_size_(max_size)
{
    initialize(strings::empty);
}

log_file_handler::~log_file_handler()
{
    finalize();
}

void log_file_handler::add_message(const log_message& message)
{
    buffer_ << message.time() << '\t';
    buffer_ << message.severity() << '\t';

    if(message.has_source())
        buffer_ << '[' << message.source() << "] ";

    buffer_ << message.get_text();
    flush_to_file();
}

void log_file_handler::initialize(const std::string& backup_filename)
{
    if(file_.is_open())
        finalize();

    if(!util::file::exists(filename_))
        util::file::create(filename_);

    file_.open(filename_.c_str(), std::ios_base::app);
    if(!file_.is_open())
        throw std::runtime_error("cannot open log file: " + filename_);

    file_.seekp(0, std::ios_base::end);
    if(file_.bad())
        throw std::runtime_error("cannot seek log file: " + filename_);

    if(file_.tellp() > 0)
    {
        file_ << std::endl;
        file_ << std::endl;
    }
    else
    {
        if(!backup_filename.empty())
            file_ << "<<< previous log file name: " << backup_filename << " >>>" << std::endl;
    }
}

const std::string log_file_handler::get_backup_filename() const
{
    std::ostringstream buffer;

    std::auto_ptr<boost::posix_time::time_facet> output_facet(new boost::posix_time::time_facet());
    output_facet->format(boost::local_time::local_time_facet::iso_time_format_specifier);

    buffer.imbue(std::locale(std::locale::classic(), output_facet.release()));
    buffer << filename_ << '_' << boost::posix_time::second_clock::local_time() << ".old";

    return buffer.str();
}

void log_file_handler::backup_old_log_file()
{
    if(file_.is_open())
        file_.close();

    if(backup_log_)
    {
        std::string backup_filename = get_backup_filename();
        util::file::rename(filename_, backup_filename);
        initialize(backup_filename);
    }
    else
    {
        util::file::truncate(filename_);
        initialize(strings::empty);
    }
}

bool log_file_handler::is_log_file_backup_required()
{
    return (max_size_ > 0 && file_.tellp() >= max_size_);
}

void log_file_handler::flush_to_file()
{
    if(file_.is_open() && buffer_.tellp() > 0)
    {
        file_ << buffer_.str() << std::endl;
        buffer_.str(strings::empty);

        if(is_log_file_backup_required())
            backup_old_log_file();
    }
}

void log_file_handler::finalize()
{
    if(file_.is_open())
    {
        flush_to_file();
        file_.close();
    }
}
