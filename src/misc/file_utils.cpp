//---------------------------------------------------------------------------
//
//    Copyright (C) 2008 - 2016 Ilya Golovenko
//    This file is part of Chat.Daemon project
//
//    spchatd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spchatd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spchatd. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include <misc/file_utils.hpp>
#include <misc/error_utils.hpp>
#include <file/file_handle.hpp>
#include <app/platform.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>

// STL headers
#include <memory>

#if defined(CHAT_PLATFORM_WINDOWS)
// Windows headers
#include <fcntl.h>
#include <share.h>
#include <io.h>
#endif


namespace chat
{
namespace util
{
namespace file
{

bool exists(std::string const& filename)
{
    LOG_TRACE_FUNCTION();

    struct stat file_stat;

    if(!::stat(filename.c_str(), &file_stat))
    {
        if((file_stat.st_mode & S_IFREG) == S_IFREG)
        {
            return true;
        }
    }

    return false;
}

std::size_t size(std::string const& filename)
{
    LOG_TRACE_FUNCTION();

    struct stat file_stat;

    if(::stat(filename.c_str(), &file_stat) < 0)
    {
        throw std::runtime_error(util::errno_to_string("stat", filename, errno));
    }

    if((file_stat.st_mode & S_IFREG) != S_IFREG)
    {
        throw std::runtime_error("target is not a file: " + filename);
    }

    return static_cast<std::size_t>(file_stat.st_size);
}

void create(std::string const& filename, mode_t mode)
{
    LOG_TRACE_FUNCTION();

    LOG_DEBUG("creating new file: ", filename);

    util::file_handle file(::creat(filename.c_str(), mode));

    if(file < 0)
        throw std::runtime_error(util::errno_to_string("creat", filename, errno));
}

void remove(std::string const& filename)
{
    LOG_TRACE_FUNCTION();

    LOG_DEBUG("removing file: ", filename);

    if(::remove(filename.c_str()) < 0)
        throw std::runtime_error(util::errno_to_string("remove", filename, errno));
}

void truncate(std::string const& filename, mode_t mode)
{
    LOG_TRACE_FUNCTION();

    LOG_DEBUG("truncating file: ", filename);

    util::file_handle file(::open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, mode));

    if(file < 0)
        throw std::runtime_error(util::errno_to_string("open", filename, errno));
}

void rename(std::string const& old_filename, std::string const& new_filename)
{
    LOG_TRACE_FUNCTION();

    LOG_DEBUG("renaming file from ", old_filename, " to ", new_filename);

    if(::rename(old_filename.c_str(), new_filename.c_str()))
        throw std::runtime_error(util::errno_to_string("rename", old_filename, errno));
}

std::string read_text(std::string const& filename)
{
    LOG_TRACE_FUNCTION();

    LOG_TRACE("reading text file: ", filename);

    std::size_t const file_size = size(filename);

    if(!file_size)
    {
        return std::string();
    }

    util::file_handle file(::open(filename.c_str(), O_RDONLY));

    if(file < 0)
    {
        throw std::runtime_error(util::errno_to_string("open", filename, errno));
    }

    std::unique_ptr<char[]> content(new char[file_size]);

    int const count = ::read(file, content.get(), static_cast<unsigned int>(file_size));

    if(count < 0)
    {
        throw std::runtime_error(util::errno_to_string("read", filename, errno));
    }

    return std::string(content.get(), static_cast<std::size_t>(count));
}

void write_text(std::string const& filename, std::string const& text, mode_t mode)
{
    LOG_TRACE_FUNCTION();

    LOG_TRACE("writing text file: ", filename);

    util::file_handle file(::creat(filename.c_str(), mode));

    if(file < 0)
    {
        throw std::runtime_error(util::errno_to_string("creat", filename, errno));
    }

    if(::write(file, text.c_str(), static_cast<unsigned int>(text.size())) < 0)
    {
        throw std::runtime_error(util::errno_to_string("write", filename, errno));
    }
}

}   // namespace file
}   // namespace util
}   // namespace chat
