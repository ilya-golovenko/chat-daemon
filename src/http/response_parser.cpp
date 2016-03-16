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
#include <http/response_parser.hpp>


namespace http
{

response_parser::response_parser() :
    state_(state_version),
    size_(0u),
    status_(0u)
{
}

void response_parser::reset()
{
    state_ = state_version;
    size_ = 0u;
    status_ = 0u;
    version_parser_.reset();
    headers_parser_.reset();
}

parse_result response_parser::consume(response& response, char c)
{
    if(++size_ > max_message_size)
    {
        return parse_result::error;
    }

    switch(state_)
    {
        case state_version:
            switch(version_parser_.consume(response, c))
            {
                case parse_result::ok:
                    if(c == ' ')
                    {
                        state_ = state_status_code_start;
                        return parse_result::more;
                    }
                    break;

                case parse_result::more:
                    return parse_result::more;

                default:
                    ;
            }
            break;

        case state_status_code_start:
            if(is_digit(c))
            {
                status_ = c - '0';
                state_ = state_status_code;
                return parse_result::more;
            }
            break;

        case state_status_code:
            if(c == ' ')
            {
                set_response_status(response);
                state_ = state_status_reason;
                return parse_result::more;
            }
            else if(is_digit(c))
            {
                status_ = status_ * 10 + c - '0';
                return parse_result::more;
            }
            break;

        case state_status_reason:
            if(c == '\r')
            {
                state_ = state_expecting_newline;
                return parse_result::more;
            }
            else if(is_char(c))
            {
                return parse_result::more;
            }
            break;

        case state_expecting_newline:
            if(c == '\n')
            {
                state_ = state_headers;
                return parse_result::more;
            }
            break;

        case state_headers:
            return headers_parser_.consume(response, c);
    }

    return parse_result::error;
}

void response_parser::set_response_status(response& response)
{
    response.set_status(status(status_));
}

}    // namespace http
