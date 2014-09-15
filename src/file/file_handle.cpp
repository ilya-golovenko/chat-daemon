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
#include <file/file_handle.hpp>
#include <misc/error_utils.hpp>
#include <app/constants.hpp>
#include <app/platform.hpp>

// STL headers
#include <stdexcept>

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

file_handle::file_handle() :
    filedesc_(-1),
    buffer_begin_(0),
    buffer_end_(0)
{
}

file_handle::~file_handle()
{
    close();
}

file_handle::file_handle(int filedesc) :
    filedesc_(filedesc),
    buffer_begin_(0),
    buffer_end_(0)
{
}

file_handle::file_handle(std::string const& filename) :
    filedesc_(-1),
    buffer_begin_(0),
    buffer_end_(0)
{
    open(filename);
}

void file_handle::open(std::string const& filename)
{
    close();

    filename_ = filename;

#if defined(CHAT_PLATFORM_WINDOWS)
    if((filedesc_ = ::sopen(filename_.c_str(), O_RDWR | O_BINARY, SH_DENYNO)) < 0)
        throw std::runtime_error(util::errno_to_string("sopen", filename_, errno));
#else
    if((filedesc_ = ::open(filename_.c_str(), O_RDWR)) < 0)
        throw std::runtime_error(util::errno_to_string("open", filename_, errno));
#endif  // defined(CHAT_PLATFORM_WINDOWS)

    seek_impl(0, SEEK_END);
}

std::string const& file_handle::filename() const
{
    return filename_;
}

file_handle::operator int() const
{
    return filedesc_;
}

void file_handle::close()
{
    if(is_open())
    {
        invalidate_read_buffer();

        ::close(filedesc_);
        filedesc_ = -1;
    }
}

bool file_handle::is_open() const
{
    return filedesc_ >= 0;
}

bool file_handle::eof()
{
    long const cur_pos = seek_impl(0, SEEK_CUR);
    long const end_pos = seek_impl(0, SEEK_END);

    seek_impl(cur_pos, SEEK_SET);

    return cur_pos >= end_pos;
}

std::size_t file_handle::tell()
{
    long const cur_pos = seek_impl(0, SEEK_CUR);

    return static_cast<std::size_t>(cur_pos);
}

std::size_t file_handle::length()
{
    long const cur_pos = seek_impl(0, SEEK_CUR);
    long const end_pos = seek_impl(0, SEEK_END);

    seek_impl(cur_pos, SEEK_SET);

    return static_cast<std::size_t>(end_pos);
}

void file_handle::truncate()
{
    invalidate_read_buffer();

#if defined(CHAT_PLATFORM_WINDOWS)
    if(::chsize(filedesc_, 0) < 0)
        throw std::runtime_error(util::errno_to_string("chsize", filename_, errno));
#else
    if(::ftruncate(filedesc_, 0) < 0)
        throw std::runtime_error(util::errno_to_string("ftruncate", filename_, errno));
#endif  // defined(CHAT_PLATFORM_WINDOWS)

    seek_impl(0, SEEK_SET);
}

bool file_handle::read_char(char& ch)
{
    if(buffer_begin_ >= buffer_end_)
    {
        if(!eof())
        {
            invalidate_read_buffer();

            if((buffer_end_ = ::read(filedesc_, buffer_, sizeof(buffer_))) < 0)
                throw std::runtime_error(util::errno_to_string("read", filename_, errno));
        }
    }

    if(buffer_begin_ < buffer_end_)
    {
        ch = buffer_[buffer_begin_++];

        return true;
    }

    return false;
}

std::size_t file_handle::read(void* buffer, std::size_t size)
{
    std::size_t count = size;

    char* pbuffer = static_cast<char*>(buffer);
    while(count && read_char(*pbuffer++)) --count;

    return size - count;
}

bool file_handle::write_char(char ch)
{
    return write(&ch, 1u) == 1u;
}

std::size_t file_handle::write(void const* buffer, std::size_t size)
{
    long const count = ::write(filedesc_, buffer, static_cast<unsigned int>(size)); 

    if(count < 0)
        throw std::runtime_error(util::errno_to_string("write", filename_, errno));

    return static_cast<std::size_t>(count);
}

std::size_t file_handle::seek(long offset, int origin)
{
    invalidate_read_buffer();

    long const pos = seek_impl(offset, origin);

    return static_cast<std::size_t>(pos);
}

void file_handle::invalidate_read_buffer()
{
    buffer_begin_ = buffer_end_ = 0;
}

long file_handle::seek_impl(long offset, int origin)
{
    long const pos = ::lseek(filedesc_, offset, origin);

    if(pos < 0)
        throw std::runtime_error(util::errno_to_string("lseek", filename_, errno));

    return pos;
}

}   // namespace util
}   // namespace chat
