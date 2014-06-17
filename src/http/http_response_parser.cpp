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
#include "http_response_parser.hpp"


namespace http
{

response_parser::response_parser() :
    state_(state_http_version),
    size_(0),
    status_(0)
{
}

void response_parser::reset()
{
    state_ = state_http_version;
    size_ = 0;
    status_ = 0;
    version_parser_.reset();
    headers_parser_.reset();
}

boost::tribool response_parser::consume(http::response& response, char c)
{
    if(++size_ > max_message_size)
        return false;

    boost::tribool result;

    switch (state_)
    {
        case state_http_version:
            result = version_parser_.consume(response, c);

            if(result)
            {
                if(c == ' ')
                {
                    state_ = state_status_code_start;
                    return boost::indeterminate;
                }
            }
            else
            {
                return result;
            }
            break;

        case state_status_code_start:
            if(is_digit(c))
            {
                status_ = c - '0';
                state_ = state_status_code;
                return boost::indeterminate;
            }
            break;

        case state_status_code:
            if(c == ' ')
            {
                set_response_status(response);
                state_ = state_status_reason;
                return boost::indeterminate;
            }
            else if(is_digit(c))
            {
                status_ = status_ * 10 + c - '0';
                return boost::indeterminate;
            }
            break;

        case state_status_reason:
            if(c == '\r')
            {
                state_ = state_expecting_newline;
                return boost::indeterminate;
            }
            else if(is_char(c))
            {
                return boost::indeterminate;
            }
            break;

        case state_expecting_newline:
            if(c == '\n')
            {
                state_ = state_headers;
                return boost::indeterminate;
            }
            break;

        case state_headers:
            return headers_parser_.consume(response, c);
    }

    return false;
}

void response_parser::set_response_status(http::response& response)
{
    response.set_status(static_cast<http::status::codes_type>(status_));
}

}    // namespace http
