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
#include <http/headers_parser.hpp>


namespace http
{

headers_parser::headers_parser() :
    state_(state_header_line_start)
{
}

void headers_parser::reset()
{
    state_= state_header_line_start;
    name_.clear();
    value_.clear();
}

parse_result headers_parser::consume(message& message, char c)
{
    switch(state_)
    {
        case state_header_line_start:
            if(c == '\r')
            {
                state_ = state_expecting_newline_2;
                return parse_result::more;
            }
            else if(c == ' ' || c == '\t')
            {
                if(!name_.empty())
                {
                    state_ = state_header_whitespace;
                    return parse_result::more;
                }
            }
            else if(is_char(c))
            {
                name_.assign(1, c);
                state_ = state_header_name;
                return parse_result::more;
            }
            break;

        case state_header_whitespace:
            if(c == '\r')
            {
                state_ = state_expecting_newline_1;
                return parse_result::more;
            }
            else if(c == ' ' || c == '\t')
            {
                return parse_result::more;
            }
            else if(!is_control(c))
            {
                if(value_.size() < max_header_value_length)
                {
                    value_.push_back(c);
                    state_ = state_header_value;
                    return parse_result::more;
                }
            }
            break;

        case state_header_name:
            if(c == ':')
            {
                state_ = state_space_before_header_value;
                return parse_result::more;
            }
            else if(is_char(c))
            {
                if(name_.size() < max_header_name_length)
                {
                    name_.push_back(c);
                    return parse_result::more;
                }
            }
            break;

        case state_space_before_header_value:
            if(c == ' ')
            {
                state_ = state_header_value_start;
                return parse_result::more;
            }
            break;

        case state_header_value_start:
            if(!is_control(c))
            {
                value_.assign(1, c);
                state_ = state_header_value;
                return parse_result::more;
            }
            break;

        case state_header_value:
            if(c == '\r')
            {
                message.add(name_, value_);
                state_ = state_expecting_newline_1;
                return parse_result::more;
            }
            else if(!is_control(c))
            {
                if(value_.size() < max_header_value_length)
                {
                    value_.push_back(c);
                    return parse_result::more;
                }
            }
            break;

        case state_expecting_newline_1:
            if(c == '\n')
            {
                state_ = state_header_line_start;
                return parse_result::more;
            }
            break;

        case state_expecting_newline_2:
            if(c == '\n')
            {
                return parse_result::ok;
            }
            break;
    }

    return parse_result::error;
}

}    // namespace http
