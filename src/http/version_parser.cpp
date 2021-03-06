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
#include <http/version_parser.hpp>


namespace http
{

version_parser::version_parser() :
    state_(state_version_h),
    major_(0u),
    minor_(0u)
{
}

void version_parser::reset()
{
    state_= state_version_h;
}

parse_result version_parser::consume(message& message, char c)
{
    switch(state_)
    {
        case state_version_h:
            if(c == 'H')
            {
                state_ = state_version_t_1;
                return parse_result::more;
            }
            break;

        case state_version_t_1:
            if(c == 'T')
            {
                state_ = state_version_t_2;
                return parse_result::more;
            }
            break;

        case state_version_t_2:
            if(c == 'T')
            {
                state_ = state_version_p;
                return parse_result::more;
            }
            break;

        case state_version_p:
            if(c == 'P')
            {
                state_ = state_version_slash;
                return parse_result::more;
            }
            break;

        case state_version_slash:
            if(c == '/')
            {
                state_ = state_version_major_start;
                return parse_result::more;
            }
            break;

        case state_version_major_start:
            if(is_digit(c))
            {
                major_ = c - '0';
                state_ = state_version_major;
                return parse_result::more;
            }
            break;

        case state_version_major:
            if(c == '.')
            {
                state_ = state_version_minor_start;
                return parse_result::more;
            }
            else if(is_digit(c))
            {
                major_ = major_ * 10 + c - '0';
                return parse_result::more;
            }
            break;

        case state_version_minor_start:
            if(is_digit(c))
            {
                minor_ = c - '0';
                state_ = state_version_minor;
                return parse_result::more;
            }
            break;

        case state_version_minor:
            if(is_digit(c))
            {
                minor_ = minor_ * 10 + c - '0';
                return parse_result::more;
            }
            else
            {
                message.set_version(major_, minor_);
                return parse_result::ok;
            }
            break;
    }

    return parse_result::error;
}

}   // namespace http
