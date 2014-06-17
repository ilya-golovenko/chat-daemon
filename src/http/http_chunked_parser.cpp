//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 Ilya Golovenko
//    This file is part of libsphttp.
//
//    libsphttp is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    libsphttp is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with libsphttp. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include "http_chunked_parser.hpp"
#include "http_utilities.hpp"


namespace http
{

chunked_parser::chunked_parser() :
    state_(state_chunk_size_start),
    size_(0),
    chunk_size_(0)
{
}

void chunked_parser::reset()
{
    state_= state_chunk_size_start;
    size_ = 0;
}

boost::tribool chunked_parser::consume(char c)
{
    if(++size_ > max_chunk_size_length)
        return false;

    switch (state_)
    {
        case state_chunk_size_start:
            if(is_xdigit(c))
            {
                chunk_size_ = http_util::hex_to_int(c);
                state_ = state_chunk_size;
                return boost::indeterminate;
            }
            else if(c == '\r' || c == '\n')
            {
                return boost::indeterminate;
            }
            break;

        case state_chunk_size:
            if(is_xdigit(c))
            {
                chunk_size_ = chunk_size_ << 4;
                chunk_size_ += http_util::hex_to_int(c);
                state_ = state_chunk_size;
                return boost::indeterminate;
            }
            else if(c == '\r')
            {
                state_ = state_expecting_newline_1;
                return boost::indeterminate;
            }
            break;

        case state_expecting_newline_1:
            if(c == '\n')
            {
                if(chunk_size_ > 0)
                {
                    return true;
                }
                else
                {
                    state_ = state_expecting_newline_2;
                    return boost::indeterminate;
                }
            }
            break;

        case state_expecting_newline_2:
            if(c == '\r')
            {
                state_ = state_expecting_newline_3;
                return boost::indeterminate;
            }
            break;

        case state_expecting_newline_3:
            if(c == '\n')
            {
                return true;
            }
            break;
    }

    return false;
}

std::size_t chunked_parser::get_chunk_size() const
{
    return chunk_size_;
}

}    // namespace http
