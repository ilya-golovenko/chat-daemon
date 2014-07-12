//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2014 Ilya Golovenko
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
#include <chat/data_file.hpp>
#include <chat/exception.hpp>
#include <app/constants.hpp>


namespace chat
{

data_file::data_file() :
    event_(file_)
{
}

data_file::~data_file()
{
    close();
}

void data_file::open(std::string const& filename)
{
    if(!file_.is_open())
    {
        file_.open(filename);
        event_.initialize();
    }
}

void data_file::close()
{
    if(file_.is_open())
    {
        event_.shutdown();
        file_.close();
    }
}

bool data_file::is_open() const
{
    return file_.is_open();
}

bool data_file::wait_event()
{
    return event_.wait();
}

bool data_file::read_line(std::string& line)
{
    char c;

    while(file_.read_char(c))
    {
        switch(c)
        {
            case '\r':
            case '\n':
                break;

            case '\x01':
                line.swap(line_);
                line_.clear();
                return true;

            default:
                line_.push_back(c);
                break;
        }
    }

    return false;
}

std::size_t data_file::tell()
{
    return file_.tell();
}

std::size_t data_file::length()
{
    return file_.length();
}

void data_file::truncate()
{
    file_.truncate();
}

}   // namespace chat
