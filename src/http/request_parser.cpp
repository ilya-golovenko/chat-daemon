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
#include <http/request_parser.hpp>


namespace http
{

request_parser::request_parser() :
    state_(state_method_start),
    size_(0u)
{
}

void request_parser::reset()
{
    state_ = state_method_start;
    size_ = 0u;
    method_.clear();
    url_.clear();
    version_parser_.reset();
    headers_parser_.reset();
}

boost::tribool request_parser::consume(request& request, char c)
{
    if(++size_ > max_message_size)
        return false;

    boost::tribool result;

    switch(state_)
    {
        case state_method_start:
            if(is_char(c))
            {
                method_.push_back(c);
                state_ = state_method;
                return boost::indeterminate;
            }
            break;

        case state_method:
            if(c == ' ')
            {
                set_request_method(request);
                state_ = state_url_start;
                return boost::indeterminate;
            }
            else if(is_char(c))
            {
                method_.push_back(c);
                return boost::indeterminate;
            }
            break;

        case state_url_start:
            if(!is_control(c))
            {
                url_.push_back(c);
                state_ = state_url;
                return boost::indeterminate;
            }
            break;

        case state_url:
            if(c == ' ')
            {
                set_request_url(request);
                state_ = state_http_version;
                return boost::indeterminate;
            }
            else if(!is_control(c))
            {
                url_.push_back(c);
                return boost::indeterminate;
            }
            break;

        case state_http_version:
            result = version_parser_.consume(request, c);

            if(result)
            {
                if(c == '\r')
                {
                    state_ = state_expecting_newline;
                    return boost::indeterminate;
                }
            }
            else
            {
                return result;
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
            return headers_parser_.consume(request, c);
    }

    return false;
}

void request_parser::set_request_method(request& request)
{
    request.set_method(method_);
}

void request_parser::set_request_url(request& request)
{
    request.set_url(url_);
}

}   // namespace http
