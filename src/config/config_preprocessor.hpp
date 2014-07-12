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
#ifndef _chat_config_preprocessor_hpp
#define _chat_config_preprocessor_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <config/config_entry.hpp>

// STL headers
#include <string>


namespace chat
{

class config_preprocessor
{
public:
    config_preprocessor();

    config_preprocessor(config_preprocessor const&) = delete;
    config_preprocessor& operator=(config_preprocessor const&) = delete;

    std::string process(std::string const& data);

private:
    void reset();
    bool consume(int c);

    void add_current_char(int c);

    bool is_newline(int c) const;
    bool is_space(int c) const;

private:
    enum state
    {
        state_start,
        state_comment_start,
        state_line_comment,
        state_c_comment,
        state_c_comment_finish,
        state_quoted_value,
        state_escaped_value
    } state_;

    bool comment_;
    std::string data_;
};

}   // namespace chat

#endif  // _chat_config_preprocessor_hpp
