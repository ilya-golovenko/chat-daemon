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
#include "file_utils.hpp"
#include <utilities.hpp>
#include <log_common.hpp>
#include <common.hpp>
#include <platform.hpp>

// BOOST headers
#include <boost/scoped_array.hpp>


namespace util
{

file::file() :
    filedesc_(-1),
    buffer_begin_(0),
    buffer_end_(0)
{
}

file::~file()
{
    close();
}

file::file(int filedesc) :
    filedesc_(filedesc),
    buffer_begin_(0),
    buffer_end_(0)
{
}

file::file(const std::string& filename) :
    filedesc_(-1),
    buffer_begin_(0),
    buffer_end_(0)
{
    open(filename);
}

void file::open(const std::string& filename)
{
    if(is_open())
        close();

    filename_ = filename;

#if defined(_WINDOWS)
    if((filedesc_ = sopen(filename_.c_str(), O_RDWR | O_BINARY, SH_DENYNO)) < 0)
        throw std::runtime_error(file_errno_to_string("sopen", filename_, errno));
#else
    if((filedesc_ = ::open(filename_.c_str(), O_RDWR)) < 0)
        throw std::runtime_error(file_errno_to_string("open", filename_, errno));
#endif  // defined(_WINDOWS)

    _seek(0, SEEK_END);
}

const std::string& file::filename() const
{
    return filename_;
}

file::operator int() const
{
    return filedesc_;
}

void file::close()
{
    if(is_open())
    {
        ::close(filedesc_);
        filedesc_ = -1;

        invalidate_read_buffer();
    }
}

bool file::is_open() const
{
    return (filedesc_ >= 0);
}

bool file::eof()
{
    long cur_pos = _seek(0, SEEK_CUR);
    long end_pos = _seek(0, SEEK_END);

    _seek(cur_pos, SEEK_SET);
    return (cur_pos >= end_pos);
}

long file::tell()
{
    return _seek(0, SEEK_CUR);
}

long file::length()
{
    long cur_pos = _seek(0, SEEK_CUR);
    long end_pos = _seek(0, SEEK_END);

    _seek(cur_pos, SEEK_SET);
    return end_pos;
}

void file::truncate()
{
#if defined(_WINDOWS)
    if(::chsize(filedesc_, 0) < 0)
        throw std::runtime_error(file_errno_to_string("chsize", filename_, errno));
#else
    if(::ftruncate(filedesc_, 0) < 0)
        throw std::runtime_error(file_errno_to_string("ftruncate", filename_, errno));
#endif  // defined(_WINDOWS)

    seek(0, SEEK_SET);
}

bool file::read_char(char& ch)
{
    if(buffer_begin_ >= buffer_end_)
    {
        if(!eof())
        {
            invalidate_read_buffer();

            if((buffer_end_ = ::read(filedesc_, buffer_, sizeof(buffer_))) < 0)
                throw std::runtime_error(file_errno_to_string("read", filename_, errno));
        }
    }

    if(buffer_begin_ < buffer_end_)
    {
        ch = buffer_[buffer_begin_++];
        return true;
    }

    return false;
}

std::size_t file::read(void* buffer, std::size_t size)
{
    std::size_t count = size;

    char* pbuffer = static_cast<char*>(buffer);
    while(count && read_char(*pbuffer++)) --count;

    return (size - count);
}

long file::seek(long offset, int origin)
{
    invalidate_read_buffer();
    return _seek(offset, origin);
}

void file::invalidate_read_buffer()
{
    buffer_begin_ = buffer_end_ = 0;
}

long file::_seek(long offset, int origin)
{
    long pos = lseek(filedesc_, offset, origin);

    if(pos < 0)
        throw std::runtime_error(file_errno_to_string("lseek", filename_, errno));

    return pos;
}

bool file::exists(const std::string& filename)
{
    struct stat file_stat;

    if(!::stat(filename.c_str(), &file_stat))
    {
        if(file_stat.st_mode & S_IFREG)
            return true;
    }

    return false;
}

std::size_t file::size(const std::string& filename)
{
    struct stat file_stat;

    if(::stat(filename.c_str(), &file_stat) < 0)
        throw std::runtime_error(file_errno_to_string("stat", filename, errno));

    if(!(file_stat.st_mode & S_IFREG))
        throw std::runtime_error("target is not a file: " + filename);

    return static_cast<std::size_t>(file_stat.st_size);
}

void file::create(const std::string& filename, mode_t mode)
{
    LOGDBG("creating new file: %1%") << filename;

    util::file file(::creat(filename.c_str(), mode));

    if(file < 0)
        throw std::runtime_error(file_errno_to_string("creat", filename, errno));
}

void file::remove(const std::string& filename)
{
    LOGDBG("removing file: %1%") << filename;

    if(::remove(filename.c_str()) < 0)
        throw std::runtime_error(file_errno_to_string("remove", filename, errno));
}

void file::truncate(const std::string& filename, mode_t mode)
{
    LOGDBG("truncating file: %1%") << filename;

    util::file file(::open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, mode));

    if(file < 0)
        throw std::runtime_error(file_errno_to_string("open", filename, errno));
}

void file::rename(const std::string& old_filename, const std::string& new_filename)
{
    LOGDBG("renaming file: old name = %1%, new name = %2%") << old_filename << new_filename;

    if(::rename(old_filename.c_str(), new_filename.c_str()))
        throw std::runtime_error(file_errno_to_string("rename", old_filename, errno));
}

const std::string file::read_text(const std::string& filename)
{
    LOGDBG("reading text file: %1%") << filename;

    std::size_t file_size = size(filename);

    if(!file_size)
        return strings::empty;

    util::file file(::open(filename.c_str(), O_RDONLY));

    if(file < 0)
        throw std::runtime_error(file_errno_to_string("open", filename, errno));

    boost::scoped_array<char> content(new char[file_size]);
    int count = ::read(file, content.get(), file_size);

    if(count < 0)
        throw std::runtime_error(file_errno_to_string("read", filename, errno));

    return std::string(content.get(), static_cast<std::size_t>(count));
}

void file::write_text(const std::string& filename, const std::string& text, mode_t mode)
{
    LOGDBG("writing text file: %1%") << filename;

    util::file file(::creat(filename.c_str(), mode));

    if(file < 0)
        throw std::runtime_error(file_errno_to_string("creat", filename, errno));

    if(::write(file, text.c_str(), text.size()) < 0)
        throw std::runtime_error(file_errno_to_string("write", filename, errno));
}

}   // namespace util
