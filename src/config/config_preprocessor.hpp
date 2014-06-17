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
#ifndef _config_preprocessor_hpp
#define _config_preprocessor_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "config_entry.hpp"

// BOOST headers
#include <boost/noncopyable.hpp>

// STL headers
#include <string>


class config_preprocessor :
    private boost::noncopyable
{
public:
    config_preprocessor();

    const std::string process(const std::string& data);

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

#endif  // _config_preprocessor_hpp
