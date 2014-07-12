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
#ifndef _chat_config_parser_hpp
#define _chat_config_parser_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <config/config_entry.hpp>

// STL headers
#include <string>
#include <stack>


namespace chat
{

class config_parser
{
public:
    config_parser();

    config_parser(config_parser const&) = delete;
    config_parser& operator=(config_parser const&) = delete;

    config_entry parse(std::string const& data);

private:
    void reset();
    bool consume(int c);

    void add_config_entry();

    void start_new_block();
    bool end_current_block();

    bool is_alpha(int c) const;
    bool is_digit(int c) const;
    bool is_alnum(int c) const;
    bool is_charsym(int c) const;
    bool is_newline(int c) const;
    bool is_space(int c) const;

    std::string get_error_message(int c) const;

private:
    enum state
    {
        state_start,
        state_name,
        state_value_start,
        state_simple_value,
        state_quoted_value,
        state_quoted_value_finish,
        state_escaped_value
    } state_;

    int line_;
    int column_;

    std::string name_;
    std::string value_;

    std::stack<config_entry> entries_;
};

}   // namespace chat

#endif  // _chat_config_parser_hpp
