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
#include <config/config_preprocessor.hpp>


namespace chat
{

config_preprocessor::config_preprocessor() :
    state_(state_start),
    comment_(false)
{
}

std::string config_preprocessor::process(std::string const& data)
{
    reset();

    data_.clear();
    data_.reserve(data.size());

    std::string::const_iterator it = data.begin();
    std::string::const_iterator end = data.end();

    while(it != end)
    {
        if(consume(*it))
            add_current_char(*it);

        ++it;
    }

    return data_;
}

void config_preprocessor::reset()
{
    comment_ = false;
    state_ = state_start;
}

bool config_preprocessor::consume(int c)
{
    switch(state_)
    {
        case state_start:
            if(c == '/')
            {
                state_ = state_comment_start;
                return false;
            }
            else if(c == '#')
            {
                comment_ = true;
                state_ = state_line_comment;
            }
            else if(c == '\"')
            {
                state_ = state_quoted_value;
            }
            break;

        case state_comment_start:
            if(c == '/')
            {
                comment_ = true;
                data_.push_back(' ');
                state_ = state_line_comment;
            }
            else if(c == '*')
            {
                comment_ = true;
                data_.push_back(' ');
                state_ = state_c_comment;
            }
            else
            {
                data_.push_back('/');
                state_ = state_start;
            }
            break;

        case state_line_comment:
            if(is_newline(c))
            {
                comment_ = false;
                state_ = state_start;
            }
            break;

        case state_c_comment:
            if(c == '*')
                state_ = state_c_comment_finish;
            break;

        case state_c_comment_finish:
            if(c == '/')
            {
                comment_ = false;
                data_.push_back(' ');
                state_ = state_start;
                return false;
            }
            else if(c != '*')
            {
                state_ = state_c_comment;
            }
            break;

        case state_quoted_value:
            if(c == '\"')
                state_ = state_start;
            else if(c == '\\')
                state_ = state_escaped_value;
            break;

        case state_escaped_value:
            state_ = state_quoted_value;
            break;
    }

    return true;
}

void config_preprocessor::add_current_char(int c)
{
    if(c != '\r')
    {
        if(comment_ && !is_space(c))
            data_.push_back(' ');
        else
            data_.push_back(c);
    }
}

bool config_preprocessor::is_newline(int c) const
{
    return c == '\r' || c == '\n';
}

bool config_preprocessor::is_space(int c) const
{
    return is_newline(c) || c == ' ' || c == '\t';
}

}   // namespace chat
