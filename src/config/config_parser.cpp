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
#include "config_parser.hpp"

// BOOST headers
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

// STL headers
#include <iostream>
#include <iomanip>


config_parser::config_parser() :
    state_(state_start),
    line_(1),
    column_(1)
{
}

const config_entry config_parser::parse(const std::string& data)
{
    reset();

    bool success = true;

    entries_ = std::stack<config_entry>();
    entries_.push(config_entry());

    std::string::const_iterator it = data.begin();
    std::string::const_iterator end = data.end();

    while(it != end)
    {
        if(!consume(*it))
        {
            success = false;
            break;
        }

        if(*it == '\n')
        {
            ++line_;
            column_ = 1;
        }
        else
        {
            ++column_;
        }

        ++it;
    }

    if(state_ != state_start)
        success = false;

    if(!success)
    {
        if(it == end)
            throw config_error("unexpected end of file", line_, column_);
        else
            throw config_error(get_error_message(*it), line_, column_);
    }

    return entries_.top();
}

void config_parser::reset()
{
    state_ = state_start;
    line_ = column_ = 1;
}

bool config_parser::consume(int c)
{
    switch(state_)
    {
        case state_start:
            if(c == '}')
            {
                return end_current_block();
            }
            else if(is_alpha(c))
            {
                name_.assign(1, c);
                state_ = state_name;
                return true;
            }
            else if(is_space(c) || c == ';')
            {
                return true;
            }
            break;

        case state_name:
            if(c == '{')
            {
                state_ = state_start;
                start_new_block();
                return true;
            }
            else if(is_space(c))
            {
                state_ = state_value_start;
                return true;
            }
            else if(is_charsym(c))
            {
                name_.push_back(c);
                return true;
            }
            break;

        case state_value_start:
            if(c == '{')
            {
                start_new_block();
                state_ = state_start;
                return true;
            }
            else if(c == '\"')
            {
                value_.clear();
                state_ = state_quoted_value;
                return true;
            }
            else if(is_space(c))
            {
                return true;
            }
            else if(c != ';')
            {
                value_.assign(1, c);
                state_ = state_simple_value;
                return true;
            }
            break;

        case state_simple_value:
            if(c == ';')
            {
                add_config_entry();
                state_ = state_start;
                return true;
            }
            else if(!is_newline(c) && c != '\"')
            {
                value_.push_back(c);
                return true;
            }
            break;

        case state_quoted_value:
            if(c == '\"')
            {
                state_ = state_quoted_value_finish;
                return true;
            }
            else if(c == '\\')
            {
                state_ = state_escaped_value;
                return true;
            }
            else if(!is_newline(c))
            {
                value_.push_back(c);
                return true;
            }
            break;

        case state_quoted_value_finish:
            if(c == ';')
            {
                add_config_entry();
                state_ = state_start;
                return true;
            }
            else if(c == '\"')
            {
                state_ = state_quoted_value;
                return true;
            }
            else if(is_space(c))
            {
                return true;
            }
            break;

        case state_escaped_value:
            switch(c)
            {
                case '\\':
                    value_.push_back('\\');
                    break;

                case '\"':
                    value_.push_back('\"');
                    break;

                case 'n':
                    value_.push_back('\n');
                    break;

                case 'r':
                    value_.push_back('\r');
                    break;

                case 't':
                    value_.push_back('\t');
                    break;

                default:
                    return false;
            }

            state_ = state_quoted_value;
            return true;
    }

    return false;
}

void config_parser::add_config_entry()
{
    if(state_ == state_simple_value)
        boost::trim_right(value_);

    entries_.top().add_entry(
        config_entry(name_, value_));
}

void config_parser::start_new_block()
{
    config_entry entry(name_);
    entries_.top().add_entry(entry);
    entries_.push(entry);
}

bool config_parser::end_current_block()
{
    if(!entries_.empty())
    {
        entries_.pop();

        if(!entries_.empty())
            return true;
    }

    return false;
}

bool config_parser::is_alpha(int c) const
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool config_parser::is_digit(int c) const
{
    return (c >= '0' && c <= '9');
}

bool config_parser::is_alnum(int c) const
{
    return (is_alpha(c) || is_digit(c));
}

bool config_parser::is_charsym(int c) const
{
    return (is_alnum(c) || c == '_' || c == '-');
}

bool config_parser::is_newline(int c) const
{
    return (c == '\r' || c == '\n');
}

bool config_parser::is_space(int c) const
{
    return (is_newline(c) || c == ' ' || c == '\t');
}

const std::string config_parser::get_error_message(int c) const
{
    std::ostringstream message;

    switch(state_)
    {
        case state_start:
            message << "expected '}' or entry name";
            break;

        case state_name:
            message << "expected '{'";
            break;

        case state_simple_value:
            if(is_newline(c))
                message << "expected semicolon ';'";
            else
                message << "unexpected symbol: '" << char(c) << "'";
            break;

        case state_value_start:
        case state_quoted_value:
            message << "unexpected symbol: '" << char(c) << "'";
            break;

        case state_quoted_value_finish:
            message << "expected semicolon ';'";
            break;

        case state_escaped_value:
            message << "bad escape character: '" << char(c) << "'";
            break;

        default:
            message << "unknown error";
    }

    return message.str();
}
