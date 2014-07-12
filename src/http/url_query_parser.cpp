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
#include <http/url_query_parser.hpp>


namespace http
{

url_query_parser::url_query_parser() :
    state_(state_name_start)
{
}

url_query url_query_parser::parse(url const& url)
{
    reset();

    for(char const c : url.get_query())
    {
        if(!consume(c))
            throw std::runtime_error("cannot parse url query: " + url.get_query());
    }

    url_query_.add(name_, value_);

    return url_query_;
}

void url_query_parser::reset()
{
    state_ = state_name_start;
    url_query_.clear();
}

bool url_query_parser::consume(char c)
{
    switch(state_)
    {
        case state_name_start:
            if(c == '&')
            {
                return true;
            }
            else if(c != '=')
            {
                name_.assign(1, c);
                state_ = state_name;
                return true;
            }
            break;

        case state_name:
            if(c == '=')
            {
                value_.clear();
                state_ = state_value;
                return true;
            }
            else if(c == '&')
            {
                url_query_.add(name_, value_);
                state_ = state_name_start;
                return true;
            }
            else
            {
                name_.push_back(c);
                return true;
            }
            break;

        case state_value:
            if(c == '&')
            {
                url_query_.add(name_, value_);
                state_ = state_name_start;
                return true;
            }
            else if(c != '=')
            {
                value_.push_back(c);
                return true;
            }
            break;
    }

    return false;
}

}   // namespace http
